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
		FULL_SCREEN);
	if (!result) {
		MessageBox(hWnd, TEXT("Could not initialize Direct3D 11."), TEXT("Error"), MB_OK);
		return false;
	}

	m_Keyboard = std::make_unique<DirectX::Keyboard>();
	m_Mouse = std::make_unique<DirectX::Mouse>();
	m_Mouse->SetWindow(hWnd);

	std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	camera->GenerateProjectionMatrices(screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	m_MainCamera = camera.get();

	model = std::make_unique<Model>();
	result = model->Initialize(m_d3d->GetDevice(), "models/teapot2.obj");
	if (!result) {
		MessageBox(hWnd, TEXT("Could not initialize the teapot model."), TEXT("Error"), MB_OK);
		return false;
	}
	m_Models.push_back(std::move(model));

	model = std::make_unique<Model>();
	result = model->Initialize(m_d3d->GetDevice(), "models/racing_car.obj");
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
	m_MainCamera->transform.position = Vector3(0.0f, 0.0f, -5.0f);

	m_SceneRoot = std::make_unique<SceneNode>();

	std::unique_ptr<SceneNode> node1 = std::make_unique<SceneNode>(m_SceneRoot.get(), m_Models[0].get());
	std::unique_ptr<SceneNode> node2 = std::make_unique<SceneNode>(node1.get(), m_Models[1].get());
	node2->transform.position = Vector3(25.0f, 0.0f, -4.0f);
	//node2->transform.UpdateGlobalMatrix();
	std::unique_ptr<SceneNode> node3 = std::make_unique<SceneNode>(node2.get(), m_Models[0].get());
	node3->transform.position = Vector3(30.0f, 0.0f, 0.0f);
	node2->AddChild(std::move(node3));
	node1->AddChild(std::move(node2));
	m_SceneRoot->AddChild(std::move(node1));
	
	//camera->SetModel(m_Models[0].get());
	m_SceneRoot->AddChild(std::move(camera));


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
		m_d3d->SetViewport(screenWidth, screenHeight);
		m_MainCamera->GenerateProjectionMatrices(screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	}

	return true;
}

bool GraphicsManager::Render(float deltaTime) {
	bool result;

	m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//m_MainCamera->Render(deltaTime);
	m_MainCamera->GenerateViewMatrix();
	m_MainCamera->UpdateTransform();

	//m_Models[1]->transform.position += Vector3(-25.0f, 0.0f, 4.0f);
	m_SceneRoot->children[0]->children[0]->children[0]->transform.rotation += Vector3::Up * DirectX::XMConvertToRadians(30.0f) * deltaTime;
	//m_SceneRoot->children[0]->children[0]->children[0]->UpdateTransform();
	m_SceneRoot->children[0]->children[0]->transform.rotation += Vector3::Up * DirectX::XMConvertToRadians(15.0f) * deltaTime;
	//m_SceneRoot->children[0]->children[0]->UpdateTransform();
	//m_Models[1]->transform.position += Vector3(25.0f, 0.0f, -4.0f);
	m_SceneRoot->children[0]->transform.rotation += Vector3::Up * DirectX::XMConvertToRadians(15.0f) * deltaTime;
	m_SceneRoot->children[0]->transform.scale = Vector3::One * 0.1f;
	m_SceneRoot->children[0]->UpdateTransform();

	/*for (auto& object : m_Objects) {
		result = object->Render(m_d3d->GetDeviceContext(), m_Shader.get(),
			m_MainCamera->GetViewMatrix(), m_d3d->GetProjectionMatrix());
		if (!result) {
			return false;
		}
	}*/
	result = m_SceneRoot->Render(m_d3d->GetDeviceContext(), m_Shader.get(),
		m_MainCamera->GetViewMatrix(), m_MainCamera->GetProjectionMatrix());
	if (!result) {
		return false;
	}

	m_d3d->EndScene();

	return true;
}

void GraphicsManager::ProcessInput(float deltaTime) {
	float cameraDelta;
	auto kb = m_Keyboard->GetState();

	if (kb.Home) {
		m_MainCamera->transform.position = Vector3::Backward * -0.5f;
		m_MainCamera->transform.rotation = Vector3::Zero;
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

	move = Vector3::Transform(move, Matrix::CreateFromYawPitchRoll(m_MainCamera->transform.rotation));

	m_MainCamera->transform.position += move;

	auto mouse = m_Mouse->GetState();

	cameraDelta = LOOK_SPEED * deltaTime;
	if (mouse.positionMode == DirectX::Mouse::MODE_RELATIVE) {
		Vector3 delta = Vector3(1.0f * mouse.x, 1.0f * mouse.y, 0.0f);
		m_MainCamera->transform.rotation.x += delta.y * cameraDelta;
		m_MainCamera->transform.rotation.y += delta.x * cameraDelta;
	}

	m_Mouse->SetMode(mouse.rightButton ? DirectX::Mouse::MODE_RELATIVE : DirectX::Mouse::MODE_ABSOLUTE);
}
