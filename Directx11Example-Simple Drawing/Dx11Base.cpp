// Dx11Base.cpp : Implements the CDx11Base class.
//
// By Geelix School of Serious Games and Edutainment.
//

#include "Dx11Base.h"

CDx11Base::CDx11Base()
{
    m_hWnd = nullptr;
    m_hInst = nullptr;
    m_pD3DDevice = nullptr;
    m_pD3DContext = nullptr;
    m_pD3DRenderTargetView = nullptr;
    m_pSwapChain = nullptr;
    m_pD2DRenderTarget = nullptr;
    m_pTextFormat = nullptr;
    m_pTextBrush = nullptr;
    m_pLinearGradientBrush = nullptr;
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

    //Set up for 2D drawing
    IDXGISurface* pDXGISurface;
    hr = m_pSwapChain->GetBuffer(
    0,
    IID_PPV_ARGS(&pDXGISurface)
    );
    if(FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"DXGI Surface Error", MB_OK);
        return false;
    }
    
    ID2D1Factory* pD2DFactory;    
    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory),
        nullptr,
        (LPVOID*)&pD2DFactory);
    
    if(FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"D2D Factory Error", MB_OK);
        return false;
    }
    
    float dpi = GetDpiForWindow(m_hWnd);

    D2D1_RENDER_TARGET_PROPERTIES props =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpi,
            dpi);

    // Create a Direct2D render target that can draw into the surface in the swap chain
    hr = pD2DFactory->CreateDxgiSurfaceRenderTarget(pDXGISurface, &props, &m_pD2DRenderTarget);
    if(FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"D2D RenderTarget Error", MB_OK);
        return false;
    }

    // Stuff for background gradient    
    ID2D1GradientStopCollection* pGradientStops;
    D2D1_GRADIENT_STOP gradientStops[] =
    {
        { 0.0f, D2D1::ColorF(0.6f, 0.1f, 0.3f) },
        { 1.0f, D2D1::ColorF(0.8f, 0.7f, 0.3f) }
    };
    
    hr = m_pD2DRenderTarget->CreateGradientStopCollection(
        gradientStops,
        ARRAYSIZE(gradientStops),
        D2D1_GAMMA_2_2,
        D2D1_EXTEND_MODE_CLAMP,
        &pGradientStops
        );
    if(FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Gradient Stops Error", MB_OK);
        return false;
    }
    
    hr =  m_pD2DRenderTarget->CreateLinearGradientBrush(
         D2D1::LinearGradientBrushProperties(
             D2D1::Point2F(0, 0),
             D2D1::Point2F(1, 1)),
         pGradientStops,
         &m_pLinearGradientBrush);
    if(FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Gradient Brush Error", MB_OK);
        return false;
    }
    
    pGradientStops->Release();

    //Set up for Text rendering    
    IDWriteFactory* pDWriteFactory = nullptr;
    // Create DirectWrite Factory
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
    if(FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"DWrite Factory Error", MB_OK);
        return false;
    }

    // Create Text Format
    hr = pDWriteFactory->CreateTextFormat(
        L"Segoe UI",                  // Font family name
        NULL,                         // Font collection (NULL sets it to use the system font collection)
        DWRITE_FONT_WEIGHT_NORMAL,    // Font weight
        DWRITE_FONT_STYLE_NORMAL,     // Font style
        DWRITE_FONT_STRETCH_NORMAL,   // Font stretch
        54.0f,                        // Font size
        L"en-us",                     // Locale
        &m_pTextFormat
        );
    if(FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Text Format Error", MB_OK);
        return false;
    }

    pDWriteFactory->Release();

    // Create Solid Color Brush
    hr = m_pD2DRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White), 
        &m_pTextBrush
        );
    if(FAILED(hr))
    {
        ::MessageBox(m_hWnd, Utils::GetMessageFromHr(hr), L"Solid Color Brush Error", MB_OK);
        return false;
    }

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
    if (m_pD2DRenderTarget != nullptr)
        m_pD2DRenderTarget->Release();
    m_pD2DRenderTarget= nullptr;
    if (m_pTextFormat != nullptr)
        m_pTextFormat->Release();
    m_pTextFormat= nullptr;    
    if (m_pTextBrush != nullptr)
        m_pTextBrush->Release();
    m_pTextBrush= nullptr;
    if (m_pLinearGradientBrush != nullptr)
        m_pLinearGradientBrush->Release();
    m_pLinearGradientBrush= nullptr;
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