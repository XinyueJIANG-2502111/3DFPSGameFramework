#pragma once

#include <memory>
#include <utility>

// forward declaration
class IModelResource;
class ModelResourceAccess;

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

    static std::unique_ptr<Model> Create(
        std::unique_ptr<IModelResource> resource);

    explicit Model(std::unique_ptr<Impl> impl);

private:
    std::unique_ptr<Impl> m_impl;

    friend class ModelLoader;
    friend class ModelResourceAccess;
};