#ifndef NXNA_GRAPHICS_VIEWPORT_H
#define NXNA_GRAPHICS_VIEWPORT_H

namespace Nxna
{
namespace Graphics
{
    struct Viewport
    {
        Viewport()
        {
            X = 0;
            Y = 0;
            Width = 0;
            Height = 0;
            MinDepth = 0;
            MaxDepth = 1.0f;
        }

        Viewport(float x, float y, float width, float height)
        {
            X = x;
            Y = y;
            Width = width;
            Height = height;
            MinDepth = 0;
            MaxDepth = 1.0f;
        }

        float GetAspectRatio() const { return Width / (float)Height; }
        Rectangle GetBounds() const { return Rectangle((int)X, (int)Y, (int)Width, (int)Height); }

#ifdef NXNA_ENABLE_MATH
        Vector3 Project(const Vector3& source,
            const Matrix& project,
            const Matrix& view,
            const Matrix& world) const;

        Vector3 Project(const Vector3& source,
            const Matrix& transform) const;
#endif
        void Project(const float* source3f, 
            const float* projectMatrix16f, 
            const float* viewMatrix16f,
            const float* worldMatrix16f,
            float* result3f) const;

#ifdef NXNA_ENABLE_MATH
        Vector3 Unproject(const Vector3& source,
            const Matrix& projection,
            const Matrix& view,
            const Matrix& world) const;
#endif


        float X, Y, Width, Height, MinDepth, MaxDepth;
    };
}
}

#endif // NXNA_GRAPHICS_VIEWPORT_H
