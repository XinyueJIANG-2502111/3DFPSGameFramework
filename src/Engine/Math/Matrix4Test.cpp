#include "Engine/Math/Matrix4.h"

#include "Engine/Math/Transform.h"

#include <cassert>
#include <cmath>

namespace
{
    constexpr float Epsilon = 0.0001f;

    bool NearlyEqual(float a, float b)
    {
        return std::fabs(a - b) < Epsilon;
    }

    bool NearlyEqual(
        const Vector3& a,
        const Vector3& b)
    {
        return
            NearlyEqual(a.x, b.x) &&
            NearlyEqual(a.y, b.y) &&
            NearlyEqual(a.z, b.z);
    }
}

void RunMatrix4Tests()
{
    // ============================================================
    // Test 1: Identity
    // ============================================================
    {
        const Matrix4 matrix =
            Matrix4::Identity();

        const Vector3 point{
            1.0f,
            2.0f,
            3.0f
        };

        const Vector3 result =
            matrix.TransformPoint(point);

        assert(NearlyEqual(
            result,
            Vector3{ 1.0f, 2.0f, 3.0f }));
    }


    // ============================================================
    // Test 2: Translation
    // ============================================================
    {
        const Matrix4 matrix =
            Matrix4::Translation(
                Vector3{ 10.0f, 20.0f, 30.0f });

        const Vector3 result =
            matrix.TransformPoint(
                Vector3{ 1.0f, 2.0f, 3.0f });

        assert(NearlyEqual(
            result,
            Vector3{ 11.0f, 22.0f, 33.0f }));
    }


    // ============================================================
    // Test 3: Translation must not affect direction
    // ============================================================
    {
        const Matrix4 matrix =
            Matrix4::Translation(
                Vector3{ 10.0f, 20.0f, 30.0f });

        const Vector3 result =
            matrix.TransformDirection(
                Vector3{ 0.0f, 0.0f, 1.0f });

        assert(NearlyEqual(
            result,
            Vector3{ 0.0f, 0.0f, 1.0f }));
    }


    // ============================================================
    // Test 4: Scale
    // ============================================================
    {
        const Matrix4 matrix =
            Matrix4::Scale(
                Vector3{ 2.0f, 3.0f, 4.0f });

        const Vector3 result =
            matrix.TransformPoint(
                Vector3{ 1.0f, 1.0f, 1.0f });

        assert(NearlyEqual(
            result,
            Vector3{ 2.0f, 3.0f, 4.0f }));
    }


    // ============================================================
    // Test 5: Quaternion rotation
    // +90 degrees around Y:
    // Forward (+Z) -> Right (+X)
    // ============================================================
    {
        constexpr float Pi =
            3.14159265358979323846f;

        const Quaternion rotation =
            Quaternion::FromAxisAngle(
                Vector3{ 0.0f, 1.0f, 0.0f },
                Pi * 0.5f);

        const Matrix4 matrix =
            Matrix4::Rotation(rotation);

        const Vector3 result =
            matrix.TransformDirection(
                Vector3{ 0.0f, 0.0f, 1.0f });

        assert(NearlyEqual(
            result,
            Vector3{ 1.0f, 0.0f, 0.0f }));
    }


    // ============================================================
    // Test 6: Matrix multiplication / TRS order
    //
    // Point = (1, 0, 0)
    // Scale x2       -> (2, 0, 0)
    // Rotate +90 Y   -> (0, 0, -2)
    // Translate +10X -> (10, 0, -2)
    // ============================================================
    {
        constexpr float Pi =
            3.14159265358979323846f;

        const Matrix4 scale =
            Matrix4::Scale(
                Vector3{ 2.0f, 2.0f, 2.0f });

        const Matrix4 rotation =
            Matrix4::Rotation(
                Quaternion::FromAxisAngle(
                    Vector3{ 0.0f, 1.0f, 0.0f },
                    Pi * 0.5f));

        const Matrix4 translation =
            Matrix4::Translation(
                Vector3{ 10.0f, 0.0f, 0.0f });

        const Matrix4 world =
            translation * rotation * scale;

        const Vector3 result =
            world.TransformPoint(
                Vector3{ 1.0f, 0.0f, 0.0f });

        assert(NearlyEqual(
            result,
            Vector3{ 10.0f, 0.0f, -2.0f }));
    }


    // test 7
    {
        constexpr float Pi =
            3.14159265358979323846f;

        Transform transform;

        transform.position =
            Vector3{ 10.0f, 0.0f, 0.0f };

        transform.rotation =
            Quaternion::FromAxisAngle(
                Vector3{ 0.0f, 1.0f, 0.0f },
                Pi * 0.5f);

        transform.scale =
            Vector3{ 2.0f, 2.0f, 2.0f };

        const Matrix4 world =
            transform.ToMatrix();

        const Vector3 result =
            world.TransformPoint(
                Vector3{ 1.0f, 0.0f, 0.0f });

        assert(NearlyEqual(
            result,
            Vector3{ 10.0f, 0.0f, -2.0f }));
    }
}