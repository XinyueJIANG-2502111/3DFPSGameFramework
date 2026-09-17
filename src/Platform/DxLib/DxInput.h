#pragma once

#include "Engine/Input/IInput.h"

class DxInput final : public IInput
{
public:
    void Update() override;

    bool IsKeyDown(KeyCode key) const override;
    bool IsKeyPressed(KeyCode key) const override;
    bool IsKeyReleased(KeyCode key) const override;

    bool IsMouseButtonDown(MouseButton button) const override;
    bool IsMouseButtonPressed(MouseButton button) const override;
    bool IsMouseButtonReleased(MouseButton button) const override;

    MouseDelta GetMouseDelta() const override;

private:
    static constexpr int KeyCount =
        static_cast<int>(KeyCode::Count);

    static constexpr int MouseButtonCount =
        static_cast<int>(MouseButton::Count);

    bool m_currentKeys[KeyCount]{};
    bool m_previousKeys[KeyCount]{};

    bool m_currentMouseButtons[MouseButtonCount]{};
    bool m_previousMouseButtons[MouseButtonCount]{};

    MouseDelta m_mouseDelta{};
    bool m_hasMousePosition = false;

    int m_previousMouseX = 0;
    int m_previousMouseY = 0;
};