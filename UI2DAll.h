#ifndef NASA_UI2DALL_H
#define NASA_UI2DALL_H
#include <string>
#include <vector>

#include "raylib.h"

struct DialogueConfig {
    float charactersPerSecond = 45.0f;

    float widthRatio = 0.94f;
    float heightRatio = 0.22f;
    float bottomMarginRatio = 0.03f;

    float referenceHeight = 1080.0f;
    float padding = 24.0f;
    int fontSize = 26;
    int smallFontSize = 20;
    float lineHeightRatio = 1.3f;

    Color dimmer{0, 0, 0, 100};
    Color panel{20, 22, 40, 245};
    Color border{140, 159, 205, 255};
    Color text{245, 245, 245, 255};
    Color button{45, 51, 78, 255};
    Color buttonHover{69, 80, 118, 255};
};

class UI2DAll {
    struct Glyph {
        int codepoint;
        Vector2 position;
    };

    using Page = std::vector<Glyph>;

    DialogueConfig _config;
    Font _font{};

    Rectangle _panel{};
    Rectangle _skipButton{};

    Vector2 _titlePosition{};
    Vector2 _buttonTextPosition{};
    Vector2 _textPosition{};
    Vector2 _hintPosition{};

    int _fontSize = 26;
    int _smallFontSize = 20;
    int _rowsPerPage = 1;

    float _textWidth = 0.0f;
    float _lineHeight = 0.0f;

    std::vector<Page> _pages;
    std::size_t _pageIndex = 0;
    std::size_t _visibleCount = 0;

    float _characterBudget = 0.0f;
    bool _waitForMouseRelease = false;

    void BuildLayout();
    void AddMessage(const std::string& message);

public:
    explicit UI2DAll(DialogueConfig config = {});
    ~UI2DAll();

    bool ShowDialogue(const std::vector<std::string>& messages);
    void CloseDialogue();

    [[nodiscard]] bool IsDialogueActive() const;

    void Update(float dt);
    void Draw() const;
};


#endif //NASA_UI2DALL_H