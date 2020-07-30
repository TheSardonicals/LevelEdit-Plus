#include "editor_menu.h"

EditorMenu::EditorMenu(int * width, int * height, ImVec4 * clear_color){
    window_width = width;
    window_height = height;
    this->clear_color = clear_color;
}

void EditorMenu::Process(){
    // menu bar with options and such
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
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Stats and information/options.
    if (!hide_stats){
        if (ImGui::Begin("Stats")){
            ImGui::ColorEdit3("clear color", (float*)clear_color); // Edit 3 floats representing a color
            ImGui::NewLine();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);  
        }
        ImGui::End();
    }

    // menu for selecting tiles and saving, just like old times.
    if (!hide_lower_options){
        ImGui::SetNextWindowPos(ImVec2(*window_width - 265, *window_height - 56));
        if (ImGui::Begin("Option Menu", NULL, ImGuiWindowFlags_NoTitleBar||ImGuiWindowFlags_NoResize)){
            if (ImGui::Button("Save File", ImVec2(120, 40))){
                // Save .mx file contents.
            }
            ImGui::SameLine();

            if (ImGui::Button("Assets", ImVec2(120, 40))){
                if (ImGui::BeginPopup("Tile Selection Menu")){
                    // Create Game Tile buttons and handle what happens when the buttons are clicked, etc... 
                    ImGui::EndPopup();
                }    
            }
        }
        ImGui::End();
    }
}