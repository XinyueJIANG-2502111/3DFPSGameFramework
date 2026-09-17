#include "Platform/DxLib/DxDebugText.h"

#include <DxLib.h>

void DxDebugText::Draw(
    int x,
    int y,
    const char* text)
{
    // EN: DxLib-specific drawing stays inside the platform implementation.
    //     Upper layers only express the intent to display diagnostic text.
    //
    // JP: DxLib 固有の描画処理を Platform 実装内部に閉じ込める。
    //     上位レイヤーは診断文字を表示するという目的だけを指定する。
    const unsigned int color = GetColor(255, 255, 255);

    DrawString(x, y, text, color);
}