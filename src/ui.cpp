#include "app.h"

void ShadeApp::initUI() {
	ImGui_ImplGlfwGL3_Init(_window, true);
}

void ShadeApp::drawUI() {
	ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("File")) {
        if(ImGui::MenuItem("Reload", "CTRL+R")) {
            loadFragmentShader(_currentShaderFile);
        }
        ImGui::EndMenu();
    }
    
    ImGui::EndMainMenuBar();

    // Framerate overlay
    ImGui::SetNextWindowPos(ImVec2(10,30));
    if (!ImGui::Begin("", &_showFramerate, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::End();
}
