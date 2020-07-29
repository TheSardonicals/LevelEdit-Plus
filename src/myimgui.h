#include "headers.h"

#pragma once

#ifndef myimgui_h
#define myimgui_h

class MyGui{
    public:
        
        void ShowWindow(bool*);
        static void ShowAppObjects();
        static void ShowAppHelp();

        //Saved Function space (Use for later)
        static void ShowAppConsole(bool*);
        static void ShowAppLog(bool*);
        static void ShowAppLayout(bool* );
        static void ShowAppPropertyEditor(bool*);
        static void ShowAppLongText(bool*);
        static void ShowAppAutoResize(bool*);
        static void ShowAppConstrainedResize(bool*);
        static void ShowAppSimpleOverlay(bool*);
        static void ShowAppWindowTitles(bool*);
        static void ShowAppCustomRendering(bool*);
        static void ShowMenuFile();

        bool show_objects = false;

};

#endif /* myimgui_h */