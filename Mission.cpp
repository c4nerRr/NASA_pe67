//
// Created by c4ner on 11/14/2026.
//
#include "Mission.h"
#include <algorithm>
#include <stdexcept>
#include <utility>

Mission::Mission(MissionRules rules)
    : _rules(std::move(rules)),
      _supplies(_rules.supplies),
      _powered(_rules.systems.size(), false) {

    if (
        _rules.finalDay < 2 ||
        _rules.stormDay < 2 ||
        _rules.stormDay > _rules.finalDay ||
        _rules.lifeSupportPower < 0 ||
        _rules.generatorPower < _rules.lifeSupportPower ||
        _rules.dailyWater < 0 ||
        _rules.dailyFood < 0 ||
        _rules.shortageDamage < 0 ||
        _rules.stormDamage < 0 ||
        _supplies.water < 0 ||
        _supplies.food < 0 ||
        _supplies.crew <= 0
    ) {
        throw std::invalid_argument("Invalid mission rules");
    }

    for (const auto& system : _rules.systems) {
        if (
            system.powerCost < 0 ||
            system.waterProduced < 0 ||
            system.foodProduced < 0
        ) {
            throw std::invalid_argument("Invalid mission system");
        }
    }
}

Mission::~Mission() = default;

Mission::Phase Mission::CurrentPhase() const {
    return _phase;
}

Mission::Outcome Mission::Result() const {
    if (!IsFinished()) {
        return Outcome::InProgress;
    }

    return EvaluateOutcome(_day, _supplies);
}

int Mission::Day() const {
    return _day;
}

bool Mission::IsFinished() const {
    return _phase == Phase::Finished;
}

const MissionRules& Mission::Rules() const {
    return _rules;
}

const MissionSupplies& Mission::Supplies() const {
    return _supplies;
}

bool Mission::IsPowered(std::size_t index) const {
    return index < _powered.size() && _powered[index];
}

int Mission::AvailablePower() const {
    return _rules.generatorPower - _rules.lifeSupportPower;
}

int Mission::UsedPower() const {
    int used = 0;

    for (std::size_t i = 0; i < _powered.size(); ++i) {
        if (_powered[i]) {
            used += _rules.systems[i].powerCost;
        }
    }

    return used;
}

bool Mission::StartOperations() {
    if (_phase != Phase::Restoration) {
        return false;
    }

    _phase = Phase::Operations;
    _day = 2;

    return true;
}

bool Mission::ToggleSystem(std::size_t index) {
    if (
        _phase != Phase::Operations ||
        index >= _powered.size()
    ) {
        return false;
    }

    if (
        !_powered[index] &&
        UsedPower() + _rules.systems[index].powerCost >
            AvailablePower()
    ) {
        return false;
    }

    _powered[index] = !_powered[index];
    return true;
}

Mission::Outcome Mission::EvaluateOutcome(
    int completedDay,
    const MissionSupplies& supplies
) const {
    if (supplies.crew <= 0) {
        return Outcome::Failure;
    }

    if (completedDay < _rules.finalDay) {
        return Outcome::InProgress;
    }

    if (supplies.crew == _rules.supplies.crew) {
        return Outcome::FullSuccess;
    }

    return Outcome::PartialSuccess;
}

std::optional<Mission::DayReport> Mission::PreviewDat() const {
    if (_phase != Phase::Operations) {
        return std::nullopt;
    }

    DayReport report;
    report.day = _day;
    report.before = _supplies;
    report.storm = (_day == _rules.stormDay);

    for (std::size_t i = 0; i < _powered.size(); ++i) {
        if (!_powered[i]) {
            continue;
        }

        const auto& system = _rules.systems[i];

        report.waterProduced += system.waterProduced;
        report.foodProduced += system.foodProduced;

        report.protectedFromStorm =
            report.protectedFromStorm || system.shieldsCrew;
    }

    const int water = _supplies.water + report.waterProduced;
    const int food = _supplies.food + report.foodProduced;

    report.waterShortage = water < _rules.dailyWater;
    report.foodShortage = food < _rules.dailyFood;

    report.waterDamage =
        report.waterShortage ? _rules.shortageDamage : 0;

    report.foodDamage =
        report.foodShortage ? _rules.shortageDamage : 0;

    report.stormDamage =
        report.storm && !report.protectedFromStorm
            ? _rules.stormDamage
            : 0;

    const int totalDamage =
        report.waterDamage +
        report.foodDamage +
        report.stormDamage;

    report.after = {
        std::max(0, water - _rules.dailyWater),
        std::max(0, food - _rules.dailyFood),
        std::max(0, _supplies.crew - totalDamage)
    };

    report.outcome = EvaluateOutcome(
        report.day,
        report.after
    );

    return report;
}

std::optional<Mission::DayReport> Mission::EndDay() {
    auto report = PreviewDat();

    if (!report) {
        return std::nullopt;
    }

    _supplies = report->after;

    if (report->outcome == Outcome::InProgress) {
        ++_day;
    }
    else {
        _phase = Phase::Finished;
    }

    return report;
}
