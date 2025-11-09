#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>

typedef struct {
    int r;
    int g;
    int b;
} RGBColor;

typedef struct {
    int x;
    int y;
} Point;

// Delay before starting farm
int initial_delay = 5;


Point convert_to_UC(int base_x, int base_y) {
    // Current screen resolution
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Base screen resolution
    const int BASE_SCREEN_WIDTH = 1366;
    const int BASE_SCREEN_HEIGHT = 768;

    // Converting
    double rel_x = (double)base_x / BASE_SCREEN_WIDTH;
    double rel_y = (double)base_y / BASE_SCREEN_HEIGHT;
    int target_x = (int)(rel_x * screenWidth);
    int target_y = (int)(rel_y * screenHeight);

    // Return a structure with universal coordinates
    Point new_point = {target_x, target_y};
    return new_point;
}


/**
 * @brief Returns the RGB color of the pixel at the specified screen coordinates (x, y).
 * @param x Horisontal coordinate (px)
 * @param y Vertical coordinate (px)
 * @return RGBColor structure. In case of an error (e.g., coordinates outside the screen)
 * returns {-1, -1, -1}.
 */
RGBColor get_pixel_color(int x, int y) {
    RGBColor color = {-1, -1, -1};

    HDC hdc = GetDC(NULL);
    if (hdc == NULL) {
        printf("Error: Failed to get DC screen.\n");
        return color;
    }

    // GetPixel returns a value of type COLORREF
    COLORREF pixelColorRef = GetPixel(hdc, x, y);

    // Very important: we free up the “canvas” that we took. If we don't do this, there will be a resource leak.
    ReleaseDC(NULL, hdc);

    // GetPixel returns CLR_INVALID if the coordinates are outside the screen
    if (pixelColorRef == CLR_INVALID) {
        printf("Error: coordinates (%d, %d) are outside the screen.\n", x, y);
        return color;
    }

    // Convert COLORREF (stored as 0x00BBGGRR) to the familiar RGB format using macros.
    color.r = GetRValue(pixelColorRef);
    color.g = GetGValue(pixelColorRef);
    color.b = GetBValue(pixelColorRef);

    return color;
}


/**
 * @brief Compares two RGB colors
 * @return 1 (true) if all fields (r, g, b) are the same
 * @return 0 (false) if at least one field differs
 */
int are_colors_equal(RGBColor color1, RGBColor color2) {
    // The expression below will automatically return 1 (true) 
    // if all three conditions are true, and 0 (false) otherwise.
    return (color1.r == color2.r && 
            color1.g == color2.g && 
            color1.b == color2.b);
}


/**
 * @brief Compares two colors taking into account tolerance (deviation)
 * @param color1
 * @param color2
 * @param tolerance Maximum deviation for EACH channel (R, G, B)
 * @return true (1) if the colors are similar, false (0) if not.
 */
bool are_colors_similar(RGBColor color1, RGBColor color2, int tolerance) {
    return (abs(color1.r - color2.r) <= tolerance &&
            abs(color1.g - color2.g) <= tolerance &&
            abs(color1.b - color2.b) <= tolerance);
}


/**
 * @brief Simulates a moving mouse to the specified screen coordinates (x, y)
 * @param x Horisontal coordinate (px)
 * @param y Vertical coordinate (px)
 */
void move(int x, int y) {
    // Current screen resolution
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Convert pixel coordinates (x, y) to absolute coordinates (0-65535)
    // MOUSEEVENTF_ABSOLUTE requires this normalized format.
    double absoluteX = (double)x / screenWidth * 65535.0;
    double absoluteY = (double)y / screenHeight * 65535.0;

    // moving mouse
    INPUT input_move = {0};
    input_move.type = INPUT_MOUSE;
    input_move.mi.dx = (LONG)absoluteX;
    input_move.mi.dy = (LONG)absoluteY;
    input_move.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input_move, sizeof(INPUT));
    Sleep(50); 
}


/**
 * @brief Simulates a left mouse click at the specified screen coordinates (x, y)
 * @param x Horisontal coordinate (px)
 * @param y Vertical coordinate (px)
 */
void click(int x, int y) {
    // Current screen resolution
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Convert pixel coordinates (x, y) to absolute coordinates (0-65535)
    // MOUSEEVENTF_ABSOLUTE requires this normalized format.
    double absoluteX = (double)x / screenWidth * 65535.0;
    double absoluteY = (double)y / screenHeight * 65535.0;

    // Clicking and releasing the mouse button
    INPUT input_down = {0};
    input_down.type = INPUT_MOUSE;
    input_down.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input_down, sizeof(INPUT));
    Sleep(100); 
    INPUT input_up = {0};
    input_up.type = INPUT_MOUSE;
    input_up.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input_up, sizeof(INPUT));
}


int main() {
    // Current screen resolution
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int choice, target_x = 0, target_y = 0, start_fight_x_coord = 0, start_fight_y_coord = 0, golden_line_x_coord = 0, golden_line_y_coord = 0;
    printf("\n----------------------------------------------------------------------------------------");
    printf("\n ____    ____  _____   ______     ______  _______     _____  _________   ______   ");
    printf("\n|_   \\  /   _||_   _|.' ____ \\  .' ___  ||_   __ \\   |_   _||  _   _  |.' ____ \\ ");
    printf("\n  |   \\/   |    | |  | (___ \\_|/ .'   \\_|  | |__) |    | |  |_/ | | \\_|| (___ \\_| ");
    printf("\n  | |\\  /| |    | |   _.____`. | |         |  __ /     | |      | |     _.____`.  ");
    printf("\n _| |_\\/_| |_  _| |_ | \\____) |\\ `.___.'\\ _| |  \\ \\_  _| |_    _| |_   | \\____) | ");
    printf("\n|_____||_____||_____| \\______.' `.____ .'|____| |___||_____|  |_____|   \\______.' ");
    printf("\n ____    ____       _        ______  _______      ___     ______                  ");
    printf("\n|_   \\  /   _|     / \\     .' ___  ||_   __ \\   .'   `. .' ____ \\   ");
    printf("\n  |   \\/   |      / _ \\   / .'   \\_|  | |__) | /  .-.  \\| (___ \\_| ");
    printf("\n  | |\\  /| |     / ___ \\  | |         |  __ /  | |   | | _.____`.  ");
    printf("\n _| |_\\/_| |_  _/ /   \\ \\_\\ `.___.'\\ _| |  \\ \\_\\  `-'  /| \\____) | ");
    printf("\n|_____||_____||____| |____|`.____ .'|____| |___|`.___.'  \\______.' ");
    printf("\n----------------------------------------------------------------------------------------");
    printf("\n1 - Gold farm");
    printf("\n\tREQUIREMENTS");
    printf("\n\t\tWindow game mode");
    printf("\n\t\tStarting position at the desired object");
    printf("\n\t\tStrong and powerfull miscrit");
    printf("\n\t\tDisabled battle animations");
    printf("\n----------------------------------------------------------------------------------------");
    printf("\n2 - Miscrit training (simple)");
    printf("\n\tREQUIREMENTS");
    printf("\n\t\tWindow game mode");
    printf("\n\t\tStarting position at the desired object");
    printf("\n\t\tStrong and powerfull miscrit on 1st slot");
    printf("\n\t\tMiscrit for training on 2nd slot");
    printf("\n\t\tDisabled battle animations");
    printf("\n----------------------------------------------------------------------------------------");
    printf("\n3 - Miscrit training (with platinum)");
    printf("\n\tREQUIREMENTS");
    printf("\n\t\tWindow game mode");
    printf("\n\t\tStarting position at the desired object");
    printf("\n\t\tStrong and powerfull miscrit on 1st slot");
    printf("\n\t\tMiscrit for training on 2nd slot");
    printf("\n\t\tDisabled battle animations");
    printf("\n\t\t34 platinum");
    while(1) {
        printf("\n----------------------------------------------------------------------------------------");
        printf("\nSelect farm type: ");
        if (scanf("%d", &choice) == 1) {
            if (choice == 1 || choice == 2 || choice == 3) {
                break; 
            }
            else {
                printf("\nWrong option! Enter a number 1, 2 or 3\n");
            }
        }
        else {
            printf("\nWrong option! Enter a number 1, 2 or 3\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }



    // Cycle for gold farm
    if(choice == 1) {
        printf("\nStarting gold farm...");

        // Base screen resolution
        const int BASE_SCREEN_WIDTH = 1366;
        const int BASE_SCREEN_HEIGHT = 768;

        // Delay before starting
        printf("\nStarting in %d seconds...", initial_delay);
        Sleep(initial_delay * 1000);

        int positions[3][2] = {{687, 339}, {414, 680}, {682, 584}};
        int positions_length = sizeof(positions) / sizeof(positions[0]);
        int intervals[] = {7000, 8000, 10000};
        int start_fight_coodinates[] = {510, 67};
        RGBColor start_fight_color = {226, 237, 255};

        // Start cycle
        while(1) {
            for(int i = 0; i < positions_length; i++) {
                // Setting universal fight coordinates
                start_fight_x_coord = convert_to_UC(start_fight_coodinates[0], start_fight_coodinates[1]).x;
                start_fight_y_coord = convert_to_UC(start_fight_coodinates[0], start_fight_coodinates[1]).y;
                // If fight NOT starting
                if(i == 1 && !are_colors_equal(get_pixel_color(start_fight_x_coord, start_fight_y_coord), start_fight_color)) {
                    while(1) {
                        // Waiting for cooldown an additional time
                        Sleep(intervals[2]+5000);

                        // Setting coodinates and converting to universal resolution
                        target_x = convert_to_UC(positions[0][0], positions[0][1]).x;
                        target_y = convert_to_UC(positions[0][0], positions[0][1]).y;

                        // Click on object with miscrit
                        move(target_x, target_y);
                        click(target_x, target_y);
                        Sleep(intervals[0]);

                        // Setting universal fight coordinates
                        start_fight_x_coord = convert_to_UC(start_fight_coodinates[0], start_fight_coodinates[1]).x;
                        start_fight_y_coord = convert_to_UC(start_fight_coodinates[0], start_fight_coodinates[1]).y;
                        // If fight is starting
                        if(are_colors_equal(get_pixel_color(start_fight_x_coord, start_fight_y_coord), start_fight_color)) {
                            // Setting coodinates and converting to universal resolution
                            target_x = convert_to_UC(positions[i][0], positions[i][1]).x;
                            target_y = convert_to_UC(positions[i][0], positions[i][1]).y;

                            // Click on miscrits ability on fight
                            move(target_x, target_y);
                            click(target_x, target_y);
                            Sleep(intervals[i]);
                            break;
                        }
                        else {
                            continue;
                        }
                    }
                }

                // Setting coodinates and converting to universal resolution
                target_x = convert_to_UC(positions[i][0], positions[i][1]).x;
                target_y = convert_to_UC(positions[i][0], positions[i][1]).y;

                // Click on object with miscrit or closing fight window
                move(target_x, target_y);
                click(target_x, target_y);
                Sleep(intervals[i]);
            }
        }
    }


    // Cycle for miscrit training
    if(choice == 2 || choice == 3) {
        if(choice == 2) printf("\nStarting miscrit training...");
        if(choice == 3) printf("\nStarting miscrit training with platinum...");
        
        // Base screen resolution
        const int BASE_SCREEN_WIDTH = 1366;
        const int BASE_SCREEN_HEIGHT = 768;

        // Delay before starting
        printf("\nStarting in %d seconds...", initial_delay);
        Sleep(initial_delay * 1000);

        int positions[3][2] = {{687, 339}, {414, 680}, {682, 584}};
        int positions_length = sizeof(positions) / sizeof(positions[0]);
        int intervals[3] = {7000, 10000, 10000};
        int start_fight_coodinates[] = {510, 67};
        int golden_line_coordinates[] = {565, 305};
        RGBColor start_fight_color = {226, 237, 255};
        RGBColor ready_to_train_color = {237, 188, 87};
        RGBColor new_ability_color = {103, 122, 144};
        RGBColor new_miscrit_evolution_color = {107, 138, 19};
        RGBColor new_user_level_color = {107, 138, 19};

        // Start cycle
        while(1) {
            bool ready_to_train = false;
            for(int i = 0; i < positions_length; i++) {
                // Setting universal fight coordinates
                start_fight_x_coord = convert_to_UC(start_fight_coodinates[0], start_fight_coodinates[1]).x;
                start_fight_y_coord = convert_to_UC(start_fight_coodinates[0], start_fight_coodinates[1]).y;
                // If fight NOT starting
                if(i == 1 && !are_colors_equal(get_pixel_color(start_fight_x_coord, start_fight_y_coord), start_fight_color)) {
                    while(1) {
                        // Waiting for cooldown an additional time
                        Sleep(intervals[2]+5000);

                        // Setting coodinates and converting to universal resolution
                        target_x = convert_to_UC(positions[0][0], positions[0][1]).x;
                        target_y = convert_to_UC(positions[0][0], positions[0][1]).y;

                        // Click on object with miscrit
                        move(target_x, target_y);
                        click(target_x, target_y);
                        Sleep(intervals[0]);

                        // Setting universal fight coordinates
                        start_fight_x_coord = convert_to_UC(start_fight_coodinates[0], start_fight_coodinates[1]).x;
                        start_fight_y_coord = convert_to_UC(start_fight_coodinates[0], start_fight_coodinates[1]).y;
                        // If fight is starting
                        if(are_colors_equal(get_pixel_color(510, 67), start_fight_color)) { // If fight is starting
                            // Setting coodinates and converting to universal resolution
                            target_x = convert_to_UC(positions[i][0], positions[i][1]).x;
                            target_y = convert_to_UC(positions[i][0], positions[i][1]).y;

                            // Click on miscrits ability on fight
                            move(target_x, target_y);
                            click(target_x, target_y);
                            Sleep(intervals[i]);
                            break;
                        }
                        else {
                            continue;
                        }
                    }
                }
                
                // Setting universal golden line coordinates
                golden_line_x_coord = convert_to_UC(golden_line_coordinates[0], golden_line_coordinates[1]).x;
                golden_line_y_coord = convert_to_UC(golden_line_coordinates[0], golden_line_coordinates[1]).y;
                // If there is a gold line on the miscrit after the battle, set the parameter to “ready to train”
                if(i == 2 && are_colors_similar(get_pixel_color(golden_line_x_coord, golden_line_y_coord), ready_to_train_color, 10)) {
                    ready_to_train = true;
                }

                // Setting coodinates and converting to universal resolution
                target_x = convert_to_UC(positions[i][0], positions[i][1]).x;
                target_y = convert_to_UC(positions[i][0], positions[i][1]).y;
                // Click on object with miscrit, fight ability or closing fight window
                move(target_x, target_y);
                click(target_x, target_y);
                Sleep(intervals[i]);

                // If miscrit ready to train
                if(ready_to_train) {
                    // click on blue button
                    move(convert_to_UC(614, 55).x, convert_to_UC(614, 55).y);
                    click(convert_to_UC(614, 55).x, convert_to_UC(614, 55).y);
                    Sleep(2000);
                    // select miscrit
                    move(convert_to_UC(461, 222).x, convert_to_UC(461, 222).y);
                    click(convert_to_UC(461, 222).x, convert_to_UC(461, 222).y);
                    Sleep(1000);
                    // click train
                    move(convert_to_UC(697, 154).x, convert_to_UC(697, 154).y);
                    click(convert_to_UC(697, 154).x, convert_to_UC(697, 154).y);
                    Sleep(1000);
                    // if was setting training WITHOUT platinum
                    if(choice == 2) {
                        // double closing train window
                        move(convert_to_UC(796, 625).x, convert_to_UC(796, 625).y);
                        click(convert_to_UC(796, 625).x, convert_to_UC(796, 625).y);
                        Sleep(1000);
                        move(convert_to_UC(796, 625).x, convert_to_UC(796, 625).y);
                        click(convert_to_UC(796, 625).x, convert_to_UC(796, 625).y);
                        Sleep(1000);
                    }
                    // if was setting training WITH platinum
                    if(choice == 3) {
                        // double click on training with platinum
                        move(convert_to_UC(620, 626).x, convert_to_UC(620, 626).y);
                        click(convert_to_UC(620, 626).x, convert_to_UC(620, 626).y);
                        Sleep(1000);
                        move(convert_to_UC(620, 626).x, convert_to_UC(620, 626).y);
                        click(convert_to_UC(620, 626).x, convert_to_UC(620, 626).y);
                        Sleep(3000);
                        move(convert_to_UC(683, 626).x, convert_to_UC(683, 626).y);
                        click(convert_to_UC(683, 626).x, convert_to_UC(683, 626).y);
                        Sleep(1000);
                    }
                    // if miscrit has received a new ablity, close the window
                    if(are_colors_equal(get_pixel_color(convert_to_UC(791, 342).x, convert_to_UC(791, 342).y), new_ability_color)) {
                        move(convert_to_UC(792, 503).x, convert_to_UC(792, 503).y);
                        click(convert_to_UC(792, 503).x, convert_to_UC(792, 503).y);
                        Sleep(1000);
                    }
                    // if miscrit has received a new evolution, close the window
                    if(are_colors_equal(get_pixel_color(convert_to_UC(479, 164).x, convert_to_UC(479, 164).y), new_miscrit_evolution_color)) {
                        move(convert_to_UC(682, 591).x, convert_to_UC(682, 591).y);
                        click(convert_to_UC(682, 591).x, convert_to_UC(682, 591).y);
                        Sleep(1000);
                    }
                    // closing general train window
                    move(convert_to_UC(980, 123).x, convert_to_UC(980, 123).y);
                    click(convert_to_UC(980, 123).x, convert_to_UC(980, 123).y);
                    Sleep(1000);
                    // if player has reached a new level, close the window
                    if(are_colors_equal(get_pixel_color(convert_to_UC(575, 237).x, convert_to_UC(575, 237).y), new_user_level_color)) {
                        move(convert_to_UC(684, 516).x, convert_to_UC(684, 516).y);
                        click(convert_to_UC(684, 516).x, convert_to_UC(684, 516).y);
                        Sleep(1000);
                    }
                    ready_to_train = false;
                }
            }
        }
    }
    
    return 0;
}