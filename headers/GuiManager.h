#ifndef _GUI_MANAGER_H_
#define _GUI_MANAGER_H_

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

#include "Helpers.h"

#include "SceneNode.h"
#include "Camera.h"
#include "Light.h"

class GuiManager {
public:
    GuiManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hWnd) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;
        //io.ConfigViewportsNoDefaultParent = true;
        //io.ConfigDockingAlwaysTabBar = true;
        //io.ConfigDockingTransparentPayload = true;
        //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
        //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplDX11_Init(device, deviceContext);

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);

        selectedNode = nullptr;
    }

    ~GuiManager() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void Update(const SceneNode* node) {
        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);

        //// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        //{
        //    static float f = 0.0f;
        //    static int counter = 0;

        //    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        //    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        //    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //    ImGui::Checkbox("Another Window", &show_another_window);

        //    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        //    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //        counter++;
        //    ImGui::SameLine();
        //    ImGui::Text("counter = %d", counter);

        //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //    ImGui::End();
        //}

        //// 3. Show another simple window.
        //if (show_another_window)
        //{
        //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //    ImGui::Text("Hello from another window!");
        //    if (ImGui::Button("Close Me"))
        //        show_another_window = false;
        //    ImGui::End();
        //}

        {
            ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
            if (!ImGui::Begin("Scene Hierarchy", &hierarchy_pane)) {
                ImGui::End();
                return;
            }
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
            //ImGui::BeginChild("split", ImVec2(150, 0), true);//ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
            //{
                // Iterate placeholder objects (all the same data)
                //for (int obj_i = 0; obj_i < 4; obj_i++)
                //{
                //    ShowPlaceholderObject("Object", obj_i);
                //    //ImGui::Separator();
                //}
                //ImGui::EndChild();
            //}
            //ImGui::EndChild();

            for (const auto& child : node->children) {
                ShowNode(child.get(), flags);
            }

            ImGui::PopStyleVar();
            ImGui::End();
        }

        if (selectedNode) {
            if (!ImGui::Begin("Node Properties", &node_pane)) {
                ImGui::End();
                return;
            }
            
            ImGui::IsItemEdited();

            float v[3] = { selectedNode->transform.position.x, selectedNode->transform.position.y, selectedNode->transform.position.z };
            if (ImGui::InputFloat3("Position", v)) {
                selectedNode->transform.position = Vector3(v[0], v[1], v[2]);
            }

            v[0] = selectedNode->transform.rotation.x;
            v[1] = selectedNode->transform.rotation.y;
            v[2] = selectedNode->transform.rotation.z;
            if (ImGui::InputFloat3("Rotation", v)) {
                selectedNode->transform.rotation = Vector3(v[0], v[1], v[2]);
            }

            v[0] = selectedNode->transform.scale.x;
            v[1] = selectedNode->transform.scale.y;
            v[2] = selectedNode->transform.scale.z;
            if (ImGui::InputFloat3("Scale", v)) {
                selectedNode->transform.scale = Vector3(v[0], v[1], v[2]);
            }

            if (selectedNode->GetType() == "camera") {
                Camera* camera = dynamic_cast<Camera*>(selectedNode);
                float v = camera->m_FieldOfView;
                if (ImGui::InputFloat("Field of view", &v)) {
                    camera->m_FieldOfView = v;
                }
            }

            if (selectedNode->GetType() == "light") {
                Light* light = dynamic_cast<Light*>(selectedNode);

                float c[3] = { light->m_Color.x, light->m_Color.y, light->m_Color.z };
                if (ImGui::ColorEdit3("Light color", c)) {
                    light->m_Color = Color(c);
                }

                c[0] = light->m_AttenuationCoef.x;
                c[1] = light->m_AttenuationCoef.y;
                c[2] = light->m_AttenuationCoef.z;
                if (ImGui::InputFloat3("Attenuation", c)) {
                    light->m_AttenuationCoef = Vector3(c);
                }

                bool b = light->m_Enabled;
                if (ImGui::Checkbox("Enabled", &b)) {
                    light->m_Enabled = b;
                }
            }

            if (selectedNode->m_Model)
                ImGui::Text("Model: %s", selectedNode->m_Model->name.c_str());
            else
                ImGui::Text("No model");
            ImGui::End();


        }
    }

    void ShowNode(SceneNode* node, ImGuiTreeNodeFlags flags) {
        ImGui::PushID(node);

        ImGuiTreeNodeFlags localFlags = flags;
        if (selectedNode == node) {
            localFlags |= ImGuiTreeNodeFlags_Selected;
        }
        if (node->children.size() == 0) {
            localFlags |= ImGuiTreeNodeFlags_Leaf;
        }
        bool node_open = ImGui::TreeNodeEx("Node", localFlags, "%s", node->name.c_str());

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            selectedNode = node;
        }

        if (node_open) {
            int i = 0;
            for (const auto& child : node->children) {
                ImGui::PushID(i);
                ShowNode(child.get(), flags);
                i++;
                ImGui::PopID();
            }

            ImGui::TreePop();
        }
        ImGui::PopID();
        /*char label[128];
        sprintf(label, "MyObject %d", i);
        if (ImGui::Selectable(label, selected == i))
            selected = i;*/
    }

    void Render() {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

private:
    bool hierarchy_pane = true;
    bool node_pane = true;
    SceneNode* selectedNode;
};

#endif // !_GUI_MANAGER_H_