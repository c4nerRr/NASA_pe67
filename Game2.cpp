#include "Game2.h"
#include "GameManager.h"
#include "InputManager.h"
#include <algorithm>

Game2::Game2() = default;
Game2::~Game2() = default;

void Game2::OnEnter()
{
    _art.Load();
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
    const auto w = static_cast<float>(GetScreenWidth());
    const auto h = static_cast<float>(GetScreenHeight());

    _uiScale = std::min(w * 0.90f / PipeDesign::boardSize.x, h * 0.88f / PipeDesign::boardSize.y);

    const float width = PipeDesign::boardSize.x * _uiScale;
    const float height = PipeDesign::boardSize.y * _uiScale;

    _boardRect = {(w - width) * 0.5f, (h - height) * 0.5f, width, height};

    _startX = _boardRect.x + PipeDesign::gridArea.x * _uiScale;
    _startY = _boardRect.y + PipeDesign::gridArea.y * _uiScale;
    _cellSize = PipeDesign::gridArea.width * _uiScale / gridSize;
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
        "Fantastic work, rookie!",

        "The life support plumbing is good as new, "
        "and water purification is pumping at a full 100%!",

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
                        for (auto & y : grid)
                        {
                            for (auto & x : y)
                            {
                                if (x.type == PipeType::Reservoir && x.filtered)
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
    PipeArt::Draw(_art.Background(), {0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())});
    PipeArt::Draw(_art.Board(), _boardRect);

    constexpr Color ink{24, 21, 49, 255};

    const auto centered = [&](const char* value, float y, int size) {
        const int font = std::max(10, static_cast<int>(size * _uiScale));
        const float x = _boardRect.x + (_boardRect.width - MeasureText(value, font)) * 0.5f;
        DrawText(value, static_cast<int>(x), static_cast<int>(y), font, ink);
    };

    centered("LIFE SUPPORT RESTORATION", _boardRect.y + 34 * _uiScale, 32);
    centered("CLICK - rotate pipe    SPACE - back to Hub", _boardRect.y + 94 * _uiScale, 20);
    centered("GRAY: DRY    BROWN: UNTREATED    BLUE: CLEAN", _boardRect.y + 960 * _uiScale, 18);

    if (_phase == Phase::Playing || _phase == Phase::Win) {
        DrawGrid();
    }
}

void Game2::DrawGrid() const
{
   const auto mouse = InputManager::MousePosition();
    const bool interactive = _phase == Phase::Playing && !GameManager::Instanse().IsDialogueActive();
    const float line = std::max(1.0f, _uiScale);
    const int font = std::max(10, static_cast<int>(14 * _uiScale));

    for (int y = 0; y < gridSize; ++y) {
        for (int x = 0; x < gridSize; ++x) {
            const auto&[type, rotation, connected, filtered] = grid[y][x];
            const Rectangle cell{_startX + x * _cellSize, _startY + y * _cellSize, _cellSize, _cellSize};

            const bool rotatable =
                type == PipeType::Straight ||
                type == PipeType::Corner ||
                type == PipeType::Filter;

            if (interactive && rotatable && CheckCollisionPointRec(mouse, cell)) {
                DrawRectangleRec(cell, Fade(WHITE, 0.08f));
            }

            DrawRectangleLinesEx(cell, line, Color{62, 53, 79, 255});

            if (type == PipeType::Empty) continue;

            const auto flow = filtered ? PipeArt::Flow::Clean : connected ? PipeArt::Flow::Dirty : PipeArt::Flow::Dry;

            const char* label = nullptr;
            auto marker = WHITE;

            if (type == PipeType::Source) {
                PipeArt::Draw(_art.Source((rotation + 1) % 4), cell);
                label = "SRC";
                marker = SKYBLUE;
            }
            else if (type == PipeType::Reservoir) {
                PipeArt::Draw(_art.Reservoir((rotation + 3) % 4, filtered), cell);
                label = "RES";
                marker = connected && !filtered ? ORANGE : LIME;
            }
            else {
                const bool corner = type == PipeType::Corner;
                const float angle = static_cast<float>((rotation + (corner ? 1 : 0)) % 4) * 90.0f;

                PipeArt::Draw(_art.Tube(corner, flow), cell, angle);

                if (type == PipeType::Filter) {
                    _art.DrawFilter(cell, connected);
                    label = "FLT";
                    marker = GOLD;
                }
            }

            if (label) {
                DrawRectangleLinesEx(cell, std::max(1.0f, 2 * _uiScale), marker);
                DrawText(label, static_cast<int>(cell.x + 5 * _uiScale), static_cast<int>(cell.y + 5 * _uiScale), font, marker);
            }
        }
    }
}

void Game2::UpdateFlow()
{
    for (auto & y : grid)
    {
        for (auto & x : y)
        {
            x.connected = false;
            x.filtered = false;
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

void Game2::CheckConnection(int x, int y, bool isFiltered, int incoming) {
    Pipe& current = grid[y][x];

    if (current.connected && (current.filtered || !isFiltered)) return;

    current.connected = true;
    if (isFiltered) current.filtered = true;

    bool nextFiltered = isFiltered;

    if (current.type == PipeType::Filter) {
        nextFiltered = true;
        current.filtered = true;
    }

    const int dx[]{0, 1, 0, -1};
    const int dy[]{-1, 0, 1, 0};

    for (int i = 0; i < 4; ++i) {
        if (i == incoming) continue;

        const int nx = x + dx[i];
        const int ny = y + dy[i];

        if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize) {
            const Pipe& neighbor = grid[ny][nx];

            if (HasConnection(current.type, current.rotation, i) &&
                HasConnection(neighbor.type, neighbor.rotation, (i + 2) % 4)) {
                CheckConnection(nx, ny, nextFiltered, (i + 2) % 4);
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
                for (auto & j : outPath)
                {
                    if (static_cast<int>(j.x) == nx && static_cast<int>(j.y) == ny)
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
    for (auto & y : grid)
    {
        for (auto & x : y)
        {
            x.type = PipeType::Empty;
            x.rotation = 0;
            x.connected = false;
            x.filtered = false;
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
        for (auto & x : y)
            x = { PipeType::Empty, 0, false, false };

    grid[1][0] = { PipeType::Source, 0, false, false };
    grid[1][1] = { PipeType::Straight, 0, false, false };
    grid[1][2] = { PipeType::Corner, 1, false, false };
    grid[2][2] = { PipeType::Filter, 1, false, false };
    grid[3][2] = { PipeType::Corner, 3, false, false };
    grid[3][3] = { PipeType::Straight, 0, false, false };
    grid[3][4] = { PipeType::Reservoir, 0, false, false };
}

void Game2::GenerateLevel()
{
    constexpr int MAX_ATTEMPTS = 100;
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
        for (auto & x : y)
        {
            if (x.type == PipeType::Empty)
            {
                x.type = (GetRandomValue(0, 1) == 0) ? PipeType::Straight : PipeType::Corner;
                x.rotation = GetRandomValue(0, 3);
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
            for (auto & x : y)
            {
                if (x.type == PipeType::Reservoir && x.filtered)
                {
                    isSolved = true;
                }
            }
        }
    }

    UpdateFlow();
}