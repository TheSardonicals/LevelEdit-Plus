#include "editor_menu.h"

#include <unordered_set>

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

void EditorMenu::Process(GameTile * &ghost_tile, Camera * camera, map<string, vector<GameTile *>> & tile_cache, vector<GameTile *> & selected_tiles, map<string, TileType> & tile_types){

    MainMenuBar(ghost_tile, tile_cache, selected_tiles);

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

        Toolbar(ghost_tile, selected_tiles);
        HierarchyPanel(tile_cache, selected_tiles, tile_types);
        InspectorPanel(selected_tiles, ghost_tile, camera, tile_cache, tile_types);
        ProjectPanel(ghost_tile);
        StatusBar(tile_cache, selected_tiles);
        SceneOutline();
    }
    else {
        ClassicMenus(ghost_tile, camera, selected_tiles);
    }

    Dialogs();
}

void EditorMenu::ClearGhostTile(GameTile * & ghost_tile){
    // Deleting a null pointer is a no-op, so this is safe to call whether or not a
    // brush is currently held.
    delete ghost_tile;
    ghost_tile = NULL;
}

bool EditorMenu::IsSelected(vector<GameTile *> & selected_tiles, GameTile * tile){
    return find(selected_tiles.begin(), selected_tiles.end(), tile) != selected_tiles.end();
}

void EditorMenu::SelectTile(vector<GameTile *> & selected_tiles, GameTile * tile, bool additive){
    if (!tile){
        return;
    }

    if (!additive){
        selected_tiles.clear();
        selected_tiles.push_back(tile);
    }
    else {
        // Ctrl click toggles, so the same click can take a tile back out of the selection.
        vector<GameTile *>::iterator found = find(selected_tiles.begin(), selected_tiles.end(), tile);
        if (found != selected_tiles.end()){
            selected_tiles.erase(found);
        }
        else {
            selected_tiles.push_back(tile);
        }
    }

    // The outlines themselves are re-synced by the editor every frame from this list.
    tile_edit_mode = !selected_tiles.empty();
}

void EditorMenu::SelectAll(map<string, vector<GameTile *>> & tile_cache, vector<GameTile *> & selected_tiles){
    selected_tiles.clear();
    for (auto & entry : tile_cache){
        for (auto & tile : entry.second){
            selected_tiles.push_back(tile);
        }
    }
    tile_edit_mode = !selected_tiles.empty();
}

void EditorMenu::MainMenuBar(GameTile * & ghost_tile, map<string, vector<GameTile *>> & tile_cache, vector<GameTile *> & selected_tiles){
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
            string delete_label = selected_tiles.size() > 1
                                ? "Delete " + to_string(selected_tiles.size()) + " Tiles"
                                : string("Delete Tile");

            if (ImGui::MenuItem(delete_label.c_str(), "Del", false, !selected_tiles.empty())){
                delete_selection = true;
            }
            if (ImGui::MenuItem("Select All", "Ctrl+A", false, !tile_cache.empty())){
                SelectAll(tile_cache, selected_tiles);
            }
            if (ImGui::MenuItem("Deselect", "X", false, !selected_tiles.empty() || ghost_tile != NULL)){
                // Dropping the selection here is enough: the selection pass in the editor
                // clears the leftover outlines on the next frame.
                selected_tiles.clear();
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

void EditorMenu::Toolbar(GameTile * & ghost_tile, vector<GameTile *> & selected_tiles){
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
        if (ImGui::IsItemHovered()){ImGui::SetTooltip("Put the brush down so tiles can be picked (X). Drag a box to select several, Ctrl+click to add one");}

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
        ImGui::BeginDisabled(selected_tiles.empty());
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.16f, 0.16f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.22f, 0.22f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.12f, 0.12f, 1.00f));

        string delete_label = selected_tiles.size() > 1
                            ? "Delete (" + to_string(selected_tiles.size()) + ")"
                            : string("Delete");
        if (ImGui::Button(delete_label.c_str())){
            delete_selection = true;
        }
        ImGui::PopStyleColor(3);
        ImGui::EndDisabled();
        if (ImGui::IsItemHovered()){ImGui::SetTooltip("Remove the selected tiles from the level (Del)");}

        ImGui::SameLine();
        ImGui::BeginDisabled(selected_tiles.empty() && ghost_tile == NULL);
        if (ImGui::Button("Deselect")){
            selected_tiles.clear();
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

void EditorMenu::HierarchyPanel(map<string, vector<GameTile *>> & tile_cache, vector<GameTile *> & selected_tiles, map<string, TileType> & tile_types){
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
        // On its own line: at the panel width this gets cut off if it trails the count.
        if (!selected_tiles.empty()){
            ImGui::TextDisabled("%d selected", (int)selected_tiles.size());
        }
        ImGui::Separator();

        if (tile_cache.empty()){
            ImGui::TextDisabled("No tiles placed yet.");
        }

        for (auto & entry : tile_cache){
            string header = entry.first + " (" + to_string(entry.second.size()) + ")";
            bool open = ImGui::TreeNodeEx(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

            // What the type means, beside its name, so the level can be read for
            // behaviour at a glance instead of by selecting each type in turn.
            auto type = tile_types.find(entry.first);
            if (type != tile_types.end() && !type->second.flags.empty()){
                string meaning;
                for (auto & flag : type->second.flags){
                    meaning += (meaning.empty() ? "" : ", ") + flag;
                }
                // A trailing "?" marks a guess from the name that has not been saved.
                const bool guessed = !type->second.declared;
                ImGui::SameLine();
                ImGui::TextDisabled("[%s%s]", meaning.c_str(), guessed ? "?" : "");
                // The panel is narrow, so a long list gets cut off; the tooltip always
                // has all of it.
                if (ImGui::IsItemHovered()){
                    ImGui::SetTooltip("%s %s: %s", entry.first.c_str(),
                                      guessed ? "is guessed to mean (not saved)" : "means",
                                      meaning.c_str());
                }
            }

            if (open){
                for (auto & tile : entry.second){
                    // The pointer is the identity here, so rows stay correct even when two
                    // tiles of the same type sit on the same spot.
                    ImGui::PushID(tile);
                    // The tree node above already names the tile type, so the row only has
                    // to carry the position. Keeps rows readable at the panel width.
                    string label = "[" + to_string(tile->x) + ", " + to_string(tile->y) + "]";

                    if (ImGui::Selectable(label.c_str(), IsSelected(selected_tiles, tile))){
                        // Ctrl adds to the selection, same as clicking in the scene does.
                        SelectTile(selected_tiles, tile, ImGui::GetIO().KeyCtrl);
                    }

                    // Right clicking a row that is not already part of the selection makes
                    // it the selection, so the delete lands on the row that was clicked.
                    if (ImGui::BeginPopupContextItem()){
                        if (!IsSelected(selected_tiles, tile)){
                            SelectTile(selected_tiles, tile, false);
                        }

                        string menu_label = selected_tiles.size() > 1
                                          ? "Delete " + to_string(selected_tiles.size()) + " Tiles"
                                          : string("Delete Tile");
                        if (ImGui::MenuItem(menu_label.c_str(), "Del")){
                            delete_selection = true;
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

void EditorMenu::InspectorPanel(vector<GameTile *> & selected_tiles, GameTile * ghost_tile, Camera * camera, map<string, vector<GameTile *>> & tile_cache, map<string, TileType> & tile_types){
    const ImGuiViewport * viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - inspector_width, viewport->WorkPos.y + toolbar_height));
    ImGui::SetNextWindowSize(ImVec2(inspector_width, viewport->WorkSize.y - toolbar_height - status_height));
    ImGui::SetNextWindowBgAlpha(alpha);

    if (ImGui::Begin("Inspector", NULL, PANEL_FLAGS)){

        if (selected_tiles.size() == 1){
            // Single tile: the full transform, editable.
            GameTile * selected_tile = selected_tiles[0];

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

            // How far the tile stands off the ground. Separate from Height above, which
            // is the tile's own size, so this one is labelled for what it does.
            ImGui::Spacing();
            stand_height = selected_tile->elevation;
            if (ImGui::InputInt("Stands Up", &stand_height)){
                selected_tile->elevation = max(0, stand_height);
            }
            if (ImGui::IsItemHovered()){ImGui::SetTooltip("Pixels this tile stands off the floor. 0 is flat ground. [ and ] step it");}
            ImGui::PopID();
        }
        else if (selected_tiles.size() > 1){
            // Several tiles: no single position to show, so the inspector offers the
            // things that do make sense across a whole selection.
            ImGui::SeparatorText("Selected Tiles");
            ImGui::Text("%d tiles selected", (int)selected_tiles.size());
            ImGui::Spacing();

            if (ImGui::BeginChild("selection_list", ImVec2(0.0f, 110.0f), ImGuiChildFlags_Borders)){
                for (auto & tile : selected_tiles){
                    ImGui::Text("%s  [%d, %d]", tile->name.c_str(), tile->x, tile->y);
                }
            }
            ImGui::EndChild();

            ImGui::Spacing();
            ImGui::TextDisabled("Resize all:");

            // Seeded from the first tile, and only written back when the user actually
            // changes the field, so opening the panel does not resize the whole selection.
            ImGui::PushID("multi_transform");
            w_increase = selected_tiles[0]->w;
            h_increase = selected_tiles[0]->h;

            if (ImGui::InputInt("Width", &w_increase)){
                for (auto & tile : selected_tiles){ tile->w = w_increase; }
            }
            if (ImGui::InputInt("Height", &h_increase)){
                for (auto & tile : selected_tiles){ tile->h = h_increase; }
            }

            stand_height = selected_tiles[0]->elevation;
            if (ImGui::InputInt("Stands Up", &stand_height)){
                for (auto & tile : selected_tiles){ tile->elevation = max(0, stand_height); }
            }
            if (ImGui::IsItemHovered()){ImGui::SetTooltip("Raise the whole selection off the floor. [ and ] step it");}
            ImGui::PopID();
        }

        // Types already given an editor this frame. A type can be both selected and
        // the brush, and two editors for one type would collide on their widget IDs.
        unordered_set<string> shown_types;

        if (!selected_tiles.empty()){
            // Every type the selection touches, found in one pass over the level against
            // a set of the selected tiles. Searching the selection for each tile instead
            // goes quadratic the moment someone selects a whole map.
            unordered_set<GameTile *> chosen(selected_tiles.begin(), selected_tiles.end());
            vector<string> touched;
            for (auto & entry : tile_cache){
                for (auto tile : entry.second){
                    if (chosen.count(tile)){
                        touched.push_back(entry.first);
                        break;
                    }
                }
            }

            ImGui::Spacing();
            ImGui::SeparatorText(touched.size() == 1 ? "Tile Type" : "Tile Types");
            for (auto & key : touched){
                TileTypeEditor(tile_types, key, static_cast<int>(tile_cache[key].size()));
                shown_types.insert(key);
            }
        }

        if (!selected_tiles.empty()){
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // The editor owns the tiles, so the button only raises a flag and lets
            // Editor::DeleteSelection() free them and drop the selection.
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.16f, 0.16f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.22f, 0.22f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.12f, 0.12f, 1.00f));

            string delete_label = selected_tiles.size() > 1
                                ? "Delete " + to_string(selected_tiles.size()) + " Tiles"
                                : string("Delete Tile");
            if (ImGui::Button(delete_label.c_str(), ImVec2(-1.0f, 32.0f))){
                delete_selection = true;
            }
            ImGui::PopStyleColor(3);

            if (ImGui::Button("Deselect", ImVec2(-1.0f, 0.0f))){
                selected_tiles.clear();
                tile_edit_mode = false;
            }
        }
        else {
            ImGui::TextDisabled("Nothing selected.");
            ImGui::TextWrapped("Put the brush down (X), then click a tile, drag a box around several, or Ctrl+click to add to the selection.");
        }

        if (ghost_tile){
            ImGui::Spacing();
            ImGui::SeparatorText("Brush");
            ImGui::Text("Name: %s", ghost_tile->key.c_str());
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

            // Every tile placed with this brush is stamped at this height, so a wall can
            // be laid down in one pass instead of raised afterwards.
            stand_height = ghost_tile->elevation;
            if (ImGui::InputInt("Stands Up", &stand_height)){
                ghost_tile->elevation = max(0, stand_height);
            }
            if (ImGui::IsItemHovered()){ImGui::SetTooltip("Height every tile this brush places will stand at. [ and ] step it");}
            ImGui::PopID();

            // What the brush's tiles will mean. Set it here and every tile laid with the
            // brush already carries it - no going back to flag them afterwards.
            if (!shown_types.count(ghost_tile->key)){
                auto placed = tile_cache.find(ghost_tile->key);
                int placed_count = (placed != tile_cache.end()) ? static_cast<int>(placed->second.size()) : -1;
                TileTypeEditor(tile_types, ghost_tile->key, placed_count);
            }
        }

        if (selected_tiles.empty() && !ghost_tile){
            ImGui::Spacing();
            ImGui::SeparatorText("Scene");
            // NoInputs keeps this to a swatch plus its label: the three number boxes do
            // not fit the inspector column and push the label off the panel.
            ImGui::ColorEdit3("Clear Color", (float*)clear_color, ImGuiColorEditFlags_NoInputs);
        }
    }
    ImGui::End();
}

void EditorMenu::TileTypeEditor(map<string, TileType> & tile_types, const string & key, int placed_count){
    TileType & type = EnsureTileType(tile_types, key);

    // Scoped by type, so two types open at once never share a widget.
    ImGui::PushID(key.c_str());

    if (ImGui::CollapsingHeader(key.c_str(), ImGuiTreeNodeFlags_DefaultOpen)){
        // Flags belong to the type, not the tile, so say how far an edit reaches.
        if (placed_count >= 0){
            ImGui::TextDisabled("Applies to all %d placed", placed_count);
        } else {
            ImGui::TextDisabled("Applies to every tile this brush places");
        }

        // A guess from the name is only a starting point. It is not saved until the
        // author changes a flag or keeps it as shown, so opening and saving a map
        // never changes what it means to a game that did not guess from names.
        if (!type.declared){
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.75f, 0.30f, 1.00f));
            ImGui::TextWrapped("Guessed from the name. Not saved until you change a flag or keep it.");
            ImGui::PopStyleColor();
            if (ImGui::SmallButton("Keep")){
                type.declared = true;
            }
            if (ImGui::IsItemHovered()){ImGui::SetTooltip("Save these flags to the map as shown");}
        }

        // The flags a game already acts on.
        for (auto & known : kKnownTileFlags){
            bool on = type.HasFlag(known.first);
            if (ImGui::Checkbox(known.first.c_str(), &on)){
                type.SetFlag(known.first, on);
                type.declared = true;
            }
            if (ImGui::IsItemHovered()){ImGui::SetTooltip("%s", known.second.c_str());}
        }

        // Everything else the author has given this type. The editor does not know
        // what these mean - that is the point, a game does. Shown as ticked boxes like
        // the known flags above, so every flag reads the same way; unticking one takes
        // it off the type.
        for (size_t i = 0; i < type.flags.size(); ){
            if (IsKnownTileFlag(type.flags[i])){
                ++i;
                continue;
            }

            bool keep = true;
            ImGui::Checkbox(type.flags[i].c_str(), &keep);
            if (ImGui::IsItemHovered()){ImGui::SetTooltip("Custom flag. Untick to remove it");}

            if (!keep){
                type.flags.erase(type.flags.begin() + i);
                type.declared = true;
            } else {
                ++i;
            }
        }

        // Any meaning at all can be added. Normalised so "Blocks Shots" and
        // "blocks_shots" are the same flag to every game that reads the map.
        string & pending = new_flag_text[key];
        ImGui::SetNextItemWidth(-60.0f);
        bool entered = ImGui::InputTextWithHint("##new_flag", "add a flag", &pending, ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();
        bool added = ImGui::Button("Add");
        if (entered || added){
            string flag = NormaliseTileFlag(pending);
            if (!flag.empty()){
                type.SetFlag(flag, true);
                type.declared = true;
            }
            pending.clear();
        }

        // A box smaller than the art, relative to the top-left corner of the tile.
        // Drawn in the scene around the selected tiles while it is on.
        ImGui::Checkbox("Collision box", &type.has_collision);
        if (ImGui::IsItemHovered()){ImGui::SetTooltip("Block with a box smaller than the art, relative to the top-left corner of the tile. Off blocks the whole tile");}
        if (type.has_collision){
            ImGui::InputInt4("x y w h", type.collision.data());
            type.collision[2] = max(0, type.collision[2]);
            type.collision[3] = max(0, type.collision[3]);
        }
    }

    ImGui::PopID();
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

void EditorMenu::StatusBar(map<string, vector<GameTile *>> & tile_cache, vector<GameTile *> & selected_tiles){
    const ImGuiViewport * viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - status_height));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, status_height));
    ImGui::SetNextWindowBgAlpha(alpha);

    if (ImGui::Begin("##StatusBar", NULL, PANEL_FLAGS | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)){
        int tile_count = 0;
        for (auto & entry : tile_cache){
            tile_count += entry.second.size();
        }

        string selection;
        if (selected_tiles.empty()){
            selection = "none";
        }
        else if (selected_tiles.size() == 1){
            selection = selected_tiles[0]->name;
        }
        else {
            selection = to_string(selected_tiles.size()) + " tiles";
        }

        // Kept short enough to survive at the default window width.
        ImGui::Text("%.1f FPS  |  Tiles: %d  |  Selected: %s  |  Drag: box  Ctrl+click: add  [ ]: height  Del: delete  X: brush",
                    ImGui::GetIO().Framerate,
                    tile_count,
                    selection.c_str());
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
        // UVs run 0..1 over the whole texture. The old (32, 32) made each thumbnail repeat
        // its tile 32 times over, which is why the assets showed up as stripes.
        if (ImGui::ImageButton("",(ImTextureID)texture, button_size, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f)))
        {
            // Handle setting ghost tile when button is clicked
            if (ghost_tile != nullptr){
                delete ghost_tile;
            }
            ghost_tile = new GameTile(cache, tile_paths[i][1], mouse->xpos, mouse->ypos, 32, 32);
            // The asset key, which is the tile's name plus any folders it sits in
            // under resources/. Everything downstream files the tile under this.
            ghost_tile->key = tile_paths[i][0];
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

void EditorMenu::ClassicMenus(GameTile * & ghost_tile, Camera * camera, vector<GameTile *> & selected_tiles){
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

    // Tile Edit Mode. The floating window edits one tile, so it follows the last tile
    // added to the selection.
    if (tile_edit_mode && !selected_tiles.empty()){
        TileEditWindow(selected_tiles.back(), camera);
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
            ImGui::TextWrapped("With no brush held, click a tile to select it, drag a box to select several, or Ctrl+click to add one to the selection.");
            ImGui::TextWrapped("Ctrl+A selects the whole level, and clicking empty space clears the selection.");
            ImGui::TextWrapped("Press Del, or use the Delete button in the Inspector, to remove everything selected.");
            ImGui::TextWrapped("Press ] to raise the selection off the floor and [ to lower it, or set Stands Up in the Inspector. A raised tile keeps the ground it was placed on, and draws a shaded side face so walls read as solid blocks.");
            ImGui::TextWrapped("With nothing selected, [ and ] set the height the brush will place at.");
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
          delete_selection = true;
      }

  }
  ImGui::End();

}
