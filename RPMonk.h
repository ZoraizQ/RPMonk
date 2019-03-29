#ifndef RPMONK_H
#define RPMONK_H
#include <iostream>
#include <string>

using namespace std;

struct Character
{
    string characterType; //Character name.
    string characterDesc; //Character description
    int HP; // Health points, death if 0, max HP varies between player and monsters
    int atk; //Attack damage of a character (Abbreviated as ATK Pts)

    Character()
    {
        characterType = "None";
        characterDesc = "";
        HP = 1;
    }
};

struct Room
{
    Character * c; // Character pointer to a character struct in memory.
    int RoomPosition[2]; // Coordinates pair for room
    bool isTreasure = false; // true if treasure room
    int isVisited = 0; // Could either be a 0 or a 1. Used in findRoom()

    Room* North; // a pointer to room north of current room etc..
    Room* West;
    Room* East;
    Room* South;

    Room() //default constructor
    {
        c = NULL;
        North = NULL;
        West = NULL;
        East = NULL;
        South = NULL; //initially all ptrs are NULL
        RoomPosition[0] = -1;
        RoomPosition[1] = -1;
    }

    // Links two Rooms' pointers (CONNECTED ROOMS ON MOVEMENT). 1 for North. 2 for West. 3 for East. 4 for South.
    // directions for room that calls the function i.e. if direction is 2 then this means that n should be linked with West.
    bool linkRoom(Room* n,int direction);
};


class GameManager
{

private:
    string** dungeon; // double ptr of string -- to store 2d array array of the entire dungeon map -- format of string = explored/character
    // Explored can be 0 or 1. Character can be P (player), G (goblin), B(bat) etc., N (no character).
    //Only explored rooms are present in memory.
    Room* head; //Used in findRoom to start search for rooms.
    Room* playerRoom; // A pointer to the Room which contains the player.

public:

    GameManager(); // Generates entire dungeon rooms, spawns player and monsters.

    ~GameManager(); // Destructor

    bool playerMove(int direction); // Move the player in given direction and changes the characterRoom and player position on dungeon.
    // it is possible that the player encounters some monster, is unable to move or dies in combat, but leaves the room to be explored.

    void findRoom(Room*& temp, Room* calling, int position[2], int direction); // Write an algorithm to find the Room with position equal to the input position. You have to write a recursive algorithm
    // for it and then save its address in the temp Room. To be used in playerMove
    // direction from which it is being called.
    // 0 for head. 1 for North. 2 for West. 3 for East. 4 for South.

    bool combat(Room* enemyRoom); // Used during function Returns true if the player has defeated the enemy (Make sure you NULL enemyRoom.c if enemy character loses)

    void viewEnemyStatus(Room* enemyRoom); // Shows the health and inventory of opposing enemy. Use in combat function.

    void useMove(Room* character,int choice, int atk); // Uses the move.

    void printDungeon(); // Print the dungeon discovered by the monk.
    // Call this function everytime playerMove is called just before returning from the function.

    void viewPlayerStatus(); // Displays the player health and inventory. Can be used in combat.

    bool isPlayerAlive(); // Returns if player is alive or not

    void healOutside(); //heal player outside combats by the healFactor given

    void overwriteMonkData(string monkName, string monkDesc); //overwrite monk data
};

#endif
