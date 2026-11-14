
#ifndef NASA_GAMEMANAGER_H
#define NASA_GAMEMANAGER_H
#include <map>
#include <memory>

#include "IState.h"
#include "UI2DAll.h"

enum class GameState {Boot, Menu, Hub, Game1, Game2, Game3, Game4, Pause, Settings};

class GameManager {

    GameManager();
    ~GameManager();

    GameState _currentState = GameState::Boot;
    bool _quitRequested = false;

    std::map<GameState, std::unique_ptr<IState>> _states;
    UI2DAll _ui;

public:
    static GameManager& Instanse();

    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    void ChangeState(GameState newState);
    void Update(float dt);

    [[nodiscard]] GameState GetCurrentState() const;

    void RequestQuit();
    [[nodiscard]] bool ShouldQuit() const;

    void RegisterState(GameState id, std::unique_ptr<IState> state);
    void Draw() const;
    void ClearStates();

    [[nodiscard]] bool CanEnterState(GameState state) const;

    bool ShowDialogue(const std::vector<std::string>& messages);

    [[nodiscard]] bool IsDialogueActive() const;

    void PauseHub();
    void ResumeHub();

    [[nodiscard]] bool IsChallengeFinished(GameState state) const;

};


#endif //NASA_GAMEMANAGER_H