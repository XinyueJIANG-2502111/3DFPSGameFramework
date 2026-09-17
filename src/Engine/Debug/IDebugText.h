#pragma once

class IDebugText
{
public:
    virtual ~IDebugText() = default;

    // EN: Draw diagnostic text in screen space without exposing the
    //     platform-specific rendering API to framework or gameplay code.
    //
    // JP: Framework や Gameplay にプラットフォーム固有の描画 API を
    //     公開せず、画面座標上に診断用テキストを描画する。
    virtual void Draw(
        int x,
        int y,
        const char* text) = 0;
};