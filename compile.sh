#! /bin/sh
c++ src/*.cpp -o bin/LevelEdit++.x86_64 -lSDL2 -lstdc++fs
ln -s bin/LevelEdit++.x86_64
