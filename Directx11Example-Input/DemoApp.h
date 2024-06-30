#pragma once

#include "Dx11Base.h"
#include "Typedefs.h"
#include "Sprite.h"
#include <dinput.h>

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
public:
    virtual bool LoadContent();
    virtual void UnloadContent();

    virtual void Update();
    void HandleMouse();
    void HandleKeyboard();
    void HandleXInput();
    virtual void Render();

private:
    void DrawSpriteInstanced(TextureName texture, int spritesCount = 1) const;
    bool CreateConstantBuffer();
    bool InitInput();
    bool LoadVertexShader(ID3DBlob** pVSBuffer);
    bool SetupInputLayout(ID3DBlob* vertexShaderBlob);
    bool LoadPixelShader();
    bool CreateVertexBuffer();
    void CreateCameraMatrix();
    bool LoadTexture(ID3D11ShaderResourceView** resource, TextureName texture);
    void SetBlendingMode();
    bool CreateTextureSampler();

    ID3D11VertexShader* m_pVertexShader;
    ID3D11PixelShader* m_pPixelShader;
    ID3D11InputLayout* m_pInputLayout;
    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pConstantBuffer;
    ID3D11ShaderResourceView* m_pColorMapMouse;
    ID3D11ShaderResourceView* m_pColorMapKeyboard;
    ID3D11ShaderResourceView* m_pColorMapGamepad;
    ID3D11SamplerState* m_pColorMapSampler;
    ID3D11Resource* m_pColorMapResource;
    ID3D11BlendState* m_pBlendState;

    Sprite m_mouseSprite = Sprite(float2(0.5f, 0.5f), 0.0f, float2(0.1f, 0.1f), Mouse);
    Sprite m_keyboardSprite = Sprite(float2(0.7f, 0.5f), 0.0f, float2(0.1f, 0.1f), Keyboard);
    Sprite m_gamepadSprite = Sprite(float2(-0.7f, 0.5f), 0.0f, float2(0.1f, 0.1f), Gamepad);
    matrix m_cameraMatrix;

    LPDIRECTINPUT8 m_pDirectInput;
    POINT* m_mousePos;
    float2 m_mousePosNorm;
    LPDIRECTINPUTDEVICE8 m_pMouseDevice;
    LPDIRECTINPUTDEVICE8 m_pKeyboardDevice;
};