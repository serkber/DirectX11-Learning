﻿#pragma once

#include <d3d11.h>
#include <vector>

#define _WIN32_WINNT 0x0602
#define _USE_MATH_DEFINES
#include "Audio.h"

class CDx11Base
{
    // Constructors
public:
    CDx11Base();
    virtual ~CDx11Base();

    // Methods
public:
    bool Initialize(HWND hWnd, HINSTANCE hInst);
    void ReInitialize(HWND hWnd, HINSTANCE hInst);
    void Terminate();
    bool CompileShader(const wchar_t* shader_name, const char* shader_entry_point_name, LPCSTR shaderModel, ID3DBlob** buffer, LPCWSTR* errorMessage);
    bool CreateDepthStencilResources();
    void OnNewAudioDevice() noexcept;

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

    ID3D11Texture2D* m_pDepthTexture;
    ID3D11DepthStencilState* m_pDepthStencilState;
    ID3D11DepthStencilView* m_pDepthStencilView;
    ID3D11ShaderResourceView* m_pDepthShaderResource;
    
    std::vector<ID3D11Resource**> m_resources;

    DirectX::AudioEngine* m_audEngine;
    bool m_retryAudio = false;

    POINT m_windSize;
};
