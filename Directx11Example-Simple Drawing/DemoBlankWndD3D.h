#ifndef _DEMOBLANKWNDD3D_H_
#define _DEMOBLANKWNDD3D_H_

#include "Dx11Base.h"
#include "DDSTextureLoader.h"
#include <DirectXMath.h>
#include "Typedefs.h"
#include "Sprite.h"
#include "Resources.h"
#include <random>

#define PARTICLES_COUNT 100

class CDemoBlankWndD3D : public CDx11Base
{
    struct Vertex
    {
        float3 pos;
        float2 uv;
        float4 col;
    };

    struct InstanceData
    {
        matrix mvp;
        float4 col;
    };

// Constructors
public:
    CDemoBlankWndD3D();
    virtual ~CDemoBlankWndD3D();

// Overrides
public:
    virtual bool LoadContent();
    virtual void UnloadContent();

    virtual void Update();
    virtual void Render();

    virtual void ProcessClick();

private:
    void DrawSpriteInstanced(TextureName texture, int spritesCount = 1) const;
    void DrawHUD() const;
    void DrawBackground() const;
    void SetParticlesData();
    bool LoadVertexShader(ID3DBlob** pVSBuffer);
    bool SetupInputLayout(ID3DBlob* vertexShaderBlob);
    bool LoadPixelShader();
    bool CreateVertexBuffer();
    bool CreateParticlesBuffer();
    bool CreateCrosshairBuffer();
    void CreateCameraMatrix();
    bool LoadTextures();
    void SetBlendingMode();
    bool CreateTextureSampler();

    ID3D11VertexShader* m_pVertexShader;
    ID3D11PixelShader* m_pPixelShader;
    ID3D11InputLayout* m_pInputLayout;
    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pParticlesDataBuffer;
    ID3D11Buffer* m_pParticlesBuffers[2];
    ID3D11Buffer* m_pCrosshairDataBuffer;
    ID3D11Buffer* m_pCrosshairBuffers[2];
    ID3D11ShaderResourceView* m_pColorMaps[6];
    ID3D11SamplerState* m_pColorMapSampler;
    ID3D11Resource* m_pColorMapResource;
    ID3D11BlendState* m_pBlendState;

    TextureName m_particleTexture;
    InstanceData m_particlesData[PARTICLES_COUNT];
    InstanceData m_crossHairData[1];
    Sprite m_crosshair = Sprite(float2(0.5f, 0.5f), 0.0f, float2(0.1f, 0.1f), Crosshair);
    int m_randomSeed;
    matrix m_cameraMatrix;

    POINT* m_mousePos;
    float2 m_mousePosNorm;

    std::mt19937 mt;
    std::uniform_real_distribution<float> rndRotation;
    std::uniform_real_distribution<float> rndScale;
    std::uniform_real_distribution<float> rndPosition;
    std::uniform_real_distribution<float> rndColor;
    std::uniform_int_distribution<int> rndTexture;
};

#endif // _DEMOBLANKWNDD3D_H_
