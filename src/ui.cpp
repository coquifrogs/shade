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
}
