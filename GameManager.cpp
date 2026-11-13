//
// Created by c4ner on 11/11/2026.
//

#include "GameManager.h"

#include "IGame.h"

GameManager::GameManager() = default;

GameManager::~GameManager() = default;

GameManager & GameManager::Instanse() {
    static GameManager instance;
    return instance;
}

void GameManager::ChangeState(GameState newState) {
    if (newState == _currentState) {
        return;
    }

    const auto it = _states.find(newState);

    if (it == _states.end()) {
        TraceLog(LOG_WARNING, "Cannot enter an unregistered state");
        return;
    }
    _ui.CloseDialogue();
    _currentState = newState;
    it->second->OnEnter();
}

void GameManager::Update(float dt) {
    if (_ui.IsDialogueActive()) {
        _ui.Update(dt);
        return;
    }

    const auto it = _states.find(_currentState);

    if (it != _states.end()) {
        it->second->Update(dt);
    }
}

GameState GameManager::GetCurrentState() const {
    return _currentState;
}

void GameManager::RequestQuit() {
    _quitRequested = true;
}

bool GameManager::ShouldQuit() const {
    return _quitRequested;
}

void GameManager::RegisterState(GameState id, std::unique_ptr<IState> state) {
    _states[id] = std::move(state);
}

void GameManager::Draw() const {
    const auto it = _states.find(_currentState);

    if (it != _states.end()) {
        it->second->Draw();
    }
    _ui.Draw();
}

void GameManager::ClearStates() {
    _ui.CloseDialogue();
    _states.clear();
}

bool GameManager::CanEnterState(GameState state) const {
    const auto it = _states.find(state);

    if (it == _states.end()) {
        return false;
    }

    const auto* game = dynamic_cast<const IGame*>(it->second.get());

    return game != nullptr && !game->GetFinished();
}

bool GameManager::ShowDialogue(const std::vector<std::string> &messages) {
    return _ui.ShowDialogue(messages);
}

bool GameManager::IsDialogueActive() const {
    return _ui.IsDialogueActive();
}
