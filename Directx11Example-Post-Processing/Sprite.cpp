#include "Sprite.h"

Sprite::Sprite() 
{
    m_position = float2(0.0f, 0.0f);
    m_rotation = 0;
    m_scale = float2(1.0f, 1.0f);
    m_texture = (TextureName)0;
};

Sprite::Sprite(float2 position, float rotation, float2 scale, TextureName texture)
{
    m_position = position;
    m_rotation = rotation;
    m_scale = scale;
    m_texture = texture;
}


Sprite::~Sprite()
{
}

// Gets the world transformation matrix
matrix Sprite::GetModelMatrix()
{
    matrix translation = DirectX::XMMatrixTranslation(m_position.x, m_position.y, 0.1f);
    matrix rotationZ = DirectX::XMMatrixRotationZ(m_rotation);
    matrix scale = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, 1.0f);
    return scale * rotationZ * translation;
}


// Sets the sprite position
void Sprite::SetPosition(float2& position)
{
    m_position = position;
}


// Sets the sprite rotation
void Sprite::SetRotation(float rotation)
{
    m_rotation = rotation;
}


// Sets the sprite scale
void Sprite::SetScale(float2& scale)
{
    m_scale = scale;
}