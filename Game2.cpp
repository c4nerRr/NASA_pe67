#include "Game2.h"
#include "GameManager.h"
#include "InputManager.h"
#include <algorithm>

namespace
{
    constexpr int titleSize = 36;
    constexpr const char* title = "LIFE SUPPORT RESTORATION";
}

Game2::Game2() = default;
Game2::~Game2() = default;

void Game2::OnEnter()
{
    UpdateLayout();

    if (!isHereOnce)
    {
        _phase = Phase::Theory;
        ShowTheory();
    }
    else
    {
        _phase = Phase::Rules;
        ShowRules();
    }
}

void Game2::UpdateLayout()
{
    float screenWidth = static_cast<float>(GetScreenWidth());
    float screenHeight = static_cast<float>(GetScreenHeight());

    float maxGridSize = std::min(screenWidth, screenHeight) * 0.65f;
    _cellSize = maxGridSize / gridSize;

    _startX = (screenWidth - (_cellSize * gridSize)) * 0.5f;
    _startY = (screenHeight - (_cellSize * gridSize)) * 0.5f;
}

void Game2::ShowTheory()
{
    GameManager::Instanse().ShowDialogue({
           "Water on board of a space craft is often maintained in different states...",
           "...as the water is processed and purified such as wastewater, cooling water, and drinking water supply.",
           "The bladders could be used as holding tanks for the liquid when not circulated or during the processing phase.",
           "Plumbing from and to the bladders is needed to circulate the fluids during normal operation cycles."
        });
}

void Game2::ShowRules()
{
    GameManager::Instanse().ShowDialogue({
           "HOW TO PLAY:",
           "Your goal is to connect the Blue Source to the Green Reservoir.",
           "Crucial: The water MUST pass through the Yellow Filter to be purified!",
           "Click on any pipe or the filter to rotate it by 90 degrees.",
           "The Source and Reservoir are fixed. Ensure all pipe openings match up perfectly."
        });
}

void Game2::ShowWin()
{
    GameManager::Instanse().ShowDialogue({
        "Fantastic work, explorer!",
        "The life support plumbing is fully restored and water purification is operating at 100% capacity.",
        "Returning you back to the Hub..."
        });
}

void Game2::Update(float dt)
{
    UpdateLayout();

    if (InputManager::PressSPACE())
    {
        GameManager::Instanse().ChangeState(GameState::Hub);
        return;
    }

    switch (_phase)
    {
    case Phase::Theory:
    {
        if (!GameManager::Instanse().IsDialogueActive())
        {
            isHereOnce = true;
            _phase = Phase::Rules;
            ShowRules();
        }
        break;
    }
    case Phase::Rules:
    {
        if (!GameManager::Instanse().IsDialogueActive())
        {
            _phase = Phase::Playing;
            GenerateLevel();
        }
        break;
    }
    case Phase::Playing:
    {
        if (InputManager::PressLeftMouse())
        {
            Vector2 mousePos = InputManager::MousePosition();
            Rectangle gridRect = { _startX, _startY, _cellSize * gridSize, _cellSize * gridSize };

            if (CheckCollisionPointRec(mousePos, gridRect))
            {
                int gridX = static_cast<int>((mousePos.x - _startX) / _cellSize);
                int gridY = static_cast<int>((mousePos.y - _startY) / _cellSize);

                if (gridX >= 0 && gridX < gridSize && gridY >= 0 && gridY < gridSize)
                {
                    Pipe& clickedPipe = grid[gridY][gridX];

                    if (clickedPipe.type == PipeType::Straight ||
                        clickedPipe.type == PipeType::Corner ||
                        clickedPipe.type == PipeType::Filter)
                    {
                        clickedPipe.rotation = (clickedPipe.rotation + 1) % 4;
                        UpdateFlow();

                        bool isWon = false;
                        for (int y = 0; y < gridSize; y++)
                        {
                            for (int x = 0; x < gridSize; x++)
                            {
                                if (grid[y][x].type == PipeType::Reservoir && grid[y][x].filtered)
                                {
                                    isWon = true;
                                }
                            }
                        }

                        if (isWon)
                        {
                            _phase = Phase::Win;
                            IsFinished = true;
                           ShowWin();
                        }
                    }
                }
            }
        }
        break;
    }
    case Phase::Win:
    {
        if (!GameManager::Instanse().IsDialogueActive())
        {
            GameManager::Instanse().ChangeState(GameState::Hub);
            return;
        }
        break;
    }
    }
}

void Game2::Draw() const
{
    const Rectangle frame = {
        GetScreenWidth() * 0.05f, GetScreenHeight() * 0.05f,
        GetScreenWidth() * 0.90f, GetScreenHeight() * 0.90f
    };

    DrawRectangleRec(frame, DARKBLUE);
    DrawRectangleLinesEx(frame, 4.0f, RAYWHITE);

    int textX = static_cast<int>(_startX);
    int textY = static_cast<int>(_startY) - 80;

    if (textY < static_cast<int>(frame.y) + 10)
    {
        textY = static_cast<int>(frame.y) + 10;
    }

    DrawText(title, textX, textY, titleSize, RAYWHITE);
    DrawText("SPACE - back to Hub", textX, textY + 40, 24, RAYWHITE);

    if (_phase == Phase::Playing || _phase == Phase::Win)
    {
        DrawGrid();
    }
}

void Game2::DrawGrid() const
{
    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            float cellX = _startX + x * _cellSize;
            float cellY = _startY + y * _cellSize;
            DrawRectangleLines(cellX, cellY, _cellSize, _cellSize, GRAY);

            const Pipe& p = grid[y][x];
            if (p.type == PipeType::Empty) continue;

            Color waterColor = GRAY;
            if (p.filtered) waterColor = SKYBLUE;
            else if (p.connected) waterColor = BLUE;

            Vector2 center = { cellX + _cellSize / 2.0f, cellY + _cellSize / 2.0f };

            if (p.type == PipeType::Source)
            {
                DrawCircleV(center, _cellSize * 0.15f, BLUE);
                DrawText("SRC", center.x - 12, center.y - 8, 10, WHITE);
            }
            else if (p.type == PipeType::Reservoir)
            {
                DrawCircleV(center, _cellSize * 0.15f, GREEN);
                DrawText("RES", center.x - 12, center.y - 8, 10, WHITE);
            }
            else if (p.type == PipeType::Filter)
            {
                DrawRectangle(center.x - _cellSize * 0.2f, center.y - _cellSize * 0.2f, _cellSize * 0.4f, _cellSize * 0.4f, YELLOW);
                DrawText("FLT", center.x - 10, center.y - 8, 10, BLACK);
            }

            for (int i = 0; i < 4; i++)
            {
                if (HasConnection(p.type, p.rotation, i))
                {
                    Vector2 end = center;
                    switch (i)
                    {
                    case 0: end.y -= _cellSize / 2.0f; break;
                    case 1: end.x += _cellSize / 2.0f; break;
                    case 2: end.y += _cellSize / 2.0f; break;
                    case 3: end.x -= _cellSize / 2.0f; break;
                    }
                    DrawLineEx(center, end, _cellSize * 0.15f, waterColor);
                }
            }
        }
    }
}

void Game2::UpdateFlow()
{
    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            grid[y][x].connected = false;
            grid[y][x].filtered = false;
        }
    }

    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            if (grid[y][x].type == PipeType::Source)
            {
                CheckConnection(x, y, false);
            }
        }
    }
}

void Game2::CheckConnection(int x, int y, bool isFiltered)
{
    Pipe& current = grid[y][x];

    if (current.connected && (current.filtered || !isFiltered))
    {
        return;
    }

    current.connected = true;
    if (isFiltered) current.filtered = true;

    bool nextFiltered = isFiltered;
    if (current.type == PipeType::Filter)
    {
        nextFiltered = true;
        current.filtered = true;
    }

    int dx[] = { 0, 1, 0, -1 };
    int dy[] = { -1, 0, 1, 0 };

    for (int i = 0; i < 4; i++)
    {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize)
        {
            Pipe& neighbor = grid[ny][nx];

            if (HasConnection(current.type, current.rotation, i) &&
                HasConnection(neighbor.type, neighbor.rotation, (i + 2) % 4))
            {
                CheckConnection(nx, ny, nextFiltered);
            }
        }
    }
}

bool Game2::HasConnection(PipeType type, int rot, int dir) const
{
    if (type == PipeType::Empty) return false;

    int LocalDir = (dir - rot + 4) % 4;

    if (type == PipeType::Straight || type == PipeType::Filter)
    {
        return (LocalDir == 0 || LocalDir == 2);
    }
    if (type == PipeType::Corner)
    {
        return (LocalDir == 0 || LocalDir == 1);
    }
    if (type == PipeType::Source)
    {
        return (LocalDir == 1);
    }
    if (type == PipeType::Reservoir)
    {
        return (LocalDir == 3);
    }
    return false;
}

bool Game2::GeneratePath(std::vector<Vector2>& outPath)
{
    outPath.clear();

    int x = 0;
    int y = GetRandomValue(0, gridSize - 1);
    outPath.push_back(Vector2{ (float)x, (float)y });

    while (true)
    {
        int dx[] = { 0, 1, 0, -1 };
        int dy[] = { -1, 0, 1, 0 };
        std::vector<int> validDirs;

        for (int i = 0; i < 4; i++)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize)
            {
                bool visited = false;
                for (size_t j = 0; j < outPath.size(); j++)
                {
                    if (static_cast<int>(outPath[j].x) == nx && static_cast<int>(outPath[j].y) == ny)
                    {
                        visited = true;
                        break;
                    }
                }
                if (!visited) validDirs.push_back(i);
            }
        }

        if (validDirs.empty()) return false;

        int randIndex = GetRandomValue(0, validDirs.size() - 1);
        int chosenDir = validDirs[randIndex];

        x += dx[chosenDir];
        y += dy[chosenDir];
        outPath.push_back(Vector2{ (float)x, (float)y });

        if (x == gridSize - 1) return true;
    }
}

bool Game2::TryGenerateLevel()
{
    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            grid[y][x].type = PipeType::Empty;
            grid[y][x].rotation = 0;
            grid[y][x].connected = false;
            grid[y][x].filtered = false;
        }
    }

    std::vector<Vector2> path;
    if (!GeneratePath(path) || path.size() < 4) return false;

    int sX = static_cast<int>(path[0].x);
    int sY = static_cast<int>(path[0].y);
    int nextX = static_cast<int>(path[1].x);
    int nextY = static_cast<int>(path[1].y);

    grid[sY][sX].type = PipeType::Source;
    if (nextX > sX) grid[sY][sX].rotation = 0;
    else if (nextX < sX) grid[sY][sX].rotation = 2;
    else if (nextY < sY) grid[sY][sX].rotation = 3;
    else if (nextY > sY) grid[sY][sX].rotation = 1;

    std::vector<size_t> validFilterSpots;
    for (size_t i = 1; i < path.size() - 1; i++)
    {
        Vector2 prev = path[i - 1];
        Vector2 next = path[i + 1];
        if (prev.x == next.x || prev.y == next.y)
        {
            validFilterSpots.push_back(i);
        }
    }

    if (validFilterSpots.empty()) return false;
    size_t filterIndex = validFilterSpots[GetRandomValue(0, validFilterSpots.size() - 1)];

    for (size_t i = 1; i < path.size() - 1; i++)
    {
        int px = static_cast<int>(path[i].x);
        int py = static_cast<int>(path[i].y);
        Vector2 prev = path[i - 1];
        Vector2 next = path[i + 1];

        if (i == filterIndex)
        {
            grid[py][px].type = PipeType::Filter;
        }
        else if (prev.x == next.x || prev.y == next.y)
        {
            grid[py][px].type = PipeType::Straight;
        }
        else
        {
            grid[py][px].type = PipeType::Corner;
        }

        int dirToPrev = (prev.x < px) ? 3 : (prev.x > px) ? 1 : (prev.y < py) ? 0 : 2;
        int dirToNext = (next.x < px) ? 3 : (next.x > px) ? 1 : (next.y < py) ? 0 : 2;

        bool rotFound = false;
        for (int r = 0; r < 4; r++)
        {
            if (HasConnection(grid[py][px].type, r, dirToPrev) && HasConnection(grid[py][px].type, r, dirToNext))
            {
                grid[py][px].rotation = r;
                rotFound = true;
                break;
            }
        }
        if (!rotFound) return false;
    }

    int eX = static_cast<int>(path.back().x);
    int eY = static_cast<int>(path.back().y);
    grid[eY][eX].type = PipeType::Reservoir;

    Vector2 prevToLast = path[path.size() - 2];
    int dirFromLast = (prevToLast.x < eX) ? 3 : (prevToLast.x > eX) ? 1 : (prevToLast.y < eY) ? 0 : 2;

    for (int r = 0; r < 4; r++)
    {
        if (HasConnection(PipeType::Reservoir, r, dirFromLast))
        {
            grid[eY][eX].rotation = r;
            break;
        }
    }

    UpdateFlow();
    return grid[eY][eX].filtered;
}

void Game2::LoadFallbackLevel()
{
    for (auto & y : grid)
        for (int x = 0; x < gridSize; x++)
            y[x] = { PipeType::Empty, 0, false, false };

    grid[1][0] = { PipeType::Source, 0, false, false };
    grid[1][1] = { PipeType::Straight, 0, false, false };
    grid[1][2] = { PipeType::Corner, 1, false, false };
    grid[2][2] = { PipeType::Filter, 1, false, false };
    grid[3][2] = { PipeType::Corner, 3, false, false };
    grid[3][3] = { PipeType::Straight, 0, false, false };
    grid[3][4] = { PipeType::Reservoir, 3, false, false };
}

void Game2::GenerateLevel()
{
    const int MAX_ATTEMPTS = 100;
    bool success = false;

    for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt)
    {
        if (TryGenerateLevel())
        {
            success = true;
            break;
        }
    }

    if (!success)
    {
        LoadFallbackLevel();
    }

    for (auto & y : grid)
    {
        for (int x = 0; x < gridSize; x++)
        {
            if (y[x].type == PipeType::Empty)
            {
                y[x].type = (GetRandomValue(0, 1) == 0) ? PipeType::Straight : PipeType::Corner;
                y[x].rotation = GetRandomValue(0, 3);
            }
        }
    }

    bool isSolved = true;
    while (isSolved)
    {
        for (auto & y : grid)
        {
            for (auto & x : y)
            {
                if (x.type == PipeType::Straight ||
                    x.type == PipeType::Corner ||
                    x.type == PipeType::Filter)
                {
                    x.rotation = GetRandomValue(0, 3);
                }
            }
        }

        UpdateFlow();
        isSolved = false;

        for (auto & y : grid)
        {
            for (int x = 0; x < gridSize; x++)
            {
                if (y[x].type == PipeType::Reservoir && y[x].filtered)
                {
                    isSolved = true;
                }
            }
        }
    }

    UpdateFlow();
}