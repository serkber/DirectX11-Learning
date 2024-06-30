#include "DemoApp.h"
#include "Utils.h"
#include "DDSTextureLoader.h"
#include <DirectXMath.h>

DemoApp::DemoApp()
{
    m_pVertexShader = NULL;
    m_pPixelShader = NULL;
    m_pInputLayout = NULL;
    m_pVertexBuffer = NULL;
    m_pColorMapResource = NULL;
    m_pColorMapSampler = NULL;
    m_pConstantBuffer = NULL;
    m_pBlendState = NULL;
    m_mousePos = new POINT;
    m_mousePosNorm = float2(0.0f, 0.0f);
    m_cameraMatrix = DirectX::XMMatrixIdentity();
    m_pColorMap = NULL;
}

DemoApp::~DemoApp()
{
    delete m_mousePos;
}

bool DemoApp::LoadVertexShader(ID3DBlob** pVSBuffer)
{
    LPCWSTR shaderError = L"";
    HRESULT hr;

    //Load vertex Shader
    if (!CompileShader(L"VertexShader.hlsl", "vsmain", "vs_5_0", pVSBuffer, &shaderError))
    {
        ::MessageBox(m_hWnd, shaderError, L"Vertex Shader Compilation Error", MB_OK);
        return false;
    }
    
    ID3DBlob* blob = (*pVSBuffer);

    // Create vertex shader
    hr = m_pD3DDevice->CreateVertexShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        0,
        &m_pVertexShader);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Vertex Shader Creation Error", MB_OK);
        if (pVSBuffer)
            blob->Release();
        return false;
    }

    return true;
}

bool DemoApp::LoadPixelShader()
{
    LPCWSTR shaderError = L"";
    HRESULT hr;

    //Load Pixel Shader
    ID3DBlob* pPSBuffer = NULL;
    if (!CompileShader(L"PixelShader.hlsl", "psmain", "ps_5_0", &pPSBuffer, &shaderError))
    {
        ::MessageBox(m_hWnd, shaderError, L"Pixel Shader Compilation Error", MB_OK);
        return false;
    }

    // Create pixel shader
    hr = m_pD3DDevice->CreatePixelShader(
        pPSBuffer->GetBufferPointer(),
        pPSBuffer->GetBufferSize(),
        0,
        &m_pPixelShader);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Pixel Shader Creation Error", MB_OK);
        return false;
    }

    return true;
}

bool DemoApp::SetupInputLayout(ID3DBlob* vertexShaderBlob)
{
    HRESULT hr;

    // Define input layout
    D3D11_INPUT_ELEMENT_DESC inputLayout[] =
    {
        // Data from the vertex buffer
        { "POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    UINT numLayoutElements = ARRAYSIZE(inputLayout);

    // Create input layout
    hr = m_pD3DDevice->CreateInputLayout(
        inputLayout,
        numLayoutElements,
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        &m_pInputLayout);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Input Layout Error", MB_OK);
        return false;
    }

    m_pD3DContext->IASetInputLayout(m_pInputLayout);

    return true;
}

bool DemoApp::CreateVertexBuffer()
{
    HRESULT hr;

    //Define vertices
    Vertex vertices[] =
    {
        //POS                           UV              COLOR
        {{-1.0f, -1.0f, 1.0f},       {0.0f, 1.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
        {{-1.0f, 1.0f, 1.0f},        {0.0f, 0.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
        {{1.0f, -1.0f, 1.0f},        {1.0f, 1.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
        {{1.0f, 1.0f, 1.0f},         {1.0f, 0.0f},      {1.0f, 1.0f, 1.0f, 1.0f}}
    };

    // Vertex buffer description
    D3D11_BUFFER_DESC vertexDesc;
    ::ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.CPUAccessFlags = 0;
    vertexDesc.ByteWidth = sizeof(Vertex) * 4;

    // Resource data
    D3D11_SUBRESOURCE_DATA verticesData;
    ZeroMemory(&verticesData, sizeof(verticesData));
    verticesData.pSysMem = vertices;

    // Create vertex buffer
    hr = m_pD3DDevice->CreateBuffer(&vertexDesc, &verticesData, &m_pVertexBuffer);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Vertex Buffer Error", MB_OK);
        return false;
    }

    return true;
}

void DemoApp::CreateCameraMatrix()
{
    matrix view = DirectX::XMMatrixIdentity();
    matrix projection = DirectX::XMMatrixOrthographicOffCenterLH(
        -1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    m_cameraMatrix = DirectX::XMMatrixMultiply(view, projection);
}

bool DemoApp::LoadTexture()
{
    HRESULT hr;

    ID3D11ShaderResourceView* colorMap;
    hr = DirectX::CreateDDSTextureFromFile(m_pD3DDevice, m_pD3DContext, Textures[Crosshair], &m_pColorMapResource, &colorMap);

    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Texture Load Error", MB_OK);
        return false;
    }

    m_pColorMap = colorMap;
    return true;
}

void DemoApp::SetBlendingMode()
{
    D3D11_BLEND_DESC blendDesc;
    ::ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//D3D11_BLEND_ONE
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
    m_pD3DDevice->CreateBlendState(&blendDesc, &m_pBlendState);
}

bool DemoApp::CreateTextureSampler()
{
    HRESULT hr;

    D3D11_SAMPLER_DESC textureDesc;
    ::ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    textureDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    textureDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    textureDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    textureDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    textureDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = m_pD3DDevice->CreateSamplerState(&textureDesc, &m_pColorMapSampler);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Texture Sampler Error", MB_OK);
        return false;
    }

    return true;
}

bool DemoApp::CreateConstantBuffer()
{
    // Create world matrix buffer
    D3D11_BUFFER_DESC constDesc;
    ::ZeroMemory(&constDesc, sizeof(constDesc));
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.ByteWidth = sizeof(matrix);
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    HRESULT hr = m_pD3DDevice->CreateBuffer(&constDesc, 0, &m_pConstantBuffer);
    if (FAILED(hr)) {
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////
// Overrides
bool DemoApp::LoadContent()
{
    ID3DBlob* pVSBuffer = nullptr;
    
    if (!LoadVertexShader(&pVSBuffer))
    {
        return false;
    }

    if (!SetupInputLayout(pVSBuffer))
    {
        return false;
    }

    pVSBuffer->Release();
    pVSBuffer = NULL;

    if (!LoadPixelShader())
    {
        return false;
    }

    if (!CreateVertexBuffer())
    {
        return false;
    }

    CreateCameraMatrix();

    if (!CreateConstantBuffer())
    {
        return false;
    }

    if (!LoadTexture())
    {
        return false;
    }

    SetBlendingMode();

    CreateTextureSampler();

    return true;
}

void DemoApp::UnloadContent()
{
    if (m_pVertexShader)
        m_pVertexShader->Release();
    m_pVertexShader = NULL;
    if (m_pPixelShader)
        m_pPixelShader->Release();
    m_pPixelShader = NULL;
    if (m_pInputLayout)
        m_pInputLayout->Release();
    m_pInputLayout = NULL;
    if (m_pVertexBuffer)
        m_pVertexBuffer->Release();
    m_pVertexBuffer = NULL;
    if (m_pColorMapSampler)
        m_pColorMapSampler->Release();
    m_pColorMapSampler = NULL;
    if (m_pColorMapResource)
        m_pColorMapResource->Release();
    m_pColorMapResource = NULL;
    if (m_pBlendState)
        m_pBlendState->Release();
    m_pBlendState = NULL;
    if (m_pConstantBuffer)
        m_pConstantBuffer->Release();
    m_pConstantBuffer = NULL;
    if (m_pColorMap)
        m_pColorMap->Release();
    m_pColorMap = NULL;
}

void DemoApp::Update()
{
    ::GetCursorPos(m_mousePos);
    ::ScreenToClient(m_hWnd, m_mousePos);
    m_mousePosNorm.x = (float)m_mousePos->x / m_windSize.x;
    m_mousePosNorm.y = (1.0f - (float)m_mousePos->y / m_windSize.y);
    m_mousePosNorm.x -= 0.5f;
    m_mousePosNorm.x *= 2.0f;
    m_mousePosNorm.y -= 0.5f;
    m_mousePosNorm.y *= 2.0f;

    if (m_mousePosNorm.x > -1.0f && m_mousePosNorm.x < 1.0f &&
        m_mousePosNorm.y > -1.0f && m_mousePosNorm.y < 1.0f)
    {
        ShowCursor(false);
    }

    m_crosshair.SetPosition(m_mousePosNorm);
    m_crosshair.SetRotation(m_crosshair.m_rotation + 0.001f);
}

void DemoApp::Render()
{
    // Check if D3D is ready
    if (m_pD3DContext == NULL)
        return;

    // Clear back buffer
    float color[4] = { 0.2f, 0.2f, 0.3f, 1.0f };
    m_pD3DContext->ClearRenderTargetView(m_pD3DRenderTargetView, color);

    // Set shaders
    m_pD3DContext->VSSetShader(m_pVertexShader, 0, 0);
    m_pD3DContext->PSSetShader(m_pPixelShader, 0, 0);
    m_pD3DContext->PSSetSamplers(0, 1, &m_pColorMapSampler);

    // Set stuff
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_pD3DContext->OMSetBlendState(m_pBlendState, blendFactor, 0xFFFFFFFF);
    m_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    //DrawCrosshair
    matrix crosshairWorld = m_crosshair.GetModelMatrix();
    matrix crosshairMvp = XMMatrixMultiply(crosshairWorld, m_cameraMatrix);
    crosshairMvp = XMMatrixTranspose(crosshairMvp);
    
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_pD3DContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
    m_pD3DContext->UpdateSubresource( m_pConstantBuffer, 0, 0, &crosshairMvp, 0, 0 );
    m_pD3DContext->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
    
    m_pD3DContext->PSSetShaderResources(0, 1, &m_pColorMap);
    m_pD3DContext->Draw(6, 0);

    // Present back buffer to display
    m_pSwapChain->Present(0, 0);
}
