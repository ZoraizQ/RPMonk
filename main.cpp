#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "RPMonk.cpp"

using namespace std;

int main( )
{
    GameManager GM;
    int d, choice;

    string monkName, monkDesc;

    cout << "Starting a new game!" << endl;
    cout << "Enter your Monk's name: ";
    getline(cin, monkName);
    cout << "Enter a short description about your character." << endl;
    getline(cin, monkDesc);
    cout << "Your monk is now positioned in the dungeon at random coordinates." << endl;

    GM.overwriteMonkData(monkName, monkDesc); // change monk data to user input data

    while(true) //main game loop
    {
        cout << "1. Step in a direction\n2. View Player Status\n3. Meditate (Heal).\n";
        GM.printDungeon(); //prints dungeon every iteration of the game loop, to update it

        if (!GM.isPlayerAlive()) //check if player is alive in every game loop iteration
        {
            cout << "Player is dead. Game Over.\n";
            exit(1); //if dead, exit
        }

        cout << "What would you like to do?\n";
        cin >> choice;
        switch(choice)
        {
        case 1: //step in a direction
            cout << "Enter direction: ";
            cin >> d;
            if (!GM.playerMove(d))
            {
                cout << "Unable to move there.\n";
            }
            break;
        case 2: //showPlayerStatus
            GM.showPlayerStatus();
            break;
        case 3: //heal
            GM.healOutside();
            break;
        default:
            cout << "\nInvalid choice.\n"; //error handling
        }
        cout << '\n';
        system("pause"); //pause screen until next input
        system("cls"); //clear screen
    }
}
