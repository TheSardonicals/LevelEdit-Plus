#! /bin/sh
c++ src/*.cpp -o bin/LevelEdit++.x86_64 -lSDL3 -lstdc++fs
unlink LevelEdit++.x86_64
ln -s bin/LevelEdit++.x86_64
