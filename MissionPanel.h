//
// Created by c4ner on 11/14/2026.
//
#pragma once

#include <cstddef>
#include <vector>

#include "Mission.h"
#include "MissionArt.h"
#include "raylib.h"

struct RestorationProgress {
    std::size_t repaired = 0;
    std::size_t total = 0;

    [[nodiscard]] bool Ready() const;
};

class MissionPanel {
public:
    MissionPanel();
    ~MissionPanel();

    enum class Action {
        None,
        Talk,
        ToggleSystem,
        RequestEndDay,
        ConfirmEndDay,
        CancelEndDay,
        Quit
    };

    struct Command {
        Action action = Action::None;
        std::size_t system = 0;
    };

    [[nodiscard]] Command HitTest(const Mission& mission,bool exitVisible,bool confirmationOpen,Vector2 mouse) const;

    [[nodiscard]] bool BlocksPointer(const Mission& mission,Vector2 mouse) const;

    void Draw(const Mission& mission,bool dialogueActive,bool exitVisible,bool confirmationOpen) const;

private:
    struct Layout {
        Rectangle content{};
        Rectangle mascot{};
        Rectangle endDay{};
        Rectangle quit{};

        Rectangle confirmation{};
        Rectangle confirmButton{};
        Rectangle cancelButton{};

        std::vector<Rectangle> systems;

        float scale = 1.0f;
    };

    MissionArt _art;

    [[nodiscard]] Layout GetLayout(std::size_t systemCount) const;

    void DrawButton(Rectangle bounds,const char* label,float scale,bool interactive = true) const;

    void DrawConfirmation(const Mission& mission,const Layout& layout) const;
};