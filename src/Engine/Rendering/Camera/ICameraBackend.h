#pragma once

class Camera;

class ICameraBackend
{
public:
    virtual ~ICameraBackend() = default;

    // EN: Applies engine camera state to the active rendering backend.
    //     The interface keeps Camera independent from platform-specific
    //     graphics APIs such as DxLib.
    //
    // JP: Engine 側の Camera 状態を現在の描画バックエンドへ適用する。
    //     このインターフェースにより、Camera を DxLib などの
    //     プラットフォーム固有の描画 API から分離する。
    virtual void Apply(const Camera& camera) = 0;
};