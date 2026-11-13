#include "Game2.h"

#include "GameManager.h"
#include "InputManager.h"

namespace
{
    constexpr int titleSize = 36;
    constexpr const char* title = "LIFE SUPPORT RESTORATION";

    constexpr float cellSize = 90.0f;
}

Game2::Game2() = default;

Game2::~Game2() = default;

void Game2::OnEnter()
{
    _phase = Phase::Theory;
    if (!isHereOnce)
    {
        ShowTheory();
    }
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
           "HOW TO PLAY: Both the bladder and the PED could be plumbed to the habitat water distribution system.",
           "System items can be stored in color coded CPCs.",
           "Click on the pipes to fix the plumbing from and to the bladders needed to circulate the fluids.",
           "Connect the route so water is processed and purified from wastewater into the drinking water supply!"
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
    float screenWidth = static_cast<float>(GetScreenWidth());
    float screenHeight = static_cast<float>(GetScreenHeight());

    float startX = (screenWidth - (cellSize * 6.0f)) * 0.5f;
    float startY = (screenHeight - (cellSize * 6.0f)) * 0.5f;

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
                Vector2 mousePos = GetMousePosition();
                int gridX = (mousePos.x - startX) / cellSize;
                int gridY = (mousePos.y - startY) / cellSize;

                if (gridX >= 0 && gridX < gridSize && gridY >= 0 && gridY < gridSize)
                {
                    Pipe& clickedPipe = grid[gridY][gridX];
                    if (clickedPipe.type == PipeType::Straight || clickedPipe.type == PipeType::Corner || clickedPipe.type == PipeType::Filter)
                    {
                        clickedPipe.rotation = (clickedPipe.rotation + 1) % 4;
                        UpdateFlow();
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
    const Rectangle frame
    {
        GetScreenWidth() * 0.15f,
        GetScreenHeight() * 0.15f,
        GetScreenWidth() * 0.70f,
        GetScreenHeight() * 0.70f
    };

    DrawRectangleRec(frame, DARKBLUE);
    DrawRectangleLinesEx(frame, 4.0f, RAYWHITE);

    const int x = static_cast<int>(frame.x) + 32;
    const int y = static_cast<int>(frame.y) + 32;

    DrawText("GAME 2", x, y, 40, RAYWHITE);
    DrawText("SPACE - back to Hub", x, y + 64, 24, RAYWHITE);

    DrawGrid();
}

void Game2::DrawGrid() const
{
    float screenWidth = static_cast<float>(GetScreenWidth());
    float screenHeight = static_cast<float>(GetScreenHeight());

    float startX = (screenWidth - (cellSize * 6.0f)) * 0.5f;
    float startY = (screenHeight - (cellSize * 6.0f)) * 0.5f;

    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            float cellX = startX + x * cellSize;
            float cellY = startY + y * cellSize;
            DrawRectangleLines(cellX, cellY, cellSize, cellSize, GRAY);

            const Pipe& p = grid[y][x];

            if (p.type == PipeType::Empty)
            {
                continue;
            }

            Color waterColor;

            if (p.connected)
            {
                waterColor = BLUE;
            }
            else
            {
                waterColor = GRAY;
            }

            Vector2 center = { cellSize / 2 + cellX, cellSize / 2 + cellY };

            if (p.type == PipeType::Source)
            {
                DrawCircleV(center, 15, waterColor);
            }
            else if (p.type == PipeType::Reservoir)
            {
                DrawCircleV(center, 15, GREEN);
            }
            else if (p.type == PipeType::Filter)
            {
                DrawRectangle(center.x - 20, center.y - 20, 40, 40, YELLOW);
            }

            for (int i = 0; i < 4; i++)
            {
                if (HasConnection(p.type, p.rotation, i))
                {
                    Vector2 end = center;

                    switch (i)
                    {
                        case 0:
                        {
                            end.y -= cellSize / 2.0f;
                            break;
                        }
                        case 1:
                        {
                            end.x += cellSize / 2.0f;
                            break;
                        }
                        case 2:
                        {
                            end.y += cellSize / 2.0f;
                            break;
                        }
                        case 3:
                        {
                            end.x -= cellSize / 2.0f;
                            break;
                        }
                    }
                    DrawLineEx(center, end, 12.0f, waterColor);
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
        }
    }
    
    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            if (grid[y][x].type == PipeType::Source)
            {
                CheckConnection(x, y);
            }
        }
    }

    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            if (grid[y][x].type == PipeType::Reservoir && grid[y][x].connected == true)
            {
                if (_phase != Phase::Win)
                {
                    _phase = Phase::Win;
                    IsFinished = true;
                    ShowWin();
                }
            }
        }
    }
}

bool Game2::HasConnection(PipeType type, int rot, int dir) const
{
    if (type == PipeType::Empty)
    {
        return false;
    }

    int LocalDir = (dir - rot + 4) % 4;

    if (type == PipeType::Straight || type == PipeType::Filter)
    {
        if (LocalDir == 0 || LocalDir == 2)
        {
            return true;
        }
    }
    if (type == PipeType::Corner)
    {
        if (LocalDir == 0 || LocalDir == 1)
        {
            return true;
        }
    }
    if (type == PipeType::Source)
    {
        if (LocalDir == 1)
        {
            return true;
        }
    }
    if (type == PipeType::Reservoir)
    {
        if (LocalDir == 3)
        {
            return true;
        }
    }
    return false;
}

void Game2::CheckConnection(int x, int y)
{
    if (grid[y][x].connected == true)
    {
        return;
    }

    grid[y][x].connected = true;
    Pipe& current = grid[y][x];

    int dx[] = { 0, 1, 0, -1 };
    int dy[] = { -1, 0, 1, 0 };

    for (int i = 0; i < 4; i++)
    {
        int nx = x + dx[i]; 
        int ny = y + dy[i];

        if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize)
        {
            Pipe& neighbor = grid[ny][nx];

            if (HasConnection(current.type, current.rotation, i) && HasConnection(neighbor.type, neighbor.rotation, (i + 2) % 4))
            {
                CheckConnection(nx, ny);
            }
        }
    }
}

bool Game2::GeneratePath(std::vector<Vector2>& outPath)
{
    outPath.clear();

    int x = 0;
    int y = GetRandomValue(0, 2);

    outPath.push_back(Vector2{ (float)x, (float)y });

    while (true)
    {
        int dx[] = { 0, 1, 0, -1 };
        int dy[] = { -1, 0, 1, 0 };

        Vector2 steps;

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

                if (visited == false)
                {
                    validDirs.push_back(i);
                }
            }
        }

        if (validDirs.empty())
        {
            return false;
        }

        int randIndex = GetRandomValue(0, validDirs.size() - 1);
        int chosenDir = validDirs[randIndex];

        x += dx[chosenDir];
        y += dy[chosenDir];

        outPath.push_back(Vector2{ (float)x, (float)y });

        if (x == gridSize - 1)
        {
            return true;
        }
    }
}

void Game2::GenerateLevel()
{
    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            grid[y][x].type = PipeType::Empty;
            grid[y][x].rotation = 0;
            grid[y][x].connected = false;
        }
    }

    std::vector<Vector2> path;
    while (!GeneratePath(path))
    {
    }

    int startXCoord = static_cast<int>(path[0].x);
    int startYCoord = static_cast<int>(path[0].y);
    grid[startYCoord][startXCoord].type = PipeType::Source;
    grid[startYCoord][startXCoord].rotation = 0;

    for (size_t i = 1; i < path.size() - 1; i++)
    {
        int px = static_cast<int>(path[i].x);
        int py = static_cast<int>(path[i].y);

        Vector2 prev = path[i - 1];
        Vector2 next = path[i + 1];

        if (prev.x == next.x || prev.y == next.y)
        {
            grid[py][px].type = PipeType::Straight;
        }
        else
        {
            grid[py][px].type = PipeType::Corner;
        }

        int dirToPrev = -1;
        int dirToNext = -1;

        if (prev.x < px)
        {
            dirToPrev = 3;
        }
        else if (prev.x > px)
        {
            dirToPrev = 1;
        }
        else if (prev.y < py)
        {
            dirToPrev = 0;
        }
        else if (prev.y > py)
        {
            dirToPrev = 2;
        }

        if (next.x < px)
        {
            dirToNext = 3;
        }
        else if (next.x > px)
        {
            dirToNext = 1;
        }
        else if (next.y < py)
        {
            dirToNext = 0;
        }
        else if (next.y > py)
        {
            dirToNext = 2;
        }

        for (int r = 0; r < 4; r++)
        {
            if (HasConnection(grid[py][px].type, r, dirToPrev) && HasConnection(grid[py][px].type, r, dirToNext))
            {
                grid[py][px].rotation = r;
                break;
            }
        }
    }

    int endXCoord = static_cast<int>(path.back().x);
    int endYCoord = static_cast<int>(path.back().y);
    grid[endYCoord][endXCoord].type = PipeType::Reservoir;

    Vector2 secondLast = path[path.size() - 2];
    int dirFromLastPipe = -1;
    if (secondLast.x < endXCoord) dirFromLastPipe = 3; 
    else if (secondLast.x > endXCoord) dirFromLastPipe = 1; 
    else if (secondLast.y < endYCoord) dirFromLastPipe = 0; 
    else if (secondLast.y > endYCoord) dirFromLastPipe = 2; 

    for (int r = 0; r < 4; r++)
    {
        if (HasConnection(PipeType::Reservoir, r, dirFromLastPipe))
        {
            grid[endYCoord][endXCoord].rotation = r;
            break;
        }
    }

    if (path.size() > 4)
    {
        size_t midIndex = path.size() / 2;
        int fx = static_cast<int>(path[midIndex].x);
        int fy = static_cast<int>(path[midIndex].y);

        Vector2 fPrev = path[midIndex - 1];
        Vector2 fNext = path[midIndex + 1];

        grid[fy][fx].type = PipeType::Filter;

        int fDir1 = -1, fDir2 = -1;
        if (fPrev.x < fx) fDir1 = 3; else if (fPrev.x > fx) fDir1 = 1; else if (fPrev.y < fy) fDir1 = 0; else if (fPrev.y > fy) fDir1 = 2;
        if (fNext.x < fx) fDir2 = 3; else if (fNext.x > fx) fDir2 = 1; else if (fNext.y < fy) fDir2 = 0; else if (fNext.y > fy) fDir2 = 2;

        for (int r = 0; r < 4; r++)
        {
            if (HasConnection(PipeType::Filter, r, fDir1) && HasConnection(PipeType::Filter, r, fDir2))
            {
                grid[fy][fx].rotation = r;
                break;
            }
        }
    }

    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            if (grid[y][x].type == PipeType::Empty)
            {
                if (GetRandomValue(0, 1) == 0)
                {
                    grid[y][x].type = PipeType::Straight;
                }
                else
                {
                    grid[y][x].type = PipeType::Corner;
                }
                grid[y][x].rotation = GetRandomValue(0, 3);
            }
        }
    }

    for (size_t i = 1; i < path.size() - 1; i++)
    {
        int px = static_cast<int>(path[i].x);
        int py = static_cast<int>(path[i].y);
        grid[py][px].rotation = GetRandomValue(0, 3);
    }

    if (path.size() > 4)
    {
        size_t midIndex = path.size() / 2;
        int fx = static_cast<int>(path[midIndex].x);
        int fy = static_cast<int>(path[midIndex].y);
        grid[fy][fx].rotation = GetRandomValue(0, 3);
    }

    UpdateFlow();
}