

#ifndef NASA_GAME3_H
#define NASA_GAME3_H
#include <array>

#include "IGame.h"
#include "IState.h"
#include "RadiationArt.h"
#include "RadiationBase.h"
#include "raylib.h"


class Game3 : public IGame, public IState{

    using Module = RadiationBase::Module;

    enum class Phase {
        Theory,
        Rules,
        Preparation,
        Storm,
        Result
    };

    struct Layout {
        Rectangle panel{};
        Rectangle dangerBar{};
        Rectangle switchPower{};
        Rectangle help{};

        std::array<Rectangle, RadiationBase::ModuleCount> modules{};

        int width = 0;
        int height = 0;
        float scale = 1.0f;
    };

    static constexpr float PreparationDuration = 20.0f;
    static constexpr float FirstThreatChange = 5.0f;
    static constexpr float LockDuration = 4.0f;
    static constexpr float StormDuration = 1.5f;

    static constexpr int MinChangeInterval = 3;
    static constexpr int MaxChangeInterval = 7;

    RadiationArt _art;
    RadiationBase _base;
    Layout _layout;

    Phase _phase = Phase::Theory;
    bool _theoryShown = false;

    float _preparationElapsed = 0.0f;
    float _nextThreatChange = FirstThreatChange;
    float _stormRemaining = 0.0f;
    float _visualTime = 0.0f;

    void BuildLayout();

    void ShowTheory();
    void ShowRules();

    void StartGame();
    void BeginRound();
    void ChangeThreat();

    void UpdatePreparation(float dt);
    void BeginStorm();
    void UpdateStorm(float dt);
    void FinishGame();

    [[nodiscard]] bool TargetLocked() const;

    void DrawHUD() const;
    void DrawModules() const;
    void DrawModule(Module module, Rectangle bounds) const;
    void DrawStorm() const;

    public:
    Game3();
    ~Game3() override;

    void Update(float dt) override;
    void Draw() const override;
    void OnEnter() override;

};


#endif //NASA_GAME3_H