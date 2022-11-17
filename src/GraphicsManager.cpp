#include "GraphicsManager.h"

bool GraphicsManager::Initialize(int screenWidth, int screenHeight, HWND hWnd) {
	bool result;
	std::unique_ptr<Model> model;

	if (!DirectX::XMVerifyCPUSupport()) {
		MessageBox(hWnd, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
		return false;
	}

	m_d3d = std::make_unique<D3D11Manager>();

	result = m_d3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hWnd,
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hWnd, TEXT("Could not initialize Direct3D 11."), TEXT("Error"), MB_OK);
		return false;
	}

	m_Keyboard = std::make_unique<DirectX::Keyboard>();
	m_Mouse = std::make_unique<DirectX::Mouse>();
	m_Mouse->SetWindow(hWnd);

	m_Camera = std::make_unique<Camera>();
	
	model = std::make_unique<Model>();
	result = model->Initialize(m_d3d->GetDevice(), "models/teapot2.obj");
	if (!result) {
		MessageBox(hWnd, TEXT("Could not initialize the teapot model."), TEXT("Error"), MB_OK);
		return false;
	}
	m_Models.push_back(std::move(model));

	model = std::make_unique<Model>();
	result = model->Initialize(m_d3d->GetDevice(), "models/racing_car.obj", { 25.0f, 0.0f, -4.0f });
	if (!result) {
		MessageBox(hWnd, TEXT("Could not initialize the car model."), TEXT("Error"), MB_OK);
		return false;
	}
	m_Models.push_back(std::move(model));

	m_Shader = std::make_unique<Shader>();
	result = m_Shader->Initialize(m_d3d->GetDevice(), hWnd);
	if (!result) {
		MessageBox(hWnd, TEXT("Could not initialize the shader object."), TEXT("Error"), MB_OK);
		return false;
	}

	// Change initial data here
	m_Camera->SetPosition({ 0.0f, 0.0f, -5.0f });

	return true;
}

void GraphicsManager::Shutdown() {
	if (m_Shader) {
		m_Shader->Shutdown();
	}

	for (auto& model : m_Models) {
		if (model) {
			model->Shutdown();
		}
	}

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
		m_d3d->SetViewportAndProjections(screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	}

	return true;
}

bool GraphicsManager::Render(float deltaTime) {
	bool result;

	m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render(deltaTime);

	m_Models[1]->Translate({-25.0f, 0.0f,  4.0f });
	m_Models[1]->Rotate({ 0.0f, 1.0f, 0.0f }, 15.0f * deltaTime);
	m_Models[1]->Translate({ 25.0f, 0.0f, -4.0f });

	for (auto& model : m_Models) {
		result = model->Render(m_d3d->GetDeviceContext(), m_Shader.get(),
			m_Camera->GetViewMatrix(), m_d3d->GetProjectionMatrix());
		if (!result) {
			return false;
		}
	}

	m_d3d->EndScene();

	return true;
}

void GraphicsManager::ProcessInput(float deltaTime) {
	float cameraDelta;
	auto kb = m_Keyboard->GetState();

	if (kb.Home) {
		m_Camera->SetPosition({ 0.0f, 0.0f, -5.0f });
		m_Camera->SetPitchYaw(0.0f, 0.0f);
	}

	DirectX::XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };
		
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

	auto q = DirectX::XMQuaternionRotationRollPitchYaw(m_Camera->GetPitch(), m_Camera->GetYaw(), 0.0f);
	
	auto moveVec = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&move), q);

	DirectX::XMStoreFloat3(&move, moveVec);
	m_Camera->Move(move);

	auto mouse = m_Mouse->GetState();

	cameraDelta = LOOK_SPEED * deltaTime;
	if (mouse.positionMode == DirectX::Mouse::MODE_RELATIVE) {
		DirectX::XMFLOAT3 delta = { 1.0f * mouse.x, 1.0f * mouse.y, 0.0f };
		m_Camera->UpdatePitch(delta.y * cameraDelta);
		m_Camera->UpdateYaw(delta.x * cameraDelta);
	}

	m_Mouse->SetMode(mouse.rightButton ? DirectX::Mouse::MODE_RELATIVE : DirectX::Mouse::MODE_ABSOLUTE);
}
