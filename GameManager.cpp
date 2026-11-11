//
// Created by c4ner on 11/11/2026.
//

#include "GameManager.h"

GameManager::GameManager() = default;

GameManager::~GameManager() = default;

GameManager & GameManager::Instanse() {
    static GameManager instance;
    return instance;
}

void GameManager::ChangeState(GameState newState) {
    _currentState = newState;
}

void GameManager::Update(float dt) {
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
}

void GameManager::ClearStates() {
    _states.clear();
}
