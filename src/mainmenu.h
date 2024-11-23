#pragma once 
#include <raylib.h>
#include <random>

void main_menu_draw_background_grid(Rectangle& rect, const float& gridSpacing, const float& gridThickness, Color& gridColor) {

         // Draw the thick grid inside the rounded rectangle
        for (int x = (int)rect.x; x < rect.x + rect.width; x += gridSpacing) {
            // Ensure the line stays inside the rounded rectangle
            if (x > rect.x && x < rect.x + rect.width) {
                DrawLineEx((Vector2){x, rect.y}, (Vector2){x, rect.y + rect.height}, gridThickness, gridColor);
            }
        }

        for (int y = (int)rect.y; y < rect.y + rect.height; y += gridSpacing) {
            // Ensure the line stays inside the rounded rectangle
            if (y > rect.y && y < rect.y + rect.height) {
                DrawLineEx((Vector2){rect.x, y}, (Vector2){rect.x + rect.width, y}, gridThickness, gridColor);
            }
        }
}

void main_menu_topbar(Font& font, std::string& subtitle, Rectangle& minimizeButton, Rectangle& maximizeButton, Rectangle& xButton, Color& outlineColor) {
    //Draw title bar "FLIPPERONE.exe text"
          std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> rand6(1, 40); // distribution in range [1, 6]
    DrawTextEx(font, subtitle.c_str(), (Vector2){210 + (rand6(rng) == 40 ? 5 : 0), 30}, 20, 2, outlineColor);
        


     if (CheckCollisionPointRec(GetMousePosition(), xButton)) {
            DrawRectangleRec(xButton, RED);  // Button background (red color for the "X" button)
        } 
        else {
            //DrawRectangleRec(xButton, outlineColor);  // Button background (red color for the "X" button)
        }
        DrawText("X", xButton.x + 8.9f, xButton.y + 5, 20, outlineColor);  // Draw "X" inside the button


         if (CheckCollisionPointRec(GetMousePosition(), maximizeButton)) {
            DrawRectangleRec(maximizeButton, RED);  
        } 
        
        DrawText("O", maximizeButton.x + 8.9f, maximizeButton.y + 5, 20, outlineColor); 


        if (CheckCollisionPointRec(GetMousePosition(), minimizeButton)) {
            DrawRectangleRec(minimizeButton, RED);  
        } 
        
        DrawText("_", minimizeButton.x + 8.9f, minimizeButton.y + 5, 20, outlineColor); 
}