#include "myimgui.h"

void MyGui::ShowWindow(bool* p_open){
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

    if(!ImGui::Begin("LevelEdit-Plus", p_open)){
        ImGui::End();
        return;
    }

    ImGui::Text("LevelEdit-Plus");
    ImGui::Spacing();

    if(ImGui::CollapsingHeader("Help"))
    {
        ImGui::Text("ABOUT THIS DEMO:");
        ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
        ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
        ImGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
                          "and Metrics (general purpose Dear ImGui debugging tool).");
        ImGui::Separator();

        ImGui::Text("PROGRAMMER GUIDE:");
        ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
        ImGui::BulletText("See comments in imgui.cpp.");
        ImGui::BulletText("See example applications in the examples/ folder.");
        ImGui::BulletText("Read the FAQ at http://www.dearimgui.org/faq/");
        ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
        ImGui::Separator();

        ImGui::Text("USER GUIDE:");
        ImGui::ShowUserGuide();
    }
    

    ImGui::End();

}
void MyGui::ShowAppDocuments(bool* p_open){

}
void MyGui::ShowAppMainMenuBar(){

}
void MyGui::ShowAppConsole(bool* p_open){

}
void MyGui::ShowAppLog(bool* p_open){

}
void MyGui::ShowAppLayout(bool*  p_open){

}
void MyGui::ShowAppPropertyEditor(bool* p_open){

}
void MyGui::ShowAppLongText(bool* p_open){

}
void MyGui::ShowAppAutoResize(bool* p_open){

}
void MyGui::ShowAppConstrainedResize(bool* p_open){

}
void MyGui::ShowAppSimpleOverlay(bool* p_open){

}
void MyGui::ShowAppWindowTitles(bool* p_open){

}
void ShowAppCustomRendering(bool* p_open){

}
void ShowMenuFile(){

}