#include "headers.h"
#include "editor.h"
#include "functions.h"
#include "tojson.h"

Editor::Editor(){}

int Editor::Start(int argc, char** argv){

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

    // Create Window and Renderer
    window = SDL_CreateWindow("LevelEdit++ - by Sardonicals", SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_FLAGS);
    if (!window){
        ShowError("LevelEdit++ Error!", "Couldn't create window: ", "Window creation failed!: ", true);
        return 0;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer){
        ShowError("LevelEdit++ Error!", "Couldn't create renderer: ", "Renderer creation failed!: ", true);
        return 0;
    }

    //Load Icon
    icon = SDL_LoadBMP("LE++.bmp");
    SDL_SetWindowIcon(window, icon);

    // ImGui Initialization.
    IMGUI_CHECKVERSION();
    ImGuiContext* ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(ctx);
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.Fonts->AddFontFromFileTTF("misc/fonts/joystix.ttf", 16.0f);
    
    SetupImGuiStyleColor();
    
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // VARIABLES
    state = EDITING;
    clear_color = ImVec4(.8627, .8627, .8627, 1);
    tile_paths = GetPaths();

    // OBJECTS
    mouse = new Pointer();
    cache = new TextureCache(renderer);
    camera = new Camera(SCREEN_WIDTH, SCREEN_HEIGHT, 3, 40);
    // The window may not be the size it was asked for (a maximised restore, or a
    // scaled display), so fit the border to what it actually is.
    FitCameraToWindow();
    keyboard = new KeyboardManager();
    gui = new EditorMenu(&SCREEN_WIDTH, &SCREEN_HEIGHT, &clear_color, mouse, &tile_paths,  cache);
    json_handler = new ToJson();

    return 1;
}


void Editor::Loop(){
    while (running){
        Process();
        Render();
        SDL_Delay(5);
    }
}


void Editor::Process()
{
    //Event Loop
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        mouse->Compute(&event);

        if (event.type == SDL_EVENT_QUIT){
            running = false;
            break;
        }

        if (event.type == event.window.type){
            if (event.window.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)){
                running = false;
                break;
            }
            if (event.window.type == SDL_EVENT_WINDOW_RESIZED){
                SCREEN_WIDTH = event.window.data1;
                SCREEN_HEIGHT = event.window.data2;
                FitCameraToWindow();
            }
            // Going fullscreen on a scaled display can change the pixel size without
            // the window size changing, and the camera border is drawn in pixels.
            if (event.window.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED){
                FitCameraToWindow();
            }
        }

    }

    // Once per frame, not once per event. Inside the event loop the press and release
    // edges fired once per event that happened to arrive, and stayed set on frames with
    // no events at all, which the marquee drag cannot work with.
    mouse->Process();

    //Application Loop
    switch (state) {
        case MENU:{}break;
        case EDITING:{
            // Start the Dear ImGui frame
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            SetKeyMapping();
            gui->Process(ghost_tile, camera, tile_cache, selected_tiles, tile_types);
            keyboard->Process();

            // Deleting has to happen before anything else walks the tile cache this frame,
            // otherwise the selection loop and the renderer would be working with a tile
            // that is on its way out.
            if (gui->delete_selection){
                DeleteSelection();
                gui->delete_selection = false;
            }

            // The rubber band belongs to the selection tool over the scene, so drop it if
            // the mouse has gone to the GUI or the user has picked up a brush mid-drag.
            if (ImGui::GetIO().WantCaptureMouse || ghost_tile){
                marquee_active = false;
            }

            // Handle every editor-related thing that works outside of the GUI underneath this conditional.
            if (!ImGui::GetIO().WantCaptureMouse){
                if (ghost_tile){
                  // TODO QOL: Add a pre-place highlight to show where user will be placing the selected block.
                    if (mouse->has_clicked){
                        // The cursor marks where the tile meets the ground, so the new
                        // tile takes the mouse position as its footprint and carries the
                        // brush height up from there.
                        GameTile * placed = new GameTile(cache, tile_paths[ghost_tile->key], mouse->xpos - camera->xpos, mouse->ypos - camera->ypos, ghost_tile->w, ghost_tile->h);
                        placed->elevation = ghost_tile->elevation;
                        placed->key = ghost_tile->key;

                        EnsureTileType(tile_types, ghost_tile->key);

                        if (tile_cache.count(ghost_tile->key) == 0){
                            tile_cache[ghost_tile->key] = {placed};
                        }
                        else{
                            tile_cache[ghost_tile->key].push_back(placed);
                        }
                    }
                } 
                else {
                  // Functionality for a tile selection mode
                  // Holding Ctrl adds to the selection instead of replacing it, the way it
                  // works in Unity.
                  bool additive = keyboard->KeyIsPressed(SDL_SCANCODE_LCTRL) || keyboard->KeyIsPressed(SDL_SCANCODE_RCTRL);

                  // Pressing down starts a rubber band. Whether it turns out to be a drag
                  // or just a click is decided on release, by how far the mouse travelled.
                  if (mouse->has_pressed){
                      marquee_active = true;
                      marquee_start_x = mouse->xpos;
                      marquee_start_y = mouse->ypos;
                  }

                  if (marquee_active){
                      // Built from the two corners so it works dragging in any direction.
                      marquee_rect.x = min(marquee_start_x, mouse->xpos);
                      marquee_rect.y = min(marquee_start_y, mouse->ypos);
                      marquee_rect.w = fabs(mouse->xpos - marquee_start_x);
                      marquee_rect.h = fabs(mouse->ypos - marquee_start_y);
                  }

                  // A few pixels of travel separates a sloppy click from a real drag.
                  bool dragging = marquee_active && (marquee_rect.w > 4 || marquee_rect.h > 4);

                  if (mouse->has_clicked && marquee_active){
                      if (dragging){
                          // Band select: everything the box touches, replacing the old
                          // selection unless Ctrl is being held.
                          if (!additive){
                              ClearSelection();
                          }
                          for (auto& tile_name : tile_cache){
                              for (auto& tile : tile_name.second){
                                  if (SDL_HasRectIntersectionFloat(&marquee_rect, &tile->rect) && !IsSelected(tile)){
                                      Select(tile, true);
                                  }
                              }
                          }
                      }
                      else {
                          // Plain click. Clicking empty space clears the selection, which is
                          // how the user gets back to nothing selected.
                          GameTile * hit = NULL;
                          for (auto& tile_name : tile_cache){
                              for (auto& tile : tile_name.second){
                                  if (mouse->IsTouching(&tile->rect)){
                                      hit = tile;
                                  }
                              }
                          }

                          if (hit){
                              Select(hit, additive);
                          }
                          else if (!additive){
                              ClearSelection();
                          }
                      }

                      marquee_active = false;
                  }
                }
            }

            // Outlines: selected tiles keep theirs, the tile under the mouse gets the hover
            // one, and anything the rubber band is currently over previews as hovered so the
            // user can see what the drag is about to catch. This runs outside the mouse
            // check above, because a tile selected from the Hierarchy has to show its
            // outline while the mouse is still sitting over the panel.
            bool over_gui = ImGui::GetIO().WantCaptureMouse;
            bool previewing = marquee_active && (marquee_rect.w > 4 || marquee_rect.h > 4);

            for (auto& tile_name : tile_cache){
                for (auto& tile : tile_name.second){
                    tile->selected = IsSelected(tile);
                    tile->highlight = !over_gui
                                   && (mouse->IsTouching(&tile->rect)
                                       || (previewing && SDL_HasRectIntersectionFloat(&marquee_rect, &tile->rect)));
                }
            }

            if (gui->tileset_import){
                try {
                    LoadMX();
                    gui->tileset_import = false;
                }
                catch (nlohmann::detail::parse_error) {
                    // Present the user a message on the screen to let them know that it failed.
                    cout << "Error: Tileset Does Not Exist" << endl;
                }
            }

            if (gui->save_to_mxpr){
                json_handler->SaveMXProject(gui->editor_states, gui->project_name);
                gui->save_to_mxpr = false;
            }

            if (gui->save_to_mx){
                gui->saving_to_mx = false;  
                json_handler->SaveToJson(gui->tileset_name, tile_cache, tile_types);
                json_handler->ExportMX(tile_cache, gui->tileset_name);
                //Reset the window to close or to show a text saying, 'Tileset Saved'.  
                //Made tile a checkbox to have that constant availability of saving.
                gui->save_to_mx = false;

            }


        }break;

        default:
            break;
    }
    SetClearColor();  
}

void Editor::LoadMX(){
    json_handler->ImportMX(gui->tileset_name);

    if (!tile_cache.empty()){
        // The selection points into the level that is about to be replaced, so it has to
        // go before the cache does, otherwise the inspector would keep editing a tile that
        // is no longer part of the level.
        ClearSelection();

        // The cache owns these tiles, so free them instead of just dropping the pointers.
        for (auto tile_list : tile_cache){
            for (auto tile : tile_list.second){
                delete tile;
            }
        }
        tile_cache.clear();
    }

    // The meanings belong to the map being loaded, not whatever was open before it.
    tile_types.clear();

    for (auto& tile : json_handler->json_blocks["tiles"].items()){
        //cout << tile.key() << endl;
        const json & entry = tile.value();
        TileType & type = tile_types[tile.key()];

        // A "flags" key, even an empty one, is the map stating what the tile means,
        // and is taken as-is. A map that never said anything only gets a guess from
        // the tile's name to show in the Inspector - left undeclared, so saving the
        // map does not quietly turn that guess into something every game obeys.
        if (entry.contains("flags") && entry["flags"].is_array()){
            for (auto & flag : entry["flags"]){
                if (flag.is_string()){
                    type.SetFlag(NormaliseTileFlag(flag.get<string>()), true);
                }
            }
            type.declared = true;
        }
        else {
            type.flags = SuggestFlagsFromName(tile.key());
        }

        if (entry.contains("collision") && entry["collision"].is_array() && entry["collision"].size() >= 4){
            type.has_collision = true;
            for (int i = 0; i < 4; ++i){
                type.collision[i] = entry["collision"][i].get<int>();
            }
        }

        for (auto& locations : json_handler->json_blocks["tiles"][tile.key()]["locations"].items()){
            //cout << locations.value() << endl;
            //cout << json_handler->json_blocks["tiles"][tile.key()]["filepath"] << endl;
            GameTile * imported = new GameTile(cache, json_handler->json_blocks["tiles"][tile.key()]["filepath"], locations.value()[0], locations.value()[1], locations.value()[2], locations.value()[3]);

            // Filed under the key the map wrote it as, which need not match the
            // image's filename - it carries the asset's folders with it.
            imported->key = tile.key();

            // Height is the fifth element, and only format version 2 and up has one.
            // A map saved before this existed loads flat rather than failing.
            if (locations.value().size() > 4){
                imported->elevation = locations.value()[4];
            }

            if (tile_cache.count(tile.key()) == 0){
                //cout << "New Import " << tile.key() << endl;
                tile_cache[tile.key().c_str()] = {imported};
                //cout << tile_cache.count(tile.key()) << endl;
            }
            else{
                //cout << "Adding to existing vector of " << tile.key() << endl;
                tile_cache[tile.key().c_str()].push_back(imported);

            }
        }
    }

}
        


bool Editor::IsSelected(GameTile * tile){
    return find(selected_tiles.begin(), selected_tiles.end(), tile) != selected_tiles.end();
}

void Editor::Select(GameTile * tile, bool additive){
    if (!tile){
        return;
    }

    if (!additive){
        // Plain click replaces whatever was selected before.
        ClearSelection();
        selected_tiles.push_back(tile);
    }
    else {
        // Ctrl click toggles, so the same click can take a tile back out of the selection.
        vector<GameTile *>::iterator found = find(selected_tiles.begin(), selected_tiles.end(), tile);
        if (found != selected_tiles.end()){
            (*found)->selected = false;
            selected_tiles.erase(found);
        }
        else {
            selected_tiles.push_back(tile);
        }
    }

    // The edit window follows the selection: open while something is selected, shut once
    // the last tile has been toggled back off.
    gui->tile_edit_mode = !selected_tiles.empty();
}

void Editor::ClearSelection(){
    // Clear the outlines before dropping the pointers, otherwise the tiles keep drawing
    // as selected with nothing pointing at them.
    for (auto tile : selected_tiles){
        tile->selected = false;
    }
    selected_tiles.clear();
    gui->tile_edit_mode = false;
}

void Editor::DeleteSelection(){
    if (selected_tiles.empty()){
        // Nothing is selected, so make sure the edit window is not left open on a tile
        // that is not there.
        gui->tile_edit_mode = false;
        return;
    }

    for (auto selected : selected_tiles){
        // Search for the pointer itself rather than looking the tile up by
        // tile_cache[name]. A tile imported from an .mx file takes its name from the
        // folder it was exported to, so its name does not always match the key it is
        // filed under in the cache.
        for (auto entry = tile_cache.begin(); entry != tile_cache.end(); ++entry){
            vector<GameTile *> & tiles = entry->second;
            vector<GameTile *>::iterator tile = find(tiles.begin(), tiles.end(), selected);

            if (tile != tiles.end()){
                // The texture belongs to the cache and is shared with every other tile of
                // the same type, so only the tile itself gets freed here.
                delete *tile;
                tiles.erase(tile);

                // Drop the whole entry once its last tile is gone, so saving and exporting
                // never write out a tile type that has nothing left in the level.
                if (tiles.empty()){
                    tile_cache.erase(entry);
                }
                break;
            }
        }
    }

    // Deselect: the tiles the Inspector was pointing at do not exist anymore. Their
    // outlines go with them, since the tiles they were drawn on are gone. Cleared
    // directly rather than through ClearSelection(), which would touch freed tiles.
    selected_tiles.clear();
    gui->tile_edit_mode = false;
}


void Editor::Render(){

    SDL_SetRenderDrawColor(renderer, r_clear_color.r, r_clear_color.g, r_clear_color.b, r_clear_color.a);
    SDL_RenderClear(renderer);
    switch (state){
        case EDITING:{
            ImGui::Render();
            // Anything that should render before the imgui-based menu, render  underneath this line.
            if (tile_cache.size() > 0 || import_finish){
                // Back to front by where each tile meets the ground, so a tall tile
                // covers whatever stands behind it. Map iteration order groups by tile
                // name, which says nothing about depth once tiles have elevation.
                // Stable, so tiles sharing a ground line keep a fixed order instead of
                // trading places between frames.
                // Tiles off the edge of the window are dropped here rather than handed
                // to SDL to clip away: a map is usually far larger than the view, and
                // the ones nobody can see cost a draw call each. They are dropped
                // before the sort, so that shrinks with them.
                //
                // The bounds match what GameTile::Render draws: the top face lifted by
                // the elevation, plus the side face filling the gap back down to the
                // footprint.
                const int cam_x = static_cast<int>(camera->xpos);
                const int cam_y = static_cast<int>(camera->ypos);

                vector<GameTile *> draw_order;
                for (auto tile_list: tile_cache){
                    for (auto tile: tile_list.second){
                        const int left = (tile->x - (tile->w/2)) + cam_x;
                        const int top = (tile->y - (tile->h/2)) + cam_y - tile->elevation;

                        if (left + tile->w <= 0 || left >= output_width) continue;
                        if (top + tile->h + tile->elevation <= 0 || top >= output_height) continue;

                        draw_order.push_back(tile);
                    }
                }
                stable_sort(draw_order.begin(), draw_order.end(), [](GameTile * a, GameTile * b){
                    return a->GroundLine() < b->GroundLine();
                });

                for (auto tile: draw_order){
                    tile->Render({static_cast<int>(camera->xpos), static_cast<int>(camera->ypos)});
                }
            }
            // Rubber band, drawn over the tiles but under the GUI so it reads as part of
            // the scene. Only worth drawing once the drag is big enough to count as one.
            if (marquee_active && (marquee_rect.w > 4 || marquee_rect.h > 4)){
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 255, 145, 0, 40);
                SDL_RenderFillRect(renderer, &marquee_rect);
                SDL_SetRenderDrawColor(renderer, 255, 145, 0, 200);
                SDL_RenderRect(renderer, &marquee_rect);
            }

            // Collision boxes for whatever is selected, so an author can see the box
            // they are typing into the Inspector. Drawn at the footprint rather than
            // the lifted top face, because the footprint is where the box blocks.
            if (!selected_tiles.empty()){
                const int cam_x = static_cast<int>(camera->xpos);
                const int cam_y = static_cast<int>(camera->ypos);

                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 0, 220, 255, 230);

                for (auto & entry : tile_cache){
                    auto type = tile_types.find(entry.first);
                    if (type == tile_types.end() || !type->second.has_collision) continue;

                    const array<int, 4> & box = type->second.collision;
                    for (auto tile : entry.second){
                        if (!tile->selected) continue;
                        // Relative to the quad's top-left corner, the same corner
                        // GameTile::Render draws from.
                        SDL_FRect box_rect = {static_cast<float>(tile->x - (tile->w/2) + box[0] + cam_x),
                                              static_cast<float>(tile->y - (tile->h/2) + box[1] + cam_y),
                                              static_cast<float>(box[2]),
                                              static_cast<float>(box[3])};
                        SDL_RenderRect(renderer, &box_rect);
                    }
                }
            }

            camera->Show(renderer);
            // Send the data imgui stored from "Imgui::Render" to the screen using the specified render api.
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
            // Anything that should render after the Imgui-based menu, render underneath this line.
            if (ghost_tile){
                ghost_tile->Render({0, 0}, .6);
            }
            
        }break;   

        default:
            break;   
    }
    SDL_RenderPresent(renderer);
}

void Editor::SetKeyMapping(){
            // Keyboard Inputs
            if (keyboard->KeyIsPressed(SDL_SCANCODE_ESCAPE)){
                running = false;
            }

            // Ignore shortcuts while a text box has the keyboard, so typing a tileset name
            // does not delete tiles out from under the user.
            bool typing = ImGui::GetIO().WantCaptureKeyboard;

            if (!typing && keyboard->KeyIsPressed(SDL_SCANCODE_X)){
                // Delete first, then drop the pointer. The other way around leaks the tile,
                // because deleting a pointer that has already been set to NULL does nothing.
                delete ghost_tile;
                ghost_tile = NULL;
            }

            // Delete the selected tile, the way Unity does it. KeyWasPressed keeps this to
            // one deletion per press instead of one per frame the key is held.
            if (!typing && !selected_tiles.empty() && keyboard->KeyWasPressed(SDL_SCANCODE_DELETE)){
                gui->delete_selection = true;
            }

            // Raise and lower whatever is selected. Works on the brush too, so the user
            // can dial in a height before placing a run of walls. KeyWasPressed keeps it
            // to one step per press.
            if (!typing && keyboard->KeyWasPressed(SDL_SCANCODE_RIGHTBRACKET)){
                for (auto tile : selected_tiles){
                    tile->elevation += kHeightStep;
                }
                if (ghost_tile && selected_tiles.empty()){
                    ghost_tile->elevation += kHeightStep;
                }
            }

            if (!typing && keyboard->KeyWasPressed(SDL_SCANCODE_LEFTBRACKET)){
                for (auto tile : selected_tiles){
                    tile->elevation = max(0, tile->elevation - kHeightStep);
                }
                if (ghost_tile && selected_tiles.empty()){
                    ghost_tile->elevation = max(0, ghost_tile->elevation - kHeightStep);
                }
            }

            // Ctrl+A selects the whole level, Unity style.
            bool ctrl_held = keyboard->KeyIsPressed(SDL_SCANCODE_LCTRL) || keyboard->KeyIsPressed(SDL_SCANCODE_RCTRL);
            if (!typing && ctrl_held && keyboard->KeyWasPressed(SDL_SCANCODE_A)){
                ClearSelection();
                for (auto& tile_list : tile_cache){
                    for (auto& tile : tile_list.second){
                        selected_tiles.push_back(tile);
                    }
                }
                gui->tile_edit_mode = !selected_tiles.empty();
            }

            if (keyboard->KeyIsPressed(SDL_SCANCODE_UP)){
                camera->ypos += camera->speed;
            }

            if (keyboard->KeyIsPressed(SDL_SCANCODE_DOWN)){
                camera->ypos -= camera->speed;
            }

            if (keyboard->KeyIsPressed(SDL_SCANCODE_LEFT)){
                camera->xpos += camera->speed;
            }

            if (keyboard->KeyIsPressed(SDL_SCANCODE_RIGHT)){
                camera->xpos -= camera->speed;
            }

            if (ghost_tile){
                ghost_tile->SetPos(mouse->xpos , mouse->ypos);
            }

}


void Editor::FitCameraToWindow(){
    // The border is drawn with SDL_RenderRect, which works in the renderer's
    // pixels, so it is sized from the render output rather than from the window.
    // The two only match at 100% display scaling (the window is created with
    // SDL_WINDOW_HIGH_PIXEL_DENSITY); sized this way it hugs the edges at any scale.
    int width = 0, height = 0;
    if (!SDL_GetRenderOutputSize(renderer, &width, &height)){
        width = SCREEN_WIDTH;
        height = SCREEN_HEIGHT;
    }

    output_width = width;
    output_height = height;
    camera->Resize(static_cast<float>(width), static_cast<float>(height));
}


void Editor::SetClearColor(){
    r_clear_color.r = clear_color.x * 255;
    r_clear_color.g = clear_color.y * 255;
    r_clear_color.b = clear_color.z * 255;
    r_clear_color.a = clear_color.w * 255;
}

void Editor::SetupImGuiStyleColor(){

    //Unreal Engine style coloring, with global alpha. 
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
    //colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
}



Editor::~Editor(){
    for (auto tile_list: tile_cache){
        for (auto tile: tile_list.second){
            delete tile;
        }
    }
    delete cache;
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroySurface(icon);
    SDL_DestroyWindow(window);
}
