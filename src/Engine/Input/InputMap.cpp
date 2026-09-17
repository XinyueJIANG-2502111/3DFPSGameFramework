#include "Engine/Input/InputMap.h"

#include "Engine/Input/IInput.h"

InputMap::InputMap(IInput& input)
    : m_input(input)
{
    // EN: Every gameplay action starts unbound.
    //     Bindings are configured explicitly by the game layer.
    //
    // JP: すべての Gameplay Action は未割り当て状態から開始する。
    //     Binding は Game レイヤー側で明示的に設定する。
    m_bindings.fill(KeyCode::None);
}

void InputMap::Bind(
    InputAction action,
    KeyCode key)
{
    m_bindings[ToIndex(action)] = key;
}

bool InputMap::IsDown(InputAction action) const
{
    const KeyCode key =
        m_bindings[ToIndex(action)];

    if (key == KeyCode::None)
    {
        return false;
    }

    return m_input.IsKeyDown(key);
}

bool InputMap::IsPressed(InputAction action) const
{
    const KeyCode key =
        m_bindings[ToIndex(action)];

    if (key == KeyCode::None)
    {
        return false;
    }

    return m_input.IsKeyPressed(key);
}

bool InputMap::IsReleased(InputAction action) const
{
    const KeyCode key =
        m_bindings[ToIndex(action)];

    if (key == KeyCode::None)
    {
        return false;
    }

    return m_input.IsKeyReleased(key);
}