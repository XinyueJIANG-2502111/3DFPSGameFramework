#pragma once

class DxModel
{
public:
    // EN: Constructs an empty model that owns no DxLib resource.
    //
    // JP: DxLib の Resource を所有しない空の Model を構築する。
    DxModel() = default;

    // EN: Loads a model from the specified file and takes unique
    //     ownership of the resulting DxLib model handle.
    //
    // JP: 指定されたファイルから Model を読み込み、
    //     生成された DxLib Model Handle の所有権を一意に保持する。
    explicit DxModel(const char* filePath);

    // EN: Releases the owned DxLib model resource, if one exists.
    //
    // JP: 所有している DxLib Model Resource が存在する場合、
    //     その Resource を解放する。
    ~DxModel();

    // EN: A native DxLib model handle has unique ownership.
    //     Copying would cause multiple objects to believe they own
    //     the same handle, so copying is forbidden.
    //
    // JP: DxLib Model Handle の所有権は一意である。
    //     Copy を許可すると複数 Object が同一 Handle を所有している
    //     状態になるため、Copy は禁止する。
    DxModel(const DxModel&) = delete;
    DxModel& operator=(const DxModel&) = delete;

    // EN: Ownership may be transferred safely through move operations.
    //
    // JP: Move 操作によって Handle の所有権を安全に移動できる。
    DxModel(DxModel&& other) noexcept;
    DxModel& operator=(DxModel&& other) noexcept;

    // EN: Returns whether this object currently owns a valid
    //     DxLib model handle.
    //
    // JP: 現在この Object が有効な DxLib Model Handle を
    //     所有しているかを返す。
    bool IsValid() const;

    // EN: Exposes the native handle only to code that must interact
    //     with the DxLib backend. Ownership is not transferred.
    //
    // JP: DxLib Backend と連携する必要がある処理にのみ
    //     Native Handle を公開する。所有権は移動しない。
    int GetHandle() const;

private:
    static constexpr int InvalidHandle = -1;

    int m_handle = InvalidHandle;
};