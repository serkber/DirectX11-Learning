#include "Dx11Base.h"
#include "Utils.h"

CDx11Base::CDx11Base()
{
    m_hWnd = nullptr;
    m_hInst = nullptr;
    m_pD3DDevice = nullptr;
    m_pD3DContext = nullptr;
    m_pD3DRenderTargetView = nullptr;
    m_pSwapChain = nullptr;
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
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

    // Flags
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

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
        MessageBox(hWnd, Utils::GetMessageFromHr(hr), TEXT("ERROR"), MB_OK);
        return false;
    }

    // Get the back buffer from the swapchain
    ID3D11Texture2D *pBackBuffer;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr)) {
        MessageBox(hWnd, TEXT("Unable to get back buffer"), TEXT("ERROR"), MB_OK);
        return false;
    }

    // Create the render target view
    hr = m_pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pD3DRenderTargetView);

    // Check render target view
    if (FAILED(hr)) {
        MessageBox(hWnd, TEXT("Unable to create render target view"), TEXT("ERROR"), MB_OK);
        return false;
    }
    
    // Release the back buffer
    if (pBackBuffer != nullptr)
        pBackBuffer->Release();

    // Set the render target
    m_pD3DContext->OMSetRenderTargets(1, &m_pD3DRenderTargetView, nullptr);

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
    if (m_pD3DRenderTargetView != nullptr)
        m_pD3DRenderTargetView->Release();
    m_pD3DRenderTargetView= nullptr;
    if (m_pSwapChain != nullptr)
        m_pSwapChain->Release();
    m_pSwapChain= nullptr;
    if (m_pD3DContext != nullptr)
        m_pD3DContext->Release();
    m_pD3DContext= nullptr;
    if (m_pD3DDevice != nullptr)
        m_pD3DDevice->Release();
    m_pD3DDevice= nullptr;
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