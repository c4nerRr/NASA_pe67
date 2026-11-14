#pragma once

#include <vector>

#include "GameManager.h"
#include "Mission.h"
#include "MissionPanel.h"

class HubMission {
public:
    explicit HubMission(std::vector<GameState> requiredStations);

    bool Update();
    void Draw() const;

private:
    enum class PendingAction {
        None,
        StartOperations,
        RevealExit
    };

    Mission _mission;
    MissionPanel _panel;

    std::vector<GameState> _requiredStations;

    PendingAction _pending = PendingAction::None;

    bool _exitVisible = false;
    bool _confirmationOpen = false;

    [[nodiscard]] RestorationProgress Progress() const;

    void TalkToGuide();
    void FinishPendingAction();
    void ShowDayReport(const Mission::DayReport& report);
};