//#include "Engine/Rendering/Model.h"
//#include "Engine/Rendering/ModelLoader.h"
//
//#include <cassert>
//#include <memory>
//
//void RunModelTests()
//{
//    constexpr const char* testModelPath =
//        "Assets/Models/Bicycle.mv1";
//
//    // Test 1: Successful loading
//    {
//        std::unique_ptr<Model> model =
//            ModelLoader::Load(testModelPath);
//
//        assert(model != nullptr);
//        assert(model->IsValid());
//    }
//
//    // Test 2: Failed loading
//    {
//        std::unique_ptr<Model> model =
//            ModelLoader::Load(
//                "assets/models/"
//                "this_file_does_not_exist.mv1");
//
//        assert(model == nullptr);
//    }
//
//    // Test 3: Ownership through unique_ptr
//    {
//        std::unique_ptr<Model> first =
//            ModelLoader::Load(testModelPath);
//
//        assert(first != nullptr);
//
//        std::unique_ptr<Model> second =
//            std::move(first);
//
//        assert(first == nullptr);
//        assert(second != nullptr);
//        assert(second->IsValid());
//    }
//}