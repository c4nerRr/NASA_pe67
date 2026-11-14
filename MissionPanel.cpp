//
// Created by c4ner on 11/14/2026.
//
#include "MissionPanel.h"

#include <algorithm>
#include <string>

#include "FontManager.h"

namespace {
    constexpr Color panelColor{15, 24, 42, 245};
    constexpr Color borderColor{118, 151, 189, 255};

    void Panel(Rectangle bounds) {
        DrawRectangleRec(bounds, panelColor);
        DrawRectangleLinesEx(bounds, 2.0f, borderColor);
    }

    void Text(
        const std::string& text,
        float x,
        float y,
        float width,
        float scale,
        Color color = RAYWHITE,
        int referenceSize = 20
    ) {
        int size = std::max(10,static_cast<int>(referenceSize * scale));

        while (size > 10 &&FontManager::Measure(text.c_str(), static_cast<float>(size)).x > width)
        {
            --size;
        }

        FontManager::Draw(text.c_str(),x,y,static_cast<float>(size),color);
    }

    void Button(Rectangle bounds,const char* label,float scale,bool interactive = true) {
        const bool hovered = interactive && CheckCollisionPointRec(GetMousePosition(), bounds);

        DrawRectangleRec(
            bounds,
            hovered
                ? Color{73, 108, 150, 255}
                : Color{39, 62, 92, 255}
        );

        DrawRectangleLinesEx(
            bounds,
            2.0f,
            hovered ? GOLD : SKYBLUE
        );

        const float fontSize = std::max(10.0f, 20.0f * scale);
        const Vector2 size = FontManager::Measure(label, fontSize);

        FontManager::Draw(
            label,
            bounds.x + (bounds.width - size.x) * 0.5f,
            bounds.y + (bounds.height - fontSize) * 0.5f,
            fontSize,
            RAYWHITE
        );
    }
}

MissionPanel::Layout MissionPanel::GetLayout(
    std::size_t systemCount
) const {
    const auto w = static_cast<float>(GetScreenWidth());
    const auto h = static_cast<float>(GetScreenHeight());

    Layout layout;
    layout.scale = std::min(w / 1280.0f, h / 720.0f);

    layout.content = {
        w * 0.08f, h * 0.14f,
        w * 0.84f, h * 0.60f
    };

    layout.mascot = {
        w * 0.84f, h * 0.81f,
        w * 0.10f, h * 0.13f
    };

    layout.endDay = {
        w * 0.08f, h * 0.83f,
        w * 0.28f, h * 0.09f
    };

    layout.quit = {
        w * 0.35f, h * 0.56f,
        w * 0.30f, h * 0.08f
    };

    layout.confirmation = {
        w * 0.15f, h * 0.18f,
        w * 0.70f, h * 0.59f
    };

    layout.cancelButton = {
        w * 0.20f, h * 0.65f,
        w * 0.27f, h * 0.08f
    };

    layout.confirmButton = {
        w * 0.53f, h * 0.65f,
        w * 0.27f, h * 0.08f
    };

    const float rowHeight =
        h * 0.27f /
        static_cast<float>(std::max(std::size_t{1}, systemCount));

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

MissionPanel::Command MissionPanel::HitTest(const Mission& mission,bool exitVisible,bool confirmationOpen,Vector2 mouse) const {
    const auto layout = GetLayout(mission.Rules().systems.size());

    if (confirmationOpen && mission.CurrentPhase() == Mission::Phase::Operations
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
        if (exitVisible && CheckCollisionPointRec(mouse, layout.quit)) {
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

bool MissionPanel::BlocksPointer(
    const Mission& mission,
    Vector2 mouse
) const {
    if (mission.CurrentPhase() != Mission::Phase::Restoration) {
        return true;
    }

    const auto layout = GetLayout(mission.Rules().systems.size());

    return CheckCollisionPointRec(mouse, layout.mascot);
}

void MissionPanel::Draw(
    const Mission& mission,
    bool dialogueActive,
    bool exitVisible,
    bool confirmationOpen
) const {
    const auto layout = GetLayout(mission.Rules().systems.size());

    const auto w = static_cast<float>(GetScreenWidth());
    const auto h = static_cast<float>(GetScreenHeight());
    const float s = layout.scale;

    const auto& rules = mission.Rules();
    const auto& supplies = mission.Supplies();

    if (mission.CurrentPhase() == Mission::Phase::Restoration) {
        if (!dialogueActive) {
            Button(layout.mascot, "GUIDE", s);
        }

        return;
    }

    Panel(layout.content);

    if (mission.IsFinished()) {
        const bool won =
            mission.CurrentPhase() == Mission::Phase::Won;

        Text(won ? "MISSION COMPLETE" : "MISSION FAILED", w * 0.13f, h * 0.25f, w * 0.74f,s, won ? LIME : ORANGE, 30);

        Text( won ? "The supply ship has arrived. Your crew survived!" : "The crew could not hold out until the ship arrived.", w * 0.13f, h * 0.34f, w * 0.74f,s);

        Text("Water: " + std::to_string(supplies.water) + "    Food: " + std::to_string(supplies.food) + "    Crew: " + std::to_string(supplies.crew) +"/" + std::to_string(rules.supplies.crew),w * 0.13f, h * 0.43f, w * 0.74f,s, SKYBLUE);

        if (exitVisible && !dialogueActive) {
            Button(layout.quit, "QUIT GAME", s);
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
        s, GOLD, 25
    );

    Text(
        "WATER: " + std::to_string(supplies.water) +
        "    FOOD: " + std::to_string(supplies.food) +
        "    CREW: " + std::to_string(supplies.crew) +
        "/" + std::to_string(rules.supplies.crew),
        w * 0.11f, h * 0.23f, w * 0.78f,
        s, RAYWHITE, 23
    );

    Text(
        "POWER: " + std::to_string(mission.UsedPower()) +
        "/" + std::to_string(mission.AvailablePower()) +
        " used. Switch a system OFF to free power.",
        w * 0.11f, h * 0.285f, w * 0.78f,
        s, GOLD
    );

    const bool interactive = !dialogueActive && !confirmationOpen;

    for (std::size_t i = 0; i < layout.systems.size(); ++i) {
        const auto row = layout.systems[i];
        const auto& system = rules.systems[i];

        const bool powered = mission.IsPowered(i);

        const bool hovered =
            interactive &&
            CheckCollisionPointRec(GetMousePosition(), row);

        DrawRectangleRec(
            row,
            powered
                ? Color{37, 79, 68, 255}
                : Color{31, 43, 63, 255}
        );

        DrawRectangleLinesEx(
            row,
            2.0f,
            hovered ? GOLD : powered ? LIME : borderColor
        );

        const float rowScale = std::min(s, row.height / 48.0f);

        Text(
            system.name,
            row.x + w * 0.012f,
            row.y + row.height * 0.12f,
            w * 0.50f,
            rowScale,
            powered ? LIME : RAYWHITE,
            19
        );

        Text(
            system.effect,
            row.x + w * 0.012f,
            row.y + row.height * 0.57f,
            w * 0.50f,
            rowScale,
            LIGHTGRAY,
            16
        );

        Text(
            std::to_string(system.powerCost) + " POWER | " +
            (powered ? "ON" : "OFF"),
            w * 0.68f,
            row.y + row.height * 0.30f,
            w * 0.19f,
            rowScale,
            powered ? LIME : LIGHTGRAY,
            18
        );
    }

    if (const auto preview = mission.PreviewDat()) {
        Text(
            "AFTER THIS DAY: water " +
            std::to_string(preview->after.water) +
            "    food " + std::to_string(preview->after.food) +
            "    crew " + std::to_string(preview->after.crew),
            w * 0.11f, h * 0.64f, w * 0.78f,
            s,
            preview->after.crew < supplies.crew ? ORANGE : SKYBLUE
        );
    }

    Text(
        "Production first. Then crew uses " +
        std::to_string(rules.dailyWater) + " water and " +
        std::to_string(rules.dailyFood) + " food.",
        w * 0.11f, h * 0.69f, w * 0.78f,
        s, LIGHTGRAY, 16
    );

    if (!dialogueActive && !confirmationOpen) {
        Button(layout.endDay, "END DAY", s);
        Button(layout.mascot, "GUIDE", s);
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
        0, 0,
        GetScreenWidth(), GetScreenHeight(),
        Color{0, 0, 0, 175}
    );

    Panel(layout.confirmation);

    Text(
        "FINISH DAY " + std::to_string(report->day) + "?",
        w * 0.19f, h * 0.22f, w * 0.62f,
        s, GOLD, 27
    );

    Text(
        "Production: +" + std::to_string(report->waterProduced) +
        " water, +" + std::to_string(report->foodProduced) + " food.",
        w * 0.19f, h * 0.30f, w * 0.62f,
        s
    );

    Text(
        "After the day: water " + std::to_string(report->after.water) +
        ", food " + std::to_string(report->after.food) +
        ", crew " + std::to_string(report->after.crew) + ".",
        w * 0.19f, h * 0.36f, w * 0.62f,
        s, SKYBLUE
    );

    float warningY = h * 0.43f;

    if (report->waterShortage) {
        Text(
            "WARNING: Not enough water.",
            w * 0.19f, warningY, w * 0.62f,
            s, ORANGE, 18
        );

        warningY += h * 0.05f;
    }

    if (report->foodShortage) {
        Text(
            "WARNING: Not enough food.",
            w * 0.19f, warningY, w * 0.62f,
            s, ORANGE, 18
        );

        warningY += h * 0.05f;
    }

    if (report->storm && !report->protectedFromStorm) {
        Text(
            "WARNING: The shelter is OFF. Storm damage will apply.",
            w * 0.19f, warningY, w * 0.62f,
            s, ORANGE, 18
        );
    }

    Text(
        "Go back to change your plan, or finish with these consequences.",
        w * 0.19f, h * 0.59f, w * 0.62f,
        s, LIGHTGRAY, 16
    );

    Button(layout.cancelButton, "GO BACK", s);
    Button(layout.confirmButton, "FINISH DAY", s);
}
