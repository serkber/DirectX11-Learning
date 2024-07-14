#pragma once

#include "Dx11Base.h"
#include "Typedefs.h"
#include "Sprite.h"

class DemoApp : public CDx11Base
{
    struct Vertex
    {
        float3 pos;
        float2 uv;
        float4 col;
    };

    struct CBuffer
    {
        matrix mvp;
        float time;
        float2 mousePos;
    };

// Constructors
public:
    DemoApp();
    virtual ~DemoApp();

// Overrides
public:
    virtual bool LoadContent();
    virtual void UnloadContent();

    virtual void Update();
    virtual void Render();

private:
    bool CreateConstantBuffer();
    bool LoadVertexShader(const wchar_t* file, ID3D11VertexShader** shader, ID3DBlob** pVSBuffer);
    bool SetupInputLayout(ID3DBlob* vertexShaderBlob);
    bool LoadPixelShader(const wchar_t* file, ID3D11PixelShader** shader);
    bool CreateVertexBuffer();
    void CreateCameraMatrix();
    bool LoadTextures();
    void SetBlendingMode();
    bool CreateTextureSampler();

    ID3D11VertexShader* m_pVertexShader;
    ID3D11PixelShader* m_pPixelShader;
    ID3D11PixelShader* m_pPostProcessShader;
    ID3D11VertexShader* m_pPostProcessVShader;
    ID3D11InputLayout* m_pInputLayout;
    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pConstantBuffer;
    ID3D11ShaderResourceView* m_pBackgroundMap;
    ID3D11ShaderResourceView* m_pColorMap;
    ID3D11SamplerState* m_pColorMapSampler;
    ID3D11Resource* m_pColorMapResource;
    ID3D11Resource* m_pBackgroundMapResource;
    ID3D11BlendState* m_pBlendState;

    Sprite m_crosshair = Sprite(float2(0.5f, 0.5f), 0.0f, float2(0.1f, 0.1f), Crosshair);
    matrix m_cameraMatrix;

    POINT* m_mousePos;
    float2 m_mousePosNorm;

    float m_elapsedTime = 0;
};