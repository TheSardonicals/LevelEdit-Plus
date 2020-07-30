#include "editor.h"

int main(int argc, char* args[]){
    Editor editor;
    
    if (!editor.Start(argc, args)){
        return -1;
    }else{
        editor.Loop();
    }
    return 0;
}