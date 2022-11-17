#include "D3D11Manager.h"

#include "Helpers.h"

bool D3D11Manager::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hWnd, 
	bool fullscreen, float screenDepth, float screenNear) {
	bool ret;

	// Store the vsync setting.
	m_VsyncEnabled = vsync;
	ret = InitializeDXGI(screenWidth, screenHeight, hWnd, fullscreen);
	if (!ret) {
		return false;
	}
	
	ret = CreateRenderTargets(screenWidth, screenHeight);
	if (!ret) {
		return false;
	}

	ret = CreateDepthStencilAndRasterizerStates();
	if (!ret) {
		return false;
	}

	SetViewportAndProjections(screenWidth, screenHeight, screenDepth, screenNear);

	return true;
}

bool D3D11Manager::InitializeDXGI(int screenWidth, int screenHeight, HWND hWnd, bool fullscreen) {
	DXGI_RATIONAL refreshRate;
	bool ret;
	// Query adapter information and refresh rate
	refreshRate = QueryAdapter(screenWidth, screenHeight);
	
	// Initialize Swap Chain using the queried refresh rate
	ret = CreateSwapChainAndDevice(screenWidth, screenHeight, hWnd, refreshRate, fullscreen);
	if (!ret) {
		return false;
	}

	return true;
}

DXGI_RATIONAL D3D11Manager::QueryAdapter(int screenWidth, int screenHeight) {
	HRESULT hr;
	IDXGIFactory1* factory;
	IDXGIAdapter1* adapter;
	IDXGIOutput* adapterOutput;
	UINT numDisplayModes;
	DXGI_MODE_DESC* displayModeList;
	DXGI_RATIONAL refreshRate;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	unsigned long long stringLength;

	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&factory));
	if (FAILED(hr)) {
		MessageBox(0,
			TEXT("Could not create DXGIFactory instance."),
			TEXT("Initialize DXGI"),
			MB_OK);

		throw new std::exception("Failed to create DXGIFactory.");
	}

	hr = factory->EnumAdapters1(0, &adapter);
	if (FAILED(hr)) {
		MessageBox(0,
			TEXT("Failed to enumerate adapters."),
			TEXT("Initialize DXGI"),
			MB_OK);

		throw new std::exception("Failed to enumerate adapters.");
	}

	hr = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(hr)) {
		MessageBox(0,
			TEXT("Failed to enumerate adapter outputs."),
			TEXT("Initialize DXGI"),
			MB_OK);

		throw new std::exception("Failed to enumerate adapter outputs.");
	}

	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr);
	if (FAILED(hr)) {
		MessageBox(0,
			TEXT("Failed to query display mode list."),
			TEXT("Initialize DXGI"),
			MB_OK);

		throw new std::exception("Failed to query display mode list.");
	}

	displayModeList = new DXGI_MODE_DESC[numDisplayModes];
	assert(displayModeList);

	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModeList);
	if (FAILED(hr)) {
		MessageBox(0,
			TEXT("Failed to query display mode list."),
			TEXT("Initialize DXGI"),
			MB_OK);

		throw new std::exception("Failed to query display mode list.");
	}

	for (UINT i = 0; i < numDisplayModes; i++) {
		if (displayModeList[i].Width == screenWidth &&
			displayModeList[i].Height == screenHeight) {
			refreshRate = displayModeList[i].RefreshRate;
		}
	}

	hr = adapter->GetDesc(&adapterDesc);
	if (FAILED(hr)) {
		MessageBox(0,
			TEXT("Failed to query adapter description."),
			TEXT("Initialize DXGI"),
			MB_OK);

		throw new std::exception("Failed to query adapter description.");
	}

	// Store video card memory in megabytes
	m_VideoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory) / (1024 * 1024);

	error = wcstombs_s(&stringLength,
		m_VideoCardDescription, 128,
		adapterDesc.Description, 128);
	if (error != 0) {
		throw new std::exception("Failed to convert video card description.");
	}

	delete[] displayModeList;
	SafeRelease(adapterOutput);
	SafeRelease(adapter);
	SafeRelease(factory);

	return refreshRate;
}

bool D3D11Manager::CreateSwapChainAndDevice(int screenWidth, int screenHeight, HWND hWnd,
	DXGI_RATIONAL refreshRate, bool fullscreen) {
	HRESULT result;
	UINT createDeviceFlags;
	D3D_FEATURE_LEVEL featureLevel;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (m_VsyncEnabled) {
		swapChainDesc.BufferDesc.RefreshRate = refreshRate;
	} else {
		swapChainDesc.BufferDesc.RefreshRate = { 0, 1 };
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	if (fullscreen) {
		swapChainDesc.Windowed = FALSE;
	} else {
		swapChainDesc.Windowed = TRUE;
	}

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	createDeviceFlags = 0;
#if _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// These are the feature levels that we will accept.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		createDeviceFlags, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, 
		&swapChainDesc, &m_SwapChain, &m_Device, &featureLevel, &m_DeviceContext);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

bool D3D11Manager::CreateRenderTargets(int screenWidth, int screenHeight) {
	HRESULT result;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ID3D11Texture2D* backBufferPtr;

	// Depth
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = m_Device->CreateTexture2D(&depthBufferDesc, nullptr, &m_DepthStencilBuffer);
	if (FAILED(result)) {
		return false;
	}

	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, 
		&m_DepthStencilView);
	if (FAILED(result)) {
		return false;
	}

	// Render target
	result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBufferPtr));
	if (FAILED(result)) {
		return false;
	}

	result = m_Device->CreateRenderTargetView(backBufferPtr, nullptr, &m_RenderTargetView);
	if (FAILED(result)) {
		return false;
	}

	SafeRelease(backBufferPtr);

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	return true;
}

bool D3D11Manager::CreateDepthStencilAndRasterizerStates() {
	HRESULT result;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_RASTERIZER_DESC rasterDesc;

	// Depth state
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);
	if (FAILED(result)) {
		return false;
	}

	// Rasterizer state
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.MultisampleEnable = FALSE;
	rasterDesc.ScissorEnable = FALSE;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = m_Device->CreateRasterizerState(&rasterDesc, &m_RasterizerState);
	if (FAILED(result)) {
		return false;
	}

	// TODO: separate rasterizer state from initialization
	m_DeviceContext->RSSetState(m_RasterizerState);

	// TODO: separate output-merger stage from initialization
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

	return true;
}

void D3D11Manager::SetViewportAndProjections(int screenWidth, int screenHeight, 
	float screenDepth, float screenNear) {
	if (!m_DeviceContext) {
		return;
	}

	D3D11_VIEWPORT viewport;
	float fieldOfView;
	float screenAspect;

	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// TODO: separate rasterizer state from initialization
	m_DeviceContext->RSSetViewports(1, &viewport);

	fieldOfView = DirectX::XM_PIDIV4;
	screenAspect = 1.0f * screenWidth / screenHeight;

	m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 
		screenNear, screenDepth);

	m_OrthoMatrix = DirectX::XMMatrixOrthographicLH(1.0f * screenWidth, 1.0f * screenHeight,
		screenNear, screenDepth);
}

void D3D11Manager::Shutdown() {
	if (m_SwapChain) {
		m_SwapChain->SetFullscreenState(FALSE, nullptr);
	}

	SafeRelease(m_RasterizerState);
	SafeRelease(m_DepthStencilView);
	SafeRelease(m_DepthStencilBuffer);
	SafeRelease(m_DepthStencilState);
	SafeRelease(m_RenderTargetView);
	SafeRelease(m_DeviceContext);
	SafeRelease(m_Device);
	SafeRelease(m_SwapChain);
}

void D3D11Manager::BeginScene(float red, float green, float blue, float alpha) {
	float color[4] = { red, green, blue, alpha };
	
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, color);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11Manager::EndScene() {
	if (m_VsyncEnabled) {
		m_SwapChain->Present(1, 0);
	} else {
		m_SwapChain->Present(0, 0);
	}
}

ID3D11Device* D3D11Manager::GetDevice() {
	return m_Device;
}

ID3D11DeviceContext* D3D11Manager::GetDeviceContext() {
	return m_DeviceContext;
}

DirectX::XMMATRIX& D3D11Manager::GetProjectionMatrix() {
	return m_ProjectionMatrix;
}

DirectX::XMMATRIX& D3D11Manager::GetOrthoMatrix() {
	return m_OrthoMatrix;
}

void D3D11Manager::GetVideoCardInfo(char* cardName, int& memory) {
	strcpy_s(cardName, 128, m_VideoCardDescription);
	memory = m_VideoCardMemory;
}

bool D3D11Manager::ResizeSwapChain(int screenWidth, int screenHeight) {
	if (m_SwapChain) {
		HRESULT result;

		// Unbind all render targets
		m_DeviceContext->OMSetRenderTargets(0, 0, 0);
		// Release the reference to the render target view
		m_RenderTargetView->Release();
		m_DepthStencilView->Release();

		// Preserve the existing buffer count and format.
		// Automatically choose the width and height to match the client rect for HWNDs.
		result = m_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(result)) {
			return false;
		}

		// Get a new handle to the updated swap chain back buffer
		CreateRenderTargets(screenWidth, screenHeight);
		/*ID3D11Texture2D* pBackBuffer;
		result = m_SwapChain->GetBuffer(0, 
			__uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
		if (FAILED(result)) {
			return false;
		}

		result = m_Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_RenderTargetView);
		if (FAILED(result)) {
			return false;
		}

		SafeRelease(pBackBuffer);

		// Bind the updated render target
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
		*/
	}

	return true;
}
