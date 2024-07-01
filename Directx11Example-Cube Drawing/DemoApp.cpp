#include "DemoApp.h"
#include "Utils.h"
#include "DDSTextureLoader.h"
#include <DirectXMath.h>
#include "Resources.h"

#define VERTEX_COUNT 8
#define INDEX_COUNT 36

DemoApp::DemoApp()
{
    m_pVertexShader = nullptr;
    m_pPixelShader = nullptr;
    m_pInputLayout = nullptr;
    m_pVertexBuffer = nullptr;
    m_pIndexBuffer = nullptr;
    m_pColorMapResource = nullptr;
    m_pColorMapSampler = nullptr;
    m_pColorMapOne = nullptr;
    m_pColorMapTwo = nullptr;
    m_pBlendState = nullptr;
    m_pViewCB = nullptr;
    m_pProjCB = nullptr;
    m_pModelCB = nullptr;
    m_mousePos = new POINT;
    m_mousePosNorm = float2(0.0f, 0.0f);
    m_viewMatrix = DirectX::XMMatrixIdentity();
    m_projMatrix = DirectX::XMMatrixIdentity();
    m_modelMatrixCubeOne = DirectX::XMMatrixIdentity();
    m_cubeTwoPosition = float3(0.0f, 0.0f, 2.0f);
    m_modelMatrixCubeTwo = XMMatrixTranspose(DirectX::XMMatrixTranslation(m_cubeTwoPosition.x, m_cubeTwoPosition.y, m_cubeTwoPosition.z));
    m_modelMatrixCubeThree =  XMMatrixTranspose(DirectX::XMMatrixScaling(0.2f, 0.2f, 0.2f) * DirectX::XMMatrixTranslation(-0.35f, 0.35f, 1.0f));
}

DemoApp::~DemoApp()
{
    delete m_mousePos;
}

bool DemoApp::LoadVertexShader(ID3DBlob** pVSBuffer)
{
    LPCWSTR shaderError = L"";

    //Load vertex Shader
    if (!CompileShader(L"VertexShader.hlsl", "vsmain", "vs_5_0", pVSBuffer, &shaderError))
    {
        ::MessageBox(m_hWnd, shaderError, L"Vertex Shader Compilation Error", MB_OK);
        return false;
    }
    
    ID3DBlob* blob = (*pVSBuffer);

    // Create vertex shader
    HRESULT hr = m_pD3DDevice->CreateVertexShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        &m_pVertexShader);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Vertex Shader Creation Error", MB_OK);
        if (pVSBuffer)
            blob->Release();
        return false;
    }

    return true;
}

bool DemoApp::LoadPixelShaders()
{
    LPCWSTR shaderError = L"";

    //Load Pixel Shader
    ID3DBlob* pPSBuffer = nullptr;
    if (!CompileShader(L"PixelShader.hlsl", "psmain", "ps_5_0", &pPSBuffer, &shaderError))
    {
        ::MessageBox(m_hWnd, shaderError, L"Pixel Shader Compilation Error", MB_OK);
        return false;
    }    
    // Create pixel shader
    HRESULT hr = m_pD3DDevice->CreatePixelShader(
        pPSBuffer->GetBufferPointer(),
        pPSBuffer->GetBufferSize(),
        nullptr,
        &m_pPixelShader);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Pixel Shader Creation Error", MB_OK);
        return false;
    }
    
    if (!CompileShader(L"PixelShaderDebugDepth.hlsl", "psmain", "ps_5_0", &pPSBuffer, &shaderError))
    {
        ::MessageBox(m_hWnd, shaderError, L"Pixel Shader Compilation Error", MB_OK);
        return false;
    }
    hr = m_pD3DDevice->CreatePixelShader(
        pPSBuffer->GetBufferPointer(),
        pPSBuffer->GetBufferSize(),
        nullptr,
        &m_pPixelShaderDepthDebug);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Pixel Shader Creation Error", MB_OK);
        return false;
    }

    return true;
}

bool DemoApp::SetupInputLayout(ID3DBlob* vertexShaderBlob)
{
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
    HRESULT hr = m_pD3DDevice->CreateInputLayout(
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
    //Define vertices
    Vertex vertices[] =
    {
        //FACE 1
        //POS                           UV              COLOR
        {{-0.5f, -0.5f, -0.5f},         {0.0f, 1.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f},          {0.0f, 0.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f},           {1.0f, 0.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
        {{0.5f, -0.5f, -0.5f},          {1.0f, 1.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
        
        {{0.5f, -0.5f, 0.5f},          {1.0f, 1.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f},           {1.0f, 0.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f},          {0.0f, 0.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f},         {0.0f, 1.0f},      {1.0f, 1.0f, 1.0f, 1.0f}},
    };

    // Vertex buffer description
    D3D11_BUFFER_DESC vertexDesc;
    ::ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.CPUAccessFlags = 0;
    vertexDesc.ByteWidth = sizeof(Vertex) * VERTEX_COUNT;

    // Resource data
    D3D11_SUBRESOURCE_DATA verticesData;
    ZeroMemory(&verticesData, sizeof(verticesData));
    verticesData.pSysMem = vertices;

    // Create vertex buffer
    HRESULT hr = m_pD3DDevice->CreateBuffer(&vertexDesc, &verticesData, &m_pVertexBuffer);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Vertex Buffer Error", MB_OK);
        return false;
    }

    return true;
}

void DemoApp::CreateCameraMatrix()
{
    m_viewMatrix = DirectX::XMMatrixIdentity();
    m_viewMatrix = XMMatrixInverse(nullptr, m_viewMatrix);
    
    m_projMatrix = DirectX::XMMatrixPerspectiveFovLH(1.0f, m_windSize.x / m_windSize.y, 0.05f, 100.0f);
}

bool DemoApp::LoadTextures()
{
    HRESULT hr = DirectX::CreateDDSTextureFromFile(m_pD3DDevice, m_pD3DContext, Textures[Dragon], &m_pColorMapResource, &m_pColorMapOne);
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Texture Load Error", MB_OK);
        return false;
    }
    hr = DirectX::CreateDDSTextureFromFile(m_pD3DDevice, m_pD3DContext, Textures[Gordo], &m_pColorMapResource, &m_pColorMapTwo);
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Texture Load Error", MB_OK);
        return false;
    }

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
    D3D11_SAMPLER_DESC textureDesc;
    ::ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    textureDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    textureDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    textureDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    textureDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    textureDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = m_pD3DDevice->CreateSamplerState(&textureDesc, &m_pColorMapSampler);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Texture Sampler Error", MB_OK);
        return false;
    }

    return true;
}

bool DemoApp::CreateConstantBuffers()
{
    // Create world matrix buffer
    D3D11_BUFFER_DESC constDesc;
    ::ZeroMemory(&constDesc, sizeof(constDesc));
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.ByteWidth = sizeof(matrix);
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    HRESULT hr = m_pD3DDevice->CreateBuffer(&constDesc, nullptr, &m_pViewCB);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"View Constant Buffer Error", MB_OK);
        return false;
    }
    hr = m_pD3DDevice->CreateBuffer(&constDesc, nullptr, &m_pProjCB);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Projection Constant Buffer Error", MB_OK);
        return false;
    }
    hr = m_pD3DDevice->CreateBuffer(&constDesc, nullptr, &m_pModelCB);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Model Constant Buffer Error", MB_OK);
        return false;
    }
    
    m_viewMatrix = DirectX::XMMatrixTranspose(m_viewMatrix);
    m_projMatrix = DirectX::XMMatrixTranspose(m_projMatrix);
    
    m_pD3DContext->UpdateSubresource(m_pViewCB, 0, nullptr, &m_viewMatrix, 0, 0);
    m_pD3DContext->UpdateSubresource(m_pProjCB, 0, nullptr, &m_projMatrix, 0, 0);
    return true;
}

bool DemoApp::CreateIndexBuffer()
{
    // Index buffer data
    WORD indices[] =
    {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        3, 2, 5, 5, 4, 3,
        7, 6, 1, 1, 0, 7,
        1, 6, 5, 5, 2, 1,
        0, 3, 4, 4, 7, 0,        
    };
    
    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(resourceData));

    // Create index buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ::ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.ByteWidth = sizeof( WORD ) * INDEX_COUNT;
    indexBufferDesc.CPUAccessFlags = 0;
    resourceData.pSysMem = indices;
    HRESULT hr = m_pD3DDevice->CreateBuffer(&indexBufferDesc, &resourceData, &m_pIndexBuffer);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Index Buffer Error", MB_OK);
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
    pVSBuffer = nullptr;

    if (!LoadPixelShaders())
    {
        return false;
    }

    if (!CreateVertexBuffer())
    {
        return false;
    }    

    if (!CreateIndexBuffer())
    {
        return false;
    }

    CreateCameraMatrix();

    if (!CreateConstantBuffers())
    {
        return false;
    }

    if (!LoadTextures())
    {
        return false;
    }

    SetBlendingMode();

    CreateTextureSampler();

    m_resources.push_back((ID3D11Resource**)&m_pVertexShader);
    m_resources.push_back((ID3D11Resource**)&m_pPixelShader);
    m_resources.push_back((ID3D11Resource**)&m_pInputLayout);
    m_resources.push_back((ID3D11Resource**)&m_pVertexBuffer);
    m_resources.push_back((ID3D11Resource**)&m_pIndexBuffer);
    m_resources.push_back((ID3D11Resource**)&m_pBlendState);
    m_resources.push_back((ID3D11Resource**)&m_pColorMapSampler);
    m_resources.push_back((ID3D11Resource**)&m_pColorMapOne);
    m_resources.push_back((ID3D11Resource**)&m_pColorMapTwo);
    m_resources.push_back(&m_pColorMapResource);
    m_resources.push_back((ID3D11Resource**)&m_pPixelShaderDepthDebug);
    m_resources.push_back((ID3D11Resource**)&m_pViewCB);
    m_resources.push_back((ID3D11Resource**)&m_pProjCB);
    m_resources.push_back((ID3D11Resource**)&m_pModelCB);

    return true;
}

void DemoApp::UnloadContent()
{
}

void DemoApp::Update()
{
    GetCursorPos(m_mousePos);
    ScreenToClient(m_hWnd, m_mousePos);
    m_mousePosNorm.x = (float)m_mousePos->x / m_windSize.x;
    m_mousePosNorm.y = (1.0f - (float)m_mousePos->y / m_windSize.y);
    m_mousePosNorm.x -= 0.5f;
    m_mousePosNorm.x *= 2.0f;
    m_mousePosNorm.y -= 0.5f;
    m_mousePosNorm.y *= 2.0f;

    if (m_mousePosNorm.x > -1.0f && m_mousePosNorm.x < 1.0f &&
        m_mousePosNorm.y > -1.0f && m_mousePosNorm.y < 1.0f)
    {
        //ShowCursor(false);
    }

    auto rotationMatrix = DirectX::XMMatrixRotationX(m_mousePosNorm.y) * DirectX::XMMatrixRotationY(-m_mousePosNorm.x);
    auto positionMatrix = DirectX::XMMatrixTranslation(0, 0, 3);

    m_modelMatrixCubeOne = rotationMatrix * positionMatrix;

    m_cubeTwoRotation += 0.0001f;
    m_cubeTwoPosition.z = m_mousePosNorm.x * 2 + 1.0f;
    m_modelMatrixCubeTwo = DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f) *
            DirectX::XMMatrixRotationY(m_cubeTwoRotation) *
            DirectX::XMMatrixTranslation(m_cubeTwoPosition.x, m_cubeTwoPosition.y, m_cubeTwoPosition.z);
    m_modelMatrixCubeTwo = DirectX::XMMatrixTranspose(m_modelMatrixCubeTwo);
}

void DemoApp::Render()
{
    // Check if D3D is ready
    if (m_pD3DContext == nullptr)
        return;

    // Clear back buffer
    float color[4] = { 0.2f, 0.2f, 0.3f, 1.0f };
    m_pD3DContext->ClearRenderTargetView(m_pD3DRenderTargetView, color);
    m_pD3DContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Set shaders
    m_pD3DContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pD3DContext->PSSetShader(m_pPixelShader, nullptr, 0);
    m_pD3DContext->PSSetSamplers(0, 1, &m_pColorMapSampler);

    // Set stuff
    //float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    //m_pD3DContext->OMSetBlendState(m_pBlendState, blendFactor, 0xFFFFFFFF);
    m_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Draw Cube One    
    // Set the render target
    m_pD3DContext->OMSetRenderTargets(1, &m_pD3DRenderTargetView, m_pDepthStencilView);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_pD3DContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
    m_pD3DContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    m_modelMatrixCubeOne = XMMatrixTranspose(m_modelMatrixCubeOne);
    m_pD3DContext->UpdateSubresource(m_pModelCB, 0, nullptr, &m_modelMatrixCubeOne, 0, 0);
    
    m_pD3DContext->VSSetConstantBuffers(0, 1, &m_pModelCB);
    m_pD3DContext->VSSetConstantBuffers(1, 1, &m_pViewCB);
    m_pD3DContext->VSSetConstantBuffers(2, 1, &m_pProjCB);    
    
    m_pD3DContext->PSSetShaderResources(0, 1, &m_pColorMapOne);
    m_pD3DContext->DrawIndexed(INDEX_COUNT, 0, 0);

    // Draw Cube Three
    // Set the render target
    m_pD3DContext->PSSetShaderResources(0, 1, &m_pColorMapTwo);
    m_pD3DContext->UpdateSubresource(m_pModelCB, 0, nullptr, &m_modelMatrixCubeTwo, 0, 0);
    m_pD3DContext->DrawIndexed(INDEX_COUNT, 0, 0);

    // Draw depth Texture
    m_pD3DContext->PSSetShader(m_pPixelShaderDepthDebug, nullptr, 0);
    m_pD3DContext->OMSetRenderTargets(1, &m_pD3DRenderTargetView, nullptr);
    m_pD3DContext->PSSetShaderResources(0, 1, &m_pDepthShaderResource);
    m_pD3DContext->UpdateSubresource(m_pModelCB, 0, nullptr, &m_modelMatrixCubeThree, 0, 0);
    m_pD3DContext->DrawIndexed(6, 0, 0);

    // Present back buffer to display
    m_pSwapChain->Present(0, 0);
}