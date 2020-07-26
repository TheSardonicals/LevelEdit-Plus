#include "headers.h"

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
        void handleEvents();
        void update();
        void Render();
        void clean();
        void Loop();

        vector<string> Get_Resources();
        vector<string> Get_Paths();

        int Start(int argc, char* args[]);

        bool running();

        int current_width = SCREEN_WIDTH, current_height = SCREEN_HEIGHT;


    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Event event;

        int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;

        Uint32 WINDOW_FLAGS = SDL_WINDOW_SHOWN;
        Uint32 RENDERER_FLAGS = SDL_RENDERER_PRESENTVSYNC;

        string current_item = NULL;
        string state = "NAMING";

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

        vector<string> tile_stack;
        

        Pointer* mouse;
        GameTile* tile;
        TextObject* text;
        Submenu* submenu;
        TextureCache* cache;
        Camera* camera;

        GameTile* ghost_tile;
        map<string, TextObject> menu_items;
        map<string, TextObject> editor_items;

        void Process();





};


#endif /* main_h */
