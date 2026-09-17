#pragma once

enum class KeyCode
{
    None,

    W,
    A,
    S,
    D,

    Space,
    LeftShift,
    LeftCtrl,

    Escape,

    R,
    E,

    Count
};

enum class MouseButton
{
    Left,
    Right,
    Middle,

    Count
};

struct MouseDelta
{
    float x = 0.0f;
    float y = 0.0f;
};