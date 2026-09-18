#pragma once

#include <memory>

class Model
{
public:
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&& other) noexcept;
    Model& operator=(Model&& other) noexcept;

    bool IsValid() const;

private:
    class Impl;

    // EN: Creates a Model and its hidden backend implementation.
    //     Backend-specific details remain inside Model.cpp.
    //
    // JP: Model と非公開の Backend Implementation を生成する。
    //     Backend 固有の詳細は Model.cpp 内部に隠蔽する。
    static std::unique_ptr<Model> Load(
        const char* filePath);

    explicit Model(std::unique_ptr<Impl> impl);

    std::unique_ptr<Impl> m_impl;

    friend class ModelLoader;
};