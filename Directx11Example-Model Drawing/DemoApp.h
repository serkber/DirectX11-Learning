#pragma once

#include "Dx11Base.h"
#include "FBXImporter.h"
#include "Resources.h"
#include "Typedefs.h"

class DemoApp : public CDx11Base
{
    struct Vertex
    {
        float3 pos;
        float3 nor;
        float2 uv;
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
    void ProcessClick();
    void ProcessRightClick();

private:
    bool CreateConstantBuffers();
    bool CreateIndexBuffer();
    bool LoadVertexShader(ID3DBlob** pVSBuffer);
    bool SetupInputLayout(ID3DBlob* vertexShaderBlob);
    bool LoadPixelShaders();
    bool CreateVertexBuffer();
    void CreateCameraMatrix();
    bool LoadTexture();
    void SetBlendingMode();
    bool CreateTextureSampler();

    ID3D11VertexShader* m_pVertexShader;
    ID3D11PixelShader* m_pPixelShader;
    ID3D11PixelShader* m_pPixelShaderWire;
    ID3D11InputLayout* m_pInputLayout;
    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pIndexBuffer;

    ID3D11Buffer* m_pViewCB;
    ID3D11Buffer* m_pProjCB;
    ID3D11Buffer* m_pModelCB;
    
    ID3D11ShaderResourceView* m_pColorMapOne;
    ID3D11SamplerState* m_pColorMapSampler;
    ID3D11Resource* m_pColorMapResource;
    ID3D11BlendState* m_pBlendState;

    matrix m_viewMatrix;
    matrix m_projMatrix;
    matrix m_modelMatrix;

    POINT* m_mousePos;
    float2 m_mousePosNorm;

    FBXImporter::FBXModel m_model;

    std::string m_models[4] {"Suzanne2.fbx", "Cube.fbx", "Torus.fbx", "Fish.fbx"};
    Tex m_textures[4] {Suzanne, CubeUnwrap, Marble, Fish};
    
    int m_currentModel = 0;
    bool m_drawWire = false;
};