#pragma once

#include <memory>

class Model;

class ModelLoader
{
public:
    // EN: Loads a model resource synchronously from the specified path.
    //     Returns nullptr when the backend resource cannot be created.
    //
    // JP: 指定された Path から Model Resource を同期的に読み込む。
    //     Backend Resource の生成に失敗した場合は nullptr を返す。
    static std::unique_ptr<Model> Load(
        const char* filePath);
};