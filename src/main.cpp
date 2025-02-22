#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <chrono>

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "window.h"
#include "mainmenu.h"


void DrawSmoothRoundedRectangleLines(Rectangle rect, float roundness, int segments, float baseThickness, Color color) {
    // Draw several layers of lines with decreasing opacity to simulate anti-aliasing
    for (int i = 0; i < 5; i++) {  // Number of feather layers
        float thickness = baseThickness + i * 0.4f;  // Increase thickness for each layer
        Color layerColor = color;
        layerColor.a = (unsigned char)(color.a / (i + 2));  // Decrease opacity for each layer

        DrawRectangleRoundedLines(rect, roundness, segments, layerColor);
    }
}

void DrawTextWithColors(const std::string &text, Vector2 position, int fontSize, Font font, Color defaultColor, Color outlineColor) {
    // Words to highlight
    const std::vector<std::pair<std::string, Color>> keywords = {
        {"#include", GREEN},
        {"#define", GREEN},
        {"int", YELLOW},
        {"while", YELLOW},
        {"void", YELLOW},
        {"for", YELLOW},
        {"break", YELLOW},
        {"return", YELLOW},
             {"{", YELLOW},
        {"}", YELLOW},
        {"const", RED},
        {"()", BLUE},
        {"(", YELLOW},
        {")", YELLOW}
        }; 

    float xPos = position.x;  // Start x position for text
    float yPos = position.y;

    std::string word; // Temporary string to build each word
    Color wordColor = defaultColor;
    Color quoteColor = outlineColor;

    bool insideQuotes = false;
    
    for (size_t i = 0; i < text.length(); i++) {
        char currentChar = text[i];

        if (currentChar == '"') {
            if (!word.empty()) {
                DrawTextEx(font, word.c_str(), (Vector2){xPos, yPos}, fontSize, 2, wordColor);
                xPos += MeasureTextEx(font, word.c_str(), fontSize, 2).x + 2;  // Update xPos for next word
                word.clear();  // Clear word for next accumulation
            }

            // Toggle the quote mode and set the color
            insideQuotes = !insideQuotes;
            
            if (insideQuotes) {
                wordColor = insideQuotes ? quoteColor : defaultColor;
            }


            // Draw the quote character
            
            std::string quoteStr(1, '"');
            DrawTextEx(font, quoteStr.c_str(), (Vector2){xPos, yPos}, fontSize, 2, wordColor);
            xPos += MeasureTextEx(font, quoteStr.c_str(), fontSize, 2).x + 2;
            if (!insideQuotes) {
                wordColor = insideQuotes ? quoteColor : defaultColor;
            }
        }
        else if (currentChar == ' ' || currentChar == '\n') {
            // Draw the accumulated word if there is one
            if (!word.empty()) {
                DrawTextEx(font, word.c_str(), (Vector2){xPos, yPos}, fontSize, 2, wordColor);
                xPos += MeasureTextEx(font, word.c_str(), fontSize, 2).x + 2;  // Update xPos for next word
                word.clear();  // Clear word for next accumulation
                
                // Reset color if we're not inside quotes
                if (!insideQuotes) {
                    wordColor = defaultColor;
                }
            }

            if (currentChar == ' ') {
                xPos += 8;  // Advance xPos for space
            } else if (currentChar == '\n') {
                xPos = position.x;  // Reset xPos for new line
                yPos += fontSize + 4;  // Move down for new line
            }
        } else {
            word += currentChar;  // Accumulate characters into word
            
            // Check if the accumulated word matches any keywords
            for (const auto &keyword : keywords) {
                if (word == keyword.first) {
                    wordColor = keyword.second;
                    break;
                }
            }
        }
    }

    // Draw the last word if any remains
    if (!word.empty()) {
        DrawTextEx(font, word.c_str(), (Vector2){xPos, yPos}, fontSize, 2, wordColor);
    }
}

int currentLineDrawingWidth(const std::string& str) {
    int relativeWidth = 0;
    for (auto it = str.rbegin(); it != str.rend(); ++it) {
        if (*it == '\n') {
            break;
        }
        if (*it == ' ') {
            std::string tex;
            tex.push_back(' ');
            relativeWidth += 8;
        }
        else {
            std::string tex;
            tex.push_back((char)*it);
            relativeWidth += MeasureText(tex.c_str(), 20) + 2;
        }
    }
    return relativeWidth;
}

int countCharsUntilNewlineBackward(const std::string& str) {
    int count = 0;
    // Start from the end of the string and move backwards
    for (auto it = str.rbegin(); it != str.rend(); ++it) {
        if (*it == '\n') {
            break;  // Stop counting if a newline character is found
        }
        ++count;  // Increment the count for each character
    }
    return count;
}

bool canTabComplete(const std::string& str) {
    int count = 0;
    // Start from the end of the string and move backwards
    for (auto it = str.rbegin(); it != str.rend(); ++it) {
        if (*it == '\n') {
            return false;  // Stop counting if a newline character is found
        }
        if (*it == ' ') {
            ++count;  // Increment the count for each spacing character
        }
        else {
            return false;
        }
        if (count == 4) {
            return true;
        }
    }
    return false;
}


int countSpacingCharsUntilNewlineBackward(const std::string& str, bool otherCharactersPermitted) {
    int count = 0;
    // Start from the end of the string and move backwards
    for (auto it = str.rbegin(); it != str.rend(); ++it) {
        if (*it == '\n') {
            break;  // Stop counting if a newline character is found
        }
        if (*it == ' ') {
            ++count;  // Increment the count for each spacing character
        }
        else if (!otherCharactersPermitted) {
            //No other characters permitted
            return -1;
        }
    }
    return count;
}

int main() 
{
    using namespace std::chrono;
    
    auto now = system_clock::now();
    auto duration = now.time_since_epoch();
    auto lastBackSpaceTime = duration_cast<std::chrono::milliseconds>(duration)
              .count();
    // Initialization
    //--------------------------------------------------------------------------------------

    Window window("FLIPPERONE", 1280, 720);
    window.create();

    Font customFont = LoadFont("fonts/haxrcorp-4089.ttf");
    Font codingFont = LoadFont("fonts/JetBrainsMono-Regular.ttf");

    std::string subtitle;

    int framesCounter = 0;
// Define "LOGS" text position and size
    const char* buttonText = "LOGS";
    const int textSize = 20;
    const int textSpacing = 2;

        Color textBoxColor = (Color){ 30, 30, 30, 255 };    // Textbox background color
    Color textBoxBorderColor = (Color){ 255, 165, 0, 255 }; // Border of the textbox
        Color textColor = (Color){ 255, 255, 255, 255 };    // Text color


   float offsetX = 10.0F;
   float cursorX = 70.0F;
   float cursorY = 122.0F;
            float offsetY = 60.0F;

            float roundness = 0.03f;                 // How round the edges should be (0.0f to 1.0f)
            int segments = 30;    
            int gridSpacing = 25;
            Color gridColor = (Color){ 34, 16, 3, 255};
                const float gridThickness = 1.5f;
                    Color backgroundColor = (Color){ 10, 4, 0, 255 };

            Color outlineColor = (Color){ 254, 138, 44, 255 };


    // Buffer for the text input
    std::string inputText;

     // Timing variables for blinking cursor
    float blinkTime = 0.5f;  // Blink interval in seconds
    float cursorBlinkTimer = 0.0f;
    bool cursorVisible = true;

    //--------------------------------------------------------------------------------------
    bool writingText = true;

    // Window position and initial values
    int windowPosX = 100;
    int windowPosY = 100;
    Vector2 mouseOffset = { 0, 0 };
    bool dragging = false;

        float smoothing = 0.1f;

    int navigationButtonSize = 30;

    bool isMaximized = false;

    // Main game loop
    while (window.isRunning())    // Detect window close button or ESC key
    {
            Vector2 textPos = { 30, window.height - 40 };

        Rectangle buttonRect = { textPos.x - 10, textPos.y - 10, MeasureTextEx(customFont, buttonText, textSize, textSpacing).x + 20, textSize + 20 };

           Rectangle rect = {offsetX, offsetY, window.width - offsetX * 2, window.height - offsetY * 2};  // Position (x, y) and size (width, height)

    Rectangle textBox = { rect.x + 52, rect.y + 52, rect.width - 54, rect.height - 54};

        Rectangle xButton = { window.width - navigationButtonSize - 10, 10, navigationButtonSize, navigationButtonSize };
    Rectangle maximizeButton = { window.width - navigationButtonSize - 45, 10, navigationButtonSize, navigationButtonSize };
    Rectangle minimizeButton = { window.width - navigationButtonSize - 80, 10, navigationButtonSize, navigationButtonSize };



         // Update
        // Check if the left mouse button is pressed down to start dragging
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), textBox)) {
            dragging = true;
            mouseOffset = (Vector2){ GetMouseX() - windowPosX, GetMouseY() - windowPosY };
        }

        // If mouse is held down, move the window
        if (dragging) {
            // Get the target window position based on the mouse position
            int targetPosX = GetMouseX() - mouseOffset.x;
            int targetPosY = GetMouseY() - mouseOffset.y;

            // Smoothly interpolate the current position towards the target position
            windowPosX = windowPosX + (targetPosX - windowPosX) * smoothing;
            windowPosY = windowPosY + (targetPosY - windowPosY) * smoothing;
        }

        // Stop dragging if left mouse button is released or right button is pressed or window is out of focus
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || !IsWindowFocused()) {
            dragging = false;
        }
        // Update the window position manually (set it to the updated position)
        SetWindowPosition(windowPosX, windowPosY);


        bool xButtonClicked = CheckCollisionPointRec(GetMousePosition(), xButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        if (xButtonClicked) {
            break;  // Close the window when the "X" button is clicked
        }

        bool maximizeButtonClicked = CheckCollisionPointRec(GetMousePosition(), maximizeButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        if (maximizeButtonClicked) {
            if (!isMaximized) {
                window.width = GetMonitorWidth(0);
                window.height = GetMonitorHeight(0);
                                SetWindowPosition(0, 0);  // Position it at the top-left corner
                SetWindowSize(window.width, window.height);
                isMaximized = true;
            } else {
                // Restore the window to its original size
                window.width = 1280;
                window.height = 720;
                SetWindowSize(window.width, window.height);
                SetWindowPosition((GetMonitorWidth(0) - window.width) / 2, (GetMonitorHeight(0) - window.height) / 2);
                isMaximized = false;
            }   
        }   

         bool minimizeButtonClicked = CheckCollisionPointRec(GetMousePosition(), minimizeButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        if (minimizeButtonClicked) {
            MinimizeWindow(); 
        }   



        int key = GetCharPressed();  // Get the next character pressed (if any)
        while (key > 0) {  // While there are characters in the input queue 
         if (key >= 32 && key <= 125) {  // ASCII printable characters
                    inputText.push_back((char)key);
                    std::string keyText;
                    keyText.push_back((char)key);
                    if ((char)key == ' ') {
                        cursorX += 8;
                    }
                    else {
                        cursorX += MeasureText(keyText.c_str(), 20) + 2;
                    }
            }
            key = GetCharPressed();  // Get the next character pressed
        }

        if (IsKeyPressed(KEY_TAB)) {
            //Move by 4 spaces
            cursorX += 8 * 4;
            inputText += "    ";
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
            //Paste text from clipboard
            const char* clip = GetClipboardText();
            std::string clipboard(clip);
            
            std::vector<std::string> dt;
            std::stringstream ss(clipboard);
            std::string temp;
            while (std::getline(ss, temp)) {
                dt.push_back(temp);
            }
            for (int i = 0; i < dt.size(); i++) {
                if (i == 0) {
                    //Add to current cursor
                    cursorX += currentLineDrawingWidth(dt[i]);
                }
                else {
                    //Next line, reset
                    cursorX = 70.0F + currentLineDrawingWidth(dt[i]);
                    cursorY += 20;
                }
            }
            inputText += clipboard;
        }
        

          if (IsKeyDown(KEY_BACKSPACE)) {
             auto now = system_clock::now();
            auto duration = now.time_since_epoch();
            auto currentTime = duration_cast<std::chrono::milliseconds>(duration)
              .count();
            if (currentTime - lastBackSpaceTime >= 110 && inputText.size() > 0) {
                auto now = system_clock::now();
                auto duration = now.time_since_epoch();
                lastBackSpaceTime = duration_cast<std::chrono::milliseconds>(duration)
              .count();

                std::string keyText;
                bool isNewLine = inputText.back() == '\n';
                bool isSpace = inputText.back() == ' ';
                keyText.push_back(inputText.back());

                bool ctrl = IsKeyDown(KEY_LEFT_CONTROL);
                bool tabDelete = false;
                if (ctrl) {
                    //Destroy the entire line.
                    while (inputText.size() != 0) {
                        if (inputText.back() == '\n') {
                            inputText.pop_back();
                            isNewLine = true;
                            break;
                        }
                        inputText.pop_back();
                    }
                }
                else if (canTabComplete(inputText) && countCharsUntilNewlineBackward(inputText) != 0) {
                    tabDelete = true;   
                    printf("Yay!");
                     for (int i = 0; i < 4; i++) {
                            std::string temp;
                            temp.push_back(inputText.back());
                            if (inputText.back() == ' ') {
                                cursorX -= 8;
                            }
                            else {
                                cursorX -= (MeasureText(temp.c_str(), 20) + 2);
                            }
                            inputText.pop_back();
                        }
                }
                else {
                    inputText.pop_back();
                }

                if (isNewLine) {
                    //Back spacing a new line, push cursor Y up, reset cursor x
                    cursorY -= 20;
                    cursorX = 70.0f + currentLineDrawingWidth(inputText);
                }
                else {
                    if (tabDelete) {

                    }
                    else {
                        if (ctrl) {
                            cursorX = 70.0f;
                        }

                        if (isSpace) {
                            cursorX -= 8;
                        }
                        else {
                            cursorX -= (MeasureText(keyText.c_str(), 20) + 2);
                        }
                    }
                }
            }
        }

        bool isHovered = CheckCollisionPointRec(GetMousePosition(), buttonRect);  // Check if mouse is hovering over the button
        bool isClicked = isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);   // Check if mouse is clicked

        if (isClicked) {
            // Button action when clicked, for example:
            printf("LOGS button clicked!\n");
        }
        // Update
        //----------------------------------------------------------------------------------
        
        if (writingText) framesCounter++;
        else framesCounter--;

        if (framesCounter > 60 * 3) {
            writingText = false;
        }
        else if (framesCounter <= -30) {
            writingText = true;
        }


        if (IsKeyPressed(KEY_ENTER)) { 
            framesCounter = 0;
            inputText.push_back('\n');
            cursorX = 70.0f;
            cursorY += 20;
        }


        // Update the cursor blink timer
        cursorBlinkTimer += GetFrameTime();  // Increase timer by the frame time
        if (cursorBlinkTimer >= blinkTime) {
            cursorVisible = !cursorVisible;  // Toggle cursor visibility
            cursorBlinkTimer = 0.0f;  // Reset timer
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLANK);

            DrawSmoothRoundedRectangleLines(rect, roundness, segments, 3.0f, outlineColor);
             // Draw the filled rounded rectangle with a nearly black background
        DrawRectangleRounded(rect, roundness, segments, backgroundColor);

        

         main_menu_draw_background_grid(rect, gridSpacing, gridThickness, gridColor);

         subtitle = TextSubtext("FLIPPERONE.EXE", 0, framesCounter / 10);

                    // Draw the text box on the right side of the rounded rectangle
        DrawRectangleRec(textBox, BLACK);  // Draw the background of the text box
        DrawSmoothRoundedRectangleLines(textBox, roundness, segments, 2.0f, backgroundColor);
        //DrawRectangleLinesEx(textBox, 2, textBoxBorderColor);  // Draw the border of the text box

        // Draw the text inside the text box: handle multi-line text
        int lineHeight = 20;  // Height of each line of text
        int yOffset = textBox.y + 10;  // Starting position for drawing text (padding inside the box)

        // Loop through each line of the input text and draw it
        std::istringstream ss(inputText);
        std::string line;

        while (getline(ss, line)) {
            // Draw the current line
            DrawTextWithColors(line, (Vector2){textBox.x + 10, yOffset}, 20, GetFontDefault(), WHITE, outlineColor);
            // Move to the next line
            yOffset += lineHeight;
        }

        // Draw the blinking cursor if it's visible
        if (cursorVisible) {
            Vector2 cursorPosition = { cursorX, cursorY};
            DrawLine(cursorPosition.x, cursorPosition.y, cursorPosition.x, cursorPosition.y + 20, textColor);
        }




        //DrawTextEx(customFont, "PRESS [ENTER] to RESTART!", (Vector2){240, 260}, 20, 2, LIGHTGRAY);
        //DrawTextEx(customFont, "HOLD [SPACE] to SPEED UP!", (Vector2){239, 300}, 20, 2, LIGHTGRAY);


        DrawTextEx(customFont, buttonText, textPos, textSize, textSpacing, outlineColor);

         // Optionally, you can also draw the button's rectangle with a background color
        // This could be useful if you want the button to stand out
        if (isClicked) {
            DrawRectangleRec(buttonRect, (Color){ 255, 165, 0, 255 });  // Light background when hovered
        }else if (isHovered) {
            DrawRectangleRec(buttonRect, (Color){ 255, 165, 0, 50 });  // Light background when hovered
        }
        
        // Draw the rectangle outline
        DrawRectangleLinesEx(buttonRect, 2, outlineColor);  // Button outline

        main_menu_topbar(customFont, subtitle, minimizeButton, maximizeButton, xButton, outlineColor);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    UnloadFont(customFont);
    UnloadFont(codingFont);
    // De-Initialization
    //--------------------------------------------------------------------------------------
    window.destroy();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}