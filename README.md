Meteorite Dash
Overview
Meteorite Dash is an interactive game developed for the FEH Proteus platform. The player navigates a character to avoid falling meteorites, with customizable map backgrounds and difficulty levels. The game features a menu driven interface with options to start the game, view credits, check statistics, read instructions, or quit.
Features


Main Menu: Options to start the game, view credits, stats, instructions, or quit.
Map Selection: Choose from four map backgrounds (Space, Lava, Mines, Desert).
Difficulty Levels: Easy (25 meteorites) or Hard (50 meteorites).
Gameplay: Control the player by touching the left or right side of the screen to move and avoid meteorites.
Stats Tracking: Displays time survived and number of meteorites avoided from the last game.
Animated Credits: Credits scroll upward for a dynamic display.
Visuals: Uses FEHImage for backgrounds and buttons, with colorful graphics and animations (e.g., explosion effect on collision).


Requirements

Hardware: FEH Proteus with a touchscreen LCD.
Libraries:
FEHLCD.h
FEHUtility.h
FEHImages.h
FEHRandom.h
math.h


Assets: Image files for backgrounds and buttons (e.g., GalaxyBackground.png, SpaceButton.png, etc.).


Installation

Set up the FEH Proteus environment: Ensure the FEH Proteus development environment is configured.
Copy the code: Place the main.cpp file in your project directory.
Add image assets: Ensure all required image files (e.g., GalaxyBackground.png, Space.png, Lava.png, Mines.png, Desert.png, and button images) are available in the project directory.
Compile and upload: Use the FEH Proteus compiler to build and upload the program to the device.


How to Play

Main Menu: Select options using the touchscreen.
Start: Choose a map and difficulty, then play the game.
Credits: View the animated credits.
Stats: See the duration and number of meteorites survived from the last game.
Instructions: Read gameplay instructions.
Quit: Exit the game.


Gameplay:
Touch the left side of the screen to move the player left, or the right side to move right.
Avoid falling meteorites to survive as long as possible.
The game ends if a meteorite collides with the player, triggering an explosion animation.
Game speed increases over time, making it more challenging.


Code Structure

Button Class: Handles button creation, drawing, and click detection.
Meteorite Class: Manages meteorite generation, movement, and respawn logic.
Player Class: Controls player movement and drawing based on touchscreen input.
Game Function: Core game loop handling player movement, meteorite updates, collision detection, and speed progression.
Menu Functions: main, MapSelect, SelectDifficulty, ShowCredits, ShowStats, ShowInstructions manage navigation and UI.


Authors

Andrew Popa: Primary developer, implemented core game logic, UI, and classes.
Woojin Jeon: Contributed to the main menu implementation.
Debugging Contributors: Johanan Abraham, Nicole Futoryansky.


Notes

The game uses a 320x240 screen resolution.
Meteorite speed and size are randomized within defined ranges for varied gameplay.
The game loop runs at 10ms intervals, with speed increasing every second.
Ensure image assets are correctly named and placed to avoid runtime errors.
