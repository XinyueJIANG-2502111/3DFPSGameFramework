//#include "Engine/Resources/ResourceSystem.h"
//
//#include "Engine/Rendering/Model.h"
//
//#include <cassert>
//#include <memory>
//
//void RunResourceSystemTests()
//{
//    constexpr const char* testModelPath =
//        "Assets/Models/Bicycle.mv1";
//
//    ResourceSystem resources;
//
//    // ============================================================
//    // Test 1: First load
//    // ============================================================
//    {
//        std::shared_ptr<Model> model =
//            resources.LoadModel(testModelPath);
//
//        assert(model != nullptr);
//        assert(model->IsValid());
//    }
//
//    resources.RemoveExpired();
//
//
//    // ============================================================
//    // Test 2: Same path shares the same resource
//    // ============================================================
//    {
//        std::shared_ptr<Model> first =
//            resources.LoadModel(testModelPath);
//
//        std::shared_ptr<Model> second =
//            resources.LoadModel(testModelPath);
//
//        assert(first != nullptr);
//        assert(second != nullptr);
//
//        assert(first.get() == second.get());
//    }
//
//    resources.RemoveExpired();
//
//
//    // ============================================================
//    // Test 3: One user releasing does not destroy shared resource
//    // ============================================================
//    {
//        std::shared_ptr<Model> first =
//            resources.LoadModel(testModelPath);
//
//        std::shared_ptr<Model> second =
//            resources.LoadModel(testModelPath);
//
//        Model* originalAddress =
//            first.get();
//
//        first.reset();
//
//        assert(first == nullptr);
//        assert(second != nullptr);
//        assert(second.get() == originalAddress);
//        assert(second->IsValid());
//    }
//
//    resources.RemoveExpired();
//
//
//    // ============================================================
//    // Test 4: Expired resource is loaded again
//    // ============================================================
//    {
//        std::shared_ptr<Model> first =
//            resources.LoadModel(testModelPath);
//
//        assert(first != nullptr);
//
//        first.reset();
//
//        // At this point the cache only contains a weak_ptr.
//        // Therefore the Model should already have been destroyed.
//
//        std::shared_ptr<Model> second =
//            resources.LoadModel(testModelPath);
//
//        assert(second != nullptr);
//        assert(second->IsValid());
//    }
//
//    resources.RemoveExpired();
//
//
//    // ============================================================
//    // Test 5: Failed load
//    // ============================================================
//    {
//        std::shared_ptr<Model> model =
//            resources.LoadModel(
//                "this_file_does_not_exist.mv1");
//
//        assert(model == nullptr);
//    }
//
//    // ============================================================
//    // Test 6: Equivalent paths share the same resource
//    // ============================================================
//    {
//        std::shared_ptr<Model> first =
//            resources.LoadModel(
//                "Assets/Models/Bicycle.mv1");
//
//        std::shared_ptr<Model> second =
//            resources.LoadModel(
//                "Assets/Models/Bicycle.mv1");
//
//        assert(first != nullptr);
//        assert(second != nullptr);
//
//        assert(first.get() == second.get());
//    }
//}