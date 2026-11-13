//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_GAME4_H
#define NASA_GAME4_H
#include "IGame.h"
#include "IState.h"


class Game4 : public IGame, public IState {

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