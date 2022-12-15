#include "GraphicsManager.h"

#include "DirectXColors.h"

bool GraphicsManager::Initialize(int screenWidth, int screenHeight, HWND hWnd) {
	bool result;
	m_hWnd = hWnd;

	if (!DirectX::XMVerifyCPUSupport()) {
		MessageBox(m_hWnd, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
		return false;
	}

	m_d3d = std::make_unique<D3D11Manager>();

	result = m_d3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, m_hWnd, FULL_SCREEN);
	if (!result) {
		MessageBox(m_hWnd, TEXT("Could not initialize Direct3D 11."), TEXT("Error"), MB_OK);
		return false;
	}

	m_Keyboard = std::make_unique<DirectX::Keyboard>();
	m_Mouse = std::make_unique<DirectX::Mouse>();
	m_Mouse->SetWindow(m_hWnd);

	m_Scene = std::make_unique<Scene>("Sample scene", m_d3d->GetDevice(), m_d3d->GetDeviceContext());
	if (!m_Scene->Initialize(screenWidth, screenHeight, hWnd)) {
		return false;
	}

	return true;
}


void GraphicsManager::Shutdown() {
	m_Scene->Shutdown();

	if (m_d3d) {
		m_d3d->Shutdown();
	}
}

bool GraphicsManager::Frame(float deltaTime) {
	bool result;

	ProcessInput(deltaTime);
	result = Render(deltaTime);
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsManager::HandleResize(int screenWidth, int screenHeight) {
	// Resize swap chain buffers, set new viewport and new projection matrix
	// https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi#handling-window-resizing
	if (m_d3d) {
		m_d3d->ResizeSwapChain(screenWidth, screenHeight);
		m_d3d->SetViewport(screenWidth, screenHeight);
		m_Scene->HandleResize(screenWidth, screenHeight);
	}

	return true;
}

bool GraphicsManager::Render(float deltaTime) {
	m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Scene->Update(deltaTime);
	if (!m_Scene->Render()) {
		return false;
	}

	m_d3d->EndScene();

	return true;
}

void GraphicsManager::ProcessInput(float deltaTime) {
	float cameraDelta;
	auto kb = m_Keyboard->GetState();
	Camera* mainCamera = m_Scene->GetMainCamera();
	if (!mainCamera) return;

	if (kb.Home) {
		mainCamera->transform.position = Vector3::Backward * -0.5f;
		mainCamera->transform.rotation = Vector3::Zero;
	}

	Vector3 move = Vector3::Zero;
		
	cameraDelta = CAMERA_SPEED * deltaTime;
	if (kb.W) {
		move.z += cameraDelta;
	}
	if (kb.S) {
		move.z -= cameraDelta;
	}
	if (kb.D) {
		move.x += cameraDelta;
	}
	if (kb.A) {
		move.x -= cameraDelta;
	}
	if (kb.E) {
		move.y += cameraDelta;
	}
	if (kb.Q) {
		move.y -= cameraDelta;
	}

	move = Vector3::Transform(move, Matrix::CreateFromYawPitchRoll(mainCamera->transform.rotation));

	mainCamera->transform.position += move;

	auto mouse = m_Mouse->GetState();

	cameraDelta = LOOK_SPEED * deltaTime;
	if (mouse.positionMode == DirectX::Mouse::MODE_RELATIVE) {
		Vector3 delta = Vector3(1.0f * mouse.x, 1.0f * mouse.y, 0.0f);
		mainCamera->transform.rotation.x += delta.y * cameraDelta;
		mainCamera->transform.rotation.y += delta.x * cameraDelta;
	}

	m_Mouse->SetMode(mouse.rightButton ? DirectX::Mouse::MODE_RELATIVE : DirectX::Mouse::MODE_ABSOLUTE);
}
