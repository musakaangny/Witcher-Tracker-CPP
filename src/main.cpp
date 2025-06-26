#include <iostream>
#include <string>
#include "WitcherTracker.h"

using namespace std;

/**
 * @brief C++ Implementation of the Witcher Inventory and Event Tracking System
 *
 * This program implements a command interpreter and inventory-event tracking system
 * for Geralt in the world of The Witcher using object-oriented design principles.
 * It handles input commands related to alchemy ingredients, potions, monster encounters,
 * and knowledge acquisition.
 */

/**
 * @brief Main program entry point - runs the Witcher tracking system command loop
 * @return 0 on successful program termination
 * 
 * Initializes the WitcherTracker system and enters an interactive command loop
 * that processes user input until EOF or "Exit" command is received.
 */
int main()
{
    // Initialize the main tracking system
    WitcherTracker tracker;
    string line;

    // Main command processing loop
    while (true)
    {
        // Display command prompt
        cout << ">> ";
        
        // Read complete line of user input
        getline(cin, line);

        // Check for program termination conditions
        if (cin.eof() || line == "Exit")
            break;

        // Process the command and handle invalid input
        int result = tracker.executeLine(line);
        if (result == -1)
        {
            cout << "INVALID\n";
        }
    }

    return 0;
}