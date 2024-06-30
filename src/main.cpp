#include "NWengineCore.h"	

#include <windows.h>
//NWin is statically linked to NWengine, you only need the headers
#include "nwin/window.h"

#include "imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "NWengineCore.h"	


//As NWengine uses directly windows api, we just need to call imgui callback to handle interactions
LRESULT CALLBACK imguiProcCallback (HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wparam, lparam)) {
        return 1;
    }
    return NWin::defaultWinProc(hwnd, umsg, wparam, lparam);
};

void Init() {
    //Configuration for imgui
    NWin::Window* win = (NWin::Window*)Context::window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_InitForOpenGL((HWND)win->_getHandle());
    ImGui_ImplOpenGL3_Init();

    win->setWndProcCallback((void*)&imguiProcCallback);

    //---------------------------------------------------
    Scene& s = Scene::CreateNew("Scene0");
    s.MakeCurrent();
    GameObject& obj = s.AddObject(); 
    obj.AddComponent<Transform>();
    Sprite& spr = *obj.AddComponent<Sprite>();
    spr.SetShader(ShaderTexturedDefaultStr, &ShaderTexturedDefaultID); spr.SetTexture(&ImageDefault, &TextureDefaultID);
    obj.AddComponent<Camera>()->Use();
    obj.GetComponent<Camera>()->ChangeOrtho(Context::WINDOW_WIDTH, Context::WINDOW_HEIGHT);
}

void UIBegin() {
     ImGui_ImplOpenGL3_NewFrame();
     ImGui_ImplWin32_NewFrame();
     ImGui::NewFrame();
}

void UIRender() {   
    Transform& t = *Scene::GetCurrent()->sceneObjs.front().GetComponent<Transform>();
    ImGui::Begin("Transform Editor");
    ImGui::SliderFloat2("Position:", &t.position.x, -2000, 2000);
    ImGui::SliderFloat2("Scale:", &t.scale.x, -20, 20);
    ImGui::SliderFloat("Rotation:", &t.rotation, 0, 360);
    ImGui::End();
    ImGui::ShowDemoWindow();
    ImGui::Render();
}

void UIEnd() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 
}

void Render() {
    (*Renderer::currentRenderer)(true); 
}


int main() {

	NWenginePushFunction(ON_MAIN_CALL_LOCATION::InitEnd, Init);
    NWenginePushFunction(ON_MAIN_CALL_LOCATION::FrameBegin, UIBegin);
    //Order matters 
    NWenginePushFunction(ON_MAIN_CALL_LOCATION::FrameIntermediate, UIRender);
    NWenginePushFunction(ON_MAIN_CALL_LOCATION::FrameIntermediate, Render);
    NWenginePushFunction(ON_MAIN_CALL_LOCATION::FrameIntermediate, UIEnd);

	NWengineInit();
	NWengineLoop();
	NWengineShutdown();
    
	return 0;
}

