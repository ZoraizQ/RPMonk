#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <fstream>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>
#include "RPMonk.h"

using namespace std;

//Monk player
const int const_p_maxHP = 15;
const int const_p_ATK = 3;
const int const_p_healFactor = 5;

//Goblin
const int const_g_maxHP = 10;
const int const_g_ATK = 2;

//Bat
const int const_b_maxHP = 5;
const int const_b_ATK = 1;

//Max map size
const int const_n = 5;

//Relative dungeon length
const int dung_max = 10;

string p_Name = "";
string p_Desc = "";
string g_Name = "Goblin";
string g_Desc = "Green monsters possessing daggers.";
string b_Name = "Bat";
string b_Desc = "Bats are mammals of the order Chiroptera.";

struct coord{
    int x;
    int y;
};

void GameManager::overwriteMonkData(string monkName, string monkDesc){
    playerRoom->c->characterType = monkName;
    playerRoom->c->characterDesc = monkDesc;
}

void setConsoleColour(int colour)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    cout.flush();
    SetConsoleTextAttribute(hOut, colour);
}

coord updateCoordWithDirection(int direction, coord old_coord){ // calculates new coordinates from old coordinates by arithmetic according to the direction given (one step ahead)
    coord new_coord;
    switch(direction)
    {
    case 1: //North
        new_coord.x = old_coord.x-1;
        new_coord.y = old_coord.y;
        break;
    case 2: //West
        new_coord.x = old_coord.x;
        new_coord.y = old_coord.y-1;
        break;
    case 3: //East
        new_coord.x = old_coord.x;
        new_coord.y = old_coord.y+1;
        break;
    case 4: //South
        new_coord.x = old_coord.x+1;
        new_coord.y = old_coord.y;
        break;
    }
    return new_coord;
}


GameManager::GameManager()
{
    int n = const_n, i,j;
    Room* newRoom = new Room(); // new room created in memory
    Character* playerchar = new Character(); //new character created in memory
    // n x n dungeon generated
    dungeon = new string*[n];
    for (i = 0; i < n; i++)
    {
        dungeon[i] = new string[n];
    }
    //initialized to unexplored '0', '-' for does not exist, NULL 'N' no character
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            dungeon[i][j] = "-/N"; // all slots are initialized such , - represents WALL, N represents NO CHARACTER
        }
    }
    srand(time(NULL)); //use time(NULL) as seed for random generator

    coord playerCoord;
    playerCoord.x = rand()%const_n; //n = 7, 0-7
    playerCoord.y = rand()%const_n; // 0-7
    playerchar->characterType = p_Name; //properties to character
    playerchar->characterDesc = p_Desc;
    playerchar->HP = const_p_maxHP;
    playerchar->atk = const_p_ATK;
    newRoom->c = playerchar; //newRoom for player
    newRoom->RoomPosition[0] = playerCoord.x;
    newRoom->RoomPosition[1] = playerCoord.y;
    head = newRoom; //player's room is the head
    playerRoom = newRoom; //member variable

    coord current_coord = playerCoord;
    //player room NOT added to the list of existing rooms

    int numberOfRooms = rand()%5+dung_max; //numberOfRooms
    cout << "Generating rooms...";
    coord new_coord;
    int timeout = 20;
    while(numberOfRooms != 0){
        GENERATE_NEW_COORD:
        int randDir = (rand()%4) + 1;
        new_coord = updateCoordWithDirection(randDir, current_coord);
        i = new_coord.x;
        j = new_coord.y;
        if (i >= const_n || i < 0 || j >= const_n || j < 0 || dungeon[i][j][0] == '0'){    //generate again.
            goto GENERATE_NEW_COORD;
        }
        dungeon[i][j][0] = '0';
        current_coord = new_coord;

        //cout << "Room generated at " << new_coord.x << ',' <<  new_coord.y << endl;
        //cout << numberOfRooms << endl;
        numberOfRooms--;
        timeout--;
        if (timeout == 0)
            break;
    }

    //spawns player at playerCoord by just editing the dungeon string
    dungeon[playerCoord.x][playerCoord.y] = "1/P"; //add the player in the end so it does not get overwritten by a 0

    int task = 0;
    while(task != 3)
    {
        G2:
        i = rand()%const_n;
        j = rand()%const_n;
        if (i >= const_n || i < 0 || j >= const_n || j < 0)
        {   //generate again.
            goto G2;
        }
        //since tasks 0 to 5 are characters, they can stand over items as well, so from tasks 6 onwards we only check if there is an empty item node for the items except that of the Boss
        if (dungeon[i][j][0] == '0')  //separate conditions for character and item tasks
        {
            switch(task)
            {
            case 0: //task 0 - - assign goblin
                dungeon[i][j] = "0/G";
                break;
            case 1:
                dungeon[i][j] = "0/B"; // assign bat
                break;
            case 2:
                dungeon[i][j] += "T"; // assign treasure room
                break;
            }
            task++;
        }
    }
}

GameManager::~GameManager()
{     //delete all existing Rooms and their contents by finding their address using findRoom
    int p[2];
    for (int i = 0; i < const_n; i++)
    {
        for (int j = 0; j < const_n; j++)
        {
            if (dungeon[i][j][0] == '1')
            {
                p[0] = i;
                p[1] = j;
                Room* del;
                findRoom(del, head, p, 0);
                if (del != NULL)
                {
                    //del now contains the Room to be deleted
                    if (del->c != NULL)
                    {
                        delete del->c;
                        del->c = NULL;
                    }
                    delete del;
                }
            }
        }
    }

    head = NULL;
    //delete head;
    playerRoom = NULL;
    for (int i = 0; i < const_n; i++)
    {
        delete dungeon[i];
    }
    delete dungeon;
    dungeon = NULL;
}

void GameManager::viewEnemyStatus(Room* enemyRoom)
{
    cout << "\n[Enemy Status]\nEnemy: " << enemyRoom->c->characterType << "\nHP: "<< enemyRoom->c->HP << '\n';
}


void GameManager::printDungeon()
{ //2d dungeon array of strings print with colors
    setConsoleColour(15);

    for (int i = 0; i < const_n; i++)
    {
        for (int j = 0; j < const_n; j++)
        {
            setConsoleColour(8);
            if (dungeon[i][j][0] == '1') setConsoleColour(2);
            if (dungeon[i][j][2] == 'P') setConsoleColour(10);
            if (dungeon[i][j][2] == 'G' || dungeon[i][j][2] == 'B') setConsoleColour(4);
            if (dungeon[i][j][0] == '-'){
              setConsoleColour(3);
            cout << "WWW      ";
            }
            else{
            cout << dungeon[i][j];
            if (dungeon[i][j].length() > 3){
                cout << "     ";
            }
            else{
                cout << "      ";
            }
            }
        }
        cout << endl;
    }
    setConsoleColour(15);
}

bool GameManager::isPlayerAlive()
{
    if (playerRoom->c->HP <= 0) return false;
    else return true;
}

void GameManager::viewPlayerStatus()
{
    cout << "\n[Player Status]\nPlayer: " << playerRoom->c->characterType << "\nHP: "<< playerRoom->c->HP << '\n' << playerRoom->c->characterDesc << '\n';
}

bool Room::linkRoom(Room* n, int direction)
{
    if (n != NULL)
    {
        switch(direction)
        {
        case 1:
            North = n;
            return true;
            break;
        case 2:
            West = n;
            return true;
            break;
        case 3:
            East = n;
            return true;
            break;
        case 4:
            South = n;
            return true;
            break;
        }
    }
    else
    {
        return false;
    }
}

bool GameManager::playerMove(int direction)
{
    int px, py, nx, ny; //px,py position coordinates of playerRoom, nx,ny position coordinates of nextRoom
    Room* nextRoom = NULL;
    Character* nextChar = NULL;
    px = playerRoom->RoomPosition[0];
    py = playerRoom->RoomPosition[1];
    switch(direction)
    {
    case 1: //North
        //(3,4) is north of (4,4)
        cout << "Moving North.\n";
        nx = px-1;
        ny = py;
        break;
    case 2: //West
        //(4,3) is west of (4,4)
        cout << "Moving West.\n";
        nx = px;
        ny = py-1;
        break;
    case 3: //East
        cout << "Moving East.\n";
        nx = px;
        ny = py+1;
        break;
    case 4: //South
        cout << "Moving South.\n";
        nx = px+1;
        ny = py;
        break;
    default:
        cout << "\nInvalid direction. Cannot move.\n";
        return false;
        break;
    }

    //if the position generated (nx/ny) any of them are out of bounds of the dungeon, we cannot move to this location
    if (nx >= const_n || nx < 0 || ny >= const_n || ny < 0)
    {
        cout << "Cannot move outside the dungeon.\n";
        return false;
    }

    //dungeon string for the nextRoom's position
    string currDungeonStr = dungeon[nx][ny];
    if (currDungeonStr[0] == '-'){ //room should not exist there
        return false;
    }

    if (currDungeonStr[0] == '0')
    {
        //create a Room if it does not exists
        dungeon[nx][ny][0] = '1'; //change dungeon string to explored
        nextRoom = new Room;
        nextRoom->RoomPosition[0] = nx;
        nextRoom->RoomPosition[1] = ny;
        if (dungeon[nx][ny].length() > 3){ //TREASURE ROOM to be created, due to $ length was increased
            nextRoom->isTreasure = true;
            cout << "Next room is a treasure room.";
        }
        //spawn Character
        switch(currDungeonStr[2])
        {
        case 'N':
            //no character here, proceed to move
            nextChar = NULL;
            break;
        case 'G':
            //creating a GOBLIN Monster Character
            nextChar = new Character;
            nextChar->characterType = g_Name;
            nextChar->characterDesc = g_Desc;
            nextChar->HP = const_g_maxHP;
            nextChar->atk = const_g_ATK;
            break;
        case 'B':
            //creating a BAT Monster Character
            nextChar = new Character;
            nextChar->characterType = b_Name;
            nextChar->characterDesc = b_Desc;
            nextChar->HP = const_b_maxHP;
            nextChar->atk = const_b_ATK;
            break;
        }
        //place this character into the nextRoom's character ptr
        nextRoom->c = nextChar;

        nextRoom->linkRoom(nextRoom->North, 1); //link this new room with every side
        nextRoom->linkRoom(nextRoom->East, 3);
        nextRoom->linkRoom(nextRoom->West, 2);
        nextRoom->linkRoom(nextRoom->South, 4);

        if (!playerRoom->linkRoom(nextRoom,direction))
        {
            cout << "Failed to link the player Room to the new Room.\n";
            return false;
        }
    }
    else
    {
        //if room exists, then FIND IT and get its ROOM* ptr
        int pos[2];
        pos[0] = nx;
        pos[1] = ny;
        findRoom(nextRoom,head,pos,direction); //gets ptr to existing room
        //nextRoom now contains the Room we're headed towards as it exists
    }
    //the Room should now exist in either cases

    //moving part comes here
    if (dungeon[nx][ny][2] == 'N')  //no character stands in front of player (on next Room position)
    {
        dungeon[playerRoom->RoomPosition[0]][playerRoom->RoomPosition[1]][2] = 'N';
        dungeon[nx][ny][2] = 'P';
        nextRoom->c = playerRoom->c; //copy player character pointer to nextRoom's character pointer
        playerRoom->c = NULL;
        //player has moved to the next Room, so playerRoom now points to the next Room
        playerRoom = nextRoom;
    }
    else
    {
        int choice;
        viewEnemyStatus(nextRoom);
        cout << "\nWould you like to initiate combat or flee?\n1. Fight\n2. Flee\n";
        cin >> choice;
        if (choice == 1)
        {
            if (combat(nextRoom) == true)
            {
                cout << "You won the combat!\n";
                dungeon[playerRoom->RoomPosition[0]][playerRoom->RoomPosition[1]][2] = 'N';
                dungeon[nx][ny][2] = 'P';
                delete nextRoom->c; //clear previous character
                nextRoom->c = playerRoom->c;
                playerRoom = nextRoom;
            }
            else
            {
                cout << "You lost the combat.\n";
                return false;
            }
        }
        else
        {
            cout << "Fled from the battle safely!\n";
            return false;
        }
    }

    // now that the movement is complete, check if your new room is a treasure room
    if (playerRoom->isTreasure == true){
        cout << "You WON THE GAME! Treasure room reached!";
        cout << "Exiting...";
        exit(1);
    }
    return true;
}

bool GameManager::combat(Room* enemyRoom)
{
    system("cls");
    cout << "An enemy " << enemyRoom->c->characterType << " has appeared!\n";
    cout << enemyRoom->c->characterDesc << endl;
    int playerchoice,enemychoice, movechoice;

    while (true) //combat loop
    {
        viewEnemyStatus(enemyRoom);
        viewPlayerStatus();
        if (enemyRoom->c->HP <= 0)
        {
            cout << "The enemy " << enemyRoom->c->characterType << " was defeated!\n";
            return true;
        }
        else if (!isPlayerAlive())
            return false;

        //player's attack phase, enemy's defense phase
        cout << "Prepare to attack!\n1. Attack\n2. Do nothing\n";
        cin >> playerchoice;
        enemychoice = rand() % 2 + 1; //1 to 2 random choice to dodge or defend
        switch(playerchoice)
        {
        case 1:
            useMove(enemyRoom, enemychoice, playerRoom->c->atk);
            break;
        case 2:
            cout << "\nYou chose not to attack this turn.\n";
            break;
        default:
            cout << "\nInvalid choice. Turn missed.\n";
            break;
        }

        system("pause");
        system("cls");
        viewEnemyStatus(enemyRoom);
        viewPlayerStatus();
        if (enemyRoom->c->HP <= 0)
        {
            cout << "The enemy " << enemyRoom->c->characterType << " was defeated!\n";
            return true;
        }
        else if (!isPlayerAlive())
            return false;

        cout << "Enemy's turn to attack! Get ready.\n1. Dodge \n2. Defend\n";
        cin >> playerchoice;
        if (playerchoice == 1 || playerchoice == 2){
            useMove(playerRoom, playerchoice, enemyRoom->c->atk); //enemy move
            continue;
        }
        else
        {
            cout << "\nInvalid choice. Turn missed.";
        }
        system("pause");
        system("cls");
    }
}
    /*
    findRoom (initially NULL)
    Traverse structure from head temp to ALL possible Rooms RECURSIVELY, if a temp was found in the specific position (1 on the dungeon) then return the temp address (temp), else return NULL in base case if temp not found
    e.g. the address of the temp at (1,2) position would be returned to the temp find if
    find.RoomPosition[0] = 1, find.RoomPosition[1] = 2---- if a temp exists there
    */
void GameManager::findRoom(Room*& temp, Room* caller, int position[2], int direction)
{
    if (caller==NULL) //no room exists
    {
        return;
    }
    else if (caller->isVisited==1) //room already visited
    {
        return;
    }

    if(caller->RoomPosition[0]==position[0] && caller->RoomPosition[1]==position[1]) //room found, coordinates found
    {
        temp = caller;
    }
    else //some other room found
    {
        caller->isVisited = 1; //that room is now visited
        findRoom(temp,caller->North,position,direction); //recursively go in all 4 directions
        findRoom(temp,caller->West,position,direction);
        findRoom(temp,caller->East,position,direction);
        findRoom(temp,caller->South,position,direction);
    }

    caller->isVisited = 0; //make all isVisited = 0 at the end, for the next function call of findRoom
}

void GameManager::useMove(Room* charRoom,int choice, int atk)
{
    //2 choices: 1, 2
    int chance;

    switch(choice)
    {
    case 1: //dodge
        //Each player has a 50% chance of dodging the other players attack
        chance = rand()%2; //0 or 1

        if (chance == 1) cout << charRoom->c->characterType << " was unable to dodge! Took a " << atk << " attack.\n";
        else cout << charRoom->c->characterType << " dodged the attack!\n";
        if (atk > charRoom->c->HP) atk = charRoom->c->HP;
        charRoom->c->HP -= atk; //if chance 1, then full atk, else 0 deduction - points nullified
        break;
    case 2: //defend
        //get half the atk of other players attack
        atk = 0.5*atk;
        cout << charRoom->c->characterType << " defended, was inflicted with a " << atk << " attack.\n";
        if (atk > charRoom->c->HP) atk = charRoom->c->HP;
        charRoom->c->HP -= atk;
        break;
    }
}

void GameManager::healOutside(){
    if (playerRoom->c->HP == const_p_maxHP){
        cout << "Already at full HP!";
        return;
    }
    playerRoom->c->HP += const_p_healFactor;
    cout << "Your monk regained " << const_p_healFactor << " HP!";
}
