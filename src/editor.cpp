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
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.Fonts->AddFontFromFileTTF("misc/fonts/joystix.ttf", 16.0f);
    SetupImGuiStyleColor();
    ImGui_ImplSDL2_InitStandalone(window);
    ImGui_ImplSDLRenderer_Init(renderer);

    // VARIABLES
    state = EDITING;
    clear_color = ImVec4(.8627, .8627, .8627, 1);
    tile_paths = GetPaths();

    // OBJECTS
    mouse = new Pointer();
    cache = new TextureCache(renderer);
    camera = new Camera(SCREEN_WIDTH, SCREEN_HEIGHT, 3, 40);
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
    //mouse->Process();

    //Event Loop
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        
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
            }
        }

    }

    //Application Loop
    switch (state) {
        case MENU:{}break;
        case EDITING:{
            // Start the Dear ImGui frame
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            gui->Process(ghost_tile, camera, tile_cache);
            mouse->Compute(&event);
            mouse->Process();
            keyboard->Process();

            // Keyboard Inputs
            if (keyboard->KeyIsPressed(SDL_SCANCODE_ESCAPE)){
            running = false;
            }

            if (keyboard->KeyIsPressed(SDL_SCANCODE_X)){
                ghost_tile = NULL;
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
                ghost_tile->SetPos(mouse->xpos, mouse->ypos);
            }

            // Handle every editor-related thing that works outside of the GUI underneath this conditional.
            if (!ImGui::GetIO().WantCaptureMouse){
                if (ghost_tile){
                    if (mouse->has_clicked){
                        if (tile_cache.count(ghost_tile->name) == 0){
                            tile_cache[ghost_tile->name] = {new GameTile(cache, tile_paths[ghost_tile->name], mouse->xpos - camera->xpos, mouse->ypos - camera->ypos, ghost_tile->w, ghost_tile->h)};
                        }
                        else{
                            tile_cache[ghost_tile->name].push_back(new GameTile(cache, tile_paths[ghost_tile->name], mouse->xpos - camera->xpos, mouse->ypos - camera->ypos, ghost_tile->w, ghost_tile->h));
                        }
                    }   
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
                json_handler->SaveToJson(gui->tileset_name, tile_cache);
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
        tile_cache.clear();
    }

    for (auto& tile : json_handler->json_blocks["tiles"].items()){
        //cout << tile.key() << endl;
        for (auto& locations : json_handler->json_blocks["tiles"][tile.key()]["locations"].items()){
            //cout << locations.value() << endl;
            //cout << json_handler->json_blocks["tiles"][tile.key()]["filepath"] << endl;
            if (tile_cache.count(tile.key()) == 0){
                //cout << "New Import " << tile.key() << endl; 
                tile_cache[tile.key().c_str()] = {new GameTile(cache, json_handler->json_blocks["tiles"][tile.key()]["filepath"], locations.value()[0], locations.value()[1], locations.value()[2], locations.value()[3])};
                //cout << tile_cache.count(tile.key()) << endl;
            } 
            else{
                //cout << "Adding to existing vector of " << tile.key() << endl;
                tile_cache[tile.key().c_str()].push_back(new GameTile(cache, json_handler->json_blocks["tiles"][tile.key()]["filepath"], locations.value()[0], locations.value()[1], locations.value()[2], locations.value()[3]));
                
            }
        }
    }

}
        


void Editor::Render(){

    SDL_SetRenderDrawColor(renderer, r_clear_color.r, r_clear_color.g, r_clear_color.b, r_clear_color.a);
    SDL_RenderClear(renderer);
    switch (state){
        case EDITING:{
            ImGui::Render();
            // Anything that should render before the imgui-based menu, render  underneath this line.
            if (tile_cache.size() > 0 || import_finish){
                for (auto tile_list: tile_cache){
                    for (auto tile: tile_list.second){
                        tile->Render({static_cast<int>(camera->xpos), static_cast<int>(camera->ypos)});
                    }
                }
            }
            camera->Show(renderer);
            // Send the data imgui stored from "Imgui::Render" to the screen using the specified render api.
            ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
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
    colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
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
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroySurface(icon);
    SDL_DestroyWindow(window);
}
