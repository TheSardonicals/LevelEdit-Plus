#include "editor_menu.h"

EditorMenu::EditorMenu(int * width, int * height, ImVec4 * clear_color, Pointer * mouse, map<string, string> * paths, TextureCache * cache){
    window_width = width;
    window_height = height;
    this->clear_color = clear_color;
    this->mouse = mouse;
    // We do this because we have to use indexing for auto resizing buttons on the menu.
    for (auto tile: *paths){
        vector<string> path = {tile.first, tile.second};
        tile_paths.push_back(path);
    }
    this->cache = cache;
    original_button_color = ImGui::GetStyle().Colors[ImGuiCol_Button];
}

void EditorMenu::Process(GameTile * &ghost_tile, map<string, vector<GameTile *>> * &tile_cache){
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

        if (ImGui::BeginMenu("Help")){
            ImGui::Checkbox("About", &about_window);
            ImGui::Checkbox("User Instructions", &instruction_manual);
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
    
    ImVec2 lower_menu_pos = ImVec2(*window_width - 265, *window_height - 55);
    // Menu for selecting tiles and saving, just like old times.
    if (!hide_lower_options){
        
        if (align_menu_to_screen){
            ImGui::SetNextWindowPos(lower_menu_pos);
        }
        ImGui::SetNextWindowBgAlpha(alpha);
        if (ImGui::Begin("Option Menu", NULL, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize)){
            if (ImGui::Button("Save File", ImVec2(120, 40))){
                // Save .mx file contents.
            }
            ImGui::SameLine();

            // Change button color if it's been clicked, else use original button color.
            if (show_item_menu){ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.46f, 0.47f, 0.48f, 1.00f));}
            else {ImGui::PushStyleColor(ImGuiCol_Button, original_button_color);}

            if (ImGui::Button("Assets", ImVec2(120, 40))){
                if (show_item_menu == false)
                    show_item_menu = true;
                else
                    show_item_menu = false;
                ImGui::SetNextWindowPos(ImVec2(lower_menu_pos.x - 45, lower_menu_pos.y - 400)); 
                ImGui::SetNextWindowSize(ImVec2(310, 400));  
            }

            ImGui::PopStyleColor(1);
        }
        ImGui::End();
    }
    
    if (show_item_menu){
        ImGui::SetNextWindowBgAlpha(max(alpha-.4f, .1f));
        ImVec2 button_size = ImVec2(52.0f, 52.0f);
        if (ImGui::Begin("Asset Menu", NULL)){
            SDL_Texture * texture;
            int button_count = tile_paths.size();
            ImGuiStyle& style = ImGui::GetStyle();
            float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
            for (int i = 0; i < button_count; i++){
                texture = cache->LoadTexture(tile_paths[i][1]);
                cache->SetTextureAlpha(texture, max(alpha, .5f));
                 ImGui::PushID(i);
                if (ImGui::ImageButton((void *)texture, button_size, ImVec2(0.0f, 0.0f), ImVec2(32.0f, 32), 1, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)))
                {   
                    // Handle setting ghost tile when button is clicked
                    if (ghost_tile == nullptr){
                        ghost_tile = new GameTile(cache, tile_paths[i][1], mouse->xpos, mouse->ypos, 32, 32);
                    }
                    else {
                        delete ghost_tile;
                        ghost_tile = new GameTile(cache, tile_paths[i][1], mouse->xpos, mouse->ypos, 32, 32);
                    }
                }
                if (ImGui::IsItemHovered()){
                    ImGui::BeginTooltip();
                    ImGui::Text(tile_paths[i][0].c_str());
                    ImGui::EndTooltip();
                }
                // This is for auto layout resizing, solution provided by imgui_demo.
                float last_button_x2 = ImGui::GetItemRectMax().x;
                float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_size.x; // Expected position if next button was on same line
                if (i + 1 < button_count && next_button_x2 < window_visible_x2)
                    ImGui::SameLine();
                ImGui::PopID();
            }   
        }
        ImGui::End();
    }

    if (about_window){
        if (ImGui::Begin("About LevelEditor++", NULL)){

        }
        ImGui::End();
    }

    if (instruction_manual){
        if (ImGui::Begin("Instruction Manual", NULL)){

        }
        ImGui::End();
    }

    if (ghost_tile){
        selected_tile = ghost_tile;
        if (ImGui::Begin("Texture Properties", NULL)){
            string x_string = "X: " + to_string(selected_tile->x);
            string y_string = "Y: " + to_string(selected_tile->y);
            string w_string = "width: " + to_string(selected_tile->w);
            string h_string = "height: " + to_string(selected_tile->h);
            ImGui::Text(x_string.c_str());
            ImGui::Text(y_string.c_str());
            ImGui::Text(w_string.c_str());
            ImGui::Text(h_string.c_str());
        }
        ImGui::End();
    }

    if (selected_tile){
        if (mouse->HasClicked(NULL)){
            if(tile_cache->count(selected_tile->name) == 0){
                vector<GameTile *> tile = {selected_tile};
                block_cache[selected_tile->name] = tile;
            }else{
                block_cache[selected_tile->name].push_back(selected_tile);
            }
        }
        tile_cache = &block_cache;
    }
}


