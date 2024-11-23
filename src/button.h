#pragma once
#include <string>
#include <raylib.h>


class Button {
public:
    std::string content;
    Rectangle size;
private:
    bool highlighted;
    bool clicked;

public:

    bool isHighlighted() const {
        return highlighted;
    }

    bool isClicked() const {
        return clicked;
    }

    void update() {
        if (CheckCollisionPointRec(GetMousePosition(), size) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

        }
    }


    void render() {
        if (isClicked()) {

        }
        else if (isHighlighted()) {
            //DrawRectangleRec
        }
        else {

        }
    }
};