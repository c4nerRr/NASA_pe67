//
// Created by c4ner on 11/14/2026.
//

#ifndef NASA_MAINMENUCONFIG_H
#define NASA_MAINMENUCONFIG_H
#include <string>
#include <vector>

enum class MenuAction {
    Play, Settings, Quit
};

struct MenuButtonConfig {
    std::string normal;
    std::string hover;
    Vector2 center;
    float width;
    MenuAction action;
};

struct MainMenuConfig {
    std::string folder = "assets/menu";
    std::string background = "main_menu.png";

    std::vector<MenuButtonConfig> buttons{
            {
                "start.png", "start_hover.png",
                {0.22f, 0.43f}, 0.17f,
                MenuAction::Play
            },
            {
                "settings.png", "settings_hover.png",
                {0.22f, 0.57f}, 0.17f,
                MenuAction::Settings
            },
            {
                "quit-1.png", "quit.png",
                {0.22f, 0.69f}, 0.17f,
                MenuAction::Quit
            }
    };
};

#endif //NASA_MAINMENUCONFIG_H