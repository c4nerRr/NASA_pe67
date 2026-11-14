#include "Game3.h"

#include "GameManager.h"
#include "InputManager.h"
#include "raylib.h"

#include "Game3.h"

#include <algorithm>
#include <cmath>

#include "GameManager.h"
#include "InputManager.h"

namespace {
    int FontSize(int referenceSize, float scale) {
        return std::max(10,static_cast<int>(referenceSize * scale));
    }

    void Label(const char* text,float x,float y,int size,Color color
    ) {
        DrawText(text,static_cast<int>(x),static_cast<int>(y),size,color);
    }

    void CenteredText(const char* text,Rectangle bounds,int size,Color color) {
        Label(text,bounds.x + (bounds.width - MeasureText(text, size)) * 0.5f,bounds.y + (bounds.height - size) * 0.5f,size,color);
    }

    void Button(Rectangle bounds,const char* text,float scale,bool enabled) {
        const bool hovered =enabled &&CheckCollisionPointRec(InputManager::MousePosition(),bounds);

        const Color background = !enabled  ? Color{28, 32, 43, 255} : hovered? Color{70, 89, 125, 255}: Color{43, 57, 84, 255};

        DrawRectangleRec(bounds, background);

        DrawRectangleLinesEx(bounds,2.0f,enabled ? SKYBLUE : DARKGRAY);

        CenteredText(text,bounds,FontSize(26, scale),enabled ? RAYWHITE : GRAY);
    }

    void RadiationSymbol(Vector2 center, float radius, Color color) {
        for (int i = 0; i < 3; ++i) {
            const float angle = -90.0f + i * 120.0f;

            DrawRing(center,radius * 0.38f,radius,angle,angle + 60.0f,12,color);
        }

        DrawCircleV(center, radius * 0.19f, color);
    }

    void PowerSymbol(Vector2 center, float radius) {
        const Vector2 points[] = {
            {center.x + radius * 0.45f, center.y - radius},
            {center.x - radius * 0.40f, center.y},
            {center.x + radius * 0.30f, center.y},
            {center.x - radius * 0.45f, center.y + radius}
        };

        for (int i = 0; i < 3; ++i) {
            DrawLineEx(points[i],points[i + 1],std::max(2.0f, radius * 0.22f),GOLD);
        }
    }
}

void Game3::BuildLayout() {
    _layout.width = GetScreenWidth();
    _layout.height = GetScreenHeight();

    const auto w = static_cast<float>(_layout.width);
    const auto h = static_cast<float>(_layout.height);

    _layout.scale = std::min(w / 1920.0f,h / 1080.0f
    );

    _layout.panel = {w * 0.06f, h * 0.07f,w * 0.88f, h * 0.86f};

    _layout.dangerBar = {w * 0.10f, h * 0.195f,w * 0.28f, h * 0.018f};

    _layout.modules = {
        Rectangle{w * 0.13f, h * 0.52f,w * 0.20f, h * 0.20f},
        Rectangle{w * 0.40f, h * 0.38f,w * 0.20f, h * 0.20f},
        Rectangle{w * 0.67f, h * 0.52f,w * 0.20f, h * 0.20f}
    };

    _layout.switchPower = {w * 0.40f, h * 0.82f,w * 0.20f, h * 0.07f};

    _layout.help = {w * 0.79f, h * 0.82f,w * 0.10f, h * 0.07f};
}

void Game3::ShowTheory() {
    _theoryShown = GameManager::Instanse().ShowDialogue({
       "RADIATION ALERT: A Moon base needs more than air, water "
       "and food. Its crew also needs protection from space radiation.",

       "Earth has a thick atmosphere and a global magnetic field "
       "that help protect us. The Moon has almost no atmosphere "
       "and no global magnetic field like Earth's.",

       "Some solar eruptions accelerate particles to very high "
       "energies. These particles can damage cells and DNA, "
       "making radiation protection important for astronauts.",

       "Protective material between people and incoming particles "
       "can reduce exposure. NASA describes using available supplies "
       "to improve a spacecraft shelter. Lunar soil, called regolith, "
       "could also help shield shelters on the Moon.",

       "NASA teams monitor space weather and radiation detectors. "
       "Their recommendations can include moving into better-shielded "
       "areas or postponing a spacewalk.",

       "Our switchable shields, short countdowns and danger percentages "
       "are made-up game rules. Zero danger means you completed this "
       "challenge; it does not mean radiation has disappeared from "
       "the Moon. The lesson is to notice warnings and prepare protection."
   });
}

void Game3::ShowRules() {
      GameManager::Instanse().ShowDialogue({
        TextFormat(
            "YOUR MISSION: Reduce the danger indicator from %.1f "
            "percent to zero. Reaching %.1f percent loses the attempt.",
            RadiationBase::InitialDanger,
            RadiationBase::MaximumDanger
        ),

        "THREE MODULES: T is the greenhouse, D is the habitat, "
        "and L is the laboratory. Power is available for only "
        "one module's shield at a time.",

        "READ THE SYMBOLS: The green radiation symbol marks the "
        "module that needs protection. The gold lightning symbol "
        "shows which module currently receives power.",

        "SWITCH: Click SWITCH to move power from T to D, "
        "then to L, then back to T. Power stays where you leave "
        "it between storms.",

        TextFormat(
            "COUNTDOWN: Each round gives you %.0f seconds. "
            "The first forecast change happens after %.0f seconds. "
            "After that, the green symbol moves to another module "
            "every %d to %d seconds.",
            PreparationDuration,
            FirstThreatChange,
            MinChangeInterval,
            MaxChangeInterval
        ),

        TextFormat(
            "FINAL WARNING: During the last %.0f seconds, "
            "the green symbol stops moving. You can still switch "
            "power until the countdown reaches zero.",
            LockDuration
        ),

        TextFormat(
            "STORM CHECK: At zero, the powered module must match "
            "the marked module. A correct shield reduces danger "
            "by %.1f percentage points. A wrong shield adds %.1f.",
            RadiationBase::SuccessReduction,
            RadiationBase::FailureIncrease
        ),

        "ONLY THE FINAL POSITION COUNTS: Earlier switches do not "
        "earn points or cause damage. During the short storm "
        "animation, the result is already fixed.",

        "KEEP GOING: A mistake does not always end the attempt. "
        "Rounds continue until danger reaches zero or 100 percent.",

        "HELP AND EXIT: RULES pauses the countdown while you read. "
        "SPACE during preparation leaves the attempt. Returning "
        "starts a fresh attempt. Only a victory closes this station."
    });
}

void Game3::StartGame() {
    _base = RadiationBase{};
    _visualTime = 0.0f;

    BeginRound();
}

void Game3::BeginRound() {
    const auto threatened = static_cast<Module>(
       GetRandomValue(0, RadiationBase::ModuleCount - 1)
   );

    _base.BeginRound(threatened);

    _preparationElapsed = 0.0f;
    _nextThreatChange = FirstThreatChange;
    _stormRemaining = 0.0f;

    _phase = Phase::Preparation;
}

void Game3::ChangeThreat() {
    const int current = static_cast<int>(
       _base.ThreatenedModule()
   );

    // Смещение 1 или 2 гарантирует другое здание.
    const int offset = GetRandomValue(
        1,
        RadiationBase::ModuleCount - 1
    );

    const int next =
        (current + offset) % RadiationBase::ModuleCount;

    _base.SetThreat(static_cast<Module>(next));
}

void Game3::UpdatePreparation(float dt) {
    auto& manager = GameManager::Instanse();

    if (InputManager::PressSPACE()) {
        manager.ChangeState(GameState::Hub);
        return;
    }

    const bool clicked = InputManager::PressLeftMouse();
    const Vector2 mouse = InputManager::MousePosition();

    if (clicked && CheckCollisionPointRec(mouse, _layout.help)) {
        ShowRules();
        return;
    }

    _preparationElapsed = std::min(PreparationDuration,_preparationElapsed + dt);

    const float lockStartsAt = PreparationDuration - LockDuration;


    while (_nextThreatChange < lockStartsAt &&_nextThreatChange <= _preparationElapsed) {
        ChangeThreat();

        _nextThreatChange += static_cast<float>(
            GetRandomValue(MinChangeInterval, MaxChangeInterval)
        );
           }


    if (_preparationElapsed >= PreparationDuration) {
        BeginStorm();
        return;
    }

    if (clicked && CheckCollisionPointRec(mouse, _layout.switchPower)) {
        _base.SwitchPower();
        }
}

void Game3::BeginStorm() {
    _base.ResolveStorm();

    _stormRemaining = StormDuration;
    _phase = Phase::Storm;
}

void Game3::UpdateStorm(float dt) {
    _stormRemaining = std::max(0.0f,_stormRemaining - dt);

    if (_stormRemaining > 0.0f) {
        return;
    }

    if (_base.Result() == RadiationBase::Outcome::Playing) {
        BeginRound();
    }
    else {
        FinishGame();
    }
}

void Game3::FinishGame() {
    _phase = Phase::Result;

    auto& manager = GameManager::Instanse();

    if (_base.Result() == RadiationBase::Outcome::Won) {
        IsFinished = true;

        manager.ShowDialogue({
            "MISSION COMPLETE! You brought the base's danger "
            "indicator down to zero and completed the protection task.",

            TextFormat(
                "Storms survived in this attempt: %d. "
                "Close this message to return to the Hub.",
                _base.Round()
            )
        });

        return;
    }

    manager.ShowDialogue({
        "MISSION FAILED! The base's danger indicator reached "
        "100 percent. The last priority module was not protected.",

        "Watch the green radiation symbol and move the gold power "
        "symbol to the same module before the countdown reaches zero.",

        "Close this message to return to the Hub. "
        "This station stays open for another attempt."
    });
}

bool Game3::TargetLocked() const {
    return _preparationElapsed >= PreparationDuration - LockDuration;
}

void Game3::DrawHUD() const {
     const auto w = static_cast<float>(_layout.width);
    const auto h = static_cast<float>(_layout.height);
    const float s = _layout.scale;

    const Color dangerColor =_base.Danger() >= 85.0f ? RED :_base.Danger() > 35.0f ? ORANGE :LIME;

    Label(TextFormat("RADIATION DANGER: %.1f%%", _base.Danger()),w * 0.10f,h * 0.15f,FontSize(26, s),dangerColor);

    DrawRectangleRec(
        _layout.dangerBar,
        Color{42, 48, 63, 255}
    );

    Rectangle fill = _layout.dangerBar;

    fill.width *=
        _base.Danger() / RadiationBase::MaximumDanger;

    DrawRectangleRec(fill, dangerColor);

    Label(TextFormat("Storm %d", _base.Round()),w * 0.10f,h * 0.225f,FontSize(22, s),LIGHTGRAY);

    const Rectangle timerBounds{w * 0.65f, h * 0.15f,w * 0.25f, h * 0.05f};

    const Rectangle statusBounds{w * 0.22f, h * 0.275f,w * 0.56f, h * 0.035f};

    if (_phase == Phase::Preparation) {
        const int secondsLeft = static_cast<int>(
            std::ceil(PreparationDuration - _preparationElapsed)
        );

        CenteredText(TextFormat("STORM IN: %02d", secondsLeft),timerBounds,FontSize(32, s),TargetLocked() ? ORANGE : RAYWHITE);
        const char* status =TargetLocked() ? "TARGET LOCKED - CHECK YOUR SHIELD": _preparationElapsed < FirstThreatChange ? "PREPARE - WATCH THE GREEN SYMBOL" : "FORECAST CHANGING - WATCH THE GREEN SYMBOL";

        CenteredText(status,statusBounds,FontSize(22, s),TargetLocked() ? ORANGE : SKYBLUE);
    }
    else {
        CenteredText(_phase == Phase::Storm ? "STORM ACTIVE" : "ATTEMPT ENDED",timerBounds,FontSize(28, s),ORANGE);
    }
}

void Game3::DrawModules() const {
    const Vector2 source{_layout.switchPower.x + _layout.switchPower.width * 0.5f,_layout.switchPower.y};

    for (int i = 0; i < RadiationBase::ModuleCount; ++i) {
        const Rectangle bounds = _layout.modules[i];
        const auto module = static_cast<Module>(i);

        const Vector2 destination{bounds.x + bounds.width * 0.5f,bounds.y + bounds.height};

        const bool powered = module == _base.PoweredModule();

        DrawLineEx(source,destination,std::max(1.0f, 3.0f * _layout.scale),powered ? GOLD : Color{48, 61, 79, 255});
    }

    for (int i = 0; i < RadiationBase::ModuleCount; ++i) {
        DrawModule(static_cast<Module>(i),_layout.modules[i]);
    }
}

void Game3::DrawModule(Module module, Rectangle bounds) const {
     constexpr const char* names[] = {"GREENHOUSE","HABITAT","LABORATORY"};

    constexpr const char* letters[] = {"T","D","L"};

    const int index = static_cast<int>(module);
    const float s = _layout.scale;

    const bool powered = module == _base.PoweredModule();
    const bool threatened = module == _base.ThreatenedModule();

    const Color border = powered ? GOLD : Color{105, 129, 158, 255};

    const Vector2 roofLeft{bounds.x,bounds.y};

    const Vector2 roofTop{bounds.x + bounds.width * 0.5f,bounds.y - bounds.height * 0.22f};

    const Vector2 roofRight{bounds.x + bounds.width,bounds.y};

    const float lineWidth = std::max(2.0f, 3.0f * s);

    DrawLineEx(roofLeft, roofTop, lineWidth, border);
    DrawLineEx(roofTop, roofRight, lineWidth, border);

    DrawRectangleRec(bounds,powered? Color{57, 54, 38, 255}: Color{28, 40, 58, 255});

    DrawRectangleLinesEx(bounds, lineWidth, border);

    CenteredText(names[index],{bounds.x,bounds.y + bounds.height * 0.10f,bounds.width,bounds.height * 0.18f},FontSize(24, s),RAYWHITE);

    CenteredText(letters[index],{bounds.x,bounds.y + bounds.height * 0.32f,bounds.width,bounds.height * 0.32f},FontSize(56, s),border);

    CenteredText(powered ? "SHIELD: ON" : "SHIELD: OFF",{bounds.x,bounds.y + bounds.height * 0.75f,bounds.width,bounds.height * 0.16f},FontSize(22, s),powered ? GOLD : LIGHTGRAY);

    const float symbolY =bounds.y - bounds.height * 0.30f;

    if (powered) {
        PowerSymbol({bounds.x + bounds.width * 0.84f,symbolY},21.0f * s);
    }

    if (threatened) {
        const float pulse =1.0f + 0.08f * std::sin(_visualTime * 4.0f);

        RadiationSymbol({bounds.x + bounds.width * 0.16f,symbolY},23.0f * s * pulse,LIME);

        CenteredText(powered ? "PRIORITY: SHIELD READY" : "PRIORITY: NEEDS POWER",{bounds.x - bounds.width * 0.10f,bounds.y + bounds.height * 1.06f,bounds.width * 1.20f,bounds.height * 0.14f},FontSize(20, s),powered ? LIME : ORANGE);
    }
}

void Game3::DrawStorm() const {
    const Rectangle panel = _layout.panel;
    const float s = _layout.scale;

    const bool protectedModule =
        _base.LastStorm() == RadiationBase::StormResult::Protected;

    const Color accent = protectedModule ? SKYBLUE : ORANGE;

    BeginScissorMode(
        static_cast<int>(panel.x),
        static_cast<int>(panel.y),
        static_cast<int>(panel.width),
        static_cast<int>(panel.height)
    );

    DrawRectangleRec(panel, Fade(accent, 0.14f));

    for (int i = 0; i < 32; ++i) {
        const float horizontal = std::fmod(i * 0.618f, 1.0f);

        const float progress = std::fmod(
            _visualTime * 0.75f + i * 0.113f,
            1.0f
        );

        const Vector2 start{
            panel.x + panel.width * horizontal,
            panel.y + panel.height * progress
        };

        const Vector2 end{
            start.x + 24.0f * s,
            start.y + 48.0f * s
        };

        DrawLineEx(
            start,
            end,
            std::max(1.0f, 2.0f * s),
            Fade(accent, 0.45f)
        );
    }

    const Rectangle message{
        panel.x + panel.width * 0.20f,
        panel.y + panel.height * 0.38f,
        panel.width * 0.60f,
        panel.height * 0.18f
    };

    DrawRectangleRec(message, Color{10, 16, 28, 240});
    DrawRectangleLinesEx(message, 2.0f, accent);

    CenteredText(
        "SOLAR RADIATION STORM",
        {
            message.x,
            message.y + message.height * 0.12f,
            message.width,
            message.height * 0.32f
        },
        FontSize(30, s),
        RAYWHITE
    );

    CenteredText(
        protectedModule
            ? "SHIELD HELD - DANGER REDUCED"
            : "PRIORITY MODULE UNPROTECTED - DANGER INCREASED",
        {
            message.x,
            message.y + message.height * 0.55f,
            message.width,
            message.height * 0.25f
        },
        FontSize(23, s),
        accent
    );

    EndScissorMode();
}

Game3::Game3() = default;

Game3::~Game3() = default;

void Game3::Update(float dt) {
    if (_layout.width != GetScreenWidth() || _layout.height != GetScreenHeight()) {
        BuildLayout();
      }

    dt = std::max(0.0f, dt);
    _visualTime += dt;

    switch (_phase) {
        case Phase::Theory:
            _phase = Phase::Rules;
            ShowRules();
            break;

        case Phase::Rules:
            StartGame();
            break;

        case Phase::Preparation:
            UpdatePreparation(dt);
            break;

        case Phase::Storm:
            UpdateStorm(dt);
            break;

        case Phase::Result:
            GameManager::Instanse().ChangeState(GameState::Hub);
            break;
    }
}

void Game3::Draw() const {
    if (_layout.width <= 0 || _layout.height <= 0) {
        return;
    }

    const auto w = static_cast<float>(_layout.width);
    const auto h = static_cast<float>(_layout.height);
    const float s = _layout.scale;

    DrawRectangleRec(_layout.panel,Color{15, 23, 39, 255});

    DrawRectangleLinesEx(_layout.panel,2.0f,Color{95, 122, 160, 255});

    CenteredText("RADIATION ALERT",{w * 0.20f, h * 0.085f, w * 0.60f, h * 0.05f},FontSize(36, s),RAYWHITE);

    if (_phase == Phase::Theory || _phase == Phase::Rules) {
        return;
    }

    DrawHUD();
    DrawModules();

    CenteredText("GREEN SYMBOL: PRIORITY MODULE    GOLD BOLT: POWER",{w * 0.10f, h * 0.77f, w * 0.80f, h * 0.035f},FontSize(20, s),LIGHTGRAY);

    const bool canInteract = _phase == Phase::Preparation;

    Button(_layout.switchPower, "SWITCH", s, canInteract);
    Button(_layout.help, "RULES", s, canInteract);

    CenteredText("SPACE - leave attempt during preparation",{w * 0.10f, h * 0.94f, w * 0.80f, h * 0.035f},FontSize(20, s),LIGHTGRAY);

    if (_phase == Phase::Storm) {
        DrawStorm();
    }
}

void Game3::OnEnter() {
    BuildLayout();

    if (IsFinished) {
        _phase = Phase::Result;

        GameManager::Instanse().ShowDialogue({
            "This radiation protection mission is already complete!"
        });

        return;
    }

    if (!_theoryShown) {
        _phase = Phase::Theory;
        ShowTheory();
    }
    else {
        _phase = Phase::Rules;
        ShowRules();
    }
}