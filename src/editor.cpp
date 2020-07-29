#include "headers.h"
#include "editor.h"

Editor::Editor(){}

int Editor::Start(int argc, char** argv){

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER);

    // Create Window and Renderer
    window = SDL_CreateWindow("LevelEdit Plus - by Sardonicals", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
            SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_FLAGS);
    if (!window){
        ShowError("LevelEdit++ Error!", "Couldn't create window: ", "Window creation failed!: ", true);
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!renderer){
        ShowError("LevelEdit Plus Error!", "Couldn't create renderer: ", "Renderer creation failed!: ", true);
        return 0;
    }

    // IMGUI Initialization.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.Fonts->AddFontFromFileTTF("misc/fonts/joystix.ttf", 16.0f);
    ImGui::StyleColorsClassic();
    ImGui_ImplSDL2_InitStandalone(window);
    ImGui_ImplSDLRenderer_Init(renderer);

    // VARIABLES
    state = EDITING;
    clear_color = ImVec4(.8627, .8627, .8627, 1);

    // OBJECTS
    //mouse = new Pointer();
    //cache = new TextureCache(renderer);
    //camera = new Camera();
    //submenu = new Submenu();

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
    bool show_window = true;
    bool show_another_window = false;

    //Event Loop
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT){
            running = false;
            break;
        }

        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)){
            running = false;
            break;
        }
            
    }

    //Application Loop
    switch (state) {
        case MENU:{}break;
        case EDITING:{
            // Start the Dear ImGui frame
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
            if (show_window)
                gui.ShowWindow(&show_window);

            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
            {
                static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                ImGui::Checkbox("Editor Window", &show_window);      // Edit bools storing our window open/close state

                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
                

                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            // 3. Show another simple window.
            if (show_another_window)
            {
                ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    show_another_window = false;

                ImGui::End();
            }

        }break;

        default:
            break;
    }
    SetClearColor();  
}
        
    //}

void Editor::Render(){

    SDL_SetRenderDrawColor(renderer, r_clear_color.r, r_clear_color.g, r_clear_color.b, r_clear_color.a);
    SDL_RenderClear(renderer);
    switch (state){
        case EDITING:{
            ImGui::Render();
            ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

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

Editor::~Editor(){

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}