#include "Game1.h"
#include "GameManager.h"
#include "InputManager.h"
#include <algorithm>
#include <array>
#include <stdexcept>

Game1::Game1() : _frame(), _sliderRects{}, _minusBtns{}, _plusBtns{}, _actionBtn(), _secondaryBtn() {
    _subsystems[0] = { "Oxygen (Critical)", 30, 30, 0, true };
    _subsystems[1] = { "Heating (Critical)", 20, 20, 0, true };
    _subsystems[2] = { "Greenhouse", 30, 30, 0, false };
    _subsystems[3] = { "Lighting", 10, 10, 0, false };
}

Game1::~Game1()
{
    if (_plashka.id != 0) {
        UnloadTexture(_plashka);
    }
}

void Game1::StartNewAttempt()
{
    _draggedSliderIndex = -1;
    _currentCycle = 1;
    _extraPoints = 0;
    _criticalWarningAcknowledged = false;
    GenerateCycleEvent();
}

void Game1::UpdateLayout() {
    const auto sw = static_cast<float>(GetScreenWidth());
    const auto sh = static_cast<float>(GetScreenHeight());

    _uiScale = std::min(sw / 1920.0f, sh / 1080.0f);
    const float s = _uiScale;

    _frame = { sw * 0.5f - 800 * s,sh * 0.5f - 450 * s,1600 * s,900 * s };

    const float sliderWidth = _frame.width - 540 * s;

    for (int i = 0; i < subsystemsCount; ++i) {
        const float y = _frame.y + (300 + i * 112) * s;

        _iconRects[i] = { _frame.x + 40 * s,y - 18 * s,64 * s,64 * s };

        _minusBtns[i] = { _frame.x + 130 * s,y - 3 * s,34 * s,34 * s };
        _sliderRects[i] = { _frame.x + 180 * s,y,sliderWidth,28 * s };

        _plusBtns[i] = { _sliderRects[i].x + sliderWidth + 16 * s,y - 3 * s,34 * s,34 * s };
    }

    _actionBtn = { sw * 0.5f - 200 * s,_frame.y + _frame.height - 150 * s,400 * s,52 * s };

    _secondaryBtn = _actionBtn;
    _secondaryBtn.y -= 72 * s;
}

Rectangle Game1::SliderHandle(int index) const {
    const auto bar = _sliderRects[index];

    const float ratio = std::clamp(static_cast<float>(_subsystems[index].allocated) / _availableEnergy, 0.0f, 1.0f);

    const float width = 8 * _uiScale;
    const float height = 48 * _uiScale;

    return { bar.x + bar.width * ratio - width * 0.5f,bar.y + (bar.height - height) * 0.5f,width,height };
}

void Game1::HandleSliders(Vector2 mousePos)
{
    if (!InputManager::IsLeftMouseDown())
    {
        _draggedSliderIndex = -1;
    }

    if (InputManager::PressLeftMouse())
    {
        for (int i = 0; i < subsystemsCount; ++i)
        {
            if (CheckCollisionPointRec(mousePos, _minusBtns[i]))
            {
                if (_subsystems[i].allocated > 0)
                {
                    _subsystems[i].allocated--;
                    _criticalWarningAcknowledged = false;
                }
            }
            if (CheckCollisionPointRec(mousePos, _plusBtns[i]))
            {
                int total = 0;
                for (const auto& s : _subsystems) total += s.allocated;
                if (total < _availableEnergy)
                {
                    _subsystems[i].allocated++;
                    _criticalWarningAcknowledged = false;
                }
            }
            if (CheckCollisionPointRec(mousePos, _sliderRects[i]) || CheckCollisionPointRec(mousePos, SliderHandle(i)))
            {
                _draggedSliderIndex = i;
            }
        }
    }

    if (_draggedSliderIndex != -1 && InputManager::IsLeftMouseDown())
    {
        Rectangle rect = _sliderRects[_draggedSliderIndex];
        float norm = (mousePos.x - rect.x) / rect.width;
        if (norm < 0.0f) norm = 0.0f;
        if (norm > 1.0f) norm = 1.0f;

        int desiredValue = static_cast<int>(norm * _availableEnergy);

        int otherAllocated = 0;
        for (int i = 0; i < subsystemsCount; ++i)
        {
            if (i != _draggedSliderIndex) otherAllocated += _subsystems[i].allocated;
        }

        int maxAllowed = _availableEnergy - otherAllocated;
        if (desiredValue > maxAllowed) desiredValue = maxAllowed;

        if (_subsystems[_draggedSliderIndex].allocated != desiredValue)
        {
            _subsystems[_draggedSliderIndex].allocated = desiredValue;
            _criticalWarningAcknowledged = false;
        }
    }
}

void Game1::CalculateCycleResult()
{
    bool isGameOverTriggered = false;
    _resultMessage = "Check " + std::to_string(_currentCycle) + " results:\n\n";

    for (auto& sys : _subsystems)
    {
        if (sys.allocated < sys.currentReq)
        {
            if (sys.isCritical)
            {
                isGameOverTriggered = true;
                _resultMessage += "- " + sys.name + ": CRITICAL FAILURE.\n";
            }
            else
            {
                _resultMessage += "- " + sys.name + ": offline (insufficient energy).\n";
            }
        }
        else
        {
            _resultMessage += "- " + sys.name + ": operating nominally.\n";
            if (!sys.isCritical)
            {
                _extraPoints++;
            }
        }
    }

    if (isGameOverTriggered)
    {
        _phase = Phase::GameOver;
    }
    else
    {
        _phase = Phase::CycleResult;
    }
}

void Game1::Update(float dt)
{
    if (InputManager::PressSPACE())
    {
        GameManager::Instanse().ChangeState(GameState::Hub);
        return;
    }

    UpdateLayout();

    Vector2 mousePos = InputManager::MousePosition();

    switch (_phase)
    {
    case Phase::Theory:
    {
        if (!GameManager::Instanse().IsDialogueActive())
        {
            _phase = Phase::Rules;
            ShowRules();
        }
        break;
    }
    case Phase::Rules:
    {
        if (!GameManager::Instanse().IsDialogueActive())
        {
            _phase = Phase::Playing;
        }
        break;
    }
    case Phase::Playing:
    {
        HandleSliders(mousePos);

        if (InputManager::PressLeftMouse() && CheckCollisionPointRec(mousePos, _actionBtn))
        {
            bool criticalsMet = (_subsystems[0].allocated >= _subsystems[0].currentReq) &&
                (_subsystems[1].allocated >= _subsystems[1].currentReq);

            if (!criticalsMet && !_criticalWarningAcknowledged)
            {
                _criticalWarningAcknowledged = true;
            }
            else
            {
                _criticalWarningAcknowledged = false;
                CalculateCycleResult();
            }
        }
        break;
    }
    case Phase::CycleResult:
    {
        if (InputManager::PressLeftMouse() && CheckCollisionPointRec(mousePos, _actionBtn))
        {
            if (_currentCycle >= 5)
            {
                _phase = Phase::Win;
                IsFinished = true;
                ShowWin();
            }
            else
            {
                _currentCycle++;
                GenerateCycleEvent();
                _phase = Phase::Playing;
            }
        }
        break;
    }
    case Phase::GameOver:
    {
        if (InputManager::PressLeftMouse())
        {
            if (CheckCollisionPointRec(mousePos, _actionBtn))
            {
                GameManager::Instanse().ChangeState(GameState::Hub);
            }
            else if (CheckCollisionPointRec(mousePos, _secondaryBtn))
            {
                StartNewAttempt();
                _phase = Phase::Playing;
            }
        }
        break;
    }
    case Phase::Win:
    {
        if (!GameManager::Instanse().IsDialogueActive())
        {
            GameManager::Instanse().ChangeState(GameState::Hub);
            return;
        }
        break;
    }
    }
}

void Game1::Draw() const
{
    using Image = EnergyArt::Image;

    const float s = _uiScale;
    const auto mouse = InputManager::MousePosition();
    const auto sw = static_cast<float>(GetScreenWidth());
    const auto sh = static_cast<float>(GetScreenHeight());

    EnergyArt::Draw(_art.Get(Image::Background), { 0, 0, sw, sh }, Color{ 80, 80, 80, 255 });

    const float pw = 1700.0f * s;
    const float ph = 950.0f * s;
    DrawTexturePro(
        _plashka,
        { 0.0f, 0.0f, static_cast<float>(_plashka.width), static_cast<float>(_plashka.height) },
        { (sw - pw) * 0.5f, (sh - ph) * 0.5f, pw, ph },
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );

    const float x = _frame.x + 120 * s;
    const float y = _frame.y + 32 * s;

    const auto text = [s](const char* value, float tx, float ty, int size, Color color) {
        DrawText(value, static_cast<int>(tx), static_cast<int>(ty), std::max(10, static_cast<int>(size * s)), color);
        };

    const auto button = [s, mouse](Rectangle bounds, const char* label) {
        const bool hovered = CheckCollisionPointRec(mouse, bounds);

        DrawRectangleRounded(bounds, 0.25f, 8, hovered ? Color{ 255, 225, 100, 255 } : Color{ 238, 181, 48, 255 });

        const int size = std::max(10, static_cast<int>(22 * s));

        constexpr Color ink{ 21, 26, 54, 255 };

        DrawText(label, static_cast<int>(bounds.x + (bounds.width - MeasureText(label, size)) * 0.5f),
            static_cast<int>(bounds.y + (bounds.height - size) * 0.5f), size, ink);
        };

    const auto centered = [&](const char* value, float ty, int size, Color color) {
        const int font = std::max(10, static_cast<int>(size * s));

        text(value, (GetScreenWidth() - MeasureText(value, font)) * 0.5f, ty, size, color);
        };

    centered("POWER SUPPLY RESTORATION", y, 36, BLACK);

    centered("SPACE - return to base", y + 46 * s, 20, DARKGRAY);

    if (_phase == Phase::Playing) {
        text(TextFormat("SYSTEM CHECK: %d / 5", _currentCycle), x, y + 120 * s, 24, RAYWHITE);

        text(_eventMessage.c_str(), x, y + 155 * s, 20, ORANGE);

        int used = 0;
        for (const auto& system : _subsystems) {
            used += system.allocated;
        }

        text(TextFormat("ENERGY USED: %d / %d    LEFT: %d", used, _availableEnergy, _availableEnergy - used), x, y + 190 * s, 24, LIGHTGRAY);

        constexpr std::array<Image, subsystemsCount> icons{
            Image::Oxygen,
            Image::Heating,
            Image::Greenhouse,
            Image::Lighting
        };

        for (int i = 0; i < subsystemsCount; ++i) {
            const auto& system = _subsystems[i];
            const auto bar = _sliderRects[i];

            const bool enough = system.allocated >= system.currentReq;

            const Color status = enough ? LIME : system.isCritical ? ORANGE : LIGHTGRAY;

            EnergyArt::Draw(_art.Get(icons[i]), _iconRects[i], status);

            text(system.name.c_str(), _minusBtns[i].x, bar.y - 32 * s, 20, RAYWHITE);

            text(TextFormat("%d / req %d", system.allocated, system.currentReq), _plusBtns[i].x + _plusBtns[i].width + 16 * s, bar.y + 2 * s, 20, status);

            button(_minusBtns[i], "-");
            button(_plusBtns[i], "+");

            DrawRectangleRec(bar, Color{ 40, 43, 65, 255 });

            const float ratio = static_cast<float>(system.allocated) / _availableEnergy;

            EnergyArt::Draw(_art.Get(enough ? Image::GoodBar : Image::BadBar), bar, WHITE, ratio);

            const auto handle = SliderHandle(i);

            EnergyArt::Draw(
                _art.Get(Image::Handle),
                handle
            );

            DrawRectangleLinesEx(
                handle,
                std::max(1.0f, s),
                RAYWHITE
            );

            const float required = std::clamp(static_cast<float>(system.currentReq) / _availableEnergy, 0.0f, 1.0f);

            const float markerX =
                bar.x + bar.width * required;

            DrawLineEx(
                { markerX, bar.y - 12 * s },
                { markerX, bar.y + bar.height + 12 * s },
                std::max(1.0f, 2 * s),
                GOLD
            );
        }

        if (_criticalWarningAcknowledged) {
            text(
                "CRITICAL SYSTEMS UNDERPOWERED. "
                "Press LAUNCH again to confirm.",
                x,
                _actionBtn.y - 42 * s,
                20,
                ORANGE
            );
        }

        button(_actionBtn, "LAUNCH");
    }
    else if (
        _phase == Phase::CycleResult ||
        _phase == Phase::GameOver
        ) {
        const bool failed = _phase == Phase::GameOver;

        text(
            failed ? "POWER SUPPLY FAILURE" : "SYSTEMS REPORT",
            x,
            y + 112 * s,
            32,
            failed ? ORANGE : RAYWHITE
        );

        text(
            _resultMessage.c_str(),
            x,
            y + 180 * s,
            24,
            LIGHTGRAY
        );

        if (failed) {
            button(_secondaryBtn, "RETRY");
            button(_actionBtn, "RETURN TO BASE");
        }
        else {
            button(_actionBtn, _currentCycle >= 5 ? "COMPLETE MISSION" : "NEXT CHECK");
        }
    }
}

void Game1::GenerateCycleEvent()
{
    for (auto& sys : _subsystems)
    {
        sys.allocated = 0;
        sys.currentReq = sys.baseReq;
    }

    int randomEvent = GetRandomValue(0, 3);

    switch (randomEvent)
    {
    case 0:
    {
        _availableEnergy = 100;
        _eventMessage = "Nominal mode: Generation stable. 100 energy units.";
        break;
    }
    case 1:
    {
        _availableEnergy = 70;
        _eventMessage = "WARNING: Solar storm! Panel efficiency dropped.";
        break;
    }
    case 2:
    {
        _availableEnergy = 60;
        _eventMessage = "WARNING: Lunar night! Base on battery power.";
        break;
    }
    case 3:
    {
        _availableEnergy = 100;
        _subsystems[1].currentReq = 45;
        _eventMessage = "WARNING: Sudden cold! Heating requires more energy.";
        break;
    }
    default:
        throw std::invalid_argument("Invalid event type");
    }
}

void Game1::ShowTheory()
{
    GameManager::Instanse().ShowDialogue({
        "Mission: Power Supply Restoration.",

        "Energy is super precious around here. "
        "We measure every drop in energy units.",

        "The station runs on solar panels and batteries. "
        "You must allocate the power.",

        "Keep Oxygen and Heating running at all costs. "
        "Our crew literally can't live without them!",

        "The Greenhouse and Lighting can take a quick nap "
        "if things get tight. It won't hurt anyone "
        "in this challenge.",

        "Stay sharp for wild space weather: "
        "storms and sudden deep freezes "
        "love to crash the party!"
    });
}

void Game1::ShowRules()
{
    GameManager::Instanse().ShowDialogue({
        "HOW TO PLAY:",
        "There are 5 power system checks.",
        "Drag sliders or press +/- to allocate energy.",
        "Do not exceed available power. Extra power over the requirement (Req) is wasted.",
        "Pay attention to alerts! Requirements and generation may change.",
        "Secure critical systems for all 5 checks. Numerical thresholds and instant failure are game simplifications."
        });
}

void Game1::OnEnter()
{
    _art.Load();
    UpdateLayout();

    if (_plashka.id == 0) {
        _plashka = LoadTexture("assets/game1/plashka.png");
    }

    StartNewAttempt();

    if (!isHereOnce)
    {
        isHereOnce = true;
        _phase = Phase::Theory;
        ShowTheory();
    }
    else
    {
        _phase = Phase::Playing;
    }
}

void Game1::ShowWin()
{
    std::string extraStr = std::to_string(_extraPoints);
    GameManager::Instanse().ShowDialogue({
        "Power supply restored.",
        "Critical systems remained stable during all 5 checks.",
        "Secondary systems (greenhouse and lighting) were secured in " + extraStr + " out of 10 cases.",
        "Return to base and complete the repairs on other stations."
        });
}
