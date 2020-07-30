#include "headers.h"
#include "editor_menu.h"
#include "texturecache.h"

#ifndef editor_h
#define editor_h

class Editor{

    public:
        Editor();
        ~Editor();

        void WindowState(SDL_Window*, SDL_Renderer*, bool);
        void Deleter(map<string, SDL_Renderer*>);
        void SavetoFile(string, string, SDL_Renderer*, int);
        void LoadFromFile(string, map<string, SDL_Texture*>, SDL_Renderer*);
        void Render();
        void Loop();
        void SetClearColor();

        int Start(int argc, char* args[]);
        ImVec4 clear_color;
        SDL_Color r_clear_color{};

        int current_width = SCREEN_WIDTH, current_height = SCREEN_HEIGHT;

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        
        int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;

        SDL_WindowFlags WINDOW_FLAGS = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        SDL_RendererFlags RENDERER_FLAGS = SDL_RENDERER_PRESENTVSYNC;

        SDL_Event event;

        enum EditorState{MENU = 0, LOADING = 1, NAMING = 2, EDITING = 3} state;

        char map_name;
        char filepath;

        bool running = true;
        bool paused = false;
        bool error = false;
        bool placement = false;
        bool increase_wh = false;
        bool removing_tile = false;
        bool show_size = true;

        int current_x = 0, current_y = 0;

        float button_cooldown = 1.0;

        array<int, 2> tile_size = {32, 32};
        array<int, 2> pos_at_push = {0, 0};

        

        // OBJ
        EditorMenu * gui;
        Pointer * mouse;
        TextureCache * cache;

        vector<string> tile_stack;
        map<string, string> tile_paths;

        void SetupImGuiStyleColor();
        void Process();
};


#endif /* main_h */
