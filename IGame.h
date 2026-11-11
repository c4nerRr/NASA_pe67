//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_IGAME_H
#define NASA_IGAME_H

class IGame {

    protected:
    bool IsPause = false;

    public:

    IGame();
    virtual ~IGame();

    void Start();
    void Update();
    void End();
};

#endif //NASA_IGAME_H