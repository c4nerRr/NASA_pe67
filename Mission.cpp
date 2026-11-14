//
// Created by c4ner on 11/14/2026.
//

#include "Mission.h"

#include <stdexcept>

Mission::Mission(MissionRules rules) : _rules(std::move(rules)), _supplies(_rules.supplies), _powered(_rules.systems.size(), false) {
    if (_rules.finalDay < 2 || _rules.stormDay < 2 || _rules.stormDay > _rules.finalDay || _rules.lifeSupportPower < 0 || _rules.generatorPower < _rules.lifeSupportPower ||_rules.dailyWater < 0 ||_rules.dailyFood < 0 ||_rules.shortageDamage < 0 ||_rules.stormDamage < 0 ||_supplies.water < 0 ||_supplies.food < 0 ||_supplies.crew <= 0) {
        throw std::invalid_argument("Mission::Mission::Mission");
    }
    for (const auto& system : _rules.systems) {
        if (system.powerCost < 0 || system.waterProduced < 0 || system.foodProduced < 0) {
            throw std::invalid_argument("Mission::Mission::Mission");
        }
    }
}

Mission::~Mission() {
}

Mission::Phase Mission::CurrentPhase() const {
    return _phase;
}

int Mission::Day() const {
    return _day;
}

bool Mission::IsFinished() const {
    return _phase == Phase::Won || _phase == Phase::Lost;
}

const MissionRules & Mission::Rules() const {
    return _rules;
}

const MissionSupplies & Mission::Supplies() const {
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
    if (_phase != Phase::Operations || index >= _powered.size()) {
        return false;
    }

    if (!_powered[index] &&
        UsedPower() + _rules.systems[index].powerCost > AvailablePower()) {

        return false;
        }

    _powered[index] = !_powered[index];
    return true;
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

    int damage = 0;

    if (report.waterShortage) {
        damage += _rules.shortageDamage;
    }

    if (report.foodShortage) {
        damage += _rules.shortageDamage;
    }

    if (report.storm && !report.protectedFromStorm) {
        damage += _rules.stormDamage;
    }

    report.after = {
        std::max(0, water - _rules.dailyWater),
        std::max(0, food - _rules.dailyFood),
        std::max(0, _supplies.crew - damage)
    };

    return report;
}

std::optional<Mission::DayReport> Mission::EndDay() {
    auto report = PreviewDat();

    if (!report) {
        return std::nullopt;
    }

    _supplies = report->after;

    if (_supplies.crew == 0) {
        _phase = Phase::Lost;
    }
    else if (_day >= _rules.finalDay) {
        _phase = Phase::Won;
    }
    else {
        ++_day;
    }

    return report;
}
