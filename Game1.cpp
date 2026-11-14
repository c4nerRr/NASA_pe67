#include "Game1.h"
#include "GameManager.h"
#include "InputManager.h"

Game1::Game1() : _frame(), _sliderRects{}, _minusBtns{}, _plusBtns{}, _actionBtn(), _secondaryBtn() {
    _subsystems[0] = {"Oxygen (Critical)", 30, 30, 0, true};
    _subsystems[1] = {"Heating (Critical)", 20, 20, 0, true};
    _subsystems[2] = {"Greenhouse", 30, 30, 0, false};
    _subsystems[3] = {"Lighting", 10, 10, 0, false};
}

Game1::~Game1() = default;

void Game1::StartNewAttempt()
{
    _currentCycle = 1;
    _extraPoints = 0;
    _criticalWarningAcknowledged = false;
    GenerateCycleEvent();
}

void Game1::UpdateLayout()
{
    auto sw = static_cast<float>(GetScreenWidth());
    auto sh = static_cast<float>(GetScreenHeight());

    _frame = { sw * 0.10f, sh * 0.10f, sw * 0.80f, sh * 0.80f };

    float startX = _frame.x + 50.0f;
    float startY = _frame.y + _frame.height * 0.35f;
    float rowSpacing = _frame.height * 0.12f;
    float btnSize = 30.0f;

    float sliderWidth = _frame.width - 450.0f;

    for (int i = 0; i < subsystemsCount; ++i)
    {
        float y = startY + i * rowSpacing;
        _minusBtns[i] = { startX, y, btnSize, btnSize };
        _sliderRects[i] = { startX + btnSize + 15.0f, y, sliderWidth, btnSize };
        _plusBtns[i] = { startX + btnSize + 15.0f + sliderWidth + 15.0f, y, btnSize, btnSize };
    }

    float actionW = 350.0f;
    float actionH = 50.0f;
    _actionBtn = { _frame.x + _frame.width / 2.0f - actionW / 2.0f, _frame.y + _frame.height - actionH - 25.0f, actionW, actionH };

    _secondaryBtn = { _frame.x + _frame.width / 2.0f - actionW / 2.0f, _actionBtn.y - actionH - 15.0f, actionW, actionH };
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
            if (CheckCollisionPointRec(mousePos, _sliderRects[i]))
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
    DrawRectangleRec(_frame, MAROON);
    DrawRectangleLinesEx(_frame, 4.0f, RAYWHITE);

    const int baseX = static_cast<int>(_frame.x) + 32;
    const int baseY = static_cast<int>(_frame.y) + 32;

    DrawText("POWER SUPPLY RESTORATION", baseX, baseY, 36, RAYWHITE);
    DrawText("SPACE - return to base", baseX, baseY + 40, 20, LIGHTGRAY);

    auto drawBtn = [](Rectangle rect, const char* text, Vector2 mouse)
        {
        bool hovered = CheckCollisionPointRec(mouse, rect);
        DrawRectangleRec(rect, hovered ? LIGHTGRAY : GRAY);
        int tW = MeasureText(text, 20);
        DrawText(text, (int)(rect.x + rect.width / 2.0f - tW / 2.0f), (int)(rect.y + rect.height / 2.0f - 10), 20, BLACK);
        };

    Vector2 mousePos = InputManager::MousePosition();

    if (_phase == Phase::Playing)
    {
        DrawText(TextFormat("SYSTEM CHECK: %d / 5", _currentCycle), baseX, baseY + 90, 24, YELLOW);
        DrawText(_eventMessage.c_str(), baseX, baseY + 120, 20, ORANGE);

        int totalAllocated = 0;
        for (const auto& sys : _subsystems) {
            totalAllocated += sys.allocated;
        }


        Color energyColor = (totalAllocated > _availableEnergy) ? RED : GREEN;
        DrawText(TextFormat("ENERGY USED: %d / %d units", totalAllocated, _availableEnergy), baseX, baseY + 150, 24, energyColor);

        for (int i = 0; i < subsystemsCount; ++i)
        {
            float currentY = _sliderRects[i].y;
            float currentX = _minusBtns[i].x;

            DrawText(_subsystems[i].name.c_str(), (int)currentX, (int)currentY - 25, 20, RAYWHITE);

            DrawText(TextFormat("Allocated: %d / req %d", _subsystems[i].allocated, _subsystems[i].currentReq),
                (int)(_plusBtns[i].x + _plusBtns[i].width + 15), (int)currentY + 5, 20, RAYWHITE);

            drawBtn(_minusBtns[i], "-", mousePos);
            drawBtn(_plusBtns[i], "+", mousePos);

            DrawRectangleRec(_sliderRects[i], DARKGRAY);

            float fillRatio = (float)_subsystems[i].allocated / _availableEnergy;
            if (fillRatio > 1.0f) fillRatio = 1.0f;

            if (fillRatio > 0.0f)
            {
                Rectangle fillRect = { _sliderRects[i].x, currentY, _sliderRects[i].width * fillRatio, _sliderRects[i].height };
                Color fillColor = (_subsystems[i].allocated < _subsystems[i].currentReq) ? RED : GREEN;
                DrawRectangleRec(fillRect, fillColor);
            }

            float reqRatio = (float)_subsystems[i].currentReq / _availableEnergy;
            if (reqRatio > 1.0f) reqRatio = 1.0f;
            float reqX = _sliderRects[i].x + _sliderRects[i].width * reqRatio;
            DrawLineEx({ reqX, currentY - 5 }, { reqX, currentY + _sliderRects[i].height + 5 }, 3.0f, YELLOW);
        }

        if (_criticalWarningAcknowledged)
        {
            DrawText("CRITICAL WARNING: Systems underpowered! Press LAUNCH again to confirm.",
                (int)_frame.x + 40, (int)_actionBtn.y - 30, 18, RED);
        }

        drawBtn(_actionBtn, "LAUNCH", mousePos);
    }
    else if (_phase == Phase::CycleResult)
    {
        DrawText("SYSTEMS REPORT", baseX, baseY + 100, 36, RAYWHITE);
        DrawText(_resultMessage.c_str(), baseX, baseY + 160, 24, LIGHTGRAY);

        const char* btnText = (_currentCycle >= 5) ? "COMPLETE MISSION" : "NEXT CHECK";
        drawBtn(_actionBtn, btnText, mousePos);
    }
    else if (_phase == Phase::GameOver)
    {
        DrawText("POWER SUPPLY FAILURE", baseX, baseY + 100, 40, RED);
        DrawText(_resultMessage.c_str(), baseX, baseY + 160, 24, LIGHTGRAY);

        drawBtn(_secondaryBtn, "RETRY", mousePos);
        drawBtn(_actionBtn, "RETURN TO BASE", mousePos);
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
        "Energy is a strictly limited resource. It is measured in energy units.",
        "The station runs on solar panels and batteries. You must allocate the power.",
        "Oxygen and Heating are critical for crew survival.",
        "Greenhouse and Lighting are secondary. Their shutdown is not fatal.",
        "Be prepared for space weather events: storms and sudden cold."
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
    UpdateLayout();
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