#include "editor_menu.h"

// Panels in the docked layout own their slot on screen, so the user cannot drag them out
// of the layout or fold them away by accident.
static const ImGuiWindowFlags PANEL_FLAGS = ImGuiWindowFlags_NoMove
                                          | ImGuiWindowFlags_NoResize
                                          | ImGuiWindowFlags_NoCollapse
                                          | ImGuiWindowFlags_NoBringToFrontOnFocus;

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

void EditorMenu::Process(GameTile * &ghost_tile, Camera * camera, map<string, vector<GameTile *>> & tile_cache, GameTile * & selected_tile){

    MainMenuBar(ghost_tile, selected_tile);

    if (unity_layout){
        // Work out what each edge takes before any panel is drawn, so they all agree on
        // where the leftover scene area is. WorkPos/WorkSize already exclude the menu bar.
        const ImGuiViewport * viewport = ImGui::GetMainViewport();
        ImGuiStyle & style = ImGui::GetStyle();

        toolbar_height = ImGui::GetFrameHeight() + (style.WindowPadding.y * 2.0f);
        status_height = ImGui::GetTextLineHeight() + (style.WindowPadding.y * 2.0f);

        scene_pos = ImVec2(viewport->WorkPos.x + hierarchy_width, viewport->WorkPos.y + toolbar_height);
        scene_size = ImVec2(max(viewport->WorkSize.x - hierarchy_width - inspector_width, 1.0f),
                            max(viewport->WorkSize.y - toolbar_height - status_height - project_height, 1.0f));

        Toolbar(ghost_tile, selected_tile);
        HierarchyPanel(tile_cache, selected_tile);
        InspectorPanel(selected_tile, ghost_tile, camera);
        ProjectPanel(ghost_tile);
        StatusBar(tile_cache, selected_tile);
        SceneOutline();
    }
    else {
        ClassicMenus(ghost_tile, camera, selected_tile);
    }

    Dialogs();
}

void EditorMenu::ClearGhostTile(GameTile * & ghost_tile){
    // Deleting a null pointer is a no-op, so this is safe to call whether or not a
    // brush is currently held.
    delete ghost_tile;
    ghost_tile = NULL;
}

void EditorMenu::MainMenuBar(GameTile * & ghost_tile, GameTile * & selected_tile){
    if (ImGui::BeginMainMenuBar()){
        if (ImGui::BeginMenu("File")){
            ImGui::Checkbox("Save File", &saving_to_mx);
            ImGui::Checkbox("Save Project", &saving_mxpr);
            ImGui::Checkbox("Load Project", &loading_project);
            ImGui::Checkbox("Load Tileset", &loading_tileset);
            ImGui::EndMenu();
        }

        // Everything that acts on the current selection lives here, like the Unity Edit menu.
        if (ImGui::BeginMenu("Edit")){
            if (ImGui::MenuItem("Delete Tile", "Del", false, selected_tile != NULL)){
                delete_selected_tile = true;
            }
            if (ImGui::MenuItem("Deselect", "X", false, selected_tile != NULL || ghost_tile != NULL)){
                // Dropping the selection here is enough: the selection pass in the editor
                // clears the leftover highlight on the next frame.
                selected_tile = NULL;
                tile_edit_mode = false;
                ClearGhostTile(ghost_tile);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options")){
            ImGui::Checkbox("Unity Layout", &unity_layout);
            if (!unity_layout){
                ImGui::Checkbox("Hide Menu", &hide_lower_options);
                ImGui::Checkbox("Hide Stats", &hide_stats);
                ImGui::Checkbox("Align Menu to bottom corner", &align_menu_to_screen);
            }
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
}

void EditorMenu::Toolbar(GameTile * & ghost_tile, GameTile * & selected_tile){
    const ImGuiViewport * viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, toolbar_height));
    ImGui::SetNextWindowBgAlpha(alpha);

    if (ImGui::Begin("##Toolbar", NULL, PANEL_FLAGS | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)){

        // The pointer tool is just "no brush held", which is the state the X key puts us in.
        if (ghost_tile == NULL){ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.46f, 0.47f, 0.48f, 1.00f));}
        else {ImGui::PushStyleColor(ImGuiCol_Button, original_button_color);}
        if (ImGui::Button("Select")){
            ClearGhostTile(ghost_tile);
        }
        ImGui::PopStyleColor(1);
        if (ImGui::IsItemHovered()){ImGui::SetTooltip("Put the brush down so tiles can be picked (X)");}

        ImGui::SameLine();
        if (ghost_tile != NULL){ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.46f, 0.47f, 0.48f, 1.00f));}
        else {ImGui::PushStyleColor(ImGuiCol_Button, original_button_color);}
        ImGui::Button("Place");
        ImGui::PopStyleColor(1);
        if (ImGui::IsItemHovered()){ImGui::SetTooltip("Pick a tile in the Project panel to start placing");}

        ImGui::SameLine();
        ImGui::TextUnformatted("|");
        ImGui::SameLine();

        // Delete stays greyed out until something is actually selected, so it can never
        // be pressed against a stale pointer.
        ImGui::BeginDisabled(selected_tile == NULL);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.16f, 0.16f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.22f, 0.22f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.12f, 0.12f, 1.00f));
        if (ImGui::Button("Delete")){
            delete_selected_tile = true;
        }
        ImGui::PopStyleColor(3);
        ImGui::EndDisabled();
        if (ImGui::IsItemHovered()){ImGui::SetTooltip("Remove the selected tile from the level (Del)");}

        ImGui::SameLine();
        ImGui::BeginDisabled(selected_tile == NULL && ghost_tile == NULL);
        if (ImGui::Button("Deselect")){
            selected_tile = NULL;
            tile_edit_mode = false;
            ClearGhostTile(ghost_tile);
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::TextUnformatted("|");
        ImGui::SameLine();
        if (ImGui::Button("Save File")){
            // Save .mx file contents.
            saving_to_mx = true;
        }
    }
    ImGui::End();
}

void EditorMenu::HierarchyPanel(map<string, vector<GameTile *>> & tile_cache, GameTile * & selected_tile){
    const ImGuiViewport * viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + toolbar_height));
    ImGui::SetNextWindowSize(ImVec2(hierarchy_width, scene_size.y));
    ImGui::SetNextWindowBgAlpha(alpha);

    if (ImGui::Begin("Hierarchy", NULL, PANEL_FLAGS)){
        int tile_count = 0;
        for (auto & entry : tile_cache){
            tile_count += entry.second.size();
        }
        ImGui::Text("Level (%d tiles)", tile_count);
        ImGui::Separator();

        if (tile_cache.empty()){
            ImGui::TextDisabled("No tiles placed yet.");
        }

        for (auto & entry : tile_cache){
            string header = entry.first + " (" + to_string(entry.second.size()) + ")";
            if (ImGui::TreeNodeEx(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen)){
                for (auto & tile : entry.second){
                    // The pointer is the identity here, so rows stay correct even when two
                    // tiles of the same type sit on the same spot.
                    ImGui::PushID(tile);
                    // The tree node above already names the tile type, so the row only has
                    // to carry the position. Keeps rows readable at the panel width.
                    string label = "[" + to_string(tile->x) + ", " + to_string(tile->y) + "]";

                    if (ImGui::Selectable(label.c_str(), tile == selected_tile)){
                        selected_tile = tile;
                        tile_edit_mode = true;
                    }

                    // Right clicking a row selects it first, so the delete always lands on
                    // the row that was clicked rather than whatever was selected before.
                    if (ImGui::BeginPopupContextItem()){
                        selected_tile = tile;
                        tile_edit_mode = true;
                        if (ImGui::MenuItem("Delete Tile", "Del")){
                            delete_selected_tile = true;
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}

void EditorMenu::InspectorPanel(GameTile * & selected_tile, GameTile * ghost_tile, Camera * camera){
    const ImGuiViewport * viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - inspector_width, viewport->WorkPos.y + toolbar_height));
    ImGui::SetNextWindowSize(ImVec2(inspector_width, viewport->WorkSize.y - toolbar_height - status_height));
    ImGui::SetNextWindowBgAlpha(alpha);

    if (ImGui::Begin("Inspector", NULL, PANEL_FLAGS)){

        if (selected_tile){
            ImGui::SeparatorText("Selected Tile");
            ImGui::Text("Name: %s", selected_tile->name.c_str());
            ImGui::Text("Screen: %d, %d", static_cast<int>(selected_tile->x + camera->xpos), static_cast<int>(selected_tile->y + camera->ypos));
            ImGui::Spacing();

            ImGui::PushID("selected_transform");
            x_delta = selected_tile->x;
            y_delta = selected_tile->y;
            w_increase = selected_tile->w;
            h_increase = selected_tile->h;

            ImGui::InputInt("X", &x_delta);
            ImGui::InputInt("Y", &y_delta);
            ImGui::InputInt("Width", &w_increase);
            ImGui::InputInt("Height", &h_increase);

            selected_tile->x = x_delta;
            selected_tile->y = y_delta;
            selected_tile->w = w_increase;
            selected_tile->h = h_increase;
            ImGui::PopID();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // The editor owns the tiles, so the button only raises a flag and lets
            // Editor::DeleteSelectedTile() free it and drop the selection.
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.16f, 0.16f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.22f, 0.22f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.12f, 0.12f, 1.00f));
            if (ImGui::Button("Delete Tile", ImVec2(-1.0f, 32.0f))){
                delete_selected_tile = true;
            }
            ImGui::PopStyleColor(3);

            if (ImGui::Button("Deselect", ImVec2(-1.0f, 0.0f))){
                selected_tile = NULL;
                tile_edit_mode = false;
            }
        }
        else {
            ImGui::TextDisabled("Nothing selected.");
            ImGui::TextWrapped("Put the brush down (X), then click a tile in the scene or a row in the Hierarchy to select it.");
        }

        if (ghost_tile){
            ImGui::Spacing();
            ImGui::SeparatorText("Brush");
            ImGui::Text("Name: %s", ghost_tile->name.c_str());
            ImGui::Text("X: %d", static_cast<int>(ghost_tile->x - camera->xpos));
            ImGui::Text("Y: %d", static_cast<int>(ghost_tile->y - camera->ypos));

            // Only the size is editable: the brush follows the mouse, so editing its
            // position here would just be overwritten on the next frame.
            ImGui::PushID("brush_transform");
            w_increase = ghost_tile->w;
            h_increase = ghost_tile->h;
            ImGui::InputInt("Width", &w_increase);
            ImGui::InputInt("Height", &h_increase);
            ghost_tile->w = w_increase;
            ghost_tile->h = h_increase;
            ImGui::PopID();
        }

        if (!selected_tile && !ghost_tile){
            ImGui::Spacing();
            ImGui::SeparatorText("Scene");
            // NoInputs keeps this to a swatch plus its label: the three number boxes do
            // not fit the inspector column and push the label off the panel.
            ImGui::ColorEdit3("Clear Color", (float*)clear_color, ImGuiColorEditFlags_NoInputs);
        }
    }
    ImGui::End();
}

void EditorMenu::ProjectPanel(GameTile * & ghost_tile){
    const ImGuiViewport * viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + toolbar_height + scene_size.y));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x - inspector_width, project_height));
    ImGui::SetNextWindowBgAlpha(alpha);

    if (ImGui::Begin("Project", NULL, PANEL_FLAGS)){
        AssetBrowser(ghost_tile, ImVec2(52.0f, 52.0f));
    }
    ImGui::End();
}

void EditorMenu::StatusBar(map<string, vector<GameTile *>> & tile_cache, GameTile * selected_tile){
    const ImGuiViewport * viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - status_height));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, status_height));
    ImGui::SetNextWindowBgAlpha(alpha);

    if (ImGui::Begin("##StatusBar", NULL, PANEL_FLAGS | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)){
        int tile_count = 0;
        for (auto & entry : tile_cache){
            tile_count += entry.second.size();
        }

        ImGui::Text("%.1f FPS   |   Tiles: %d   |   Selected: %s   |   Del: delete selected tile    X: put the brush down",
                    ImGui::GetIO().Framerate,
                    tile_count,
                    selected_tile ? selected_tile->name.c_str() : "none");
    }
    ImGui::End();
}

void EditorMenu::SceneOutline(){
    // A border around the area the panels left free, so the editable region reads as a
    // scene view rather than as empty background.
    ImGui::GetBackgroundDrawList()->AddRect(scene_pos,
                                            ImVec2(scene_pos.x + scene_size.x, scene_pos.y + scene_size.y),
                                            ImGui::GetColorU32(ImVec4(0.43f, 0.43f, 0.50f, 0.50f)));
}

void EditorMenu::AssetBrowser(GameTile * & ghost_tile, ImVec2 button_size){
    SDL_Texture * texture;
    int button_count = tile_paths.size();
    ImGuiStyle& style = ImGui::GetStyle();
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    for (int i = 0; i < button_count; i++){
        texture = cache->LoadTexture(tile_paths[i][1]);
        cache->SetTextureAlpha(texture, max(alpha, .5f));
         ImGui::PushID(i);
        if (ImGui::ImageButton("",(ImTextureID)texture, button_size, ImVec2(0.0f, 0.0f), ImVec2(32.0f, 32), ImVec4(0.0f, 0.0f, 0.0f, 0.0f)))
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
            ImGui::TextUnformatted(tile_paths[i][0].c_str());
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

void EditorMenu::ClassicMenus(GameTile * & ghost_tile, Camera * camera, GameTile * selected_tile){
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
                saving_to_mx = true;
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
        if (ImGui::Begin("Asset Menu", NULL)){
            AssetBrowser(ghost_tile, ImVec2(52.0f, 52.0f));
        }
        ImGui::End();
    }

    if(ghost_tile){
      GhostTileWindow(ghost_tile, camera);
    }

    // Tile Edit Mode
    if (tile_edit_mode && selected_tile){
        TileEditWindow(selected_tile, camera);
    }
}

void EditorMenu::Dialogs(){
    if (about_window){
        if (ImGui::Begin("About LevelEditor++", NULL)){
            ImGui::TextWrapped("Welcome To LevelEditor++ created by The Sardonicals.  LevelEdit++ is a upgraded edition from the Python counterpart.");
            ImGui::TextWrapped("The Plus Edition will feature many APIs to enhance the user experience of the Level Editor.");

        }
        ImGui::End();
    }

    if (instruction_manual){
        if (ImGui::Begin("Instruction Manual", NULL)){
            ImGui::TextWrapped("Pick a tile in the Project panel, then click in the scene to place it.");
            ImGui::TextWrapped("Press X, or the Select tool, to put the brush down.");
            ImGui::TextWrapped("With no brush held, click a tile in the scene or a row in the Hierarchy to select it.");
            ImGui::TextWrapped("Press Del, or use the Delete Tile button in the Inspector, to remove the selected tile.");
        }
        ImGui::End();
    }

    if (saving_to_mx){
        string label_name = "(.mx)";
        if (ImGui::Begin("Please Enter a name for your tileset", NULL)){
            ImGui::InputText(label_name.c_str(), &tileset_name);
            if (ImGui::Button("Save")){
                save_to_mx = true;
            }
        }
        ImGui::End();
    }

    // Saving MXPR settings and Booleans Statuses
    if (saving_mxpr){
        if (ImGui::Begin("Please Enter a Project name", NULL)){
            ImGui::InputText(prlabel_name.c_str(), &project_name);
            if (ImGui::Button("Save")){
                save_to_mxpr = true;
                editor_states["show_item_menu"] = show_item_menu;
                editor_states["hide_stats"] = hide_stats;
                editor_states["align_menu_to_screen"] = align_menu_to_screen;
                editor_states["about_window"] = about_window;
                editor_states["instruction_manual"] = instruction_manual;
                editor_states["unity_layout"] = unity_layout;
            }
        }
        ImGui::End();
    }

    if (loading_project){
        if(ImGui::Begin("Please Input the name of the tileset you want to load", NULL)){
            ImGui::InputText(project_name.c_str(), &project_name);
            if (ImGui::Button("Load")){

            }
        }
        ImGui::End();
    }

    if (loading_tileset){
        if (ImGui::Begin("Input the filepath to the tileset you want to load")){
            ImGui::InputText(load_label.c_str(), &tileset_name);
                if (ImGui::Button("Load")){
                    tileset_import = true;
                }
        }
        ImGui::End();

    }
}

void EditorMenu::GhostTileWindow(GameTile * ghost_tile, Camera * camera){
  w_increase = ghost_tile->w;
  h_increase = ghost_tile->h;

  if (ImGui::Begin("Texture Properties", NULL)){
    string x_string = "X: " + to_string(ghost_tile->x - camera->xpos);
    string y_string = "Y: " + to_string(ghost_tile->y - camera->ypos);
    ImGui::TextUnformatted(x_string.c_str());
    ImGui::TextUnformatted(y_string.c_str());
    ImGui::InputInt("Width", &w_increase);
    ImGui::InputInt("Height", &h_increase);
  }

  ImGui::End();
  ghost_tile->w = w_increase;
  ghost_tile->h = h_increase;

}

void EditorMenu::TileEditWindow(GameTile * selected_tile, Camera * camera){
  x_delta = selected_tile->x;
  y_delta = selected_tile->y;
  w_increase = selected_tile->w;
  h_increase = selected_tile->h;

  if (ImGui::Begin("Tile Edit Window")){

      ImGui::InputInt("X: ", &x_delta);
      ImGui::InputInt("Y: ", &y_delta);
      ImGui::InputInt("Width", &w_increase);
      ImGui::InputInt("Height", &h_increase);

      selected_tile->x = x_delta;
      selected_tile->y = y_delta;
      selected_tile->w = w_increase;
      selected_tile->h = h_increase;

      ImGui::Separator();

      // Same delete path as the docked Inspector: raise the flag, let the editor free it.
      if (ImGui::Button("Delete Tile")){
          delete_selected_tile = true;
      }

  }
  ImGui::End();

}
