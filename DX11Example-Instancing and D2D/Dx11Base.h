// Dx11Base.h : Defines the CDx11Base class.
//
// By Geelix School of Serious Games and Edutainment.
//

#ifndef _DX11BASE_H_
#define _DX11BASE_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include "../CommonScripts//Utils.h"

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
    ID3D11RenderTargetView*	m_pD3DRenderTargetView;
    IDXGISwapChain* m_pSwapChain;

    //2D stuff
    ID2D1RenderTarget* m_pD2DRenderTarget;    
    IDWriteTextFormat* m_pTextFormat;
    ID2D1SolidColorBrush* m_pTextBrush;
    ID2D1LinearGradientBrush* m_pLinearGradientBrush;

    POINT m_windSize;
};

#endif // _DX11BASE_H_
