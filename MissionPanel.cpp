//
// Created by c4ner on 11/14/2026.
//
#include "MissionPanel.h"

#include <algorithm>
#include <string>

#include "FontManager.h"
#include "GuideSptire.h"
#include "InputManager.h"

namespace {
       constexpr Color ink{24, 21, 49, 255};
    constexpr Color warningInk{145, 32, 32, 255};
    constexpr Color successInk{24, 74, 35, 255};

    void Text(
        const std::string& text,
        float x,
        float y,
        float width,
        float scale,
        Color color = ink,
        int referenceSize = 20
    ) {
        float size = std::max(
            10.0f,
            static_cast<float>(referenceSize) * scale
        );

        while (
            size > 10.0f &&
            FontManager::Measure(text.c_str(), size).x > width
        ) {
            size -= 1.0f;
        }

        FontManager::Draw(
            text.c_str(),
            x,
            y,
            size,
            color
        );
    }

    const char* OutcomeTitle(Mission::Outcome outcome) {
        switch (outcome) {
            case Mission::Outcome::FullSuccess:
                return "FULL SUCCESS";

            case Mission::Outcome::PartialSuccess:
                return "CREW RESCUED - PARTIAL SUCCESS";

            case Mission::Outcome::Failure:
                return "MISSION FAILED";

            case Mission::Outcome::InProgress:
                return "MISSION CONTINUES";
        }

        return "MISSION CONTINUES";
    }

    const char* OutcomeDescription(Mission::Outcome outcome) {
        switch (outcome) {
            case Mission::Outcome::FullSuccess:
                return "The ship arrived. You kept the crew healthy!";

            case Mission::Outcome::PartialSuccess:
                return "The ship arrived, but the crew suffered damage.";

            case Mission::Outcome::Failure:
                return "Crew condition reached zero before rescue.";

            case Mission::Outcome::InProgress:
                return "Prepare supplies and protect the crew.";
        }

        return "";
    }

    Color OutcomeColor(Mission::Outcome outcome) {
        switch (outcome) {
            case Mission::Outcome::FullSuccess:
                return successInk;

            case Mission::Outcome::PartialSuccess:
            case Mission::Outcome::Failure:
                return warningInk;

            case Mission::Outcome::InProgress:
                return ink;
        }

        return ink;
    }
}

MissionPanel::Layout MissionPanel::GetLayout(std::size_t systemCount) const {
    const auto w = static_cast<float>(GetScreenWidth());
    const auto h = static_cast<float>(GetScreenHeight());

    Layout layout;
    layout.scale = std::min(w / 1280.0f, h / 720.0f);

    layout.content = {
        w * 0.08f,
        h * 0.14f,
        w * 0.84f,
        h * 0.60f
    };

    layout.mascot = {
        w * 0.83f,
        h * 0.75f,
        w * 0.14f,
        h * 0.22f
    };

    layout.endDay = {
        w * 0.08f,
        h * 0.83f,
        w * 0.28f,
        h * 0.09f
    };

    layout.quit = {
        w * 0.35f,
        h * 0.56f,
        w * 0.30f,
        h * 0.08f
    };

    layout.confirmation = {
        w * 0.15f,
        h * 0.18f,
        w * 0.70f,
        h * 0.59f
    };

    layout.cancelButton = {
        w * 0.20f,
        h * 0.65f,
        w * 0.27f,
        h * 0.08f
    };

    layout.confirmButton = {
        w * 0.53f,
        h * 0.65f,
        w * 0.27f,
        h * 0.08f
    };

    const float rowHeight =
        h * 0.27f /
        static_cast<float>(
            std::max(std::size_t{1}, systemCount)
        );

    for (std::size_t i = 0; i < systemCount; ++i) {
        layout.systems.push_back({
            w * 0.11f,
            h * 0.34f + static_cast<float>(i) * rowHeight,
            w * 0.78f,
            rowHeight * 0.84f
        });
    }

    return layout;
}

void MissionPanel::DrawButton(Rectangle bounds, const char *label, float scale, bool interactive) const {
    const bool hovered =
        interactive &&CheckCollisionPointRec(InputManager::MousePosition(),bounds);

    const auto image = hovered ? MissionArt::Image::FrameActive : MissionArt::Image::FrameIdle;

    MissionArt::Draw(_art.Get(image), bounds);

    float fontSize = std::max(10.0f, 20.0f * scale);

    while (
        fontSize > 10.0f &&
        FontManager::Measure(label, fontSize).x >
            bounds.width * 0.88f
    ) {
        fontSize -= 1.0f;
    }

    const Vector2 size = FontManager::Measure(label, fontSize);

    FontManager::Draw(
        label,
        bounds.x + (bounds.width - size.x) * 0.5f,
        bounds.y + (bounds.height - size.y) * 0.5f,
        fontSize,
        RAYWHITE
    );
}

bool RestorationProgress::Ready() const {
    return total > 0 && repaired == total;
}

MissionPanel::MissionPanel() {
    _art.Load();
}

MissionPanel::~MissionPanel() = default;

MissionPanel::Command MissionPanel::HitTest(const Mission& mission,bool exitVisible,bool confirmationOpen,Vector2 mouse) const {
    const auto layout = GetLayout(
       mission.Rules().systems.size()
   );

    if (
        confirmationOpen &&
        mission.CurrentPhase() == Mission::Phase::Operations
    ) {
        if (CheckCollisionPointRec(mouse, layout.cancelButton)) {
            return {Action::CancelEndDay};
        }

        if (CheckCollisionPointRec(mouse, layout.confirmButton)) {
            return {Action::ConfirmEndDay};
        }

        return {};
    }

    if (mission.IsFinished()) {
        if (
            exitVisible &&
            CheckCollisionPointRec(mouse, layout.quit)
        ) {
            return {Action::Quit};
        }

        return {};
    }

    if (CheckCollisionPointRec(mouse, layout.mascot)) {
        return {Action::Talk};
    }

    if (mission.CurrentPhase() == Mission::Phase::Operations) {
        if (CheckCollisionPointRec(mouse, layout.endDay)) {
            return {Action::RequestEndDay};
        }

        for (std::size_t i = 0; i < layout.systems.size(); ++i) {
            if (CheckCollisionPointRec(mouse, layout.systems[i])) {
                return {Action::ToggleSystem, i};
            }
        }
    }

    return {};
}

bool MissionPanel::BlocksPointer(const Mission& mission,Vector2 mouse) const {
    if (mission.CurrentPhase() != Mission::Phase::Restoration) {
        return true;
    }

    const auto layout = GetLayout(mission.Rules().systems.size());

    return CheckCollisionPointRec(mouse, layout.mascot);
}

void MissionPanel::Draw(const Mission& mission,bool dialogueActive,bool exitVisible,bool confirmationOpen) const {
     using Image = MissionArt::Image;

    const auto layout = GetLayout(
        mission.Rules().systems.size()
    );

    const auto w = static_cast<float>(GetScreenWidth());
    const auto h = static_cast<float>(GetScreenHeight());
    const float s = layout.scale;

    const auto& rules = mission.Rules();
    const auto& supplies = mission.Supplies();

    if (mission.CurrentPhase() == Mission::Phase::Restoration) {
        if (!dialogueActive) {
            GuideSprite::Draw(layout.mascot, false);
        }

        return;
    }

    MissionArt::Draw(
        _art.Get(
            mission.IsFinished() ? Image::Result : Image::Panel
        ),
        layout.content
    );

    if (mission.IsFinished()) {
        const auto outcome = mission.Result();

        const float textX = w * 0.13f;
        const float textWidth = w * 0.45f;

        Text(
            OutcomeTitle(outcome),
            textX, h * 0.24f, textWidth,
            s, OutcomeColor(outcome), 30
        );

        Text(
            OutcomeDescription(outcome),
            textX, h * 0.33f, textWidth,
            s
        );

        Text(
            "Water: " + std::to_string(supplies.water) +
            "    Food: " + std::to_string(supplies.food),
            textX, h * 0.41f, textWidth,
            s
        );

        Text(
            "Crew condition: " +
            std::to_string(supplies.crew) + "/" +
            std::to_string(rules.supplies.crew),
            textX, h * 0.47f, textWidth,
            s, OutcomeColor(outcome), 23
        );

        if (exitVisible && !dialogueActive) {
            DrawButton(layout.quit, "QUIT GAME", s);
        }

        return;
    }

    Text(
        "DAY " + std::to_string(mission.Day()) +
        " / " + std::to_string(rules.finalDay) +
        (mission.Day() == rules.stormDay
            ? "  -  SOLAR STORM"
            : "  -  PREPARE SUPPLIES"),
        w * 0.11f, h * 0.17f, w * 0.78f,
        s, ink, 25
    );

    Text(
        "WATER: " + std::to_string(supplies.water) +
        "    FOOD: " + std::to_string(supplies.food) +
        "    CREW CONDITION: " +
        std::to_string(supplies.crew) + "/" +
        std::to_string(rules.supplies.crew),
        w * 0.11f, h * 0.23f, w * 0.78f,
        s, ink, 23
    );

    Text(
        "POWER: " + std::to_string(mission.UsedPower()) +
        "/" + std::to_string(mission.AvailablePower()) +
        " used. Switch a system OFF to free power.",
        w * 0.11f, h * 0.285f, w * 0.78f,
        s
    );

    const bool interactive =
        !dialogueActive && !confirmationOpen;

    for (std::size_t i = 0; i < layout.systems.size(); ++i) {
        const auto row = layout.systems[i];
        const auto& system = rules.systems[i];

        const bool powered = mission.IsPowered(i);

        const bool hovered =
            interactive &&
            CheckCollisionPointRec(
                InputManager::MousePosition(),
                row
            );

        MissionArt::Draw(
            _art.Get(
                powered ? Image::FrameActive : Image::FrameIdle
            ),
            row
        );

        if (hovered) {
            DrawRectangleRoundedLinesEx(
                row,
                0.25f,
                8,
                std::max(1.0f, 2.0f * s),
                RAYWHITE
            );
        }

        const float rowScale = std::min(
            s,
            row.height / 48.0f
        );

        Text(
            system.name,
            row.x + w * 0.012f,
            row.y + row.height * 0.12f,
            w * 0.50f,
            rowScale,
            RAYWHITE,
            19
        );

        Text(
            system.effect,
            row.x + w * 0.012f,
            row.y + row.height * 0.57f,
            w * 0.50f,
            rowScale,
            RAYWHITE,
            16
        );

        Text(
            std::to_string(system.powerCost) + " POWER | " +
            (powered ? "ON" : "OFF"),
            w * 0.68f,
            row.y + row.height * 0.30f,
            w * 0.19f,
            rowScale,
            RAYWHITE,
            18
        );
    }

    if (const auto preview = mission.PreviewDat()) {
        Text(
            "AFTER THIS DAY: water " +
            std::to_string(preview->after.water) +
            "    food " + std::to_string(preview->after.food) +
            "    condition " +
            std::to_string(preview->after.crew) + "/" +
            std::to_string(rules.supplies.crew),
            w * 0.11f, h * 0.64f, w * 0.78f,
            s,
            preview->after.crew < supplies.crew
                ? warningInk
                : ink
        );
    }

    Text(
        "Production first. Then crew uses " +
        std::to_string(rules.dailyWater) + " water and " +
        std::to_string(rules.dailyFood) + " food.",
        w * 0.11f, h * 0.69f, w * 0.78f,
        s, ink, 16
    );

    if (interactive) {
        DrawButton(layout.endDay, "END DAY", s);
        GuideSprite::Draw(layout.mascot, false);
    }

    if (confirmationOpen && !dialogueActive) {
        DrawConfirmation(mission, layout);
    }
}

void MissionPanel::DrawConfirmation(
    const Mission& mission,
    const Layout& layout
) const {
    const auto report = mission.PreviewDat();

    if (!report) {
        return;
    }

    const auto w = static_cast<float>(GetScreenWidth());
    const auto h = static_cast<float>(GetScreenHeight());
    const float s = layout.scale;

    DrawRectangle(
        0,
        0,
        GetScreenWidth(),
        GetScreenHeight(),
        Color{0, 0, 0, 175}
    );

    MissionArt::Draw(
        _art.Get(MissionArt::Image::Confirmation),
        layout.confirmation
    );

    Text(
        "FINISH DAY " + std::to_string(report->day) + "?",
        w * 0.19f, h * 0.22f, w * 0.62f,
        s, ink, 27
    );

    Text(
        "Production: +" +
        std::to_string(report->waterProduced) +
        " water, +" +
        std::to_string(report->foodProduced) + " food.",
        w * 0.19f, h * 0.28f, w * 0.62f,
        s
    );

    Text(
        "After: water " +
        std::to_string(report->after.water) +
        ", food " + std::to_string(report->after.food) +
        ", crew condition " +
        std::to_string(report->after.crew) + "/" +
        std::to_string(mission.Rules().supplies.crew) + ".",
        w * 0.19f, h * 0.335f, w * 0.62f,
        s
    );

    float warningY = h * 0.395f;

    const auto warning = [&](const std::string& message) {
        Text(
            message,
            w * 0.19f, warningY, w * 0.62f,
            s, warningInk, 18
        );

        warningY += h * 0.045f;
    };

    if (report->waterShortage) {
        warning(
            "Water shortage. Condition penalty: " +
            std::to_string(report->waterDamage) + "."
        );
    }

    if (report->foodShortage) {
        warning(
            "Food shortage. Condition penalty: " +
            std::to_string(report->foodDamage) + "."
        );
    }

    if (report->storm && !report->protectedFromStorm) {
        warning(
            "Shelter OFF. Storm condition penalty: " +
            std::to_string(report->stormDamage) + "."
        );
    }

    if (report->outcome == Mission::Outcome::InProgress) {
        Text(
            "The mission continues. Condition damage carries over.",
            w * 0.19f, h * 0.545f, w * 0.62f,
            s, ink, 18
        );
    }
    else {
        Text(
            std::string("EXPECTED RESULT: ") +
            OutcomeTitle(report->outcome),
            w * 0.19f, h * 0.545f, w * 0.62f,
            s, OutcomeColor(report->outcome), 20
        );
    }

    Text(
        "Go back to change your plan, or confirm these consequences.",
        w * 0.19f, h * 0.60f, w * 0.62f,
        s, ink, 16
    );

    DrawButton(layout.cancelButton, "GO BACK", s);
    DrawButton(layout.confirmButton, "FINISH DAY", s);
}