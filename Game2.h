//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_GAME2_H
#define NASA_GAME2_H
#include "IGame.h"
#include "IState.h"


class Game2 : public IGame, public IState {
    public:
    Game2();
    ~Game2() override;

    void Update(float dt) override;
    void Draw() const override;
};


#endif //NASA_GAME2_H