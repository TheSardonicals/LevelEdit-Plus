#include "editor_menu.h"

EditorMenu::EditorMenu(int * width, int * height, ImVec4 * clear_color){
    window_width = width;
    window_height = height;
    this->clear_color = clear_color;
}

void EditorMenu::Process(){
    // Menu Bar with options and such
    if (ImGui::BeginMainMenuBar()){
        if (ImGui::BeginMenu("File")){
            if (ImGui::MenuItem("Save File", NULL, false)){
                //Save .mx file contents.
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options")){
            ImGui::Checkbox("Hide Menu", &hide_lower_options);
            ImGui::Checkbox("Hide Stats", &hide_stats);
            ImGui::Checkbox("Align Menu to bottom corner", &align_menu_to_screen);
            ImGui::SliderFloat("Alpha", &alpha, 0.0f, 1.0f, "Alpha = %.3f");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Stats and information.
    if (!hide_stats){
        ImGui::SetNextWindowBgAlpha(alpha);
        if (ImGui::Begin("Stats")){
            ImGui::ColorEdit3("clear color", (float*)clear_color); // Edit 3 floats representing a color
            ImGui::NewLine();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);  
        }
        ImGui::End();
    }

    // Menu for selecting tiles and saving, just like old times.
    if (!hide_lower_options){
        if (align_menu_to_screen){
            ImGui::SetNextWindowPos(ImVec2(*window_width - 265, *window_height - 55));
        }
        ImGui::SetNextWindowBgAlpha(alpha);
        if (ImGui::Begin("Option Menu", NULL, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize)){
            if (ImGui::Button("Save File", ImVec2(120, 40))){
                // Save .mx file contents.
            }
            ImGui::SameLine();

            if (ImGui::Button("Assets", ImVec2(120, 40))){
                ImGui::SetNextWindowBgAlpha(max(alpha-.4f, .25f));
                if (ImGui::BeginPopup("Tile Selection Menu")){
                    // Create Game Tile buttons and handle what happens when the buttons are clicked, etc... 
                    ImGui::EndPopup();
                }    
            }
        }
        ImGui::End();
    }
}