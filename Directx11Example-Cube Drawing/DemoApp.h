#pragma once

#include "Dx11Base.h"
#include "Typedefs.h"

class DemoApp : public CDx11Base
{
    struct Vertex
    {
        float3 pos;
        float2 uv;
        float4 col;
    };

// Constructors
public:
    DemoApp();
    virtual ~DemoApp();

// Overrides
    virtual bool LoadContent();
    virtual void UnloadContent();

    virtual void Update();
    virtual void Render();

private:
    bool CreateConstantBuffers();
    bool CreateIndexBuffer();
    bool LoadVertexShader(ID3DBlob** pVSBuffer);
    bool SetupInputLayout(ID3DBlob* vertexShaderBlob);
    bool LoadPixelShader();
    bool CreateVertexBuffer();
    void CreateCameraMatrix();
    bool LoadTextures();
    void SetBlendingMode();
    bool CreateTextureSampler();

    ID3D11VertexShader* m_pVertexShader;
    ID3D11PixelShader* m_pPixelShader;
    ID3D11InputLayout* m_pInputLayout;
    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pIndexBuffer;

    ID3D11Buffer* m_pViewCB;
    ID3D11Buffer* m_pProjCB;
    ID3D11Buffer* m_pModelCB;
    
    ID3D11ShaderResourceView* m_pColorMapOne;
    ID3D11ShaderResourceView* m_pColorMapTwo;
    ID3D11SamplerState* m_pColorMapSampler;
    ID3D11Resource* m_pColorMapResource;
    ID3D11BlendState* m_pBlendState;

    matrix m_viewMatrix;
    matrix m_projMatrix;
    matrix m_modelMatrixCubeOne;
    matrix m_modelMatrixCubeTwo;

    POINT* m_mousePos;
    float2 m_mousePosNorm;
};