#pragma once

#include "Typedefs.h"
#include <DirectXMath.h>
#include "Resources.h"

class Sprite
{
    // Constructors
public:
    Sprite();
    Sprite(float2 position, float rotation, float2 scale, TextureName texture);
    virtual ~Sprite();

    // Methods
public:
    matrix GetModelMatrix();
    void SetPosition(float2& position);
    void SetRotation(float rotation);
    void SetScale(float2& scale);

    // Attributes
public:
    float2 m_position;
    float m_rotation;
    float2 m_scale;
    TextureName m_texture;
};