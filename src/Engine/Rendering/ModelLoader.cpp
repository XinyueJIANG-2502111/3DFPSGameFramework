#include "Engine/Rendering/ModelLoader.h"

#include "Engine/Rendering/Model.h"

std::unique_ptr<Model> ModelLoader::Load(
    const char* filePath)
{
    return Model::Load(filePath);
}