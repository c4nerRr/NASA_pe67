#pragma once
#include "IGame.h"
#include "IState.h"
#include <string>
#include "raylib.h"
#include <array>
#include "EnergyArt.h"

enum class Phase
{
    Theory,
    Rules,
    Playing,
    CycleResult,
    GameOver,
    Win
};

struct Subsystem
{
    std::string name;
    int baseReq;
    int currentReq;
    int allocated;
    bool isCritical;
};

class Game1 : public IGame, public IState
{
public:
    Game1();
    ~Game1() override;

    void OnEnter() override;
    void Update(float dt) override;
    void Draw() const override;

private:
    void StartNewAttempt();
    void UpdateLayout();
    [[nodiscard]] Rectangle SliderHandle(int index) const;
    void HandleSliders(Vector2 mousePos);
    void CalculateCycleResult();
    void GenerateCycleEvent();

    void ShowTheory();
    void ShowRules();
    void ShowWin();

    Phase _phase = Phase::Theory;
    bool isHereOnce = false;

    static const int subsystemsCount = 4;
    Subsystem _subsystems[subsystemsCount];

    int _availableEnergy = 100;
    int _currentCycle = 1;
    int _extraPoints = 0;

    std::string _eventMessage;
    std::string _resultMessage;

    EnergyArt _art;
    float _uiScale = 1.0f;
    std::array<Rectangle, subsystemsCount> _iconRects{};
    Rectangle _frame;
    Rectangle _sliderRects[subsystemsCount];
    Rectangle _minusBtns[subsystemsCount];
    Rectangle _plusBtns[subsystemsCount];
    Rectangle _actionBtn;
    Rectangle _secondaryBtn;

    int _draggedSliderIndex = -1;
    bool _criticalWarningAcknowledged = false;
};