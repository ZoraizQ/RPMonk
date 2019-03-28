#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "RPMonk.cpp"
using namespace std;

int main( )
{
    GameManager GM;
    int dir, choice;

    while(true)
    {
        cout << "1. Move\n2. Show Player Status\n3. Meditate (Heal).\n";
        GM.printDungeon();

        if (!GM.isPlayerAlive())
        {
            cout << "Player is dead. Game Over.\n";
            return 0;
        }
        cout << "What would you like to do?\n";
        cin >> choice;
        switch(choice)
        {
        case 1: //move
            cout << "Enter direction to move: ";
            cin >> dir;
            if (!GM.playerMove(dir))
            {
                cout << "Was unable to move to that spot.\n";
            }
            break;
        case 2: //showPlayerStatus
            GM.showPlayerStatus();
            break;
        case 3: //heal
            GM.healOutside();
            break;
        default:
            cout << "\nInvalid choice.\n";
        }
        cout << '\n';
        system("pause");
        system("cls");
    }
}
