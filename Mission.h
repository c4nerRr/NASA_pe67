//
// Created by c4ner on 11/14/2026.
//
#ifndef NASA_MISSION_H
#define NASA_MISSION_H

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

struct MissionSupplies {
    int water = 3;
    int food = 1;
    int crew = 3;
};

struct MissionSystem {
    std::string name{};
    std::string effect{};

    int powerCost{};
    int waterProduced{};
    int foodProduced{};

    bool shieldsCrew{};
};

struct MissionRules {
    MissionSupplies supplies{};

    int finalDay = 3;
    int stormDay = 3;

    int generatorPower = 6;
    int lifeSupportPower = 2;

    int dailyWater = 2;
    int dailyFood = 2;

    int shortageDamage = 1;
    int stormDamage = 2;

    std::vector<MissionSystem> systems{
        {"WATER PURIFIER","+3 water per day",2, 3, 0, false},
        {"GREENHOUSE","+3 food per day",2, 0, 3, false},
        {"RADIATION SHELTER","Protects crew during the storm",2, 0, 0, true}
    };
};

class Mission {
public:
    enum class Phase {
        Restoration,
        Operations,
        Finished
    };

    enum class Outcome {
        InProgress,
        FullSuccess,
        PartialSuccess,
        Failure
    };

    struct DayReport {
        int day = 0;

        MissionSupplies before{};
        MissionSupplies after{};

        int waterProduced = 0;
        int foodProduced = 0;

        bool waterShortage = false;
        bool foodShortage = false;

        bool storm = false;
        bool protectedFromStorm = false;

        int waterDamage = 0;
        int foodDamage = 0;
        int stormDamage = 0;


        Outcome outcome = Outcome::InProgress;
    };

    explicit Mission(MissionRules rules = {});
    ~Mission();

    [[nodiscard]] Phase CurrentPhase() const;
    [[nodiscard]] Outcome Result() const;

    [[nodiscard]] int Day() const;
    [[nodiscard]] bool IsFinished() const;

    [[nodiscard]] const MissionRules& Rules() const;
    [[nodiscard]] const MissionSupplies& Supplies() const;

    [[nodiscard]] bool IsPowered(std::size_t index) const;
    [[nodiscard]] int AvailablePower() const;
    [[nodiscard]] int UsedPower() const;

    bool StartOperations();
    bool ToggleSystem(std::size_t index);

    [[nodiscard]] std::optional<DayReport> PreviewDat() const;
    std::optional<DayReport> EndDay();

private:
    MissionRules _rules;
    MissionSupplies _supplies;

    std::vector<bool> _powered;

    int _day = 1;
    Phase _phase = Phase::Restoration;
    [[nodiscard]] Outcome EvaluateOutcome(int completedDay,const MissionSupplies& supplies) const;
};

#endif // NASA_MISSION_H