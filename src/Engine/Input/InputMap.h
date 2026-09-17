#pragma once

#include "Engine/Input/InputAction.h"
#include "Engine/Input/InputTypes.h"

#include <array>
#include <cstddef>

class IInput;

class InputMap
{
public:
    explicit InputMap(IInput& input);

    // EN: Associates a gameplay action with a physical keyboard key.
    //
    // JP: Gameplay の Action と物理キーボードのキーを関連付ける。
    void Bind(
        InputAction action,
        KeyCode key);

    // EN: Returns whether the physical input bound to the action
    //     is currently held down.
    //
    // JP: Action に割り当てられた物理入力が
    //     現在押されているかを返す。
    bool IsDown(InputAction action) const;

    // EN: Returns true only on the frame in which the action
    //     changes from released to pressed.
    //
    // JP: Action が離された状態から押された状態へ変化した
    //     フレームだけ true を返す。
    bool IsPressed(InputAction action) const;

    // EN: Returns true only on the frame in which the action
    //     changes from pressed to released.
    //
    // JP: Action が押された状態から離された状態へ変化した
    //     フレームだけ true を返す。
    bool IsReleased(InputAction action) const;

private:
    static constexpr std::size_t ActionCount =
        static_cast<std::size_t>(InputAction::Count);

    static constexpr std::size_t ToIndex(InputAction action)
    {
        return static_cast<std::size_t>(action);
    }

    IInput& m_input;

    std::array<KeyCode, ActionCount> m_bindings{};
};