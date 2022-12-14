#include "Scene.h"

#include "DirectXColors.h"

Scene::Scene(ID3D11Device* device, ID3D11DeviceContext* deviceContext): m_Device(device), m_DeviceContext(deviceContext),
	m_MainCamera(nullptr), m_hWnd(nullptr) {}

bool Scene::Initialize(int screenWidth, int screenHeight, HWND hWnd) {
	m_hWnd = hWnd;

	if (!InitializeShaders()) {
		return false;
	}

	InitializeTextures();
	InitializeMaterials();
	if (!InitializeModels()) {
		return false;
	}

	InitializeScene(screenWidth, screenHeight);

	return true;
}

bool Scene::InitializeShaders() {
	bool result;

	std::unique_ptr<Shader> shader = std::make_unique<SimpleShader>();
	result = shader->Initialize(m_Device, m_hWnd, L"bin/shaders/VertexShader.cso", L"bin/shaders/PixelShader.cso");
	if (!result) {
		MessageBox(m_hWnd, TEXT("Could not initialize the normal-as-color shader object."), TEXT("Error"), MB_OK);
		return false;
	}
	m_Shaders.insert({ "Normal To Color", std::move(shader) });

	shader = std::make_unique<PhongShader>();
	result = shader->Initialize(m_Device, m_hWnd, L"bin/shaders/PhongVertexShader.cso", L"bin/shaders/PhongPixelShader.cso");
	if (!result) {
		MessageBox(m_hWnd, TEXT("Could not initialize the Phong Lighting shader object."), TEXT("Error"), MB_OK);
		return false;
	}
	m_Shaders.insert({ "Phong Lighting", std::move(shader) });

	return true;
}

void Scene::InitializeTextures() {
	std::unique_ptr<Texture> texture = std::make_unique<Texture>(m_Device, m_DeviceContext,
		L"textures/thread_texture.jpg", m_hWnd);
	m_Textures.insert({ "Thread", std::move(texture) });

	texture = std::make_unique<Texture>(m_Device, m_DeviceContext,
		L"textures/emerald_texture.jpg", m_hWnd);
	m_Textures.insert({ "Emerald", std::move(texture) });
}

void Scene::InitializeMaterials() {
	std::unique_ptr<Material> material = std::make_unique<NormalAsColorMaterial>(m_Shaders["Normal To Color"].get());
	m_Materials.insert({ "Normal Color", std::move(material) });

	material = std::make_unique<PhongMaterial>(m_Device,
		m_Shaders["Phong Lighting"].get(),
		m_Textures["Emerald"].get(),
		PhongProperties::RedRubber);
	m_Materials.insert({ "Emerald Red Rubber", std::move(material) });

	material = std::make_unique<PhongMaterial>(m_Device,
		m_Shaders["Phong Lighting"].get(),
		m_Textures["Emerald"].get(),
		PhongProperties::GreenPlastic);
	m_Materials.insert({ "Emerald Plastic", std::move(material) });

	material = std::make_unique<PhongMaterial>(m_Device,
		m_Shaders["Phong Lighting"].get(),
		m_Textures["Thread"].get(),
		PhongProperties::DefaultMaterial);
	m_Materials.insert({ "Lit Thread", std::move(material) });
}

bool Scene::InitializeModels() {
	bool result;
	std::unique_ptr<Model> model;

	model = std::make_unique<Model>();
	result = model->Initialize(m_Device,
		"models/teapot2.obj",
		m_Materials["Emerald Plastic"].get());
	if (!result) {
		MessageBox(m_hWnd, TEXT("Could not initialize the teapot model."), TEXT("Error"), MB_OK);
		return false;
	}
	m_Models.insert({ "Teapot", std::move(model) });

	model = std::make_unique<Model>();
	result = model->Initialize(m_Device,
		"models/racing_car.obj",
		m_Materials["Emerald Red Rubber"].get());
	if (!result) {
		MessageBox(m_hWnd, TEXT("Could not initialize the car model."), TEXT("Error"), MB_OK);
		return false;
	}
	m_Models.insert({ "Car", std::move(model) });

	model = std::make_unique<Model>();
	result = model->Initialize(m_Device,
		"models/primitives/sphere.obj",
		m_Materials["Normal Color"].get());
	if (!result) {
		MessageBox(m_hWnd, TEXT("Could not initialize the sphere model."), TEXT("Error"), MB_OK);
		return false;
	}
	m_Models.insert({ "Sphere", std::move(model) });

	model = std::make_unique<Model>();
	result = model->Initialize(m_Device,
		"models/primitives/plane.obj",
		m_Materials["Lit Thread"].get());
	if (!result) {
		MessageBox(m_hWnd, TEXT("Could not initialize the plane model."), TEXT("Error"), MB_OK);
		return false;
	}
	m_Models.insert({ "Plane", std::move(model) });

	return true;
}

void Scene::InitializeScene(int screenWidth, int screenHeight) {
	std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	camera->GenerateProjectionMatrices(screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	m_MainCamera = camera.get();

	// Change initial data here
	m_MainCamera->transform.position = Vector3(0.0f, 0.0f, -5.0f);

	m_SceneRoot = std::make_unique<SceneNode>();

	std::unique_ptr<SceneNode> node1 = std::make_unique<SceneNode>(m_SceneRoot.get(), m_Models["Teapot"].get());
	std::unique_ptr<SceneNode> node2 = std::make_unique<SceneNode>(node1.get(), m_Models["Car"].get());
	node2->transform.position = Vector3(25.0f, 0.0f, -4.0f);
	//node2->transform.UpdateGlobalMatrix();
	std::unique_ptr<SceneNode> node3 = std::make_unique<SceneNode>(node2.get(), m_Models["Teapot"].get());
	node3->transform.position = Vector3(30.0f, 0.0f, 0.0f);
	node2->AddChild(std::move(node3));
	node1->AddChild(std::move(node2));



	std::unique_ptr<Light> light = std::make_unique<Light>(DirectX::Colors::AliceBlue.v, Vector3(1.0f, 0.2f, 0.0f),
		true, camera.get(), nullptr);
	// light->transform.position = Vector3(2.0f, 5.0f, 1.0f);
	//light->transform.scale = Vector3(0.1f);
	m_Lights.insert({ "Light1", light.get() });

	camera->AddChild(std::move(light));
	m_SceneRoot->AddChild(std::move(node1));

	light = std::make_unique<Light>(DirectX::Colors::LightBlue.v, Vector3(1.0f, 0.2f, 0.1f),
		true, m_SceneRoot.get(), m_Models["Sphere"].get());
	light->transform.position = Vector3(-2.0f, 4.0f, -1.0f);
	light->transform.scale = Vector3(0.3f);
	m_Lights.insert({ "Light2", light.get() });

	m_SceneRoot->AddChild(std::move(light));

	std::unique_ptr<SceneNode> ground = std::make_unique<SceneNode>(m_SceneRoot.get(), m_Models["Plane"].get());
	ground->transform.scale *= Vector3(50.0f, 1.0f, 50.0f);
	m_SceneRoot->AddChild(std::move(ground));

	//camera->SetModel(m_Models[0].get());
	m_SceneRoot->AddChild(std::move(camera));
}

void Scene::Shutdown() {
	for (auto& shader : m_Shaders) {
		if (shader.second) {
			shader.second->Shutdown();
		}
	}

	for (auto& model : m_Models) {
		if (model.second) {
			model.second->Shutdown();
		}
	}
}

void Scene::Update(float deltaTime) {
	//m_MainCamera->Render(deltaTime);
	m_MainCamera->GenerateViewMatrix();
	m_MainCamera->UpdateTransform();

	//m_Models[1]->transform.position += Vector3(-25.0f, 0.0f, 4.0f);
	//m_SceneRoot->children[0]->children[0]->children[0]->transform.rotation += Vector3::Up * DirectX::XMConvertToRadians(30.0f) * deltaTime;
	//m_SceneRoot->children[0]->children[0]->children[0]->UpdateTransform();
	//m_SceneRoot->children[0]->children[0]->transform.rotation += Vector3::Up * DirectX::XMConvertToRadians(15.0f) * deltaTime;
	//m_SceneRoot->children[0]->children[0]->UpdateTransform();
	//m_Models[1]->transform.position += Vector3(25.0f, 0.0f, -4.0f);
	m_SceneRoot->children[0]->transform.rotation += Vector3::Up * DirectX::XMConvertToRadians(15.0f) * deltaTime;
	//m_SceneRoot->children[0]->transform.scale = Vector3::One * 0.1f;
	//m_SceneRoot->children[0]->UpdateTransform();
	m_SceneRoot->UpdateTransform();

	m_ShaderPayload.matrices.view = m_MainCamera->GetViewMatrix();
	m_ShaderPayload.matrices.projection = m_MainCamera->GetProjectionMatrix();
	m_ShaderPayload.viewProjectionMatrix = m_ShaderPayload.matrices.view * m_ShaderPayload.matrices.projection;

	m_ShaderPayload.lightProperties.eyePosition = Vector4(m_MainCamera->transform.position);
	m_ShaderPayload.lightProperties.globalAmbient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	m_ShaderPayload.lightProperties.specularType = false;
	int i = 0;
	for (auto light : m_Lights) {
		if (i >= 8)
			break;
		m_ShaderPayload.lightProperties.lights[i] = light.second->GetStruct();
		i++;
	}
}

bool Scene::Render() {
	if (!m_SceneRoot->Render(m_DeviceContext, &m_ShaderPayload)) {
		return false;
	}
	return true;
}

void Scene::HandleResize(int screenWidth, int screenHeight) {
	m_MainCamera->GenerateProjectionMatrices(screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
}

Camera* Scene::GetMainCamera() {
	return m_MainCamera;
}
