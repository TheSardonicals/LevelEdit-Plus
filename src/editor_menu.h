#pragma once
#include "headers.h"
#include "gametile.h"
#include "tojson.h"
#include "imgui_stdlib.h"

#ifndef editor_menu_h
#define editor_menu_h

class EditorMenu{
    public:
        bool hide_lower_options = false;
        bool edit_texture_window = false;
        EditorMenu(int *, int *, ImVec4 *, Pointer *, map<string, string> *, TextureCache *);
        ~EditorMenu();
        // The tile cache and the selection are taken by reference now, so the panels can
        // browse the level and change what is selected instead of working off of a copy.
        void Process(GameTile * &ghost_tile, Camera * camera, map<string, vector<GameTile * >> &, GameTile * &);

        // Window Functions

        void TileEditWindow(GameTile *, Camera *);
        void GhostTileWindow(GameTile *, Camera *);

        // Unity-style layout. Every panel pins itself to an edge of the viewport, which
        // leaves the middle of the screen free for the scene to be edited in.
        void MainMenuBar(GameTile * &, GameTile * &);
        void Toolbar(GameTile * &, GameTile * &);
        void HierarchyPanel(map<string, vector<GameTile *>> &, GameTile * &);
        void InspectorPanel(GameTile * &, GameTile *, Camera *);
        void ProjectPanel(GameTile * &);
        void StatusBar(map<string, vector<GameTile *>> &, GameTile *);
        void SceneOutline();

        // Pieces shared between the docked layout and the original floating menus.
        void AssetBrowser(GameTile * &, ImVec2);
        void Dialogs();
        void ClassicMenus(GameTile * &, Camera *, GameTile *);

        // Drops the brush, the same way the X key does.
        void ClearGhostTile(GameTile * &);

        string current_item = "";

        bool show_item_menu = false;
        float alpha = 1.0f;
        bool hide_stats = false;
        bool align_menu_to_screen = true;
        bool about_window = false;
        bool instruction_manual = false;
        bool saving_to_mx = false;
        bool save_to_mx = false;
        bool saving_mxpr = false;
        bool save_to_mxpr = false;
        bool loading_tileset = false;
        bool loading_project = false;
        bool tileset_import = false;
        bool tile_edit_mode = false;

        // Raised by the UI when the user asks for the selected tile to be removed. The
        // editor owns every tile in the tile cache, so it is the one that does the
        // deleting, the same way it handles the save/import flags.
        bool delete_selected_tile = false;

        // Docked layout on by default. Switch it off to get the original floating menus.
        bool unity_layout = true;

        int * window_width;
        int * window_height;

        string load_label = "../exports/";
        string tileset_name = "";
        string prlabel_name = "(.mxpr)";
        string project_name = "";
        string project_location = "";

        map<string, bool> editor_states;

        ImVec4 * clear_color;
        Pointer * mouse;
        map<string, SDL_Texture *> * tile_list;
        vector<vector<string>> tile_paths;
        ImVec4 original_button_color;
        TextureCache * cache;
        SDL_Renderer * renderer;

        //  Tile Properties

        int x_delta;
        int y_delta;
        int w_increase;
        int h_increase;

        //  Layout Metrics

        float hierarchy_width = 260.0f;
        float inspector_width = 320.0f;
        float project_height = 190.0f;
        float toolbar_height = 0.0f;
        float status_height = 0.0f;

        // What is left over in the middle once the panels have taken their edges. Kept
        // around so the editor can eventually clip the scene render to it.
        ImVec2 scene_pos = ImVec2(0.0f, 0.0f);
        ImVec2 scene_size = ImVec2(0.0f, 0.0f);


};
#endif
