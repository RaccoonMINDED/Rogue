//base
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
//base
//for correct work of game input
#include <sys/wait.h>
#include <signal.h>
//for consistent fps and mb thm else(not yet used or mb i forgot to recomment)
#include <sys/time.h>
//Console sizing
#include <sys/ioctl.h>
//Commands?
#include <iostream>
//just strings manipulations
#include <string.h>
//For getChar() / fork
#include <unistd.h>
//struct for right work of console size checks
struct winsize ws;
//Colors
char normalCl[30] = "\033[0m";
char blueCl[30] = "\033[0;34m";
char greenCl[30] = "\033[0;32m";
//how many columns should console have
const int GRID_X = 120 ;
//how many rows should console have
const int GRID_Y = 30 ;
//maximum omount of floor's
const int maxFloorAmount = 1;

//so enemies wouldn't be so fast
int tickCount = 1;

//game fps
const float fps = 30;
//nano seconds per frame
const int uspf = 1000000 / fps;
//hardcoded path for now
const char lastGame[200] = "/home/raccoon/.Rogue/LastGame.txt";
const char statsFile[200] = "/home/raccoon/.Rogue/statsFile.txt";
const char achivementsFile[200] = "/home/raccoon/.Rogue/achivementsFile.txt";
const char settingsFile[200] = "/home/raccoon/.Rogue/settingsFile.txt";


const char menuSound[200] = "play /home/raccoon/.Rogue/click.wav -q";
const char menuEnterSound[200] = "play /home/raccoon/.Rogue/enter.wav -q";

const int MenuOptionsNum = 6;
const char optionsString[MenuOptionsNum][30] =  {"-New Game-","-Continue-","-Settings-","-Achivements-","-Stats-","-Exit-"};

struct {
    char model;
    int X;
    int Y;
    int Gold;
    int HP;
    int DMG;
    int Level;
    int Floor;
    int Room;
    int LevelProgress;
    int LevelMinReq;
} player;
typedef struct nameRoom{
    int X;
    int Y;
    int height;
    int length;
    int Num;
    int Floor;
    struct nameRoom *next;

} room;
room *roomHead = NULL;
typedef struct nameObject{
    //d - door
    //c - chest
    //# - wall
    //^ - spike
    //H - healing
    char model;
    int X;
    int Y;
    int Floor;
    int Room;
    //0 - closed/standart status if doesn't need it
    int isOpen;
    struct nameObject *next;
} object;
object *objectHead = NULL;
const int spikeDmg = 10;
const int healing = 50;
typedef struct nameEnemy{
    //s - snake(walks in zig-zags and 1 attack = 2DMG*3times)
    //z - zombie(walks in straight lines 4 DMG)
    char model;
    int X;
    int Y;
    int HP;
    int DMG;
    int Room;
    int Floor;
    int ticksToDo;
    struct nameEnemy *next;
} enemy;
enemy *enemyHead = NULL;


//defining MenuOptions in List
typedef struct name{
    char option[30];
    int optionNum;
    int isChosen;
    struct name *next;
} MenuOption;
//first element
MenuOption *head = NULL;



//clear's console
void consoleClear(){
    printf("\e[2J\e[3J");
}
//places "pointer" reletive to the top
void addSpacingUp(int height){
    printf("\e[%dH",height);
}
//places "pointer" reletive to the left
void addSpacingLeft(int length){
    printf("\033[%dC",length);
}
void removePointer(){
    addSpacingLeft(1);
    addSpacingUp(1);
}
void winScreen(){
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    consoleClear();
    addSpacingUp(ws.ws_row/2-3);
    addSpacingLeft(ws.ws_col/2-18);
printf(" ▄· ▄▌      ▄• ▄▌    ▄▄▌ ▐ ▄▌▪   ▐ ▄ \n");
    addSpacingLeft(ws.ws_col/2-18);
printf("▐█▪██▌▪     █▪██▌    ██· █▌▐███ •█▌▐█\n");
    addSpacingLeft(ws.ws_col/2-18);
printf("▐█▌▐█▪ ▄█▀▄ █▌▐█▌    ██▪▐█▐▐▌▐█·▐█▐▐▌\n");
    addSpacingLeft(ws.ws_col/2-18);
printf(" ▐█▀·.▐█▌.▐▌▐█▄█▌    ▐█▌██▐█▌▐█▌██▐█▌\n");
    addSpacingLeft(ws.ws_col/2-18);
printf("  ▀ •  ▀█▄▀▪ ▀▀▀      ▀▀▀▀ ▀▪▀▀▀▀▀ █▪\n");
sleep(3);
return;
}

void gameOver(){
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    consoleClear();
    addSpacingUp(ws.ws_row/4);
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("███▀▀▀██┼███▀▀▀███┼███▀█▄█▀███┼██▀▀▀\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("██┼┼┼┼██┼██┼┼┼┼┼██┼██┼┼┼█┼┼┼██┼██┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("██┼┼┼▄▄▄┼██▄▄▄▄▄██┼██┼┼┼▀┼┼┼██┼██▀▀▀\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("██┼┼┼┼██┼██┼┼┼┼┼██┼██┼┼┼┼┼┼┼██┼██┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("███▄▄▄██┼██┼┼┼┼┼██┼██┼┼┼┼┼┼┼██┼██▄▄▄\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("███▀▀▀███┼▀███┼┼██▀┼██▀▀▀┼██▀▀▀▀██▄┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("██┼┼┼┼┼██┼┼┼██┼┼██┼┼██┼┼┼┼██┼┼┼┼┼██┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("██┼┼┼┼┼██┼┼┼██┼┼██┼┼██▀▀▀┼██▄▄▄▄▄▀▀┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("██┼┼┼┼┼██┼┼┼██┼┼█▀┼┼██┼┼┼┼██┼┼┼┼┼██┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("███▄▄▄███┼┼┼─▀█▀┼┼─┼██▄▄▄┼██┼┼┼┼┼██▄\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼██┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼██┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼████▄┼┼┼▄▄▄▄▄▄▄┼┼┼▄████┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼▀▀█▄█████████▄█▀▀┼┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼┼┼█████████████┼┼┼┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼┼┼██▀▀▀███▀▀▀██┼┼┼┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼┼┼██┼┼┼███┼┼┼██┼┼┼┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼┼┼█████▀▄▀█████┼┼┼┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼┼┼┼███████████┼┼┼┼┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼▄▄▄██┼┼█▀█▀█┼┼██▄▄▄┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼▀▀██┼┼┼┼┼┼┼┼┼┼┼██▀▀┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼┼▀▀┼┼┼┼┼┼┼┼┼┼┼▀▀┼┼┼┼┼┼┼┼┼┼┼\n");
    addSpacingLeft(ws.ws_col/2-18);
    printf("┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼\n");
    sleep(3);
    return;
}


//draws rectangle with y*x size starting from (ya,xb) point
void drawRect(int x,int y,int a,int b){
    addSpacingUp(a);
    addSpacingLeft(b+1);
    for(int i = 0;i<y; i++ ){
        printf("-");
    }
    addSpacingUp(a+x+1);
    addSpacingLeft(b+1);
    for(int i = 0;i<y; i++ ){
        printf("-");
    }
    for(int i = 0;i<x; i++ ){
        addSpacingUp(i+a+1);
        addSpacingLeft(b);
        printf("|");
    }
    for(int i = 0;i<x; i++ ){
        addSpacingUp(i+a+1);
        addSpacingLeft(b+y+1);
        printf("|");
    }
}
//returns time in 00.00 seconds format (sec.xx without a dot)
int sTime(){
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    start_time.tv_usec = start_time.tv_usec / 10000;
    start_time.tv_sec = (start_time.tv_sec * 100) % 10000;
    int t = start_time.tv_usec + start_time.tv_sec;
    return t;
}
//conio.h for linux - input without ENTER
int getChar(){
    struct termios oldt, newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~ICANON;
    newt.c_lflag &= ~ECHO;
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}

int getCharFork(){
    int input=0,pid,childExitStatus;
    if ((pid=fork()) < 0) {return 0;}
    if(pid == 0){
        input = getChar();
        //arrow pressed
        if(input == 27){
            input = getChar();
            input = getChar();
            switch(input){
                //up
                case 65:
                    input = 119;
                    break;
                    //down
                case 66:
                    input = 115;
                    break;
                    //right
                case 67:
                    input = 100;
                    break;
                    //left
                case 68:
                    input = 97;
                    break;
            }
        }
        exit(input);
    }
    else{
        usleep(uspf);
        waitpid(pid, &childExitStatus, WNOHANG);
        //sets input to exit status or 0
        if(WIFEXITED(childExitStatus) == 1){
            input = WEXITSTATUS(childExitStatus);
        }
        else {
            kill(pid,SIGTERM);
        }
    }
    return input;
}


int randomSize(const char* sw){
    int size,minSize,i;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    if (strcmp(sw,"y") == 0){
        size = GRID_Y/3;
        minSize = 5;
    }
    if (strcmp(sw,"x") == 0){
        size = GRID_X/6;
        minSize = 5;
    }
    i = minSize + rand() % (size - minSize);
    return i;
}

//checks if console if GRID_X by GRID_Y
void isConsoleSizeRight(){
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    int height = ws.ws_row;
    int length = ws.ws_col;
    while(height<GRID_Y || length<GRID_X){
        consoleClear();
        addSpacingUp(height/2);
        addSpacingLeft(length/2);
        printf("Please, reajust\n");
        addSpacingUp(height/2+1);
        addSpacingLeft(length/2);
        printf("Your console\n");
        addSpacingUp(height/2+2);
        addSpacingLeft(length/2);
        printf("%d x %d",GRID_X,GRID_Y);
        drawRect(GRID_Y,GRID_X,1,1);
        ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
        height = ws.ws_row;
        length = ws.ws_col;
    }
}
//opens lastGameFile.txt checks if it's ended
char IsContinue(){
    char first;
    FILE *fp;
    if ((fp = fopen(lastGame, "r"))==NULL) {
        fp  = fopen (lastGame, "w");
        fputs("0",fp);
        fclose(fp);
        fp = fopen(lastGame, "r");

    }
    else{
        first = getc(fp);
        if (first != '1'){
            first = '0';
        }
    }
    fclose(fp);
    return first;
}
//asks for a yes or no answer and returns 1 or 0
int AreYouSure(){
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    int height = ws.ws_row;
    int length = ws.ws_col;
    consoleClear();
    drawRect(6,36,ws.ws_row/2-2,ws.ws_col/2-18);
    addSpacingUp(height/2);
    addSpacingLeft(length/2-10);
    printf("Are you sure?");
    addSpacingUp(height/2+1);
    addSpacingLeft(length/2-15);
    printf("Your last game will be deleted!");
    addSpacingUp(height/2+2);
    addSpacingLeft(length/2-2);
    printf("y/n");
    int yn = 0;
    while(true){
        yn = getChar();
        if(yn == 121 || yn == 89){
            consoleClear();
            return 1;
        }
        else if(yn == 78 || yn == 110){
            consoleClear();
            return 0;
        }
    }


}
//clear's current save in case New game starts and writes basic starting stats in save file
//ISN'T ENDED YET
void clearSave(){
    // int randomS,doorX1, doorY1, doorY2, doorX2;
    // char stringFromInt[100];
    FILE *fp;
    fp = fopen(lastGame,"w");
    fputs("1", fp);
    //player reset possition and status
    fputs("\n__________________________\n", fp);
    fputs("player\n", fp);
    fputs("playerX 10\n", fp);
    fputs("playerY 6\n", fp);
    fputs("playerHP 80\n", fp);
    fputs("playerGold 0\n", fp);
    fputs("playerDMG 10\n", fp);
    fputs("playerLVL 1\n", fp);
    fputs("playerLevelProgress 0\n", fp);
    fputs("playerLevelMinReq 6\n", fp);
    fputs("playerFloor 1\n", fp);
    fputs("playerRoom 1\n", fp);
    fputs("__________________________\n", fp);
    fputs("rooms\n", fp);
    fputs("floor 1\n", fp);
    // RoomNumber y*x in (y,x)
    // fputs("room 1\n", fp);
    // randomS = randomSize("y");
    // doorY1 = randomS/2;
    // doorY2 = randomS+1;
    // sprintf(stringFromInt,"%d",randomS);
    // fputs(stringFromInt, fp);
    // fputs(" ", fp);
    // randomS = randomSize("x");
    // doorX2 = randomS/2;
    // doorX1 = randomS+1;
    // sprintf(stringFromInt,"%d",randomS);
    // fputs(stringFromInt, fp);
    // fputs(" 1 1\n", fp);
    fputs("room 1 7 15 2 2\n", fp);
    fputs("room 2 7 15 2 23\n", fp);
    fputs("room 3 7 15 12 23\n", fp);
    fputs("room 4 7 55 22 23\n", fp);
    fputs("room 5 7 15 12 63\n", fp);
    fputs("room 6 7 15 2 63\n", fp);
    //
    fputs("__________________________\n", fp);
    fputs("objects\n",fp);
    fputs("floor 1\n", fp);
    // FloorNumber RoomNumber what in (y,x)
    fputs("object d 1 1 6 18 0\n", fp);
    fputs("object d 1 1 6 23 0\n", fp);
    fputs("object d 1 2 10 31 0\n", fp);

    fputs("object c 1 2 6 37 0\n", fp);
    fputs("object H 1 2 4 31 0\n", fp);

    fputs("object d 1 2 12 31 0\n", fp);

    fputs("object # 1 3 13 30 0\n", fp);
    fputs("object # 1 3 13 32 0\n", fp);

    fputs("object # 1 3 14 30 0\n", fp);
    fputs("object # 1 3 14 32 0\n", fp);

    fputs("object # 1 3 15 30 0\n", fp);
    fputs("object # 1 3 15 32 0\n", fp);

    fputs("object # 1 3 15 24 0\n", fp);
    fputs("object # 1 3 15 25 0\n", fp);
    fputs("object # 1 3 15 26 0\n", fp);
    fputs("object # 1 3 15 27 0\n", fp);
    fputs("object # 1 3 15 28 0\n", fp);
    fputs("object # 1 3 15 29 0\n", fp);
    fputs("object # 1 3 15 33 0\n", fp);
    fputs("object # 1 3 15 34 0\n", fp);
    fputs("object # 1 3 15 35 0\n", fp);
    fputs("object # 1 3 15 36 0\n", fp);
    fputs("object # 1 3 15 37 0\n", fp);
    fputs("object # 1 3 15 38 0\n", fp);

    fputs("object ^ 1 3 17 30 0\n", fp);
    fputs("object ^ 1 3 17 31 0\n", fp);
    fputs("object ^ 1 3 17 32 0\n", fp);

    fputs("object ^ 1 3 19 27 0\n", fp);
    fputs("object ^ 1 3 19 28 0\n", fp);
    fputs("object ^ 1 3 19 29 0\n", fp);
    fputs("object ^ 1 3 19 33 0\n", fp);
    fputs("object ^ 1 3 19 34 0\n", fp);
    fputs("object ^ 1 3 19 35 0\n", fp);

    fputs("object d 1 3 20 31 0\n", fp);

    fputs("object ^ 1 4 23 48 0\n", fp);
    fputs("object ^ 1 4 24 47 0\n", fp);
    fputs("object ^ 1 4 24 49 0\n", fp);
    fputs("object ^ 1 4 25 48 0\n", fp);
    fputs("object c 1 4 24 48 0\n", fp);

    fputs("object ^ 1 4 23 51 0\n", fp);
    fputs("object ^ 1 4 24 50 0\n", fp);
    fputs("object ^ 1 4 24 52 0\n", fp);
    fputs("object ^ 1 4 25 51 0\n", fp);
    fputs("object H 1 4 24 51 0\n", fp);

    fputs("object d 1 3 22 31 0\n", fp);
    fputs("object d 1 4 22 71 0\n", fp);
    fputs("object $ 1 5 16 71 0\n", fp);
    fputs("object d 1 4 20 71 0\n", fp);
    fputs("object d 1 5 12 71 0\n", fp);
    fputs("object = 1 6 6 71 0\n", fp);
    fputs("object d 1 5 10 71 0\n", fp);
    // fputs("room 1\ndoor ", fp);
    // sprintf(stringFromInt,"%d",doorY1);
    // fputs(stringFromInt, fp);
    // fputs(" ", fp);
    // sprintf(stringFromInt,"%d",doorX1);
    // fputs(stringFromInt, fp);
    // fputs("\ndoor ", fp);
    // sprintf(stringFromInt,"%d",doorY2);
    // fputs(stringFromInt, fp);
    // fputs(" ", fp);
    // sprintf(stringFromInt,"%d",doorX2);
    // fputs(stringFromInt, fp);
    // fputs("\n", fp);
    fputs("__________________________\n", fp);
    fputs("enemies\n", fp);
    fputs("floor 1\n", fp);
    fputs("enemy s 1 4 28 41 20 6 15\n", fp);
    fputs("enemy z 1 4 24 71 10 4 31\n", fp);
    fputs("enemy z 1 4 26 71 10 4 33\n", fp);
    fputs("__________________________\n", fp);
    fputs("THEEND\n",fp);
    fclose(fp);
}
//saves all the game data
void saveGame(int cont){
    FILE *fp;
    char stringFromInt[100],stringFromChar[10];
    int floorCounter = 1;
    fp = fopen(lastGame,"w");
    room *currentRoom = roomHead;
    //
    sprintf(stringFromInt,"%d",cont);
    fputs(stringFromInt, fp);
    //player save
    fputs("\n__________________________\n", fp);
    fputs("player\n", fp);
    fputs("playerX ", fp);
    sprintf(stringFromInt,"%d",player.X);
    fputs(stringFromInt, fp);
    fputs("\n", fp);
    fputs("playerY ", fp);
    sprintf(stringFromInt,"%d",player.Y);
    fputs(stringFromInt, fp);
    fputs("\n", fp);
    fputs("playerHP ", fp);
    sprintf(stringFromInt,"%d",player.HP);
    fputs(stringFromInt, fp);
    fputs("\n", fp);
    fputs("playerGold ", fp);
    sprintf(stringFromInt,"%d",player.Gold);
    fputs(stringFromInt, fp);
    fputs("\n", fp);
    fputs("playerDMG ", fp);
    sprintf(stringFromInt,"%d",player.DMG);
    fputs(stringFromInt, fp);
    fputs("\n", fp);
    fputs("playerLVL ", fp);
    sprintf(stringFromInt,"%d",player.Level);
    fputs(stringFromInt, fp);
    fputs("\n", fp);
    fputs("playerLevelProgress ", fp);
    sprintf(stringFromInt,"%d",player.LevelProgress);
    fputs(stringFromInt, fp);
    fputs("\n", fp);
    fputs("playerLevelMinReq ", fp);
    sprintf(stringFromInt,"%d",player.LevelMinReq);
    fputs(stringFromInt, fp);
    fputs("\n", fp);
    fputs("playerFloor ", fp);
    sprintf(stringFromInt,"%d",player.Floor);
    fputs(stringFromInt, fp);
    fputs("\n", fp);
    fputs("playerRoom ", fp);
    sprintf(stringFromInt,"%d",player.Room);
    fputs(stringFromInt, fp);
    fputs("\n", fp);
    //save's info about rooms'
    fputs("__________________________\n", fp);
    fputs("rooms\n", fp);

    currentRoom = roomHead;
    while(floorCounter<=maxFloorAmount){
        fputs("floor ", fp);
        sprintf(stringFromInt,"%d",floorCounter);
        fputs(stringFromInt, fp);
        fputs("\n", fp);
        while(currentRoom){
            if(currentRoom -> Floor == floorCounter){
                fputs("room ", fp);
                sprintf(stringFromInt,"%d",currentRoom -> Num);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentRoom -> height);
                fputs(stringFromInt, fp);
                fputs (" ", fp);
                sprintf(stringFromInt,"%d",currentRoom -> length);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentRoom -> Y);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentRoom -> X);
                fputs(stringFromInt, fp);
                fputs("\n", fp);
            }
            currentRoom = currentRoom -> next;
        }
        currentRoom = roomHead;
        floorCounter++;
    }

    //save's info about objects on a map
    fputs("__________________________\n", fp);
    fputs("objects\n",fp);
    object *currentObject = objectHead;
    floorCounter = 1;
    while(floorCounter<=maxFloorAmount){
        fputs("floor ", fp);
        sprintf(stringFromInt,"%d",floorCounter);
        fputs(stringFromInt, fp);
        fputs("\n", fp);
        while(currentObject){
            if(currentObject -> Floor == floorCounter){
                fputs("object ", fp);
                fputs(&(currentObject -> model), fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentObject -> Floor);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentObject -> Room);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentObject -> Y);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentObject -> X);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentObject -> isOpen);
                fputs(stringFromInt, fp);
                fputs("\n", fp);
            }
            currentObject = currentObject -> next;
        }
        currentObject = objectHead;
        floorCounter++;
    }
    //save's enemies if there're
    enemy *currentEnemy = enemyHead;
    fputs("__________________________\n", fp);
    fputs("enemies\n", fp);
    floorCounter = 1;
    while(floorCounter<=maxFloorAmount){
        fputs("floor ", fp);
        sprintf(stringFromInt,"%d",floorCounter);
        fputs(stringFromInt, fp);
        fputs("\n", fp);
        while(currentEnemy){
            if(currentEnemy -> Floor == floorCounter){
                fputs("enemy ", fp);
                fputs(&(currentEnemy -> model), fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentEnemy -> Floor);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentEnemy -> Room);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentEnemy -> Y);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentEnemy -> X);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentEnemy -> HP);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentEnemy -> DMG);
                fputs(stringFromInt, fp);
                fputs(" ", fp);
                sprintf(stringFromInt,"%d",currentEnemy -> ticksToDo);
                fputs(stringFromInt, fp);
                fputs("\n", fp);
            }
            currentEnemy = currentEnemy -> next;
        }
        currentEnemy = enemyHead;
        floorCounter++;
    }
    fputs("__________________________\n", fp);
    fputs("THEEND\n",fp);
    fclose(fp);

}
//giveLoot now gives only a little bit of gold and xp
void giveLoot(int minGold, int maxGold, int minXp, int maxXp){
    player.Gold += minGold + rand() % (maxGold - minGold);
    player.LevelProgress += minXp + rand() % (maxXp - minXp);
}

void enemyMove(){
    enemy *currentEnemy = enemyHead;
    while(currentEnemy){
        if(player.Room == currentEnemy -> Room && tickCount % currentEnemy -> ticksToDo == 0 && currentEnemy -> HP > 0){
            if(player.X>currentEnemy -> X){
                currentEnemy->X++;
                if(player.X == currentEnemy -> X){
                    if(player.Y == currentEnemy -> Y){
                        player.HP-=currentEnemy->DMG;
                        currentEnemy->X--;
                    }

                    goto nextEnemy;
                }
            }
            if(player.X<currentEnemy -> X){
                currentEnemy->X--;
                if(player.X == currentEnemy -> X){
                    if(player.Y == currentEnemy -> Y){
                        player.HP-=currentEnemy->DMG;
                        currentEnemy->X++;
                    }
                    goto nextEnemy;
                }
            }
            if(player.Y>currentEnemy -> Y){
                currentEnemy->Y++;
                if(player.Y == currentEnemy -> Y){
                    if(player.X == currentEnemy -> X){
                        player.HP-=currentEnemy->DMG;
                        currentEnemy->Y--;
                    }
                    goto nextEnemy;
                }
            }
            if(player.Y<currentEnemy -> Y){
                currentEnemy->Y--;
                if(player.Y == currentEnemy -> Y){
                    if(player.X == currentEnemy -> X){
                        player.HP-=currentEnemy->DMG;
                        currentEnemy->Y++;
                    }
                    goto nextEnemy;
                }
            }
        }
        nextEnemy:
// object *currentObject = objectHead;
//             while(currentObject != NULL){
//                 if(currentObject -> Y == currentEnemy -> Y && currentObject -> X == currentEnemy -> X){
//
//                 }
//                 currentObject = currentObject -> next;
//             }
        currentEnemy = currentEnemy -> next;
    }
    return;
}

int isEnemyInRoom(){
    enemy *currentEnemy = enemyHead;
    while(currentEnemy){
        if(player.Room == currentEnemy -> Room && currentEnemy -> HP > 0){
            return 1;
        }
        currentEnemy = currentEnemy -> next;
    }
    return 0;
}

void shop(){
int chosen = 0;
int add[3];
int val[3];
int input;
//1 - +DMG
add[0] = 5;
//2 - +HP
add[1] = 25;
//3 - +LevelProgress
add[2] = 4;
char shopItems[3][20] = {"Dmg","Hp","Xp"};

while(1){
consoleClear();
isConsoleSizeRight();
if(player.LevelProgress>player.LevelMinReq){
        player.HP+=20;
        player.DMG+=5;
        player.LevelProgress-=player.LevelMinReq;
        player.LevelMinReq+=player.LevelMinReq/2;
        player.Level++;
    }
    addSpacingUp(1);
    addSpacingLeft(3);
    printf("Hp=%d/100       Dmg=%d      Gold=%d     lvl=%d(%d/%d)",player.HP,player.DMG,player.Gold,player.Level,player.LevelProgress,player.LevelMinReq);
val[0]=5 * player.DMG/add[0];
val[1]=5;
val[2]=5 * player.Level;

ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
addSpacingUp(ws.ws_row/2-6);
addSpacingLeft(ws.ws_col/3);
printf("%s",greenCl);
printf("Gold = %d\n\n\n",player.Gold);
printf("%s\n",normalCl);
for(int i = 0; i<3; i++){
addSpacingLeft(ws.ws_col/3);
if(val[i]<=player.Gold){
printf("%s",greenCl);
}
if(i == chosen){
printf("%s",blueCl);
}
printf("+%d %s ",add[i],shopItems[i]);
printf("(%dgold)",val[i]);
printf("%s\n",normalCl);
}
drawRect(ws.ws_row/2,ws.ws_col/2,ws.ws_row/4,ws.ws_col/4);
input = getCharFork();
inputStart:
switch(input){
            case 10:
                switch(chosen){
                    case 0:
                        if(val[0]<=player.Gold){
                            player.Gold -= val[0];
                            player.DMG += add[0];
                        }
                        break;
                    case 1:
                        if(val[1]<=player.Gold){
                            player.Gold -= val[1];
                            player.HP += add[1];
                        }
                        break;
                    case 2:
                        if(val[2]<=player.Gold){
                            player.Gold -= val[2];
                            player.LevelProgress += add[2];
                        }
                        break;

                }
                break;
                //s/S down
            case 83:
                input = 115;
                goto inputStart;
                break;
            case 115:
                input = 100;
                goto inputStart;
                break;
                //w/W up
            case 87:
                input = 119;
                goto inputStart;
                break;
            case 119:
                input = 97;
                goto inputStart;
                break;
                //a/A left
            case 65:
                input = 97;
                goto inputStart;
                break;
            case 97:
                if (tickCount>fps*5){
                tickCount = 1;
                saveGame(1);
                }
                if(chosen>0){
                chosen--;
                }
                break;
                //d/D right
            case 68:
                input = 100;
                goto inputStart;
                break;
            case 100:
                if (tickCount>fps*5){
                tickCount = 1;
                saveGame(1);
                }
                if(chosen<2){
                chosen++;
                }
                break;
                //exit e/E
            case 69:
                input = 101;
                goto inputStart;
                break;
            case 101:
                saveGame(1);
                return;
                break;
            default:
                break;
        }
}
}

//first 1 - Y (x - 0)
//second 1 - direction is possitive (0 - negative)
//last is a room num
int coolDown = 0;
int coolDownTime = 10;
void playerMove(int xy, int direction, int roomNum){
    if (coolDown>0){
        coolDown++;
    }
    if (coolDown>=coolDownTime){
        coolDown = 0;
    }
    if(direction == 0){
        direction = -1;
    }
    room *currentRoom = roomHead;
    //set's currentRoom to the room in which player is
    while(currentRoom -> Num != player.Room || currentRoom -> Floor != player.Floor){
        currentRoom = currentRoom -> next;
    }
    //check if there is a wall(room's are basicly a squares)

    int roomBottomY, roomBottomX;
    roomBottomX = (currentRoom -> X) + (currentRoom -> length) + 1;
    roomBottomY = (currentRoom -> Y) + (currentRoom -> height) + 1;

    if (xy==1){
        player.Y = player.Y + direction;
    }
    else if(xy==0){
        player.X = player.X + direction;
    }
    //check's if enemy ia a way'
    if (isEnemyInRoom() == 1){
        enemy *currentEnemy = enemyHead;
        while(currentEnemy){
            if(currentEnemy -> X == player.X && currentEnemy -> Y == player.Y && currentEnemy -> HP > 0){
                if (xy==1){
                    player.Y = player.Y - direction;
                }
                else if(xy==0){
                    player.X = player.X - direction;
                }
                if(coolDown == 0){
                    currentEnemy -> HP -= player.DMG;
                    coolDown++;
                    player.HP++;
                }
                if (currentEnemy -> HP <= 0){
                    giveLoot(1,4,2,4);
                }
                return;
            }

            currentEnemy = currentEnemy -> next;
        }
    }
    //check's for collisions with obj
    object *currentObject = objectHead;
    while(currentObject != NULL){
        if(currentObject -> Y == player.Y && currentObject -> X == player.X && currentObject -> Floor == player.Floor){
            //what will happen on collision with door
            if(currentObject -> model == 'd' && isEnemyInRoom() == 0){
                if (player.Room == currentObject -> Room){
                    player.Room = player.Room + 1;
                }
                else{
                    player.Room = player.Room - 1;
                }
                if (xy==1){
                    player.Y = player.Y + 3 * direction;
                }
                else if(xy==0){
                    player.X = player.X + 6 * direction;
                }
                return;
            }
            //other objects
            else{
                if(currentObject -> model == '^'){
                    player.HP -= spikeDmg;
                    return;
                }
                if (xy==1){
                    player.Y = player.Y - direction;
                }
                else if(xy==0){
                    player.X = player.X - direction;
                }
                if(isEnemyInRoom() == 0){
                    if(currentObject -> model == 'c' && currentObject -> isOpen == 0){
                        currentObject -> isOpen = 1;
                        giveLoot(2,10,1,4);
                        currentObject -> model = '*';
                    }
                    if(currentObject -> model == 'H' && currentObject -> isOpen == 0){
                        currentObject -> isOpen = 1;
                        player.HP += healing;
                        currentObject -> model = '*';
                    }
                    if(currentObject -> model == '$'){
                        shop();
                    }
                    if(currentObject -> model == '='){
                        if(maxFloorAmount<player.Floor){
                        // createNewFloor();
                        player.Floor++;
                        // possitionPlayerInNewFloor();
                        saveGame(1);
                        }
                        else{
                        winScreen();
                        saveGame(0);
                        //saveStats();
                        }
                    }
                    return;
                }
            }
        }
        currentObject = currentObject -> next;
    }


    if(currentRoom -> X == player.X  || roomBottomX == player.X){
        player.X = player.X - direction;
    }
    if(currentRoom -> Y == player.Y || roomBottomY == player.Y){
        player.Y = player.Y - direction;
    }
    return;
}
void playerDraw(int playerX, int playerY){
    addSpacingUp(1);
    addSpacingLeft(3);
    if(player.HP>100 && tickCount % 5 == 0){
        player.HP--;
    }
    if(player.HP<=0){
        //saveStats();
        saveGame(0);
        gameOver();
        return;
    }
    if(player.LevelProgress>player.LevelMinReq){
        player.HP+=20;
        player.DMG+=5;
        player.LevelProgress-=player.LevelMinReq;
        player.LevelMinReq+=player.LevelMinReq/2;
        player.Level++;
    }
    printf("Hp=%d/100       Dmg=%d      Gold=%d     lvl=%d(%d/%d)",player.HP,player.DMG,player.Gold,player.Level,player.LevelProgress,player.LevelMinReq);
    //possitioning x and y coordinates in square
    addSpacingUp(playerY);
    addSpacingLeft(playerX);
    printf("%c",player.model);
}
void roomsDraw(){
    room *current = roomHead;
    while(current!=NULL && current -> Num != 0){
        drawRect(current -> height, current -> length, current -> Y, current -> X);
        // printf("%d %d",current -> Num ,current -> Floor);
        current = current -> next;
    }
    if(player.Floor == 1){
        if (player.Room == 1){
            addSpacingUp(3);
            addSpacingLeft(42);
            printf("0 is your charecter\n\n");
            addSpacingLeft(44);
            printf("wasd - to move\n\n");
            addSpacingLeft(41);
            printf("e - to save and exit\n\n");
            addSpacingLeft(42);
            printf("d is a door symbol\n");
            addSpacingLeft(41);
            printf("you can go through it\n");
        }
        if (player.Room == 2){
            addSpacingUp(3);
            addSpacingLeft(41);
            printf("Step on object to open\n\n");
            addSpacingLeft(41);
            printf("c->chest(gold and xp)\n\n");
            addSpacingLeft(44);
            printf("H->healing(+50HP)\n\n");
            addSpacingLeft(41);
            printf("*->already open/used\n\n");
            addSpacingLeft(41);
            printf("When you're done, plz,\n");
            addSpacingLeft(42);
            printf("go to the next room");
        }
        if (player.Room == 3){
            addSpacingUp(3);
            addSpacingLeft(40);
            printf("^->spike, dont step on\n\n");
            addSpacingLeft(43);
            printf("#->just a wall...\n\n");
            addSpacingLeft(43);
            printf("Go around to the\n");
            addSpacingLeft(43);
            printf("Next room and try\n");
            addSpacingLeft(43);
            printf("Not to die plz");
        }
        if (player.Room == 4){
            if(isEnemyInRoom()==1){
                addSpacingUp(3);
                addSpacingLeft(41);
                printf("Enemies are attacking\n\n");
                addSpacingLeft(42);
                printf("They can kill you\n");
                addSpacingLeft(44);
                printf("But so as you\n\n");
                addSpacingLeft(42);
                printf("Go and step on them\n\n");
                addSpacingLeft(43);
                printf("Kill them all to\n");
                addSpacingLeft(42);
                printf("go to the next room");
            }
            else{
                addSpacingUp(3);
                addSpacingLeft(43);
                printf("Yeah! you got it\n");
                addSpacingLeft(41);
                printf("Come to the next room\n");
            }
        }
        if (player.Room == 5){
            addSpacingUp(3);
            addSpacingLeft(41);
            printf("$ - shop(step to use)\n");
            addSpacingLeft(41);
            printf("e - to exit the shop\n\n");
            addSpacingLeft(42);
            printf("Shop will stay open\n\n");
            addSpacingLeft(43);
            printf("I mb add gambling\n");
            addSpacingLeft(42);
            printf("In the future shops");
        }
        if (player.Room == 6){
            addSpacingUp(3);
            addSpacingLeft(41);
            printf("That's the last level\n");
            addSpacingLeft(43);
            printf("In this pre-alpha\n\n");
            addSpacingLeft(43);
            printf("Thanks for plaing\n\n");
            addSpacingLeft(42);
            printf("You can go in a '='\n");
            addSpacingLeft(42);
            printf("To end the tutorial");
        }
    }
}
void objectsDraw(){
    object *current = objectHead;

    while(current!=NULL){
        if(current -> Room == player.Room || current -> model == 'd'){
            addSpacingUp(current -> Y);
            addSpacingLeft(current -> X);
            printf("%c",current -> model);
            // printf("x=%d, y=%d, Room=%d",current -> X,current -> Y,current -> Room);
        }
        current = current -> next;
    }
}
void enemyDraw(){
    enemy *currentEnemy = enemyHead;
    while(currentEnemy){
        if(currentEnemy -> Room == player.Room && currentEnemy -> HP>0){
            addSpacingUp(currentEnemy -> Y);
            addSpacingLeft(currentEnemy -> X);
            printf("%c",currentEnemy -> model);
            // printf("x=%d, y=%d, Room=%d",currentEnemy -> X,currentEnemy -> Y,currentEnemy -> Room);
        }
        currentEnemy = currentEnemy -> next;
    }
}
//Read's where rooms at from LastGame.txt and puts them in room struct
void readRoomInfo(){
    FILE *fp = fopen(lastGame, "r");
    char stringCheck[50],floorCheck[50];
    //?! this was in guide
    if (roomHead == NULL) {
        exit(1);
    }
    //?!
    room *current = roomHead;
    while(feof(fp)==0){
        fscanf(fp,"%s",floorCheck);
        if(strcmp(floorCheck,"objects") == 0){
            return;
        }
        if (strcmp(floorCheck,"floor") == 0 ){
            fscanf(fp,"%s",floorCheck);
            while(feof(fp)==0 && strcmp(stringCheck,"floor")!=0){
                if(strcmp(stringCheck,"room")==0){
                    current -> next = (room*) malloc(sizeof(room));
                    current -> Floor = atoi(floorCheck);
                    fscanf(fp,"%s",stringCheck);
                    current -> Num = atoi(stringCheck);
                    fscanf(fp,"%s",stringCheck);
                    current -> height = atoi(stringCheck);
                    fscanf(fp,"%s",stringCheck);
                    current -> length = atoi(stringCheck);
                    fscanf(fp,"%s",stringCheck);
                    current -> Y = atoi(stringCheck);
                    fscanf(fp,"%s",stringCheck);
                    current -> X = atoi(stringCheck);
                    current = current -> next;
                }
                fscanf(fp,"%s",stringCheck);
            }
        }
    }
    fclose(fp);
}
//Read's where objects at from LastGame.txt and puts them in objects struct
void readObjectInfo(){
    FILE *fp = fopen(lastGame, "r");
    char stringCheck[50],objectsCheck[50];
    //?! this was in guide
    if (objectHead == NULL) {
        exit(1);
    }
    //?!
    object *current = objectHead;
    while(strcmp(objectsCheck,"objects")!=0){
        fscanf(fp,"%s",objectsCheck);
    }
    while(feof(fp)==0){
        current -> next = (object*) malloc(sizeof(object));
        while(strcmp(objectsCheck,"object")!=0){
            fscanf(fp,"%s",objectsCheck);
            if(strcmp(objectsCheck,"enemies")==0 || feof(fp)!=0){
                fclose(fp);
                return;
            }
        }
        fscanf(fp,"%s",objectsCheck);
        current -> model = objectsCheck[0];
        fscanf(fp,"%s",stringCheck);
        current -> Floor = atoi(stringCheck);
        fscanf(fp,"%s",stringCheck);
        current -> Room = atoi(stringCheck);
        fscanf(fp,"%s",stringCheck);
        current -> Y = atoi(stringCheck);
        fscanf(fp,"%s",stringCheck);
        current -> X = atoi(stringCheck);
        fscanf(fp,"%s",stringCheck);
        current -> isOpen = atoi(stringCheck);
        current = current -> next;
    }
    fclose(fp);
}
//Read's where Enemies at from LastGame.txt and puts them in enemy struct
void readEnemyInfo(){
    FILE *fp = fopen(lastGame, "r");
    char stringCheck[50],enemyCheck[50];
    //?! this was in guide
    if (enemyHead == NULL) {
        exit(1);
    }
    //?!


    while(strcmp(enemyCheck,"enemies")!=0){
        fscanf(fp,"%s",enemyCheck);
        if(feof(fp)!=0 || strcmp(enemyCheck,"THEEND")==0){
            fclose(fp);
            return;
        }
    }
    enemy *current = enemyHead;
    while(feof(fp)==0){

        while(strcmp(enemyCheck,"enemy")!=0){
            fscanf(fp,"%s",enemyCheck);
            if(feof(fp)!=0 || strcmp(enemyCheck,"THEEND")==0){
                fclose(fp);
                return;
            }
        }
        // PROBLEM in this
        current -> next = (enemy*) malloc(sizeof(enemy));

        fscanf(fp,"%s",enemyCheck);
        current -> model = enemyCheck[0];
        fscanf(fp,"%s",stringCheck);
        current -> Floor = atoi(stringCheck);
        fscanf(fp,"%s",stringCheck);
        current -> Room = atoi(stringCheck);
        fscanf(fp,"%s",stringCheck);
        current -> Y = atoi(stringCheck);
        fscanf(fp,"%s",stringCheck);
        current -> X = atoi(stringCheck);
        fscanf(fp,"%s",stringCheck);
        current -> HP = atoi(stringCheck);
        fscanf(fp,"%s",stringCheck);
        current -> DMG = atoi(stringCheck);
        fscanf(fp,"%s",stringCheck);
        current -> ticksToDo = atoi(stringCheck);
        current = current -> next;
    }
    fclose(fp);
}
//Read's player stats from LastGame.txt and puts them in player struct
void readPlayerInfo(){
    FILE *fp = fopen(lastGame, "r");
    char stringCheck[50];
    while(feof(fp)==0 && strcmp(stringCheck,"room")!=0){
        fscanf(fp,"%s",stringCheck);
        if(strcmp(stringCheck,"playerX")==0){
            fscanf(fp,"%s",stringCheck);
            player.X = atoi(stringCheck);
        }
        else if(strcmp(stringCheck,"playerY")==0){
            fscanf(fp,"%s",stringCheck);
            player.Y = atoi(stringCheck);
        }
        else if(strcmp(stringCheck,"playerGold")==0){
            fscanf(fp,"%s",stringCheck);
            player.Gold = atoi(stringCheck);
        }
        else if(strcmp(stringCheck,"playerHP")==0){
            fscanf(fp,"%s",stringCheck);
            player.HP = atoi(stringCheck);
        }
        else if(strcmp(stringCheck,"playerDMG")==0){
            fscanf(fp,"%s",stringCheck);
            player.DMG = atoi(stringCheck);
        }
        else if(strcmp(stringCheck,"playerLVL")==0){
            fscanf(fp,"%s",stringCheck);
            player.Level = atoi(stringCheck);
        }
        else if(strcmp(stringCheck,"playerFloor")==0){
            fscanf(fp,"%s",stringCheck);
            player.Floor = atoi(stringCheck);
        }
        else if(strcmp(stringCheck,"playerRoom")==0){
            fscanf(fp,"%s",stringCheck);
            player.Room = atoi(stringCheck);
        }
        else if(strcmp(stringCheck,"playerLevelProgress")==0){
            fscanf(fp,"%s",stringCheck);
            player.LevelProgress = atoi(stringCheck);
        }
        else if(strcmp(stringCheck,"playerLevelMinReq")==0){
            fscanf(fp,"%s",stringCheck);
            player.LevelMinReq = atoi(stringCheck);
        }
    }
    fclose(fp);
}



//here's the fun part(main gameplay here)
void game(){
    int input;
    player.model = '0';
    readPlayerInfo();

    readRoomInfo();

    readObjectInfo();

    readEnemyInfo();
    while(1){
        consoleClear();
        isConsoleSizeRight();
        if(IsContinue() == '0'){
            return;
        }
        //getting size's of a console
        // ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
        roomsDraw();
        objectsDraw();
        enemyMove();
        enemyDraw();
        playerDraw(player.X,player.Y);
        //draws rectangle around text
        //size is min possible rect in +- center
        removePointer();
        input = getCharFork();
        //if afk for 2 minutes - saves game and exit's
        if (tickCount>fps*120){
            tickCount = 1;
            input=101;
        }
        else{
            tickCount++;
        }

        // input = getChar();
        inputStart:
        switch(input){

            case 0:

                break;
                //s/S down
            case 83:
                input = 115;
                goto inputStart;
                break;
            case 115:
                if (tickCount>fps*10){
                tickCount = 1;
                saveGame(1);
                }
                //first 1 - Y (x - 0)
                //second 1 - direction is possitive (0 - negative)
                //last is a room num
                playerMove(1,1,player.Room);
                break;
                //w/W up
            case 87:
                input = 119;
                goto inputStart;
                break;
            case 119:
                if (tickCount>fps*5){
                tickCount = 1;
                saveGame(1);
                }
                playerMove(1,0,player.Room);
                break;
                //a/A left
            case 65:
                input = 97;
                goto inputStart;
                break;
            case 97:
                if (tickCount>fps*5){
                tickCount = 1;
                saveGame(1);
                }
                playerMove(0,0,player.Room);
                break;
                //d/D right
            case 68:
                input = 100;
                goto inputStart;
                break;
            case 100:
                if (tickCount>fps*5){
                tickCount = 1;
                saveGame(1);
                }
                playerMove(0,1,player.Room);
                break;
                //exit e/E
            case 69:
                input = 101;
                goto inputStart;
                break;
            case 101:
                // void startMenu();
                //saveGame(1) if you can continue
                saveGame(1);
                // startMenu();
                return;
                break;
            default:
                break;
        }

    }

}
//there will be different cosmetics/sound settings... mb smth else, idk
void Settings(){
    FILE *fp;
    if ((fp = fopen(settingsFile, "r"))==NULL) {
        fp  = fopen (settingsFile, "w");
        fclose(fp);
    }
    char stringCheck[50];
    int input;
    while(1){
        fopen(settingsFile, "r");
        isConsoleSizeRight();
        //getting size's of a console
        ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
        //clearing the console a fancy way
        consoleClear();
        //margin up
        // addSpacingUp(ws.ws_row/4+2);
        // while(feof(fp)==0){
        // addSpacingLeft(ws.ws_col/4+2);
        // fscanf(fp,"%s",stringCheck);
        // printf("%s",stringCheck);
        // }
        addSpacingUp(ws.ws_row/2);
        addSpacingLeft(ws.ws_col/2-1);
        printf("404\n");
        addSpacingLeft(ws.ws_col/2-10);
        printf("There is nothing here for now\n");
        drawRect(ws.ws_row/2,ws.ws_col/2,ws.ws_row/4,ws.ws_col/4);
        fclose(fp);
        input = getCharFork();
        switch(input){
            case 101:
                return;
                break;
        }
    }
}
void Stats(){
    FILE *fp;
    if ((fp = fopen(statsFile, "r"))==NULL) {
        fp  = fopen (statsFile, "w");
        fclose(fp);
    }
    char stringCheck[50];
    int input;
    while(1){
        fopen(statsFile, "r");
        isConsoleSizeRight();
        //getting size's of a console
        ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
        //clearing the console a fancy way
        consoleClear();
        //margin up
        // addSpacingUp(ws.ws_row/4+2);
        // while(feof(fp)==0){
        // addSpacingLeft(ws.ws_col/4+2);
        // fscanf(fp,"%s",stringCheck);
        // printf("%s",stringCheck);
        // }
        addSpacingUp(ws.ws_row/2);
        addSpacingLeft(ws.ws_col/2-1);

        printf("404\n");
        addSpacingLeft(ws.ws_col/2-10);
        printf("There is nothing here for now\n");
        drawRect(ws.ws_row/2,ws.ws_col/2,ws.ws_row/4,ws.ws_col/4);
        fclose(fp);
        input = getCharFork();
        switch(input){
            case 101:
                return;
                break;
        }
    }
}
void Achivements(){
    FILE *fp;
    if ((fp = fopen(achivementsFile, "r"))==NULL) {
        fp  = fopen (achivementsFile, "w");
        fclose(fp);
    }
    char stringCheck[50];
    int input;
    while(1){
        fopen(achivementsFile, "r");
        isConsoleSizeRight();
        //getting size's of a console
        ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
        //clearing the console a fancy way
        consoleClear();
        //margin up
        // addSpacingUp(ws.ws_row/4+2);
        // while(feof(fp)==0){
        // addSpacingLeft(ws.ws_col/4+2);
        // fscanf(fp,"%s",stringCheck);
        // printf("%s",stringCheck);
        // }
        addSpacingUp(ws.ws_row/2);
        addSpacingLeft(ws.ws_col/2-1);
        printf("404\n");
        addSpacingLeft(ws.ws_col/2-10);
        printf("There is nothing here for now\n");
        drawRect(ws.ws_row/2,ws.ws_col/2,ws.ws_row/4,ws.ws_col/4);
        fclose(fp);
        input = getCharFork();
        switch(input){
            case 101:
                return;
                break;
        }
    }
}


//All functions of a starting screen/menu here
//buttons functions aren't comlited yet'
void startMenu(){




    //check's if mem alloc is ok
    if (head == NULL) {
        return ;
    }


    //first element var's'
    strcpy(head->option, optionsString[0]);
    head->optionNum = 0;
    head->isChosen = 1;
    MenuOption *current = head;
    //giving var's to all next elements
    for (int x = 1; x<MenuOptionsNum; x++){
        current->next = (MenuOption*) malloc(sizeof(MenuOption));
        current = current->next;
        current->optionNum = x;
        current->isChosen = 0;
        strcpy(current->option, optionsString[x]);
    }


    // need to add apptading menu
    int input, pid;
    int whichIsChosen;
    int columns,lines;
    // true/false
    int yOrN;
    while(true){
        isConsoleSizeRight();
        //getting size's of a console
        ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
        lines = (ws.ws_row / 2)-(MenuOptionsNum-1);
        //clearing the console a fancy way
        consoleClear();
        //margin up
        addSpacingUp(lines);
        current = head;
        //setting normal color
        char color[30];
        strcpy(color, greenCl);


        //console memu drawing
        while(current){
            columns = (ws.ws_col / 2) - (strlen(current->option) / 2);
            //margin from left
            addSpacingLeft(columns);

            //color
            if (current->isChosen == 1){
                strcpy(color, blueCl);
                whichIsChosen = current->optionNum;
            }
            if(current->optionNum == 1 && IsContinue() == '0'){
                strcpy(color, normalCl);
            }
            //printing menu parts
            printf("%s%s%s\n\n", color, current->option, normalCl);
            current=current->next;
            //reseting clr
            strcpy(color, greenCl);
        }
        //draws rectangle around text
        ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
        drawRect(ws.ws_row/2,ws.ws_col/2,ws.ws_row/4,ws.ws_col/4);
        input = getCharFork();
        //main menu functions
        inputStart:
        switch(input){
            //Enter pressed
            case 10:
                if ((pid=fork()) < 0) {}
                if(pid == 0){
                    system(menuEnterSound);
                    exit(1);
                }
                switch(whichIsChosen){
                    //NewGameBtn
                    //clear's preveous save and starts a new game
                    //New Game button in starting menu
                    case 0:
                        if((IsContinue() == '1' && AreYouSure() == 1) || IsContinue() == '0'){
                            clearSave();
                            game();
                        }
                        break;
                        //ContinueBtn
                    case 1:
                        if(IsContinue() == '1' ){
                            game();
                        }
                        break;
                        //Settings(mb i'll do some)
                    case 2:
                        Settings();
                        break;
                        //Achivements
                    case 3:
                        Achivements();
                        break;
                        //Stats
                    case 4:
                        Stats();
                        break;
                        //exit
                    case 5:
                        while(head)
                        {
                            current = head->next;
                            free(head);
                            head = current;
                        }
                        room *currentRoom = roomHead;
                        while(roomHead)
                        {
                            currentRoom = roomHead->next;
                            free(roomHead);
                            roomHead = currentRoom;
                        }
                        object *currentObject = objectHead;
                        while(objectHead)
                        {
                            currentObject = objectHead->next;
                            free(objectHead);
                            objectHead = currentObject;
                        }
                        enemy *currentEnemy = enemyHead;
                        while(enemyHead)
                        {
                            currentEnemy = enemyHead->next;
                            free(enemyHead);
                            enemyHead = currentEnemy;
                        }

                        exit(1);
                        break;
                }
                break;
                //up
                    case 87:
                        input = 119;
                        goto inputStart;
                        break;
                        //down
                    case 83:
                        input = 115;
                        goto inputStart;
                        break;
                    case 115:

                        if(whichIsChosen<MenuOptionsNum-1){
                            current = head;
                            for (int i = 0; i<whichIsChosen; i++){
                                current = current->next;
                            }
                            current->isChosen = 0;
                            current = current->next;
                            current->isChosen = 1;
                            if ((pid=fork()) < 0) {}
                            if(pid == 0){
                                system(menuSound);
                                exit(1);
                            }
                        }

                        break;
                    case 119:
                        if(whichIsChosen>0){
                            current = head;
                            for (int i = 0; i<whichIsChosen-1; i++){
                                current = current->next;
                            }
                            current->isChosen = 1;
                            current = current->next;
                            current->isChosen = 0;
                            if ((pid=fork()) < 0) {}
                            if(pid == 0){
                                system(menuSound);
                                exit(1);
                            }
                        }

                        break;
                    default:
                        printf("Change your keyboard layout or go to Settings");
                        break;


        }
    }


}


//global starting time to use
const int start_time = sTime();
//starting a programm i guess... nothing interesting here
int main(){
    FILE *fp;
    if ((fp = fopen(achivementsFile, "r"))==NULL) {
        fp  = fopen (achivementsFile, "w");
        fclose(fp);
    }
    if ((fp = fopen(statsFile, "r"))==NULL) {
        fp  = fopen (statsFile, "w");
        fclose(fp);
    }
    if ((fp = fopen(settingsFile, "r"))==NULL) {
        fp  = fopen (settingsFile, "w");
        fclose(fp);
    }
    if ((fp = fopen(lastGame, "r"))==NULL) {
        fp  = fopen (lastGame, "w");
        fclose(fp);
    }
    objectHead = (object*) malloc(sizeof(object));
    roomHead = (room*) malloc(sizeof(room));
    enemyHead = (enemy*) malloc(sizeof(enemy));
    head = (MenuOption*) malloc(sizeof(MenuOption));
    srand(start_time);
    startMenu();
    return 0;
}
