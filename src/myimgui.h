#include "headers.h"

#pragma once

#ifndef myimgui_h
#define myimgui_h

class MyGui{
    public:
        void ShowWindow(bool*);
        static void ShowAppDocuments(bool*);
        static void ShowAppMainMenuBar();
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

};

#endif /* myimgui_h */