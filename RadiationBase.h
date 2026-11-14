
#ifndef NASA_RADIATIONBASE_H
#define NASA_RADIATIONBASE_H


class RadiationBase {

    public:
    static constexpr int ModuleCount = 3;

    static constexpr float InitialDanger = 70.0f;
    static constexpr float MaximumDanger = 100.0f;

    static constexpr float SuccessReduction = InitialDanger / 4.0f;
    static constexpr float FailureIncrease = 15.0f;

    enum class Module {Greenhouse = 0,Habitat = 1,Laboratory = 2};

    enum class Outcome {
        Playing,
        Won,
        Lost
    };

    enum class StormResult {
        None,
        Protected,
        Missed
    };

    RadiationBase();
    ~RadiationBase();

    [[nodiscard]] float Danger() const;
    [[nodiscard]] int Round() const;

    [[nodiscard]] Module PoweredModule() const;
    [[nodiscard]] Module ThreatenedModule() const;

    [[nodiscard]] Outcome Result() const;
    [[nodiscard]] StormResult LastStorm() const;


    bool BeginRound(Module module);
    bool SetThreat(Module threatened);
    void SwitchPower();
    bool ResolveStorm();

    private:
    float _danger = InitialDanger;
    int _round = 0;

    Module _powered = Module::Greenhouse;
    Module _threatened = Module::Greenhouse;

    StormResult _lastStorm = StormResult::None;
    bool _roundOpen = false;

    [[nodiscard]] static bool IsValidModule(Module module);

};


#endif //NASA_RADIATIONBASE_H