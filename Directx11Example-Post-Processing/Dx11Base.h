#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>

class CDx11Base
{
    // Constructors
public:
    CDx11Base();
    virtual ~CDx11Base();

    // Methods
public:
    bool Initialize(HWND hWnd, HINSTANCE hInst);
    void Terminate();
    bool CompileShader(const wchar_t* shader_name, const char* shader_entry_point_name, LPCSTR shaderModel, ID3DBlob** buffer, LPCWSTR* errorMessage);

    // Overrides
public:
    virtual bool LoadContent() = 0;
    virtual void UnloadContent() = 0;

    virtual void Update() = 0;
    virtual void Render() = 0;

    // Attributes
public:
    HWND m_hWnd;
    HINSTANCE m_hInst;
    ID3D11Device* m_pD3DDevice;
    ID3D11DeviceContext* m_pD3DContext;
    ID3D11RenderTargetView*	m_pRenderTargetView;
    IDXGISwapChain* m_pSwapChain;
    ID3D11Texture2D* m_pPostProcessTexture;
    ID3D11ShaderResourceView* m_pPostProcessResource;
    ID3D11Texture2D* m_pBackBuffer;
    POINT m_windSize;
};