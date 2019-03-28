#ifndef RPMONK_H
#define RPMONK_H
#include <iostream>
#include <string>

using namespace std;

struct Character
{
    string characterType; // A string that would be representing the character. A character could be a "Player" or a "Minion".
    int HP; // Health points, death if 0, max HP capped to 100
    int atk; //Attack damage of a character (Abbreviated as ATK Pts)

    Character()
    {
        characterType = "None";
        HP = 100;
    }
};

struct Room
{
    Character *c; // Can host a character.
    int RoomPosition[2]; // Position of Room on the dungeon
    bool isTreasure = false; // true if treasure room
    int isVisited = 0; // Could either be a 0 or a 1. This could be helpful when writing the search algorithm.

    Room* North; // a pointer to adjacent Room on north.
    Room* West;  // a pointer to adjacent Room on west.
    Room* East;  // a pointer to adjacent Room on east.
    Room* South; // a pointer to adjacent Room on south.

    Room()
    {
        c = NULL;
        RoomPosition[0] = -1;
        RoomPosition[1] = -1;
        North = NULL;
        West = NULL;
        East = NULL;
        South = NULL;
    }

    // Links two Rooms. 1 for North. 2 for West. 3 for East. 4 for South.
    // Directions are for the Room calling the function i.e. if direction is 1 then this means that n should be linked with North.
    bool linkRoom(Room* n,int direction);
};


class GameManager
{

private:
    string** dungeon; // A 2d array of strings to show what a new Room will contain. format of string = Explored/Character
    // Explored can be 0 or 1. Character can be P (player), M (minion), N (no character).
    // Explored basically keeps track of the Rooms which have been created in the memory and using it you have to link it with the other adjacent Rooms.
    Room* head; // Use this to find all the other Rooms in the game.
    Room* playerRoom; // A pointer to the Room which contains the player.

public:

    GameManager(); // Starts the game with a Player character in the head Room and initializes the positions for player & minions on the dungeon.

    ~GameManager(); // Deallocate all the dynamic memory.

    bool playerMove(int direction); // Moves the player in a certain direction and changes the characterRoom and player position on dungeon accordingly.
    // 1 for North. 2 for West. 3 for East. 4 for South
    // it is possible that the player encounters some minion/boss and refuses to move or dies while fighting. In any case a new Room has been explored.
    void findRoom(Room*& temp, Room* calling, int position[2], int direction); // Write an algorithm to find the Room with position equal to the input position. You have to write a recursive algorithm
    // for it and then save its address in the temp Room. To be used in playerMove
    // direction from which it is being called.
    // 0 for head. 1 for North. 2 for West. 3 for East. 4 for South.

    bool fight(Room* enemyRoom); // Use this function in playerMove to fight the boss or a minion. Returns true if the player has defeated the enemy (Make sure you NULL enemyRoom.c if enemy character loses)
    // DO NOT NULL characterRoom.c (Once a player has died the game would end and died player remains in that place)
    void showEnemy (Room* enemyRoom); // Shows the health and inventory of opposing enemy. Use in fight function.

    void useMove(Room* character,int choice, int atk); // Uses the move.

    void printDungeon(); // Print the dungeon discovered by the monk.
    // Call this function everytime playerMove is called just before returning from the function.

    void showPlayerStatus(); // Displays the player health and inventory. Can be used in fight.

    bool isPlayerAlive(); // Returns if player is alive or not

    void healOutside(); //heal player outside fights by the healFactor given
};

#endif
