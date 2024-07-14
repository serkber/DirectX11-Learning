#include "Dx11Base.h"
#include "Utils.h"

CDx11Base::CDx11Base()
{
    m_hWnd = nullptr;
    m_hInst = nullptr;
    m_pD3DDevice = nullptr;
    m_pD3DContext = nullptr;
    m_pRenderTargetView = nullptr;
    m_pSwapChain = nullptr;
    m_pPostProcessResource = nullptr;
    m_pPostProcessTexture = nullptr;
    m_pBackBuffer = nullptr;
}

CDx11Base::~CDx11Base()
{
}

//////////////////////////////////////////////////////////////////////
// Overrides

bool CDx11Base::Initialize(HWND hWnd, HINSTANCE hInst)
{
    // Set attributes
    m_hWnd = hWnd;
    m_hInst = hInst;

    // Get window size
    RECT rc;
    ::GetClientRect(hWnd, &rc);
    m_windSize.x = rc.right - rc.left;
    m_windSize.y = rc.bottom - rc.top;

    // Swap chain structure
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ::ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = m_windSize.x;
    swapChainDesc.BufferDesc.Height = m_windSize.y;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_B8G8R8A8_UNORM; for use in tandem with D2D
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = (HWND)hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = true;

    // Supported feature levels
    D3D_FEATURE_LEVEL featureLevel;
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    // Supported driver levels
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE, D3D_DRIVER_TYPE_SOFTWARE
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    // Flags to use in tandem with D2D
    //UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    UINT flags = 0;

    // Create the D3D device and the swap chain
    HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
        nullptr, 
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr, 
        flags,
        featureLevels,
        numFeatureLevels,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &m_pSwapChain, 
        &m_pD3DDevice, 
        &featureLevel,
        &m_pD3DContext
        );

    // Check device
    if (FAILED(hr))	{
        MessageBox(hWnd, Utils::GetMessageFromHr(hr), TEXT("Swapchain and device ERROR"), MB_OK);
        return false;
    }

    // Get the back buffer from the swapchain
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pBackBuffer);
    if (FAILED(hr)) {
        MessageBox(hWnd, Utils::GetMessageFromHr(hr), TEXT("Get back buffer ERROR"), MB_OK);
        return false;
    }

    // Create the render target view
    hr = m_pD3DDevice->CreateRenderTargetView(m_pBackBuffer, nullptr, &m_pRenderTargetView);
    // Check render target view
    if (FAILED(hr)) {
        MessageBox(hWnd, Utils::GetMessageFromHr(hr), TEXT("Create render target ERROR"), MB_OK);
        return false;
    }
    D3D11_TEXTURE2D_DESC desc;
    m_pBackBuffer->GetDesc(&desc);
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    hr = m_pD3DDevice->CreateTexture2D(&desc, nullptr, &m_pPostProcessTexture);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, L"Failed to create destination texture.", L"Error", MB_OK);
        return false;
    }

    hr = m_pD3DDevice->CreateShaderResourceView(m_pPostProcessTexture, nullptr, &m_pPostProcessResource);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, L"Failed to create Shader Resource View from destination texture.", L"Error", MB_OK);
        return false;
    }

    // Set the viewport
    D3D11_VIEWPORT viewPort;
    viewPort.Width = (float)m_windSize.x;
    viewPort.Height = (float)m_windSize.y;
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;
    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    m_pD3DContext->RSSetViewports(1, &viewPort);

    // Load content
    return LoadContent();
}

void CDx11Base::Terminate()
{
    // Unload content
    UnloadContent();

    // Clean up
    if (m_pRenderTargetView != nullptr)
        m_pRenderTargetView->Release();
    m_pRenderTargetView= nullptr;
    if (m_pSwapChain != nullptr)
        m_pSwapChain->Release();
    m_pSwapChain= nullptr;
    if (m_pD3DContext != nullptr)
        m_pD3DContext->Release();
    m_pD3DContext= nullptr;
    if (m_pD3DDevice != nullptr)
        m_pD3DDevice->Release();
    m_pD3DDevice= nullptr;
    if (m_pPostProcessResource != nullptr)
        m_pPostProcessResource->Release();
    m_pPostProcessResource= nullptr;
    if (m_pPostProcessTexture != nullptr)
        m_pPostProcessTexture->Release();
    m_pPostProcessTexture = nullptr;
    if (m_pBackBuffer != nullptr)
        m_pBackBuffer->Release();
    m_pBackBuffer = nullptr;
}

bool CDx11Base::CompileShader(const wchar_t* shaderName, const char* shaderEntryPoint, LPCSTR shaderModel, ID3DBlob** buffer, LPCWSTR* errorMessage)
{
    ID3DBlob* errorBlob = nullptr;

    D3DCompileFromFile(shaderName, nullptr, nullptr, shaderEntryPoint, shaderModel, 0, 0, buffer, &errorBlob);
    if (errorBlob) {
        *errorMessage = Utils::GetMessageFromBlob((char*)errorBlob->GetBufferPointer());
        errorBlob->Release();
        return false;
    }

    return true;
}