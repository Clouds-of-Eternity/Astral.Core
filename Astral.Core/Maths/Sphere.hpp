#pragma once
#include "Maths/Matrix4x4.hpp"

struct Sphere
{
    Maths::Vec3 center;
    float radius;

    inline Sphere()
    {
        center = Maths::Vec3();
        radius = 0.0f;
    }
    inline Sphere(Maths::Vec3 center, float radius)
    {
        this->center = center;
        this->radius = radius;
    }

    inline float GetScreenSpaceRadius(const Maths::Matrix4x4 &viewMatrix, float cameraFieldOfView, Maths::Vec3 *outViewSpaceCenter) const
    {
        Maths::Vec3 viewSpaceCenter = viewMatrix.Transform(Maths::Vec4(center, 1.0f)).ToVector3();
        if (outViewSpaceCenter != NULL)
        {
            *outViewSpaceCenter = viewSpaceCenter;
        }
        if (cameraFieldOfView == 0.0f)
        {
            return 0.0f;
        }
        float distanceToCenter = viewSpaceCenter.Z;

        float projectedRadius = (1.0f / tanf(cameraFieldOfView * 0.5f)) * (radius / sqrtf(distanceToCenter * distanceToCenter - radius * radius));
        return fabsf(projectedRadius);
    }
    inline Sphere ToScreenSpace(const Maths::Matrix4x4 &viewMatrix, const Maths::Matrix4x4 &projectionMatrix, float cameraFieldOfView) const
    {
        Maths::Vec3 viewSpaceCenter;
        float screenSpaceRadius;
        //is orthographic
        if (cameraFieldOfView == 0.0f)
        {
            viewSpaceCenter = viewMatrix.Transform(Maths::Vec4(center, 1.0f)).ToVector3();
            Maths::Vec3 viewSpaceRight = viewMatrix.Transform(Maths::Vec4(center + Maths::Vec3(radius, 0.0f, 0.0f), 1.0f)).ToVector3();
            screenSpaceRadius = viewSpaceRight.X - viewSpaceCenter.X;
        }
        else
        {
            viewSpaceCenter = {};
            screenSpaceRadius = GetScreenSpaceRadius(viewMatrix, cameraFieldOfView, &viewSpaceCenter);
        }

        Maths::Vec4 screenSpaceCenter = projectionMatrix.Transform(Maths::Vec4(viewSpaceCenter, 1.0f));

        Maths::Vec3 result = screenSpaceCenter.ToVector3();
        return Sphere(result / screenSpaceCenter.W, screenSpaceRadius);
    }
};