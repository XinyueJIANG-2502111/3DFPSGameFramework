#include "Platform/DxLib/DxInput.h"

#include <DxLib.h>

namespace
{
    int ToDxKeyCode(KeyCode key)
    {
        switch (key)
        {
        case KeyCode::W:
            return KEY_INPUT_W;

        case KeyCode::A:
            return KEY_INPUT_A;

        case KeyCode::S:
            return KEY_INPUT_S;

        case KeyCode::D:
            return KEY_INPUT_D;

        case KeyCode::Space:
            return KEY_INPUT_SPACE;

        case KeyCode::LeftShift:
            return KEY_INPUT_LSHIFT;

        case KeyCode::LeftCtrl:
            return KEY_INPUT_LCONTROL;

        case KeyCode::Escape:
            return KEY_INPUT_ESCAPE;

        case KeyCode::R:
            return KEY_INPUT_R;

        case KeyCode::E:
            return KEY_INPUT_E;

        default:
            return -1;
        }
    }

    int ToDxMouseButton(MouseButton button)
    {
        switch (button)
        {
        case MouseButton::Left:
            return MOUSE_INPUT_LEFT;

        case MouseButton::Right:
            return MOUSE_INPUT_RIGHT;

        case MouseButton::Middle:
            return MOUSE_INPUT_MIDDLE;

        default:
            return 0;
        }
    }
}

void DxInput::Update()
{
    for (int i = 0; i < KeyCount; ++i)
    {
        m_previousKeys[i] = m_currentKeys[i];

        const KeyCode key =
            static_cast<KeyCode>(i);

        const int dxKey = ToDxKeyCode(key);

        if (dxKey >= 0)
        {
            m_currentKeys[i] =
                CheckHitKey(dxKey) != 0;
        }
        else
        {
            m_currentKeys[i] = false;
        }
    }

    for (int i = 0; i < MouseButtonCount; ++i)
    {
        m_previousMouseButtons[i] =
            m_currentMouseButtons[i];

        const MouseButton button =
            static_cast<MouseButton>(i);

        const int dxButton =
            ToDxMouseButton(button);

        m_currentMouseButtons[i] =
            (GetMouseInput() & dxButton) != 0;
    }

    int mouseX = 0;
    int mouseY = 0;

    GetMousePoint(&mouseX, &mouseY);

    if (!m_hasMousePosition)
    {
        // EN: The first sample establishes the reference position only.
        //     Reporting movement here would create a fake delta from
        //     the default member values to the actual cursor position.
        //
        // JP: 初回サンプルでは基準となる位置だけを記録する。
        //     ここで移動量を計算すると、メンバーの初期値から実際の
        //     カーソル位置までを偽の移動量として扱ってしまう。
        m_previousMouseX = mouseX;
        m_previousMouseY = mouseY;

        m_mouseDelta = {};

        m_hasMousePosition = true;
        //return;
    }
    else
    {
        m_mouseDelta.x =
            static_cast<float>(mouseX - m_previousMouseX);

        m_mouseDelta.y =
            static_cast<float>(mouseY - m_previousMouseY);

        m_previousMouseX = mouseX;
        m_previousMouseY = mouseY;
    }

    //m_mouseDelta.x =
    //    static_cast<float>(mouseX - m_previousMouseX);

    //m_mouseDelta.y =
    //    static_cast<float>(mouseY - m_previousMouseY);

    //// EN: Store this frame's position as the reference for the next frame.
    ////
    //// JP: 次フレームの移動量を計算するため、
    ////     今フレームの位置を新しい基準位置として保存する。
    //m_previousMouseX = mouseX;
    //m_previousMouseY = mouseY;
}

// keyboard
bool DxInput::IsKeyDown(KeyCode key) const
{
    const int index =
        static_cast<int>(key);

    if (index < 0 || index >= KeyCount)
    {
        return false;
    }

    return m_currentKeys[index];
}

bool DxInput::IsKeyPressed(KeyCode key) const
{
    const int index =
        static_cast<int>(key);

    if (index < 0 || index >= KeyCount)
    {
        return false;
    }

    return m_currentKeys[index]
        && !m_previousKeys[index];
}

bool DxInput::IsKeyReleased(KeyCode key) const
{
    const int index =
        static_cast<int>(key);

    if (index < 0 || index >= KeyCount)
    {
        return false;
    }

    return !m_currentKeys[index]
        && m_previousKeys[index];
}

// mouse
bool DxInput::IsMouseButtonDown(MouseButton button) const
{
    const int index =
        static_cast<int>(button);

    if (index < 0 || index >= MouseButtonCount)
    {
        return false;
    }

    return m_currentMouseButtons[index];
}

bool DxInput::IsMouseButtonPressed(MouseButton button) const
{
    const int index =
        static_cast<int>(button);

    if (index < 0 || index >= MouseButtonCount)
    {
        return false;
    }

    return m_currentMouseButtons[index]
        && !m_previousMouseButtons[index];
}

bool DxInput::IsMouseButtonReleased(MouseButton button) const
{
    const int index =
        static_cast<int>(button);

    if (index < 0 || index >= MouseButtonCount)
    {
        return false;
    }

    return !m_currentMouseButtons[index]
        && m_previousMouseButtons[index];
}

MouseDelta DxInput::GetMouseDelta() const
{
    return m_mouseDelta;
}