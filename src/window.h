#pragma once 
#include <string>
#include <raylib.h>

class Window {
    public:
    std::string title;
    int width;
    int height;

    Window(std::string title, int width, int height) {
        this->title = title;
        this->width = width;
        this->height = height;
    }

    void create() {
        InitWindow(width, height, title.c_str());
        SetWindowState(FLAG_WINDOW_UNDECORATED);
        SetTargetFPS(60);
    }

    bool isRunning() {
        return !WindowShouldClose();
    }

    void destroy() {
        CloseWindow();
    }
};