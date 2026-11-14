
#pragma once
#define NASA_GAME2_H
#include "IGame.h"
#include "IState.h"
#include <vector>
#include "raylib.h"

enum class PipeType
{
    Empty,
    Straight,
    Corner,
    Source,
    Filter,
    Reservoir
};

struct Pipe
{
    PipeType type;
    int rotation;
    bool connected;
    bool filtered;
};

class Game2 : public IGame, public IState
{
public:
    Game2();
    ~Game2() override;

    void OnEnter() override;
    void Update(float dt) override;
    void Draw() const override;

private:
    enum class Phase
    {
        Theory,
        Rules,
        Playing,
        Win
    };

    Phase _phase = Phase::Theory;
    bool isHereOnce = false;

    static const int gridSize = 6;
    Pipe grid[gridSize][gridSize];

    float _cellSize = 90.0f;
    float _startX = 0.0f;
    float _startY = 0.0f;
    void UpdateLayout();

    void ShowTheory();
    void ShowRules();
    void ShowWin();
    void DrawGrid() const;

    void UpdateFlow();
    void CheckConnection(int x, int y, bool isFiltered);
    [[nodiscard]] bool HasConnection(PipeType type, int rot, int dir) const;

    void GenerateLevel();
    bool TryGenerateLevel();
    void LoadFallbackLevel();
    bool GeneratePath(std::vector<Vector2>& outPath);
};