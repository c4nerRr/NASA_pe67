//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_GAME4_H
#define NASA_GAME4_H
#include "IGame.h"
#include "IState.h"
#include <array>
#include "raylib.h"


class Game4 : public IGame, public IState {

    enum class Tool {None, Water, Light };

    struct Bed {
        bool _watered = false, _light = false;
    };

    static constexpr int _bedCount = 3;

    struct Layout {
        Rectangle _waterButton{};
        Rectangle _lightButton{};
        std::array<Rectangle, _bedCount> _beds{};
    };

    void BuildLayout();
    void ToggleTool(Tool tool);
    void ApplyTool(Bed& bed);

    Tool _selectedTool = Tool::None;
    std::array<Bed, _bedCount> _beds{};
    Layout _layout{};

    bool IsHereOnce = false;
    enum class Phase {
        Theory,
        Rules,
        Playing
    };

    Phase _phase = Phase::Theory;

    void ShowTheory();//учєбний текст
    void ShowRules(); //діалог з правилами
    void StartGame();
    void UpdateGame(float dt);
    void DrawGarden() const;

    public:
    Game4();
    ~Game4() override;

    void OnEnter() override;
    void Update(float dt) override;
    void Draw() const override;
};


#endif //NASA_GAME4_H