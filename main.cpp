#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHImages.h"
#include "FEHRandom.h"
#include "math.h"

#define CHAR_HEIGHT 17
#define CHAR_WIDTH 12

bool StartGameConfigure();
int MapSelect();
int SelectDifficulty();
void ShowCredits();
void ShowStats();
void ShowInstructions();
void Game(int mapType, int meteoriteCount);
//Last game duration in MS
unsigned long gameDuration = 0;
int meteoriteSurvived = 0;
float gameSpeed = 0;

/*
Description:Class Button that creates an object button that is used to draw the button and check
if the button has been clicked. Returns the button if clicked 
Private: width, height, location, color, text color, text, image
Public: Constructor, Deconstructor, DrawButton function, Clicked function
Author: Andrew Popa 
*/
class Button {
    int x, y, width, height, buttonColor, textColor;
    char *text;
    FEHImage *pImage;
public: 
    /*
    Description: Constructor that initializes the values for the button
    Input: Top left x and y coordinate, height and width, button color, text to display, and image to display
    Output: N/A
    Author: Andrew Popa
    */
    Button (int _x, int _y, int _width, int _height, int _buttonColor, int _textColor, char* _text, char* imageName = NULL) {
        x = _x;
        y = _y;
        width = _width;
        height = _height;
        buttonColor = _buttonColor;
        textColor = _textColor;
        text = _text;
        //Inline if checking if the passed pointer for the image is null or not, and creating an image object if not NULL 
        pImage =  imageName ? new FEHImage(imageName) : NULL;
    }
    /*
    Description: Deconstructor that destroys image object because it remains in memeory and it is not deleted when object is deleted.
    When the object is destroyed, the pointer to the image is destroyed and not the image so the deconstructor must be 
    used to ensure the image object is destroyed, to free memory.
    Input: N/A
    Output: N/A
    Author: Andrew Popa
    */
    ~Button () {
        if (pImage)
            delete pImage;
    }
    /*
    Description: Draws out the button with the given parameters passed when the Button object is called
    Input: N/A
    Output: N/A, just displays to screen
    Author: Andrew Popa
    */
    void DrawButton() {
        //If the pointer to the image is not NULL, then draw the image in the coordinates shown
        if (pImage) 
            //x + 1 and y + 1, because the border takes a pixel
            pImage->Draw(x + 1, y + 1);
        LCD.SetFontColor(buttonColor);
        LCD.DrawRectangle(x, y, width, height);
        LCD.SetFontColor(textColor);
        /*Takes the length of the box and the length of the text and then subtracts them from each other, and divides
        this length by 2 to determine the distance between the centered text and the edge of the box, writing the text
        at the start of this margin*/
        LCD.WriteAt(text, x + (width - (strlen(text)*CHAR_WIDTH)) / 2, y + 2 + (height-CHAR_HEIGHT) / 2);
    }
    /*
    Description: Checks if the inputted values match with were a button was clicked.
    Input: X and Y coordinates of the click
    Output: True if clicked inside of the button it was called on
    Author: Andrew Popa
    */
    bool Clicked(int xClick, int yClick) {
        return xClick > x && xClick < x + width && yClick > y && yClick < y + height;
    }
};

/*
Description: Cycles through an array of objects of type Button and calls the DrawButton function
Input: Array of pointers to each Button
Output: N/A just draws the buttons
Author: Andrew Popa
*/
void DrawButtons(Button *pButtons[]) {
    for (int i = 0; pButtons[i] != NULL; i++)
        pButtons[i]->DrawButton();
}

/*
Description: Function checks if a button was clicked, and returns the button that was clicked
Input: Array of pointers to Buttons, bool if previous instance the button was pressed (used to check if released)
Output: button that was pressed
Author: Andrew Popa
*/
Button* CheckButtonsClick(Button *pButtons[], bool bWaitForClick) {
    Button *pReturnButtons = NULL;

    do {
        int x, y;
        static bool bPrevBtnPress = false;
        static int touchX = INT32_MAX, touchY = INT32_MAX;
        //Initial press recorded
        bool bCurrBtnPress = LCD.Touch(&x, &y);
        //If user just pressed the finger, record the press location
        if (bPrevBtnPress == false && bCurrBtnPress == true) {
            touchX = x;
            touchY = y;
        } else
        //If user just lifted the finger
        if (bPrevBtnPress == true && bCurrBtnPress == false) {
            //Check if any buttons was clicked when the finger was lifted
            for(int i = 0; pButtons[i] != NULL; i++) {
                //if the initial button clicked is in the same area  where the finger was lifted then return what button was clicked
                if (pButtons[i]->Clicked(x, y) && pButtons[i]->Clicked(touchX, touchY)) {
                    pReturnButtons = pButtons[i];
                    break;
                }
            }
        }
        bPrevBtnPress = bCurrBtnPress;
    //While loop is designed with bool bWaitForClick to run once if false, so the loop can be asynchronus if needed
    } while (bWaitForClick && pReturnButtons == NULL);

    return pReturnButtons;
}

/*
Description: Main is just the start menu function that branches out to all other screens
Input: N/A
Output: N/A
Author: Andrew Popa, Woojin Jeon
*/
int main() {
    //Display background and area and create 5 buttons of object Button
    FEHImage GalaxyBackground("GalaxyBackground.png");
    //Spacing for easier viewing*
    Button start_btn(       10,  60,  150, 70, WHITE, GOLDENROD, "Start");
    Button credits_btn(     165, 60,  150, 70, WHITE, BLUE,      "Credits");
    Button stats_btn(       10,  160, 150, 70, WHITE, GREEN,     "Stats");
    Button instructions_btn(165, 160, 150, 70, WHITE, PURPLE,    "Instructions");
    Button quit_btn(        265, 10,  50,  30, RED,   RED,       "Quit");
    //load all buttons into button array
    Button *buttons[] = {&start_btn, &credits_btn, &stats_btn, &instructions_btn, &quit_btn, NULL};

    while(true) {
        GalaxyBackground.Draw(0, 0);

        //Draw Menu Screen
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("Meteorite Dash", 65, 30);
        DrawButtons(buttons);

        //Wait for a button to be clicked
        Button *pButtonClicked = CheckButtonsClick(buttons, true);;

        //If else statment to determine which secondary menu to open based on return value of pButtonClicked 
        if (pButtonClicked == &start_btn) {
            if (StartGameConfigure()) {
                ShowStats();
            }
        }
        else if (pButtonClicked == &credits_btn) {
            ShowCredits();
        }
        else if (pButtonClicked == &stats_btn) {
            ShowStats();
        }
        else if (pButtonClicked == &instructions_btn) {
            ShowInstructions();
        }
        else if (pButtonClicked == &quit_btn) {
            break;
        }
    }
    return 1;
} 

/*
Description: Start game function that displays what maps to select and difficulty after map selected. This is 
the game setup function and the game follows afterwards. Game must be built after this function
Input: N/A
Output: True if game has started and false if not
Author: Andrew Popa
*/
bool StartGameConfigure()
{
    LCD.Clear(BLACK);
    FEHImage GalaxyBackground("GalaxyBackground.png");
    GalaxyBackground.Draw(0, 0);
    while (true) {
        //Call Map type function and if map type is default (back button pressed), then break and return to main
        int mapType = MapSelect();
        if (mapType == 0) break;
        //Select difficulty after maptype
        int meteoriteCount = SelectDifficulty();
        //If difficulty is either choice, then continue (Black screen shown so far, FIX ME with Game)
        if (meteoriteCount != 0) {
            Game(mapType, meteoriteCount);
            return true;
        } 
    }
    return false;
}

#define SLEEP_MS 10                 //Game Sleeps for 10 ms 
#define METEORITE_DEATH_MIN 0.2     //20% of life cicles
#define METEORITE_DEATH_MAX 0.8     //80% of life cicles
#define SPEED_START 0.5             //the initial speed
#define SPEED_INCREASE_SECONDS 1    //10 Seconds
#define SPEED_INCREASE_AMOUNT 0.0075 //Percentage speed the game increases by
#define METEORITE_RADIUS_MIN 4      //Meteorite min radius
#define METEORITE_RADIUS_MAX 14     //Meteorite max radius
#define RANDOM_MAX 32767            //Maximum random value using RANDOM
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define METEORITES_EASY 25          //Number of meteorites on easy mode
#define METEORITES_HARD 50          //Number of meteorites on hard mode
#define METEORITE_SPEED_MIN 0.75     //Percentage of gameSpeed
#define METEORITE_SPEED_MAX 1.25     //Percentage of gameSpeed
#define PLAYER_RADIUS 6         
#define PLAYER_Y_HEIGHT 6           //How many pixels from bottom of screen player is raised by

/*
Description: Meteorite function that controls the function of the meteorite such as the generation, or the
movment and redrawing 
Private: startDelay which is used for the first time the meteorites are initialized
Public: radius, x coordinate, y coordinate, speed, respawn delay, Constructor, meteorite reset function, check if alive function, check if just destroyed function
Author: Andrew Popa
*/
class Meteorite {

    int startDelay;

    public:
    //if respawnDelay > 0, then wait that many sleep
    int r, x, respawnDelay = 0;
    float y, speed;

    /*
    Description: Constructor that creates each meteorite with a random speed, size and position
    Input: N/A
    Output: N/A
    Author: Andrew Popa
    */
    Meteorite () {
        resetMeteorite();
        //Ensures that the meteorites will spawn uniformly distributed on the screen
        int AvgMeteoriteCycle = (SCREEN_HEIGHT - (METEORITE_RADIUS_MIN + METEORITE_RADIUS_MAX)) / speed + (DeathMin() + DeathMax()) / 2;
        //Creates the start delay for each meteorite that makes them uniformly distributed
        startDelay = Random.RandInt() * AvgMeteoriteCycle / RANDOM_MAX;
    }

    /*
    Description: Resets the meteorite once it has passed the death event
    Input: N/A
    Output: N/A, resets the meteorite position
    Author: Andrew Popa
    */
    void resetMeteorite() {
        //Generates a random speed for each meteorite
        speed = gameSpeed * (METEORITE_SPEED_MIN + METEORITE_SPEED_MAX * Random.RandInt() / RANDOM_MAX);
        r = METEORITE_RADIUS_MIN + (Random.RandInt() * (METEORITE_RADIUS_MAX - METEORITE_RADIUS_MIN)) / RANDOM_MAX;
        y = r + 2;
        x = r + (Random.RandInt() * (SCREEN_WIDTH - 2 - r - r))/RAND_MAX;
    }

    /*
    Description: Checks if the respawn delay is 0 or not
    Input: N/A
    Output: Bool true if delay is 0, False otherwise
    Author: Andrew Popa
    */
    bool IsAlive() {
        return respawnDelay == 0;
    }

    /*
    Description: Checks if the meteorite was just destroyed and decrements values of delays if 
    its not destroyed, and resets meteorites if there is no respawn delay
    Input: N/A
    Output: Return true if meteorite was just destroyed, else false
    Author: Andrew Popa
    */    
    bool runMeteorite () {
        bool bJustDestroyed = false;
        //If initial delay still exists, then decrement
        if (startDelay > 0)
            startDelay--;         
        //If meteorite dead, decrement respawnDelay
        else if ( !IsAlive() ) {
            respawnDelay--;
            //Respawn Event if meteorite respawnDelay is 0
            if (respawnDelay == 0)
                resetMeteorite();
        }
        //If meteorite alive then repaint it
        else if (y < SCREEN_HEIGHT - r) {
            LCD.SetFontColor(BLACK);
            LCD.FillCircle(x + 2, y - 2, r);
            LCD.SetFontColor(DARKSLATEGRAY);
            LCD.FillCircle(x + 1, y - 1, r);
            LCD.SetFontColor(LIGHTSLATEGRAY);
            LCD.FillCircle(x, y, r);
            y += speed;
        }
        else {
        //If just destroyed, then generate a respawn delay
            respawnDelay = (DeathMin() + (Random.RandInt() * (DeathMax() - DeathMin())) / RANDOM_MAX) / speed;
            bJustDestroyed = true;
        }
        return bJustDestroyed;
    }
    
    /*
    Description: Get the number of cycles the meteorite is on the screen
    Input: N/A
    Output: Number of cycles the meteorite is on the screen
    Author: Andrew Popa
    */
    float AliveCycles() {
        return (SCREEN_HEIGHT - 2 * r) / speed;
    }
    
    /*
    Description: calculates the minimum cycles a meteorite can have before death
    Input: N/A
    Output: Minimum number of cycles the meteorite can be on the screen
    Author: Andrew Popa
    */
    float DeathMin() {
        return AliveCycles() * METEORITE_DEATH_MIN;
    }
    
    /*
    Description: calculates the maximum cycles a meteorite can have before death
    Input: N/A
    Output: Maximum number of cycles the meteorite can be on the screen
    Author: Andrew Popa
    */
    float DeathMax() {
        return AliveCycles() * METEORITE_DEATH_MAX;
    }
};
 
/*
Description: Player class that dictates movement and collisions of the Player. It redraws the player when 
needed and checks where the player has moved. 
Private: touchDirection function that checks direction pressed, redrawPlayer function
Public: current x location, PlayerY location function DrawPlayer function
Author: Andrew Popa
*/
class Player {
    /*
    Description: From Game function, if the touch is not -1 (as given from Game if the screen is not touched), 
    then find which direction pressed (left or right)
    Input: x value of where touched
    Output: 1, if right, -1 if left, 0 if none
    Author: Andrew Popa
    */
    int touchDirection(int xTouch) {
        if (xTouch >= 0)
            //return 1 if right and -1 if left side touched
            return xTouch > SCREEN_WIDTH / 2 ? 1 : -1;
        //0 as default if no touch
        return 0;
    }
    
    /*
    Description: Checks which side has been touched and then increments or decrements player on x axis
    Input: x value of where touched
    Output: N/A, changes next x position of player
    Author: Andrew Popa
    */
    void redrawplayer(int xTouch) {
        int direction = touchDirection(xTouch);
        //If statment also checks if the player reaches the boundary of the screen
        if (xCurrent - PLAYER_RADIUS - 1 > 0 && direction == -1)
            xCurrent-=2;
        else if (xCurrent + PLAYER_RADIUS < SCREEN_WIDTH -2 && direction == 1)
            xCurrent+=2;
    }
    public:
    int xCurrent;

    /*
    Description: Constructor creating the start x value to 160
    Input: N/A
    Output: N/A, initializes start x value to midway of the screen
    Author: Andrew Popa
    */
    Player () {
        //Spawns player on center of screen
        xCurrent = 160;
    }
    
    /*
    Description: Returns the y value of the player
    Input: N/A
    Output: int of the y value of the player
    Author: Andrew Popa
    */
    int PlayerY () {
        return SCREEN_HEIGHT - PLAYER_RADIUS - PLAYER_Y_HEIGHT;
    }
    
    /*
    Description: Draws all parts of the player
    Input: x value of the player
    Output: N/A draws the player
    Author: Andrew Popa
    */
    void DrawPlayer(int xTouch) {
        redrawplayer(xTouch);
        LCD.SetFontColor(YELLOWGREEN);
        LCD.FillCircle(xCurrent, PlayerY(), PLAYER_RADIUS);
        LCD.SetFontColor(BLUEVIOLET);
        LCD.DrawPixel(xCurrent - 2, PlayerY() - 3);
        LCD.DrawPixel(xCurrent + 2, PlayerY() - 3);
        LCD.DrawHorizontalLine(PlayerY() + 2, xCurrent - 3, xCurrent + 4);
        LCD.DrawCircle(xCurrent, PlayerY(), PLAYER_RADIUS);
    }
};

/*
Description: Checks collisions for all meteorites by using the pythagorean theorem between the x and y distance of the radius of 
the player and meteorite and checks if its less than the current distance between both of them
Input: Number of meteorites, array of pointers to each Meteorite object, object of class Player
Output: True if there is a collision, false otherwise
Author: Andrew Popa
*/
bool checkCollisions(int meteoriteCount, Meteorite *pMeteorites[], Player player) {
    for (int i = 0; i < meteoriteCount; i++) {
        if ((float)sqrt(pow(player.xCurrent - pMeteorites[i]->x, 2) + pow((player.PlayerY() - pMeteorites[i]->y), 2)) <= (PLAYER_RADIUS + pMeteorites[i]->r) && pMeteorites[i]->IsAlive())
            return true;
    }
    return false;

}

/*
Description: Main game function that only executes after difficulty is selected. It calls all meteorite functions and player functions
Input: map type, number of meteorites
Output: N/A, just plays the game
Author: Andrew Popa
*/
void Game (int mapType, int meteoriteCount) 
{
    //Create a pointer to the background name and then depending on the maptype passed, assign the value of the pointer
    char *pBackgroundName = NULL;
    if (mapType == 1) 
        pBackgroundName = "Space.png";
    else if (mapType == 2) 
        pBackgroundName = "Lava.png";
    else if (mapType == 3) 
        pBackgroundName = "Mines.png";
    else    //mapType == 4, and the default map if pointer is not initialized
        pBackgroundName = "Desert.png";
    FEHImage background(pBackgroundName);
    
    //Create the default game speed, and the meteorites survived counter, alongside the cycle counter
    gameSpeed = SPEED_START;
    meteoriteSurvived = 0;
    //create an array of pointers to the meteorites
    Meteorite *pMeteorites[METEORITES_HARD];
    Player player;
    for (int i = 0; i < meteoriteCount; i++)
        pMeteorites[i] = new Meteorite();
    //Number of miliseconds since last game started
    unsigned long gameStart = TimeNowMSec();
    gameDuration = 1;

    while (true) {
        //Check if there is a touch, if there isn't assign xTouch to -1, so then there isnt a predefined touch when the loop executes again
        int xTouch ,yTouch;
        if (!LCD.Touch(&xTouch, &yTouch))
            xTouch = -1;

        //Clear the previous screen and draw the background then player
        LCD.Clear(BLACK);
        background.Draw(0,0);
        player.DrawPlayer(xTouch);
        
        /* In case a border for the game is desired
        LCD.SetFontColor(__________);
        LCD.DrawLine(0, 0, SCREEN_WIDTH - 1, 0);
        LCD.DrawLine(SCREEN_WIDTH - 1, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        LCD.DrawLine(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 0, SCREEN_HEIGHT - 1);
        LCD.DrawLine(0, SCREEN_HEIGHT - 1, 0, 0); */

        //For all the meteorites, if the meteorite has just died, then increment the number of meteorites survived
        for (int i = 0; i < meteoriteCount; i++)
            if (pMeteorites[i]->runMeteorite())
                meteoriteSurvived++;
        
        gameDuration = TimeNowMSec() - gameStart;
        //increment the number of cycles occured, and if the number of cycles divided by SPEED_INCREASE_SECONDS(100) is 0, then increase the speed of the meteorites
        gameSpeed = SPEED_START + SPEED_INCREASE_AMOUNT * 
            ( ( gameDuration / 1000 ) / SPEED_INCREASE_SECONDS );

        //sleep, causing the end of the cycle
        Sleep(SLEEP_MS);
        
        //check if there are any collisions and if there are then break from the cycle loop, ending the game 
        if(checkCollisions(meteoriteCount, pMeteorites, player))
            break;
    }
    //Explosion Animation where the circles increase in size and darken in color
    LCD.SetFontColor(TOMATO);
    LCD.FillCircle(player.xCurrent, player.PlayerY(), 16);
    Sleep(0.2);
    LCD.SetFontColor(RED);
    LCD.FillCircle(player.xCurrent, player.PlayerY(), 32);
    Sleep(0.2);
    LCD.SetFontColor(DARKRED);
    LCD.FillCircle(player.xCurrent, player.PlayerY(), 64);
    Sleep(0.2);
}

/*
Description: Selects which map for the backgroudn. Function is very similar to main in structure
Input: N/A
Output: int 1-4 of which maptype
Author: Andrew Popa
*/
int MapSelect() 
{
    //Draw Background and Title
    FEHImage GalaxyBackground("GalaxyBackground.png");
    GalaxyBackground.Draw(0, 0);
    LCD.SetFontColor(GOLDENROD);
    LCD.WriteAt("Choose Your Map", 65, 30);

    //Create Buttons objects
    Button space_btn( 10,  60,  150, 70, DARKTURQUOISE, DARKTURQUOISE, "Space",  "SpaceButton.png");
    Button lava_btn(  165, 60,  150, 70, ORANGERED,     ORANGERED,     "Lava",   "LavaButton.png");
    Button mines_btn( 10,  160, 150, 70, LIGHTGRAY,     LIGHTGRAY,     "Mines",  "MinesButton.png");
    Button desert_btn(165, 160, 150, 70, TAN,           TAN,           "Desert", "DesertButton.png");
    Button back_btn(  265, 10,  50,  30, WHITE,         WHITE,         "Back");

    //Load Buttons into array with button pointers.
    Button *buttons[] = {&space_btn, &lava_btn, &mines_btn, &desert_btn, &back_btn, NULL};
    DrawButtons(buttons);
    int mapType = 0;

    while(true) {
        Button *pButtonClicked = CheckButtonsClick(buttons, true);;

        //If else statment to determine which map to select based on return value of pButtonClicked 
        if (pButtonClicked == &space_btn) {
            mapType = 1;
            break;
        }
        else if (pButtonClicked == &lava_btn) {
            mapType = 2;
            break;
        }
        else if (pButtonClicked == &mines_btn) {
            mapType = 3;
            break;
        }
        else if (pButtonClicked == &desert_btn) {
            mapType = 4;
            break;
        }
        else if (pButtonClicked == &back_btn) {
            break;
        }
    }
    return mapType;
}

/*
Description: Selects which difficulty. Function is very similar to main in structure, just less buttons
Input: N/A
Output: int dictating number of meteorites.
Author: Andrew Popa
*/
int SelectDifficulty() 
{
    //Draw Background and title
    LCD.Clear(BLACK);
    FEHImage GalaxyBackground("GalaxyBackground.png");
    GalaxyBackground.Draw(0, 0);
    LCD.SetFontColor(GOLDENROD);
    LCD.WriteAt("Choose Your Difficulty", 28, 75);
    
    //Create Buttons objects and array
    Button easy_btn(10,  100, 150, 70, CYAN,   CYAN,   "Easy", "EasyButton.png");
    Button hard_btn(165, 100, 150, 70, YELLOW, YELLOW, "Hard", "HardButton.png");
    Button back_btn(265, 10,  50,  30, WHITE,  WHITE,  "Back");
    Button *buttons[] = {&easy_btn, &hard_btn,&back_btn, NULL};
    DrawButtons(buttons);
    int meteoriteCount = 0;
    
    while (true) {
        Button *pButtonClicked = CheckButtonsClick(buttons, true);;

        //If else statment to determine which difficulty to select based on return value of pButtonClicked
        if (pButtonClicked == &easy_btn) {
            meteoriteCount = METEORITES_EASY;
            break;
        }
        else if (pButtonClicked == &hard_btn) {
            meteoriteCount = METEORITES_HARD;
            break;
        }
        else if (pButtonClicked == &back_btn) {
            break;
        }
    }
    return meteoriteCount;
}

/*
Description: Shows the credits of the game in an animated way
Input: N/A
Output: N/A, the animated credits printed
Author: Andrew Popa
*/
void ShowCredits() 
{
    //Creates just back button and loads it in same button array template as main menu
    Button back_btn(265, 10, 50, 30, WHITE, WHITE, "Back");
    Button *buttons[] = {&back_btn, NULL};
    
    FEHImage GalaxyBackground("GalaxyBackground.png");
    
    //Starting values for text when it animates upwards
    int x = 10;
    int y = 150;
    /*While loop runs indefinitley and checks for a button clicked. This is loop permits the button to
    be asynchronus because it checks once if the button is pressed, then animates for the next frame, then repeats.
    If the button is pressed then it exits the whileloop/function*/
    while (true) {
        /*Checks once for a button to be clicked asynchronously by using the false button, so in the CheckButtonsClick function
        it won't run through the do while loop multiple times.*/
        Button *pButtonClicked = CheckButtonsClick(buttons, false);;
        if (pButtonClicked != NULL)
            break;

        //Draw whole screen and the back button
        LCD.Clear(BLACK);
        GalaxyBackground.Draw(0,0);
        LCD.SetFontColor(BLUE);
        LCD.WriteAt("Credits:",                     120, 30);
        LCD.WriteAt("Meteorite Dash created by:",   x,   y + 15 * 0);
        LCD.WriteAt("Andrew Popa",                  x,   y + 15 * 1);
        LCD.WriteAt("Woojin Jeon",                  x,   y + 15 * 2);
        LCD.WriteAt("Debugging contributors:",      x,   y + 15 * 3);
        LCD.WriteAt("Johanan Abraham",              x,   y + 15 * 4);
        LCD.WriteAt("Nicole Futoryansky",           x,   y + 15 * 5);
        DrawButtons(buttons);

        //If the animation has not reached the top, subtract a pixel from next drawing
        if(y > 50)
            y -= 1;

        LCD.Update();
        //Sleep for 0.01 seconds and update again. This is like the framerate
        Sleep(10);
    }
}

/*
Description: Shows the stats menu with the stats of the previous round
Input: N/A
Output: N/A, prints stats to screen
Author: Andrew Popa
*/
void ShowStats() {

    LCD.Clear(BLACK);
    FEHImage GalaxyBackground("GalaxyBackground.png");
    GalaxyBackground.Draw(0, 0);
    LCD.SetFontColor(GREEN);
    LCD.WriteAt("Stats:",             130, 30);
    LCD.WriteAt("Time Survived: ",    10,  50);
    LCD.WriteAt("Total Meteorites: ", 10,  70);
    LCD.SetFontColor(WHITE);
    LCD.WriteAt((float)gameDuration/1000, 220, 50);
    LCD.WriteAt(meteoriteSurvived, 220, 70);
    
    
    //Same Structured back button
    Button back_btn(265, 10, 50, 30, WHITE, WHITE, "Back");
    Button *buttons[] = {&back_btn, NULL};
    DrawButtons(buttons);
    
    Button *pButtonClicked = CheckButtonsClick(buttons, true);;
}

/*
Description: Shows the instructions for the game
Input: N/A
Output: N/A, just draws a screen
Author: Andrew Popa
*/
void ShowInstructions()
{   
    //Display the instructions 
    LCD.Clear(BLACK);
    FEHImage GalaxyBackground("GalaxyBackground.png");
    GalaxyBackground.Draw(0, 0);
    LCD.SetFontColor(PURPLE);
    int x = 10;
    int y = 30;
    LCD.WriteAt("Instructions:",            50, y+20*0);
    LCD.WriteAt("Move your player around",  x,  y+20*1);
    LCD.WriteAt("the map by touching each", x,  y+20*2);
    LCD.WriteAt("side of the screen based", x,  y+20*3);
    LCD.WriteAt("on direction. Make sure",  x,  y+20*4);
    LCD.WriteAt("that you don't hit the",   x,  y+20*5);
    LCD.WriteAt("falling objects. Enjoy!",  x,  y+20*6);

    //Same back button structure as ShowStats
    Button back_btn(265, 10, 50, 30, WHITE, WHITE, "Back");
    Button *buttons[] = {&back_btn, NULL};
    DrawButtons(buttons);

    Button *pButtonClicked = CheckButtonsClick(buttons, true);
}