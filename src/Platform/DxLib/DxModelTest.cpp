#include "Platform/DxLib/DxModel.h"

#include <cassert>
#include <utility>

void RunDxModelTests()
{
    constexpr const char* testModelPath =
        "Assets/Models/Bicycle.mv1";

    // ============================================================
    // Test 0: Default construction
    // ============================================================
    /*{
        DxModel model;

        assert(!model.IsValid());
        assert(model.GetHandle() == -1);
    }*/


    // ============================================================
    // Test 1: Successful load
    // ============================================================
    /*{
        DxModel model(testModelPath);

        assert(model.IsValid());
        assert(model.GetHandle() != -1);
    }*/


    // ============================================================
    // Test 2: Failed load
    // ============================================================
    /*{
        DxModel model(
            "assets/models/this_file_does_not_exist.mv1");

        assert(!model.IsValid());
        assert(model.GetHandle() == -1);
    }*/


    // ============================================================
    // Test 3: Move constructor
    // ============================================================
    /*{
        DxModel first(testModelPath);

        assert(first.IsValid());

        const int originalHandle =
            first.GetHandle();

        DxModel second(std::move(first));

        assert(!first.IsValid());
        assert(first.GetHandle() == -1);

        assert(second.IsValid());
        assert(second.GetHandle() == originalHandle);
    }*/


    // ============================================================
    // Test 4: Move assignment
    // ============================================================
    /*{
        DxModel first(testModelPath);
        DxModel second(testModelPath);

        assert(first.IsValid());
        assert(second.IsValid());

        const int firstHandle =
            first.GetHandle();

        const int secondHandle =
            second.GetHandle();

        assert(firstHandle != secondHandle);

        second = std::move(first);

        assert(!first.IsValid());
        assert(first.GetHandle() == -1);

        assert(second.IsValid());
        assert(second.GetHandle() == firstHandle);
    }*/


    // ============================================================
    // Test 5: Self move assignment
    // ============================================================
    /*{
        DxModel model(testModelPath);

        assert(model.IsValid());

        const int originalHandle =
            model.GetHandle();

        model = std::move(model);

        assert(model.IsValid());
        assert(model.GetHandle() == originalHandle);
    }*/
}