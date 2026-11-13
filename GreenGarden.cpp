//
// Created by c4ner on 11/13/2026.
//

#include "GreenGarden.h"

GreenGarden::GreenGarden() = default;

GreenGarden::~GreenGarden() = default;

int GreenGarden::Bed::Moisture() const {
    return pours + (storedWater ? 1 : 0);
}

bool GreenGarden::Bed::Ready() const {
    return !dead && growth >= GrowthNeeded;
}

const GreenGarden::Bed & GreenGarden::GetBed(int index) const {
    return _beds[index];
}

int GreenGarden::Day() const {
    return _day;
}

int GreenGarden::WaterLeft() const {
    return _water;
}

int GreenGarden::Reputation() const {
    return _reputation;
}

int GreenGarden::FailedBed() const {
    return _failedBed;
}

GreenGarden::Outcome GreenGarden::Result() const {
    return _outcome;
}

bool GreenGarden::Water(int index) {
    if (!CanCareFor(index)) {
        return false;
    }

    Bed& bed = _beds[index];

    if (_water == 0 || bed.pours >= MaxPours) {
        return false;
    }

    ++bed.pours;
    --_water;

    return true;
}

void GreenGarden::ToggleLight(int index) {
    if (CanCareFor(index)) {
        _beds[index].lightOn = !_beds[index].lightOn;
    }
}

void GreenGarden::AdvanceDay(const std::array<bool, BedCount> &rot) {
    if (_outcome != Outcome::Playing) {
        return;
    }

    bool allCaredFor = true;
    bool allReady = true;

    for (int i = 0; i < BedCount; ++i) {
        Bed& bed = _beds[i];

        if (bed.Ready()) {
            continue;
        }

        const int moisture = bed.Moisture();

        const bool overwatered =
            moisture >= 3 ||
            (moisture == 2 && rot[i]);

        bed.dryDays =
            moisture == 0 ? bed.dryDays + 1 : 0;

        bed.dead =
            overwatered || bed.dryDays >= 2;

        if (bed.dead) {
            if (_failedBed < 0) {
                _failedBed = i;

                _outcome = overwatered
                    ? Outcome::Overwatered
                    : Outcome::DriedOut;
            }

            allCaredFor = false;
        }
        else if (moisture > 0 && bed.lightOn) {
            ++bed.growth;
        }
        else {
            allCaredFor = false;
        }

        bed.storedWater =
            !bed.dead &&
            !bed.Ready() &&
            moisture == 2;

        bed.pours = 0;

        if (!bed.Ready()) {
            allReady = false;
        }
    }
    if (_outcome != Outcome::Playing) { //NOLINT
        _reputation = 0;
        return;
    }

    if (allCaredFor) {
        _reputation += _water;
    }

    if (allReady) {
        _outcome = Outcome::Won;
    }
    else if (_day >= MaxDays) {
        _outcome = Outcome::TimeUp;
        _reputation = 0;
    }
    else {
        ++_day;
        _water = WaterPerDay;
    }
}

bool GreenGarden::CanCareFor(int index) const {
}
