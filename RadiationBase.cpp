
#include "RadiationBase.h"

#include <algorithm>

RadiationBase::RadiationBase() = default;

RadiationBase::~RadiationBase() = default;

float RadiationBase::Danger() const {
    return _danger;
}

int RadiationBase::Round() const {
    return _round;
}

RadiationBase::Module RadiationBase::PoweredModule() const {
    return _powered;
}

RadiationBase::Module RadiationBase::ThreatenedModule() const {
    return _threatened;
}

RadiationBase::Outcome RadiationBase::Result() const {
    if (_danger <= 0.0f) {
        return Outcome::Won;
    }

    if (_danger >= MaximumDanger) {
        return Outcome::Lost;
    }

    return Outcome::Playing;
}

RadiationBase::StormResult RadiationBase::LastStorm() const {
    return _lastStorm;
}

bool RadiationBase::BeginRound(Module module) {
    if (_roundOpen ||Result() != Outcome::Playing ||!IsValidModule(module))
        {
        return false;
        }

    ++_round;

    _threatened = module;
    _lastStorm = StormResult::None;
    _roundOpen = true;

    return true;
}

bool RadiationBase::SetThreat(Module threatened) {
    if (!_roundOpen || !IsValidModule(threatened)) {
        return false;
    }

    _threatened = threatened;

    return true;
}

void RadiationBase::SwitchPower() {
    if (!_roundOpen) {
        return;
    }

    const int current = static_cast<int>(_powered);
    const int next = (current + 1) % ModuleCount;

    _powered = static_cast<Module>(next);
}

bool RadiationBase::ResolveStorm() {
    if (!_roundOpen) {
        return false;
    }

    _roundOpen = false;

    const bool protectedModule = _powered == _threatened;

    if (protectedModule) {
        _lastStorm = StormResult::Protected;
        _danger -= SuccessReduction;
    }
    else {
        _lastStorm = StormResult::Missed;
        _danger += FailureIncrease;
    }

    _danger = std::clamp(_danger,0.0f,MaximumDanger);

    return true;
}



bool RadiationBase::IsValidModule(Module module) {
    const int index = static_cast<int>(module);

    return index >= 0 && index < ModuleCount;
}
