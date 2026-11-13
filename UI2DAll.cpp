//
// Created by c4ner on 11/11/2026.
//

#include "UI2DAll.h"

#include <sstream>

#include "InputManager.h"

namespace {
    constexpr const char* skipLabel = "SKIP ALL";

    std::vector<std::string> WrapText(const std::string& text,float maxWidth,int fontSize) {
        std::vector<std::string> rows;
        std::istringstream words(text);

        std::string word;
        std::string line;

        while (words >> word) {
            const std::string candidate = line.empty() ? word : line + " " + word;

            if (MeasureText(candidate.c_str(), fontSize) <= maxWidth) {
                line = candidate;
                continue;
            }

            if (!line.empty()) {
                rows.push_back(std::move(line));
                line.clear();
            }

            for (char character : word) {
                line.push_back(character);

                if (line.size() > 1 && MeasureText(line.c_str(), fontSize) > maxWidth) { //NOLINT

                    line.pop_back();
                    rows.push_back(std::move(line));
                    line.assign(1, character);
                }
            }
        }

        if (!line.empty()) {
            rows.push_back(std::move(line));
        }

        return rows;
    }
}

void UI2DAll::BuildLayout() {
    _font = GetFontDefault();

    const auto width = static_cast<float>(GetScreenWidth());
    const auto height = static_cast<float>(GetScreenHeight());

    const float scale = height / _config.referenceHeight;
    const float padding = _config.padding * scale;

    _fontSize = std::max(10,static_cast<int>(_config.fontSize * scale)); //NOLINT

    _smallFontSize = std::max(10,static_cast<int>(_config.smallFontSize * scale)); //NOLINT

    const float panelWidth = width * _config.widthRatio;
    const float panelHeight = height * _config.heightRatio;

    _panel = {(width - panelWidth) * 0.5f,height - height * _config.bottomMarginRatio - panelHeight,panelWidth,panelHeight};
    const float buttonWidth =MeasureText(skipLabel, _smallFontSize) + padding * 1.5f; //NOLINT

    const float buttonHeight = _smallFontSize + padding; //NOLINT

    _skipButton = {_panel.x + _panel.width - padding - buttonWidth,_panel.y + padding * 0.5f,buttonWidth,buttonHeight};

    _buttonTextPosition = {_skipButton.x +(_skipButton.width -MeasureText(skipLabel, _smallFontSize)) * 0.5f,_skipButton.y +(_skipButton.height - _smallFontSize) * 0.5f}; //NOLINT

    _titlePosition = {_panel.x + padding,_buttonTextPosition.y};

    _textPosition = {_panel.x + padding,_skipButton.y + _skipButton.height + padding * 0.5f};

    _hintPosition = {_panel.x + padding,_panel.y + _panel.height - padding * 0.5f - _smallFontSize}; //NOLINT

    _textWidth = _panel.width - padding * 2.0f;
    _lineHeight = _fontSize * _config.lineHeightRatio; //NOLINT

    const float textHeight =_hintPosition.y - padding * 0.5f - _textPosition.y;

    _rowsPerPage = std::max(1,1 + static_cast<int>((textHeight - _fontSize) / _lineHeight)); //NOLINT
}

void UI2DAll::AddMessage(const std::string &message) {
    const auto rows = WrapText(message, _textWidth, _fontSize);

    const auto rowsPerPage = static_cast<std::size_t>(_rowsPerPage);

    const float fontScale =static_cast<float>(_fontSize) / _font.baseSize; //NOLINT

    const auto spacing = static_cast<float>(_fontSize / 10); //NOLINT
    for (std::size_t first = 0; first < rows.size();
         first += rowsPerPage) {
        Page page;

        const std::size_t last =std::min(first + rowsPerPage, rows.size());

        for (std::size_t row = first; row < last; ++row) {
            float x = _textPosition.x;

            const float y =_textPosition.y +static_cast<float>(row - first) * _lineHeight;

            for (unsigned char character : rows[row]) {
                page.push_back({static_cast<int>(character),{x, y}});

                const int index = GetGlyphIndex(_font, character);

                const float advance =_font.glyphs[index].advanceX != 0? static_cast<float>(_font.glyphs[index].advanceX): _font.recs[index].width;

                x += advance * fontScale + spacing;
            }
        }

        if (!page.empty()) {
            _pages.push_back(std::move(page));
        }
         }
}

UI2DAll::UI2DAll(DialogueConfig config) : _config(config) {
}


UI2DAll::~UI2DAll() = default;

bool UI2DAll::ShowDialogue(const std::vector<std::string> &messages) {
    if (IsDialogueActive()) {
        return false;
    }
    BuildLayout();
    for (const std::string& message : messages) {
        AddMessage(message);
    }

    if (_pages.empty()) {
        return false;
    }

    _pageIndex = 0;
    _visibleCount = 0;
    _characterBudget = 0.0f;
    _waitForMouseRelease = InputManager::IsLeftMouseDown();

    return true;
}

void UI2DAll::CloseDialogue() {
    _pages.clear();

    _pageIndex = 0;
    _visibleCount = 0;
    _characterBudget = 0.0f;
    _waitForMouseRelease = false;
}

bool UI2DAll::IsDialogueActive() const {
    return !_pages.empty();
}

void UI2DAll::Update(float dt) {
    if (!IsDialogueActive()) {
        return;
    }

    const Page& page = _pages[_pageIndex];

    if (_waitForMouseRelease) {
        _waitForMouseRelease = InputManager::IsLeftMouseDown();
    }
    else if (InputManager::PressLeftMouse()) {
        if (CheckCollisionPointRec(InputManager::MousePosition(), _skipButton)) {
            CloseDialogue();
            return;
            }

        if (_visibleCount < page.size()) {
            _visibleCount = page.size();
            _characterBudget = 0.0f;
            return;
        }

        ++_pageIndex;

        if (_pageIndex >= _pages.size()) {
            CloseDialogue();
            return;
        }

        _visibleCount = 0;
        _characterBudget = 0.0f;
        return;
    }

    if (_visibleCount >= page.size()) {
        return;
    }

    _characterBudget +=std::max(dt, 0.0f) *std::max(_config.charactersPerSecond, 1.0f);
    const std::size_t count = std::min(page.size() - _visibleCount,static_cast<std::size_t>(_characterBudget));

    _visibleCount += count;
    _characterBudget -= static_cast<float>(count);
}

void UI2DAll::Draw() const {
    if (!IsDialogueActive()) {
        return;
    }

    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),_config.dimmer);

    DrawRectangleRec(_panel, _config.panel);
    DrawRectangleLinesEx(_panel, 2.0f, _config.border);

    DrawText("MISSION CONTROL",static_cast<int>(_titlePosition.x),static_cast<int>(_titlePosition.y),_smallFontSize,_config.border);

    const bool hovered = CheckCollisionPointRec(InputManager::MousePosition(), _skipButton);

    DrawRectangleRec(_skipButton,hovered ? _config.buttonHover : _config.button);

    DrawText(skipLabel,static_cast<int>(_buttonTextPosition.x),static_cast<int>(_buttonTextPosition.y),_smallFontSize,_config.text);

    const Page& page = _pages[_pageIndex];

    for (std::size_t i = 0; i < _visibleCount; ++i) {
        const Glyph& glyph = page[i];

        if (glyph.codepoint == ' ') {
            continue;
        }

        DrawTextCodepoint(_font,glyph.codepoint,glyph.position,static_cast<float>(_fontSize),_config.text);
    }

    const char* hint = "LMB - show full text";

    if (_visibleCount == page.size()) {
        hint = _pageIndex + 1 < _pages.size() ? "LMB - next": "LMB - close";
    }

    DrawText(hint,static_cast<int>(_hintPosition.x),static_cast<int>(_hintPosition.y),_smallFontSize,_config.border);
}

