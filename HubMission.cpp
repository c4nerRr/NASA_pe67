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
            "All stations are repaired! Close this briefing "
            "to begin Day 2.",

            "Choose which equipment receives power. "
            "Stations produce resources first; then the crew consumes " +
            std::to_string(rules.dailyWater) + " water and " +
            std::to_string(rules.dailyFood) + " food.",

            "A solar storm arrives on Day " +
            std::to_string(rules.stormDay) +
            ". The supply ship arrives after Day " +
            std::to_string(rules.finalDay) + ".",

            "Click END DAY when ready. You can review the result "
            "and either confirm or go back. "
            "Click me whenever you need help."
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
        "Day " + std::to_string(_mission.Day()) + ". "
        "Available power for equipment: " +
        std::to_string(_mission.AvailablePower()) +
        ". Life support already has its reserved power.",

        "Click a system to switch it ON or OFF. "
        "If there is not enough free power, switch another system OFF first."
    };

    if (_mission.Day() == rules.stormDay) {
        messages.emplace_back(
            "The solar storm is here. Power the shelter to prevent "
            "storm damage. Use stored water or food while one "
            "production system is switched off."
        );
    }
    else {
        messages.emplace_back(
            "Build water and food reserves before the storm. "
            "Unused supplies carry over to the next day."
        );
    }

    messages.emplace_back(
        "Click END DAY to review your plan. "
        "GO BACK lets you change it. FINISH DAY applies it. "
        "Warnings explain consequences; they do not forbid your decision."
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
        " complete. Stations produced " +
        std::to_string(report.waterProduced) + " water and " +
        std::to_string(report.foodProduced) +
        " food. Supplies left after consumption: " +
        std::to_string(report.after.water) + " water and " +
        std::to_string(report.after.food) + " food."
    };

    if (report.waterShortage) {
        messages.emplace_back(
            "Water shortage: crew condition fell by " +
            std::to_string(rules.shortageDamage) + "."
        );
    }

    if (report.foodShortage) {
        messages.emplace_back(
            "Food shortage: crew condition fell by " +
            std::to_string(rules.shortageDamage) + "."
        );
    }

    if (report.storm) {
        messages.emplace_back(
            report.protectedFromStorm
                ? "The powered shelter protected the crew "
                  "from the solar storm."
                : "The unpowered shelter left the crew exposed. "
                  "Storm damage: " +
                  std::to_string(rules.stormDamage) + "."
        );
    }

    messages.emplace_back(
        "Crew condition: " +
        std::to_string(report.after.crew) + "/" +
        std::to_string(rules.supplies.crew) + "."
    );

    if (_mission.CurrentPhase() == Mission::Phase::Won) {
        messages.emplace_back(
            "We made it! The supply ship has arrived and the crew is alive. "
            "Your repairs and resource decisions kept this outpost running!"
        );

        messages.emplace_back(
            "Mission complete. Close this message, "
            "then click QUIT GAME to leave."
        );
    }
    else if (_mission.CurrentPhase() == Mission::Phase::Lost) {
        messages.emplace_back(
            "Mission failed. The crew could not hold out until "
            "the supply ship arrived. Balance food and water production, "
            "and remember the storm shelter."
        );

        messages.emplace_back(
            "Close this message, then click QUIT GAME to leave."
        );
    }
    else {
        messages.emplace_back(
            "Day " + std::to_string(_mission.Day()) + " begins. " +
            (_mission.Day() == rules.stormDay
                ? "The solar storm is here! Power the shelter "
                  "and use your reserves wisely."
                : "Review your reserves and plan today's power allocation.")
        );

        messages.emplace_back(
            "Yesterday's switches stay where you left them. "
            "Change the allocation as needed, then click END DAY. "
            "Click the guide if you need help."
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