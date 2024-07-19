#include "DemoBlankWndD3D.h"
#include "Utils.h"

CDemoBlankWndD3D::CDemoBlankWndD3D()
{
    mt.seed(m_randomSeed);

    rndRotation = std::uniform_real_distribution<float>(-1.0f, 1.0f);
    rndScale = std::uniform_real_distribution<float>(0.04f, 0.08f);
    rndPosition = std::uniform_real_distribution<float>(-1.0f, 1.0f);
    rndColor = std::uniform_real_distribution<float>(0.0f, 1.0f);
    rndTexture = std::uniform_int_distribution<int>(1, 5);

    m_pVertexShader = NULL;
    m_pPixelShader = NULL;
    m_pInputLayout = NULL;
    m_pVertexBuffer = NULL;
    m_pParticlesDataBuffer = NULL;
    m_pCrosshairDataBuffer = NULL;
    m_pColorMapResource = NULL;
    m_pColorMapSampler = NULL;
    m_pBlendState = NULL;
    m_mousePos = new POINT;
    m_mousePosNorm = float2(0.0f, 0.0f);
    m_cameraMatrix = DirectX::XMMatrixIdentity();

    for (auto& colorMap : m_pColorMaps) {
        colorMap = NULL;
    }
}

CDemoBlankWndD3D::~CDemoBlankWndD3D()
{
    delete m_mousePos;
}

bool CDemoBlankWndD3D::LoadVertexShader(ID3DBlob** pVSBuffer)
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

bool CDemoBlankWndD3D::LoadPixelShader()
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

bool CDemoBlankWndD3D::SetupInputLayout(ID3DBlob* vertexShaderBlob)
{
    HRESULT hr;

    // Define input layout
    D3D11_INPUT_ELEMENT_DESC inputLayout[] =
    {
        // Data from the vertex buffer
        { "POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        // Data from the instance buffer
        { "INSTANCE_MVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE_MVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE_MVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE_MVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
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

bool CDemoBlankWndD3D::CreateVertexBuffer()
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

bool CDemoBlankWndD3D::CreateParticlesBuffer()
{
    HRESULT hr;

    // Instance buffer description
    D3D11_BUFFER_DESC instanceDesc;
    ::ZeroMemory(&instanceDesc, sizeof(instanceDesc));
    instanceDesc.Usage = D3D11_USAGE_DEFAULT;
    instanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    instanceDesc.CPUAccessFlags = 0;
    instanceDesc.ByteWidth = sizeof(InstanceData) * PARTICLES_COUNT;

    // Set particles data
    SetParticlesData();
    D3D11_SUBRESOURCE_DATA instancesData;
    ZeroMemory(&instancesData, sizeof(instancesData));
    instancesData.pSysMem = m_particlesData;

    // Create particles buffer
    hr = m_pD3DDevice->CreateBuffer(&instanceDesc, &instancesData, &m_pParticlesDataBuffer);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Particles Buffer Error", MB_OK);
        return false;
    }

    return true;
}

bool CDemoBlankWndD3D::CreateCrosshairBuffer()
{
    HRESULT hr;

    // Instance buffer description
    D3D11_BUFFER_DESC instanceDesc;
    ::ZeroMemory(&instanceDesc, sizeof(instanceDesc));
    instanceDesc.Usage = D3D11_USAGE_DEFAULT;
    instanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    instanceDesc.CPUAccessFlags = 0;
    instanceDesc.ByteWidth = sizeof(InstanceData) * 1;

    // Set crosshairData
    D3D11_SUBRESOURCE_DATA instancesData;
    ZeroMemory(&instancesData, sizeof(instancesData));
    instancesData.pSysMem = m_crossHairData;

    // Create crosshair buffer
    hr = m_pD3DDevice->CreateBuffer(&instanceDesc, &instancesData, &m_pCrosshairDataBuffer);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Crosshair Buffer Error", MB_OK);
        return false;
    }

    return true;
}

void CDemoBlankWndD3D::CreateCameraMatrix()
{
    matrix view = DirectX::XMMatrixIdentity();
    matrix projection = DirectX::XMMatrixOrthographicOffCenterLH(
        -1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    m_cameraMatrix = DirectX::XMMatrixMultiply(view, projection);
}

bool CDemoBlankWndD3D::LoadTextures()
{
    HRESULT hr;

    for (int i = 0; i < 6; i++)
    {
        ID3D11ShaderResourceView* colorMap;
        hr = DirectX::CreateDDSTextureFromFile(m_pD3DDevice, m_pD3DContext, Textures[i], &m_pColorMapResource, &colorMap);

        if (FAILED(hr))
        {
            ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Texture Load Error", MB_OK);
            return false;
        }

        m_pColorMaps[i] = colorMap;
    }
    return true;
}

void CDemoBlankWndD3D::SetBlendingMode()
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

bool CDemoBlankWndD3D::CreateTextureSampler()
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

void CDemoBlankWndD3D::DrawBackground() const
{
    HRESULT hr;
    
    D2D1_SIZE_F targetSize = m_pD2DRenderTarget->GetSize();
    
    m_pD2DRenderTarget->BeginDraw();
    
    m_pLinearGradientBrush->SetTransform(
        D2D1::Matrix3x2F::Scale(targetSize)
        );
    
    D2D1_RECT_F rect = D2D1::RectF(
        0.0f,
        0.0f,
        targetSize.width,
        targetSize.height
        );
    
    m_pD2DRenderTarget->FillRectangle(&rect, m_pLinearGradientBrush);
    
    hr = m_pD2DRenderTarget->EndDraw();
    if(FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"End Draw Error", MB_OK);
    }
}

void CDemoBlankWndD3D::DrawSpriteInstanced(Tex texture, int spritesCount) const
{
    m_pD3DContext->PSSetShaderResources(0, 1, &m_pColorMaps[texture]);
    m_pD3DContext->DrawInstanced(6, spritesCount, 0, 0);
}

void CDemoBlankWndD3D::DrawHUD() const
{
    HRESULT hr;
    m_pD2DRenderTarget->BeginDraw();
    
    D2D1_RECT_F layoutRect = D2D1::RectF(
        100,
        100,
        m_pD2DRenderTarget->GetSize().width,
        m_pD2DRenderTarget->GetSize().height
    );

    auto text = L"Rendering text to the backBuffer\nUsing Direct2D!";

    m_pD2DRenderTarget->DrawTextW(
        text,         // The string to render
        wcslen(text), // The string length
        m_pTextFormat,                 // The text format
        layoutRect,                  // The layout rectangle
        m_pTextBrush                   // The brush to use for drawing the text
    );
    
    hr = m_pD2DRenderTarget->EndDraw();
    if(FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"End Draw Error", MB_OK);
    }
}

void CDemoBlankWndD3D::ProcessClick()
{
    m_randomSeed++;

    mt.seed(m_randomSeed);

    SetParticlesData();
    m_pD3DContext->UpdateSubresource(m_pParticlesDataBuffer, 0, 0, m_particlesData, 0, 0);
}

void CDemoBlankWndD3D::SetParticlesData()
{
    m_particleTexture = (Tex)rndTexture(mt);
    for (int i = 0; i < PARTICLES_COUNT; i++) {
        float scale = rndScale(mt);

        matrix world = Utils::GetModelMatrix(float2(rndPosition(mt), rndPosition(mt)), float2(scale, scale), rndRotation(mt));
        matrix mvp = XMMatrixMultiply(world, m_cameraMatrix);
        mvp = XMMatrixTranspose(mvp);
        m_particlesData[i].mvp = mvp;
        m_particlesData[i].col = float4(rndColor(mt), rndColor(mt), rndColor(mt), rndColor(mt));
    }
}

//////////////////////////////////////////////////////////////////////
// Overrides
bool CDemoBlankWndD3D::LoadContent()
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

    if (!CreateParticlesBuffer())
    {
        return false;
    }

    if (!CreateCrosshairBuffer())
    {
        return false;
    }

    // Join particle buffers
    m_pParticlesBuffers[0] = m_pVertexBuffer;
    m_pParticlesBuffers[1] = m_pParticlesDataBuffer;
    // Join crosshair buffers
    m_pCrosshairBuffers[0] = m_pVertexBuffer;
    m_pCrosshairBuffers[1] = m_pCrosshairDataBuffer;

    if (!LoadTextures())
    {
        return false;
    }

    SetBlendingMode();

    CreateTextureSampler();

    return true;
}

void CDemoBlankWndD3D::UnloadContent()
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
    if (m_pParticlesDataBuffer)
        m_pParticlesDataBuffer->Release();
    m_pParticlesDataBuffer = NULL;
    if (m_pCrosshairDataBuffer)
        m_pCrosshairDataBuffer->Release();
    m_pCrosshairDataBuffer = NULL;
    if (m_pColorMapSampler)
        m_pColorMapSampler->Release();
    m_pColorMapSampler = NULL;
    if (m_pColorMapResource)
        m_pColorMapResource->Release();
    m_pColorMapResource = NULL;
    if (m_pBlendState)
        m_pBlendState->Release();
    m_pBlendState = NULL;

    for (auto& colorMap : m_pColorMaps) {
        colorMap->Release();
        colorMap = NULL;
    }
}

void CDemoBlankWndD3D::Update()
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

void CDemoBlankWndD3D::Render()
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

    DrawBackground();

    UINT strides[2] = { sizeof(Vertex), sizeof(InstanceData) };
    UINT offsets[2] = { 0, 0 };

    //Draw Sprites
    m_pD3DContext->IASetVertexBuffers(0, 2, m_pParticlesBuffers, strides, offsets);
    DrawSpriteInstanced(m_particleTexture, PARTICLES_COUNT);

    //DrawCrosshair
    matrix crosshairWorld = m_crosshair.GetModelMatrix();
    matrix crosshairMvp = XMMatrixMultiply(crosshairWorld, m_cameraMatrix);
    crosshairMvp = XMMatrixTranspose(crosshairMvp);
    m_crossHairData[0].mvp = crosshairMvp;
    m_crossHairData[0].col = float4(1.0f, 1.0f, 1.0f, 1.0f);
    m_pD3DContext->UpdateSubresource(m_pCrosshairDataBuffer, 0, 0, m_crossHairData, 0, 0);
    m_pD3DContext->IASetVertexBuffers(0, 2, m_pCrosshairBuffers, strides, offsets);
    DrawSpriteInstanced(m_crosshair.m_texture, 1);

    DrawHUD();

    // Present back buffer to display
    m_pSwapChain->Present(0, 0);
}
