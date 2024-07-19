#include "DemoApp.h"
#include "Utils.h"
#include "DDSTextureLoader.h"
#include <DirectXMath.h>
#include <iostream>
#include <xinput.h>

DemoApp::DemoApp()
{
    m_pVertexShader = nullptr;
    m_pPixelShader = nullptr;
    m_pInputLayout = nullptr;
    m_pVertexBuffer = nullptr;
    m_pColorMapResource = nullptr;
    m_pColorMapSampler = nullptr;
    m_pConstantBuffer = nullptr;
    m_pBlendState = nullptr;
    m_mousePos = new POINT;
    m_mousePosNorm = float2(0.0f, 0.0f);
    m_cameraMatrix = DirectX::XMMatrixIdentity();
    m_pColorMapMouse = nullptr;
    m_pDirectInput = nullptr;
    m_pMouseDevice = nullptr;
    m_pKeyboardDevice = nullptr;
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

bool DemoApp::LoadPixelShader()
{
    LPCWSTR shaderError = L"";
    HRESULT hr;

    //Load Pixel Shader
    ID3DBlob* pPSBuffer = nullptr;
    if (!CompileShader(L"PixelShader.hlsl", "psmain", "ps_5_0", &pPSBuffer, &shaderError))
    {
        ::MessageBox(m_hWnd, shaderError, L"Pixel Shader Compilation Error", MB_OK);
        return false;
    }

    // Create pixel shader
    hr = m_pD3DDevice->CreatePixelShader(
        pPSBuffer->GetBufferPointer(),
        pPSBuffer->GetBufferSize(),
        nullptr,
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

bool DemoApp::LoadTexture(ID3D11ShaderResourceView** resource, Tex texture)
{
    HRESULT hr;

    hr = DirectX::CreateDDSTextureFromFile(m_pD3DDevice, m_pD3DContext, Textures[texture], &m_pColorMapResource, resource);

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
    HRESULT hr = m_pD3DDevice->CreateBuffer(&constDesc, nullptr, &m_pConstantBuffer);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Constant Buffer Error", MB_OK);
        return false;
    }
    return true;
}

bool DemoApp::InitInput()
{
    HRESULT hr;
    
    // Create DirectInput8 device
    hr = ::DirectInput8Create(
        m_hInst, DIRECTINPUT_VERSION, 
        IID_IDirectInput8, (void**)&m_pDirectInput, 0);
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Direct Input Device Error", MB_OK);
        return false;
    }
    
    // Create mouse device
    hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, nullptr);
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Mouse Create Device Error", MB_OK);
        return false;
    }
    hr = m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Mouse Data Format Error", MB_OK);
        return false;
    }
    hr = m_pMouseDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Mouse Cooperative Level Error", MB_OK);
        return false;
    }
    hr = m_pMouseDevice->Acquire();
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Mouse Acquire Error", MB_OK);
        return false;
    }
    
    // Create keyboard device
    hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, 0);
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Keyboard Create Device Error", MB_OK);
        return false;
    }
    hr = m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Keyboard Data Format Error", MB_OK);
        return false;
    }
    hr = m_pKeyboardDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Keyboard Cooperative Level Error", MB_OK);
        return false;
    }
    hr = m_pKeyboardDevice->Acquire();
    if (FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Keyboard Acquire Error", MB_OK);
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

    if (!LoadTexture(&m_pColorMapMouse, Mouse))
    {
        return false;
    }

    if (!LoadTexture(&m_pColorMapKeyboard, Keyboard))
    {
        return false;
    }
    
    if (!LoadTexture(&m_pColorMapGamepad, Gamepad))
    {
        return false;
    }

    SetBlendingMode();

    CreateTextureSampler();

    if(!InitInput())
    {
        return false;
    }

    return true;
}

void DemoApp::UnloadContent()
{
    if (m_pVertexShader)
        m_pVertexShader->Release();
    m_pVertexShader = nullptr;
    if (m_pPixelShader)
        m_pPixelShader->Release();
    m_pPixelShader = nullptr;
    if (m_pInputLayout)
        m_pInputLayout->Release();
    m_pInputLayout = nullptr;
    if (m_pVertexBuffer)
        m_pVertexBuffer->Release();
    m_pVertexBuffer = nullptr;
    if (m_pColorMapSampler)
        m_pColorMapSampler->Release();
    m_pColorMapSampler = nullptr;
    if (m_pColorMapResource)
        m_pColorMapResource->Release();
    m_pColorMapResource = nullptr;
    if (m_pBlendState)
        m_pBlendState->Release();
    m_pBlendState = nullptr;
    if (m_pConstantBuffer)
        m_pConstantBuffer->Release();
    m_pConstantBuffer = nullptr;
    if (m_pColorMapMouse)
        m_pColorMapMouse->Release();
    m_pColorMapMouse = nullptr;
    
    if (m_pDirectInput)
        m_pDirectInput->Release();
    m_pDirectInput = nullptr;
    if (m_pMouseDevice) {
        m_pMouseDevice->Unacquire();
        m_pMouseDevice->Release();
    }
    if (m_pKeyboardDevice) {
        m_pKeyboardDevice->Unacquire();
        m_pKeyboardDevice->Release();
    }
}

void DemoApp::Update()
{
    // ::GetCursorPos(m_mousePos);
    // ::ScreenToClient(m_hWnd, m_mousePos);
    // m_mousePosNorm.x = (float)m_mousePos->x / m_windSize.x;
    // m_mousePosNorm.y = (1.0f - (float)m_mousePos->y / m_windSize.y);
    // m_mousePosNorm.x -= 0.5f;
    // m_mousePosNorm.x *= 2.0f;
    // m_mousePosNorm.y -= 0.5f;
    // m_mousePosNorm.y *= 2.0f;
    //
    // if (m_mousePosNorm.x > -1.0f && m_mousePosNorm.x < 1.0f &&
    //     m_mousePosNorm.y > -1.0f && m_mousePosNorm.y < 1.0f)
    // {
    //     ShowCursor(false);
    // }
    //
    // m_mouseSprite.SetPosition(m_mousePosNorm);
    HandleMouse();
    HandleKeyboard();
    HandleXInput();
}

void DemoApp::HandleMouse()
{
    // Get mouse state data
    DIMOUSESTATE mouseData;
    m_pMouseDevice->GetDeviceState(sizeof(mouseData), &mouseData);
    
    // Button: Left mouse button
    if (mouseData.rgbButtons[0] & 0x80) {
        m_mouseSprite.m_rotation += 0.0001f;
    }
    
    // Button: Right mouse button
    if (mouseData.rgbButtons[1] & 0x80) {
        m_mouseSprite.m_rotation -= 0.0001f;
    }

    // Mouse move
    m_mouseSprite.m_position.x += (mouseData.lX / 100.0f);
    m_mouseSprite.m_position.y -= (mouseData.lY / 100.0f);
}

void DemoApp::HandleKeyboard()
{
    // Get keyboard state data
    char keyboardData[256];
    m_pKeyboardDevice->GetDeviceState(sizeof(keyboardData), (void*)&keyboardData);

    // Key: Down arrow
    if (keyboardData[DIK_DOWN] & 0x80) { 
        m_keyboardSprite.m_position.y -= 0.0001f;
    }

    // Key: Up arrow
    if (keyboardData[DIK_UP] & 0x80) { 
        m_keyboardSprite.m_position.y += 0.0001f;
    }

    // Key: Left arrow
    if (keyboardData[DIK_LEFT] & 0x80) { 
        m_keyboardSprite.m_position.x -= 0.0001f;
    }

    // Key: Right arrow
    if (keyboardData[DIK_RIGHT] & 0x80) { 
        m_keyboardSprite.m_position.x += 0.0001f;
    }
    
    // Key: Escape
    if (keyboardData[DIK_ESCAPE] & 0x80) { 
        ::PostQuitMessage(0);
    }
}

void DemoApp::HandleXInput()
{
    // Get XInput state data
    XINPUT_STATE xinputData;
    ::XInputGetState(0, &xinputData);
    
    m_gamepadSprite.m_position.x += xinputData.Gamepad.sThumbLX * 0.00000001f;
    m_gamepadSprite.m_position.y += xinputData.Gamepad.sThumbLY * 0.00000001f;

    // Button: Back
    if (xinputData.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) { 
        ::PostQuitMessage(0);
    }

    // Button: B
    if (xinputData.Gamepad.wButtons & XINPUT_GAMEPAD_B) { 
        m_gamepadSprite.m_scale.x += 0.0001f;
        m_gamepadSprite.m_scale.y += 0.0001f;
    }

    // Button: A
    if (xinputData.Gamepad.wButtons & XINPUT_GAMEPAD_A) { 
        m_gamepadSprite.m_scale.x -= 0.0001f;
        m_gamepadSprite.m_scale.y -= 0.0001f;
    }
}

void DemoApp::Render()
{
    // Check if D3D is ready
    if (m_pD3DContext == nullptr)
        return;

    // Clear back buffer
    float color[4] = { 1.0f, 0.3f, 0.6f, 1.0f };
    m_pD3DContext->ClearRenderTargetView(m_pD3DRenderTargetView, color);

    // Set shaders
    m_pD3DContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pD3DContext->PSSetShader(m_pPixelShader, nullptr, 0);
    m_pD3DContext->PSSetSamplers(0, 1, &m_pColorMapSampler);

    // Set stuff
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_pD3DContext->OMSetBlendState(m_pBlendState, blendFactor, 0xFFFFFFFF);
    m_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_pD3DContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    //DrawMouse
    matrix mouseWorld = m_mouseSprite.GetModelMatrix();
    matrix mouseMvp = XMMatrixMultiply(mouseWorld, m_cameraMatrix);
    mouseMvp = XMMatrixTranspose(mouseMvp);
    m_pD3DContext->UpdateSubresource( m_pConstantBuffer, 0, nullptr, &mouseMvp, 0, 0 );
    m_pD3DContext->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );    
    m_pD3DContext->PSSetShaderResources(0, 1, &m_pColorMapMouse);
    m_pD3DContext->Draw(6, 0);

    //Draw Keyboard
    matrix keyboardWorld = m_keyboardSprite.GetModelMatrix();
    matrix keyboardMvp = XMMatrixMultiply(keyboardWorld, m_cameraMatrix);
    keyboardMvp = XMMatrixTranspose(keyboardMvp);
    m_pD3DContext->UpdateSubresource( m_pConstantBuffer, 0, nullptr, &keyboardMvp, 0, 0 );
    m_pD3DContext->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );    
    m_pD3DContext->PSSetShaderResources(0, 1, &m_pColorMapKeyboard);
    m_pD3DContext->Draw(6, 0);
    
    //Draw Gamepad
    matrix gamepadWorld = m_gamepadSprite.GetModelMatrix();
    matrix gamepadMvp = XMMatrixMultiply(gamepadWorld, m_cameraMatrix);
    gamepadMvp = XMMatrixTranspose(gamepadMvp);
    m_pD3DContext->UpdateSubresource( m_pConstantBuffer, 0, nullptr, &gamepadMvp, 0, 0 );
    m_pD3DContext->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );    
    m_pD3DContext->PSSetShaderResources(0, 1, &m_pColorMapGamepad);
    m_pD3DContext->Draw(6, 0);

    // Present back buffer to display
    m_pSwapChain->Present(0, 0);
}
