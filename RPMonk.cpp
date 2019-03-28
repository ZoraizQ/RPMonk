#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <fstream>
#include <ctime>
#include <string>
#include <windows.h>
#include "RPMonk.h"

using namespace std;

const int const_p_maxHP = 100;
const int const_p_ATK = 15;
const int const_p_healFactor = 5;
const int const_m_maxHP = 100;
const int const_m_ATK = 20;
const int const_n = 5;
const string const_p_Name = "Player Monk";
const string const_m_Name = "Monster";

void setConsoleColour(int colour)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    cout.flush(); //To ensure that the active screen buffer is cleared and the characters are written to their destination.
    SetConsoleTextAttribute(hOut, colour);
    //Sets the console text color attribute to specified 'colour' value.
}

GameManager::GameManager()
{
    /*
    //LINKED by LINK function
    //Room is created when explored by player. So dungeon initially starts with single Room containing the player character
    //HEAD Room - is the initially created Room by the constructor, it is just like the head of a linked list, in this case the head of this dungeon structure
    RANDOMLY determine position on dungeon containing PLAYER, MINION
    SINGLE Room IN THE BEGINNING - head Room points to this, player Room points to this
    ALL OTHER positions UNEXPLORED (0), while head Room position 1
    There is only one Room
    */
    int n = const_n, i,j;
    Room* temp = new Room();
    Character* playerchar = new Character();
//7x7 or nxn dungeon generated
    dungeon = new string*[n];
    for (i = 0; i < n; i++)
    {
        dungeon[i] = new string[n];
    }
//initialized to unexplored '0', NULL 'N' no character
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            dungeon[i][j] = "0/N";
        }
    }
    srand(time(NULL)); //use time(NULL) as seed for random generator

    i = rand()%const_n;
    j = rand()%const_n;

    dungeon[i][j] = "1/P";
    //create player Room here, same as head initially
    playerchar->characterType = const_p_Name;
    playerchar->HP = const_p_maxHP;
    playerchar->atk = const_p_ATK;

    temp->c = playerchar;
    temp->RoomPosition[0] = i;
    temp->RoomPosition[1] = j;
    head = temp;
    playerRoom = temp;

    i = rand()%const_n;
    j = rand()%const_n;

    dungeon[i][j] = "0/M";

    i = rand()%const_n;
    j = rand()%const_n;
    dungeon[i][j] += "$"; //to signify a treasure room
    cout << "The treasure room will be present at " << i << "," << j << endl;
}

GameManager::~GameManager()
{
    int pos[2];
    //delete all existing Rooms and their contents by finding their address using findRoom
    for (int i = 0; i < const_n; i++)
    {
        for (int j = 0; j < const_n; j++)
        {
            if (dungeon[i][j][0] == '1')
            {
                pos[0] = i;
                pos[1] = j;
                Room* delRoom;
                findRoom(delRoom, head, pos, 0);
                if (delRoom != NULL)
                {
                    //delRoom now contains the Room to be deleted
                    if (delRoom->c != NULL)
                    {
                        delete delRoom->c;
                        delRoom->c = NULL;
                    }
                    delete delRoom;
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

void GameManager::showEnemy (Room* enemyRoom)
{
    cout << "\n[Enemy Status]" << "\nEnemy: " << enemyRoom->c->characterType << "\nHP: "<< enemyRoom->c->HP << '\n';
}


void GameManager::printDungeon()
{
    setConsoleColour(15);

    for (int i = 0; i < const_n; i++)
    {
        for (int j = 0; j < const_n; j++)
        {
            setConsoleColour(8);
            if (dungeon[i][j][0] == '1') setConsoleColour(2);
            if (dungeon[i][j][2] == 'P') setConsoleColour(10);
            if (dungeon[i][j][2] == 'M') setConsoleColour(4);
            cout << dungeon[i][j].substr(0,5) << "     ";
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

void GameManager::showPlayerStatus()
{
    cout << "\n[Player Status]" << "\nHP: " << playerRoom->c->HP << '\n';
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

    if (currDungeonStr[0] == '0')
    {
        //create a Room if it does not exists
        dungeon[nx][ny][0] = '1'; //updated dungeon string to explored
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
        case 'M':
            //creating a Monster Character
            nextChar = new Character;
            nextChar->characterType = const_m_Name;
            nextChar->HP = const_m_maxHP;
            nextChar->atk = const_m_ATK;
            break;
        }
        //insert this character into the nextRoom's character spot
        nextRoom->c = nextChar;

        nextRoom->linkRoom(nextRoom->North, 1);
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
        //Room exists
        int pos[2];
        pos[0] = nx;
        pos[1] = ny;
        findRoom(nextRoom,head,pos,direction);
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
        return true;
    }
    else
    {
        int choice;
        showEnemy(nextRoom);
        cout << "\nWould you like to initiate fight or flee?\n1. Fight\n2. Flee\n";
        cin >> choice;
        if (choice == 1)
        {
            if (fight(nextRoom) == true)
            {
                cout << "You won the fight!\n";
                dungeon[playerRoom->RoomPosition[0]][playerRoom->RoomPosition[1]][2] = 'N';
                dungeon[nx][ny][2] = 'P';
                delete nextRoom->c; //clear previous character
                nextRoom->c = playerRoom->c;
                playerRoom = nextRoom;
                return true;
            }
            else
            {
                cout << "You lost the fight.\n";
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
}

bool GameManager::fight(Room* enemyRoom)
{
    system("cls");
    cout << "An enemy " << enemyRoom->c->characterType << " has appeared!\n";
    int playerchoice,enemychoice, movechoice;

    while (true)
    {
        showEnemy(enemyRoom);
        showPlayerStatus();
        if (enemyRoom->c->HP <= 0)
        {
            cout << "The enemy " << enemyRoom->c->characterType << " was defeated!\n";
            return true;
        }
        else if (!isPlayerAlive())
            return false;

//player's attack phase, enemy's defense phase
        cout << "Prepare to attack!\n1. Attack\n2. Skip Turn\n";
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
        showEnemy(enemyRoom);
        showPlayerStatus();
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
    Room* find, (initially NULL)
    Traverse structure from head temp to ALL possible Rooms RECURSIVELY, if a temp was found in the specific position (1 on the dungeon) then return the temp address (temp), else return NULL in base case if temp not found
    e.g. the address of the temp at (1,2) position would be returned to the temp find if
    find.RoomPosition[0] = 1, find.RoomPosition[1] = 2---- if a temp exists there(edited)

    if (temp == NULL){
    return NULL;
    }
    */
void GameManager::findRoom(Room*& temp, Room* calling, int position[2], int direction)
{
    if (calling==NULL)
    {
        return;
    }
    else if (calling->isVisited==1)
    {
        return;
    }

    if(calling->RoomPosition[0]==position[0] && calling->RoomPosition[1]==position[1])
    {
        temp = calling;
    }
    else
    {
        calling->isVisited = 1;
        findRoom(temp,calling->North,position,direction);
        findRoom(temp,calling->West,position,direction);
        findRoom(temp,calling->East,position,direction);
        findRoom(temp,calling->South,position,direction);
    }

    calling->isVisited = 0;
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
        //Your chance of dodging the other players attack falls to 0% but you get half the atk of other players attack
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
