#include "headers.h"
#include "editor_menu.h"
#include "texturecache.h"

using json = nlohmann::json;

#ifndef editor_h
#define editor_h

class Editor{

    public:
        Editor();
        ~Editor();

        void WindowState(SDL_Window*, SDL_Renderer*, bool);
        void Deleter(map<string, SDL_Renderer*>);
        void SavetoFile(string, string, SDL_Renderer*, int);
        void LoadMX();
        void Render();
        void Loop();
        void SetClearColor();
        void SaveMXProject(string, ToJson,  map<string, bool>);
        void LoadMXProject();

        // Removes every tile in the current selection from the tile cache and drops the
        // selection with it. The editor owns every tile in the cache, so the menu only
        // asks for the deletion and this does the freeing.
        void DeleteSelection();

        // Selection helpers. Selecting is additive when the user is holding Ctrl, which
        // toggles the tile in and out of the selection instead of replacing it.
        bool IsSelected(GameTile *);
        void Select(GameTile *, bool additive);
        void ClearSelection();

        int Start(int argc, char* args[]);
        ImVec4 clear_color;
        SDL_Color r_clear_color{};

        int current_width = SCREEN_WIDTH, current_height = SCREEN_HEIGHT;
        

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Surface * icon;
        
        int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;
        
        SDL_WindowFlags WINDOW_FLAGS = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
        //SDL_Render RENDERER_FLAGS = SDL_RENDERER_PRESENTVSYNC;

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
        bool import_finish = false;
        bool selection_mode = false;

        int current_x = 0, current_y = 0;

        float button_cooldown = 1.0;

        array<int, 2> tile_size = {32, 32};
        array<int, 2> pos_at_push = {0, 0};

        // OBJ
        EditorMenu * gui;
        Pointer * mouse;
        TextureCache * cache;
        map<string, string> tile_paths;
        GameTile * ghost_tile = NULL;

        // Everything currently selected. Order is selection order, so the last entry is
        // the one the inspector treats as the primary tile.
        vector<GameTile *> selected_tiles;

        // Rubber band box. Held in screen space, the same space the tile rects are in
        // once the camera offset has been applied.
        bool marquee_active = false;
        float marquee_start_x = 0, marquee_start_y = 0;
        SDL_FRect marquee_rect = {};
        map<string, vector<GameTile *>> tile_cache;
        Camera * camera; 
        KeyboardManager * keyboard;
        ToJson * json_handler;


        void SetupImGuiStyleColor();
        void Process();
        void SetKeyMapping();
};


#endif /* main_h */
