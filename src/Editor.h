#include "headers.h"

#ifndef Editor_h
#define Editor_h

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
        void render();
        void clean();

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

        string current_item = NULL;

        bool running = true;
        bool paused = false;

        Pointer* mouse;
        GameTile* tile;
        TextObject* text;
        Submenu* submenu;
        TextureCache* cache;
        Camera* camera;




};


#endif /* main_h */
