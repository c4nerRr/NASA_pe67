//
// Created by c4ner on 11/14/2026.
//
#include "HubMission.h"

#include "InputManager.h"

#include <algorithm>
#include <string>
#include <utility>

HubMission::HubMission(std::vector<GameState> requiredStations)
    : _requiredStations(std::move(requiredStations)) {
    std::sort(_requiredStations.begin(), _requiredStations.end());
    _requiredStations.erase(std::unique(_requiredStations.begin(),_requiredStations.end()),_requiredStations.end());
}

RestorationProgress HubMission::Progress() const {
    RestorationProgress progress{0,_requiredStations.size()};

    for (const auto station : _requiredStations) {
        if (GameManager::Instanse().IsChallengeFinished(station)) {
            ++progress.repaired;
        }
    }

    return progress;
}


bool HubMission::Update() {
    auto& manager = GameManager::Instanse();

    if (manager.IsDialogueActive()) {
        return true;
    }

    if (_pending != PendingAction::None) {
        FinishPendingAction();
        return true;
    }

    const Vector2 mouse = InputManager::MousePosition();

    if (InputManager::PressLeftMouse()) {
        const auto command = _panel.HitTest(
            _mission,
            _exitVisible,
            _confirmationOpen,
            mouse
        );

        switch (command.action) {
            case MissionPanel::Action::Talk:
                TalkToGuide();
                return true;

            case MissionPanel::Action::ToggleSystem:
                if (!_mission.ToggleSystem(command.system)) {
                    manager.ShowDialogue({
                        "Not enough free power. You can power only "
                        "two of the three systems with the current equipment. "
                        "Switch one active system OFF, then switch another ON."
                    });
                }
                return true;

            case MissionPanel::Action::RequestEndDay:
                if (
                    _mission.CurrentPhase() ==
                    Mission::Phase::Operations
                ) {
                    _confirmationOpen = true;
                }
                return true;

            case MissionPanel::Action::CancelEndDay:
                _confirmationOpen = false;
                return true;

            case MissionPanel::Action::ConfirmEndDay:
                if (_confirmationOpen) {
                    _confirmationOpen = false;

                    if (const auto report = _mission.EndDay()) {
                        ShowDayReport(*report);
                    }
                }
                return true;

            case MissionPanel::Action::Quit:
                manager.RequestQuit();
                return true;

            case MissionPanel::Action::None:
                break;
        }
    }

    return _confirmationOpen ||
           _panel.BlocksPointer(_mission, mouse);
}

void HubMission::Draw() const {
    _panel.Draw(
        _mission,
        GameManager::Instanse().IsDialogueActive(),
        _exitVisible,
        _confirmationOpen
    );
}

void HubMission::TalkToGuide() {
    auto& manager = GameManager::Instanse();

    if (_mission.CurrentPhase() == Mission::Phase::Restoration) {
        const auto progress = Progress();

        if (!progress.Ready()) {
            manager.ShowDialogue({
                "Day 1: restore the lunar base. "
                "Click a station and complete its challenge.",

                "Stations restored: " +
                std::to_string(progress.repaired) + "/" +
                std::to_string(progress.total) + ". "
                "Return to me after repairing every station."
            });

            return;
        }

        const auto& rules = _mission.Rules();

        if (manager.ShowDialogue({
            "All stations are repaired! "
            "Close this briefing to begin Day 2.",

            "Your goal: keep crew condition at " +
            std::to_string(rules.supplies.crew) + "/" +
            std::to_string(rules.supplies.crew) +
            " until the supply ship arrives. "
            "Condition measures crew health, not the number of people.",

            "Power only two systems at once. "
            "Production happens first; then the crew uses " +
            std::to_string(rules.dailyWater) + " water and " +
            std::to_string(rules.dailyFood) + " food.",

            "Prepare reserves on Day 2. The storm arrives on Day " +
            std::to_string(rules.stormDay) +
            ". Power the shelter and use your reserves. "
            "The ship arrives after Day " +
            std::to_string(rules.finalDay) + ".",

            "Shortages and an unprotected storm reduce condition. "
            "Damage carries over and cannot be healed in this mission. "
            "Surviving with damage gives a partial success.",

            "Click END DAY to review your plan. "
            "GO BACK changes it; FINISH DAY applies it."
        })) {
            _pending = PendingAction::StartOperations;
        }

        return;
    }

    if (_mission.CurrentPhase() != Mission::Phase::Operations) {
        return;
    }

    const auto& rules = _mission.Rules();

    std::vector<std::string> messages{
        "Day " + std::to_string(_mission.Day()) +
        ". Equipment power available: " +
        std::to_string(_mission.AvailablePower()) +
        ". Life support already has its reserved power.",

        "Switch a system OFF to free power for another. "
        "Keep the crew supplied and protected."
    };

    if (_mission.Day() == rules.stormDay) {
        messages.emplace_back(
            "The storm is here. Power the shelter to prevent " +
            std::to_string(rules.stormDamage) +
            " condition damage. Use stored supplies while "
            "one production system is OFF."
        );
    }
    else {
        messages.emplace_back(
            "Prepare water and food reserves before the storm. "
            "Unused supplies carry over to the next day."
        );
    }

    messages.emplace_back(
        "Full success means keeping the starting crew condition. "
        "Survival with damage is a partial success. "
        "Zero condition means mission failure."
    );

    messages.emplace_back(
        "END DAY shows the consequences before you confirm."
    );

    manager.ShowDialogue(messages);
}

void HubMission::FinishPendingAction() {
    const auto action = _pending;
    _pending = PendingAction::None;

    switch (action) {
        case PendingAction::StartOperations:
            if (Progress().Ready()) {
                _mission.StartOperations();
            }
            break;

        case PendingAction::RevealExit:
            _exitVisible = true;
            break;

        case PendingAction::None:
            break;
    }
}

void HubMission::ShowDayReport(const Mission::DayReport& report) {
  const auto& rules = _mission.Rules();

    std::vector<std::string> messages{
        "Day " + std::to_string(report.day) +
        " complete. Produced " +
        std::to_string(report.waterProduced) + " water and " +
        std::to_string(report.foodProduced) +
        " food. Supplies left: " +
        std::to_string(report.after.water) + " water, " +
        std::to_string(report.after.food) + " food."
    };

    if (report.waterShortage) {
        messages.emplace_back(
            "Water shortage. Condition penalty: " +
            std::to_string(report.waterDamage) + "."
        );
    }

    if (report.foodShortage) {
        messages.emplace_back(
            "Food shortage. Condition penalty: " +
            std::to_string(report.foodDamage) + "."
        );
    }

    if (report.storm) {
        if (report.protectedFromStorm) {
            messages.emplace_back(
                "The powered shelter protected the crew "
                "from the storm."
            );
        }
        else {
            messages.emplace_back(
                "The shelter was OFF. Storm condition penalty: " +
                std::to_string(report.stormDamage) + "."
            );
        }
    }

    messages.emplace_back(
        "Crew condition: " +
        std::to_string(report.before.crew) + " -> " +
        std::to_string(report.after.crew) + "/" +
        std::to_string(rules.supplies.crew) + "."
    );

    switch (report.outcome) {
        case Mission::Outcome::FullSuccess:
            messages.emplace_back(
                "FULL SUCCESS! The supply ship has arrived. "
                "The crew stayed healthy throughout the mission. "
                "Your supplies and protection were enough!"
            );
            break;

        case Mission::Outcome::PartialSuccess:
            messages.emplace_back(
                "CREW RESCUED - PARTIAL SUCCESS. "
                "The ship has arrived, but the crew suffered damage. "
                "Next time, prepare reserves and keep the shelter "
                "powered during the storm."
            );
            break;

        case Mission::Outcome::Failure:
            messages.emplace_back(
                "MISSION FAILED. Crew condition reached zero. "
                "The crew could not hold out until rescue. "
                "Plan supplies and storm protection together."
            );
            break;

        case Mission::Outcome::InProgress:
            messages.emplace_back(
                "Day " + std::to_string(_mission.Day()) +
                " begins. " +
                (_mission.Day() == rules.stormDay
                    ? "The storm is here. Check the shelter!"
                    : "Review your reserves and power allocation.")
            );

            messages.emplace_back(
                "Switches stay where you left them. "
                "Remaining supplies and condition carry over. "
                "Click END DAY when your plan is ready."
            );
            break;
    }

    if (_mission.IsFinished()) {
        messages.emplace_back(
            "Close this report to see the mission result."
        );
    }

    const bool shown =
        GameManager::Instanse().ShowDialogue(messages);

    if (_mission.IsFinished()) {
        if (shown) {
            _pending = PendingAction::RevealExit;
        }
        else {
            _exitVisible = true;
        }
    }
}