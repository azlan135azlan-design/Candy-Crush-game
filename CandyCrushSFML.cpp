
//                                      CANDY CRUSH 

//  Programming Fundamentals - 1st Semester Project
//
//  Team :  Muhammad Azlan Asad      (26L-2618)  
//          Muhammad Abdul Rehman    (26L-2585)  
//          Muhammad Hassan Mukhtar  (26L-2627)  
//  version : Almost 60% (first evaluation)


#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;
using namespace sf;

// SECTION 1

const int rows = 8;
const int colms = 8;
const int EMPTY = -1;            
const int max_color = 6;         
const int max_level = 3;
const int max_score = 10;        
const int numofbosters = 2;     

const int boosterprice[numofbosters] = { 20, 40 };
const string bostername[numofbosters] = { "Shuffle", "+5 Moves" };
const string difficulityname[2] = { "Normal", "Hard" };

const int GOAL_SCORE = 0;       
const int GOAL_COLLECT = 1;     
const int GOAL_JELLY = 2;        
const int GOAL_ENDLESS = 3;

const int LS_PLAYING = 0;
const int LS_WON = 1;
const int LS_LOST = 2;

int boardColor[rows][colms];
int boardJelly[rows][colms];

int levelNumber = 1;
int goalType = GOAL_SCORE;
int goalColor = 0;
int goalCount = 0;
int starScore[3] = { 1000, 1600, 2200 };
int startMoves = 20;
int numColors = 6;

int gameScore = 0;
int movesLeft = 0;
int collected = 0;
int combo = 0;
int gameDifficulty = 0;

string playerName;
int playerCoins = 0;
int playerDifficulty = 0;
int playerBoosters[numofbosters] = { 3, 3 };
int levelsUnlocked = 1;
int playerStars[max_level] = { 0, 0, 0 };

string leaderNames[max_score];
int leaderScores[max_score];
int leaderDifficulties[max_score];
int leaderCount = 0;

const int levelGoals[max_level] = { GOAL_SCORE, GOAL_COLLECT, GOAL_JELLY };
const int levelGoalColors[max_level] = { 0, 4, 0 };
const int levelGoalCounts[max_level] = { 0, 10, 9 };
const int levelMoves[max_level] = { 20, 22, 24 };
const int levelStars[max_level][3] = {
    { 800, 1200, 1600 },
    { 600, 1000, 1400 },
    { 900, 1400, 2000 }
};

inline bool inBoard(int r, int c) {
    return r >= 0 && r < rows && c >= 0 && c < colms;
}

inline int randInt(int lo, int hi) {
    return lo + rand() % (hi - lo + 1);
}


//SECTION 2


//engine - is - integrated - here-with-same-variables-names-as-used;;;

bool canswap(int r1, int c1, int r2, int c2) {
    if (!inBoard(r1, c1) || !inBoard(r2, c2)) {
        return false;
    }
    int rowDifference = abs(r1 - r2);
    int colDifference = abs(c1 - c2);
    if (rowDifference + colDifference != 1) {
        return false;
    }
    if (boardColor[r1][c1] == EMPTY || boardColor[r2][c2] == EMPTY) {
        return false;
    }
    return true;
}

void swapPositions(int r1, int c1, int r2, int c2){
    int temp = boardColor[r1][c1];
    boardColor[r1][c1] = boardColor[r2][c2];
    boardColor[r2][c2] = temp;
}


bool lookmatches(bool marked[rows][colms]) {
    bool foundanymatches = false;

    for (int r = 0; r < rows; r++)
        for (int c = 0; c < colms; c++)
            marked[r][c] = false;

// horizontal lines
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < colms - 2; c++)
        {
            int candy = boardColor[r][c];
            if (candy != EMPTY && boardColor[r][c + 1] == candy && boardColor[r][c + 2] == candy)
            {
                marked[r][c] = true;
                marked[r][c + 1] = true;
                marked[r][c + 2] = true;
                foundanymatches = true;
            }
        }
    }

    // vertical lines
    for (int r = 0; r < rows - 2; r++)
    {
        for (int c = 0; c < colms; c++)
        {
            int candy = boardColor[r][c];
            if (candy != EMPTY && boardColor[r + 1][c] == candy && boardColor[r + 2][c] == candy)
            {
                marked[r][c] = true;
                marked[r + 1][c] = true;
                marked[r + 2][c] = true;
                foundanymatches = true;
            }
        }
    }
    return foundanymatches;
}
// swap two candies; if they do not make a match, put them back
bool swapCandies(int r1, int c1, int r2, int c2, bool marked[rows][colms]) {
    if (!canswap(r1, c1, r2, c2)) 
    {
        return false;
    }
    swapPositions(r1, c1, r2, c2);
    if (lookmatches(marked)) {
        return true;
    }
    swapPositions(r1, c1, r2, c2);
    
    return false;
    
}

void gravity(int fall[rows][colms]) {
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < colms; c++)
            fall[r][c] = 0;

    for (int c = 0; c < colms; c++) {
        for (int r = rows - 1; r >= 0; r--)
        {
            if (boardColor[r][c] == EMPTY)
            {
                for (int i = r - 1; i >= 0; i--)
                {
                    if (boardColor[i][c] != EMPTY){
                        boardColor[r][c] = boardColor[i][c];
                        boardColor[i][c] = EMPTY;
                        fall[r][c] = r - i;
                        break;
                    }
                }
            }
        }
    }
}

void refillboard(int fall[rows][colms]) {
    for (int c = 0; c < colms; c++) {
        int emptyCount = 0;
        for (int r = 0; r < rows; r++)
            if (boardColor[r][c] == EMPTY) emptyCount++;

        for (int r = 0; r < rows; r++) {
            if (boardColor[r][c] == EMPTY){
                boardColor[r][c] = randInt(0, numColors - 1);
                fall[r][c] = emptyCount;
          }
      }
  }
}
void dropcandies(int fall[rows][colms]) {
    gravity(fall);
    refillboard(fall);
}


bool findvalid_moves(){
    bool tempMarked[rows][colms];

    for (int r = 0; r < rows; r++)  {
        for (int c = 0; c < colms; c++) {
            if (c + 1 < colms && canswap(r, c, r, c + 1)){
                swapPositions(r, c, r, c + 1);
                bool found = lookmatches(tempMarked);
                swapPositions(r, c, r, c + 1);
                if (found) return true;
            }
            if (r + 1 < rows && canswap(r, c, r + 1, c)){
                swapPositions(r, c, r + 1, c);
                bool found = lookmatches(tempMarked);
                swapPositions(r, c, r + 1, c);
                if (found) return true;
            }
         }
    }
     return false;
}


void initializeboard() {
    for (int attempt = 0; attempt < 200; attempt++){
        for (int r = 0; r < rows; r++){
            for (int c = 0; c < colms; c++) {
                int candy;
                bool makesMatch;
                do{
                    makesMatch = false;
                    candy = randInt(0, numColors - 1);
                    if (c >= 2 && boardColor[r][c - 1] == candy && boardColor[r][c - 2] == candy)
                        makesMatch = true;
                    if (r >= 2 && boardColor[r - 1][c] == candy && boardColor[r - 2][c] == candy)
                        makesMatch = true;
                } while (makesMatch);

                boardColor[r][c] = candy;
           }
        }
        if (findvalid_moves()) return;
     }
}
void shuffleboard(){
    bool tempMarked[rows][colms];

    for (int attempt = 0; attempt < 400; attempt++){
        for (int i = 0; i < rows * colms * 3; i++)
        {
            int r1 = randInt(0, rows - 1);
            int c1 = randInt(0, colms - 1);
            int r2 = randInt(0, rows - 1);
            int c2 = randInt(0, colms - 1);
            swapPositions(r1, c1, r2, c2);
        }
        if (!lookmatches(tempMarked) && findvalid_moves()) return;
    }
    initializeboard();
}

//  SECTIOn 3 
void getlevelDefer(int number,int diff)
{
levelNumber=number;
if(diff<0)
{
    diff=0;
}
if(diff>1)
{
diff=1;
}
numColors=6;
if(number>=1&&number<=max_level)
{
int index=number-1;
goalType=levelGoals[index];
goalColor = levelGoalColors[index];
goalCount = levelGoalCounts[index];
float factor = 1.f;
if (diff == 1) 
{
    factor = 0.8f; 
}
startMoves = (int)(levelMoves[index] * factor + 0.5f);
 for (int i = 0; i < 3; i++)
     {
         starScore[i] = levelStars[index][i];
     }
    }
    else{
        goalType = GOAL_ENDLESS;
        goalColor = 0;
        goalCount = 0;
        startMoves = 25;
        if (diff == 1)
{
startMoves = 20;
}
starScore[0]=starScore[1]=starScore[2]=0;
    }
}
void locallevels(int number, int diff) 
{
    getlevelDefer(number, diff);
    gameScore = 0;
    movesLeft = startMoves;
    collected = 0;
    combo = 0;
    gameDifficulty = diff;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < colms; c++)
        {
            boardColor[r][c] = EMPTY;
            boardJelly[r][c] = 0;
            // level 3: a 3x3 jelly area in the centre
            if (number == 3 && r >= 2 && r <= 4 && c >= 2 && c <= 4)
            {
                boardJelly[r][c] = 1;
            }
       }
     }
}

// remove the marked candies, count score, goal and jelly
int removedcandies(bool marked[rows][colms], int comboValue) 
{
    int cleared = 0;
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < colms; c++)
        {
            if (!marked[r][c] || boardColor[r][c] == EMPTY) continue;
            cleared++;
            if (goalType == GOAL_COLLECT && boardColor[r][c] == goalColor)
                collected++;
            if (boardJelly[r][c] > 0)
                boardJelly[r][c]--;
            boardColor[r][c] = EMPTY;
        }
    }
    gameScore += cleared * 20 * comboValue;
    if (goalType == GOAL_ENDLESS && comboValue >= 3 && cleared > 0)
    {
        movesLeft++;                           // bonus move for a big combo
    }
    return cleared;
}
int countJelly()  {
    int count = 0;
    for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < colms; c++)
            {
            if (boardJelly[r][c] > 0) 
            {
                count++;
            }
            }
        }
    return count;
}

int checklevelstatus() {
    bool won = false;
    if (goalType == GOAL_SCORE) 
    {
        won = (gameScore >= starScore[0]);
    }
    if (goalType == GOAL_COLLECT) 
    {
        won = (collected >= goalCount);
    }
    if (goalType == GOAL_JELLY)  
    {
        won = (countJelly() == 0);
    }

    if (won)
    { 
        return LS_WON;
    }
    if (movesLeft <= 0)
    {
        return LS_LOST;
    }
    return LS_PLAYING;
}

int calcstars(){
    int stars = 1;
    if (gameScore >= starScore[1]) 
    {
        stars++;
    }
    if (gameScore >= starScore[2]) 
    {
        stars++;
    }
    return stars;
}

int calccoins(int stars){
    return 10 * stars;
}



//game - rules - levels - scoring - and -progression - is - integrated - here;;;




//  SECTION 4 

const int   WIN_W = 1100;
const int   WIN_H = 740;
const float CELL = 70.f;            
const float BOARD_X = 60.f;             
const float BOARD_Y = 140.f;
const float PI = 3.14159265f;

const string PROFILE_FILE = "progress.txt";
const int    NAME_MAX_LEN = 12;


const int SCR_MENU = 0;
const int SCR_NAME = 1;
const int SCR_LEVELS = 2;
const int SCR_SHOP = 3;
const int SCR_LEADER = 4;
const int SCR_GAME = 5;


const int OV_NONE = 0;
const int OV_PAUSE = 1;
const int OV_WIN = 2;
const int OV_LOSE = 3;
const int OV_ENDLESS = 4;

const int NA_LEVELS = 0;                
const int NA_ENDLESS = 1;

RenderWindow gWindow(VideoMode({ (unsigned)WIN_W, (unsigned)WIN_H }), "Candy Crush",
    Style::Titlebar | Style::Close);
Font gFont;
float gTime = 0.f;

int screen = SCR_MENU;
int afterName = NA_LEVELS;
int overlay = OV_NONE;
int currentLevel = 1;
int endStars = 0, endCoins = 0, endRank = -1;

float mouseX = 0.f, mouseY = 0.f;
bool mouseDown = false, clicked = false;
bool keyEsc = false, keyEnter = false, keyBackspace = false;
bool keyP = false, keyNum1 = false, keyNum2 = false;
string typedText;
string nameBuf, nameMsg;                

string toastMsg;
float  toastT = 0.f;
Color  toastCol(255, 255, 255);

void showToast(string msg, Color col) {
    toastMsg = msg;
    toastCol = col;
    toastT = 2.5f;
}

float clampf(float v, float lo, float hi) {
    if (v< lo) return lo;
    if (v> hi) return hi;
    return v;
}

int clampInt(int v, int lo, int hi){
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

int toByte(float v){
    return (int)clampf(v, 0.f, 255.f);
}

Color withAlpha(Color c, float k){
    c.a = toByte(c.a * k);
    return c;
}

Color lighten(Color c, float t)  {
    return Color((int)(c.r + (255 - c.r) * t), (int)(c.g + (255 - c.g) * t),
        (int)(c.b + (255 - c.b) * t), c.a);
}

Color darken(Color c, float t) {
    return Color((int)(c.r * (1.f - t)), (int)(c.g * (1.f - t)),
        (int)(c.b * (1.f - t)), c.a);
}

const Color COL_WHITE(255, 255, 255);
const Color COL_SUB(214, 198, 246);
const Color COL_GOLD(255, 206, 62);
const Color COL_PINK(255, 96, 166);
const Color COL_GREEN(96, 220, 128);
const Color COL_RED(255, 92, 100);
const Color COL_CLEAR(0, 0, 0, 0);
const Color COL_PANEL(38, 18, 78, 226);

const Color CANDY_MAIN[max_color] = {
    Color(236, 64, 86),  Color(76, 196, 96),  Color(64, 140, 240),
    Color(252, 208, 48), Color(168, 92, 224), Color(255, 146, 40)
};
const string COLOR_NAME[max_color] = { "red", "green", "blue", "yellow", "purple", "orange" };

bool isValidName(string s) {
    if (s.empty() || (int)s.size() > NAME_MAX_LEN) {
        return false;
    }
    for (size_t i = 0; i < s.size(); i++)
    {
        char ch = s[i];
        bool isLetter = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
        bool isDigit = (ch >= '0' && ch <= '9');
        if (!isLetter && !isDigit) {
            return false;
        }
     }
    return true;
}

void setdefaultprofile(){
    playerName = "";
    playerCoins = 0;
    playerDifficulty = 0;
    for (int i = 0; i < numofbosters; i++) playerBoosters[i] = 3;
    levelsUnlocked = 1;
    for (int i = 0; i < max_level; i++) playerStars[i] = 0;
}
 
bool loadprofile() {
    setdefaultprofile();
    ifstream file(PROFILE_FILE);
    if (!file) return false;

    string key;
    while (file >> key){
        int value;
        if (key == "name") {
            string name;
            if ((file >> name) && isValidName(name)) playerName = name;
        }
        else if (key == "coins")
        {
            if (file >> value) playerCoins = clampInt(value, 0, 999999);
        }
        else if (key == "difficulty")  {
            if (file >> value) playerDifficulty = clampInt(value, 0, 1);
        }
        else if (key == "boosters")
        {
            for (int i = 0; i < numofbosters; i++)
                if (file >> value) playerBoosters[i] = clampInt(value, 0, 99);
        }
        else if (key == "unlocked")
        {
            if (file >> value) levelsUnlocked = clampInt(value, 1, max_level);
        }
        else if (key == "stars")
        {
            for (int i = 0; i < max_level; i++)
                if (file >> value) playerStars[i] = clampInt(value, 0, 3);
        }

        if (file.fail())                       // bad number: skip the line
        {
            file.clear();
            file.ignore(1000, '\n');
        }
    }
    return true;
}

bool saveprofiles(){
    ofstream file(PROFILE_FILE);
    if (!file)
    {
        showToast("Could not save your progress!", COL_RED);
        return false;
    }
    if (playerName.empty()) file << "name Player\n";
    else                    file << "name " << playerName << "\n";

    file << "coins " << playerCoins << "\n";
    file << "difficulty " << playerDifficulty << "\n";
    file << "boosters";
    for (int i = 0; i < numofbosters; i++) file << " " << playerBoosters[i];
    file << "\n";
    file << "unlocked " << levelsUnlocked << "\n";
    file << "stars";
    for (int i = 0; i < max_level; i++) file << " " << playerStars[i];
    file << "\n";
    return true;
}

const int AL_LEFT = 0, AL_CENTER = 1, AL_RIGHT = 2;

void drawbox(float x, float y, float w, float h, Color fill,Color outline = COL_CLEAR, float thick = 0.f) {
    RectangleShape shape;
    shape.setSize({ w, h });
    shape.setPosition({ x, y });
    shape.setFillColor(fill);
    shape.setOutlineColor(outline);
    shape.setOutlineThickness(thick);
    gWindow.draw(shape);
}

void drawcircle(float cx, float cy, float rad, Color fill,Color outline = COL_CLEAR, float thick = 0.f) {
    CircleShape shape(rad);
    shape.setPointCount(36);
    shape.setOrigin({ rad, rad });
    shape.setPosition({ cx, cy });
    shape.setFillColor(fill);
    shape.setOutlineColor(outline);
    shape.setOutlineThickness(thick);
    gWindow.draw(shape);
}

void drawbar(float x1, float y1, float x2, float y2, float thick, Color col){
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrt(dx * dx + dy * dy);
    float angle = atan2(dy, dx) * 180.f / PI;

    RectangleShape shape;
    shape.setSize({ len, thick });
    shape.setOrigin({ len * 0.5f, thick * 0.5f });
    shape.setPosition({ (x1 + x2) * 0.5f, (y1 + y2) * 0.5f });
    shape.setRotation(degrees(angle));
    shape.setFillColor(col);
    gWindow.draw(shape);
}

// background colour that fades from top to bottom (20 strips)
void drawgradient(float x, float y, float w, float h, Color top, Color bottom) {
    for (int i = 0; i < 20; i++){
        float t = i / 19.f;
        Color current((int)(top.r + (bottom.r - top.r) * t),
            (int)(top.g + (bottom.g - top.g) * t),
            (int)(top.b + (bottom.b - top.b) * t));
        drawbox(x, y + h * i / 20.f, w, h / 20.f + 1.f, current);
      }
}

// five pointed star
void drawstar(float cx, float cy, float r, Color fill, Color outline = COL_CLEAR, float thick = 0.f){
    ConvexShape star;
    star.setPointCount(10);
    for (int i = 0; i < 10; i++){
        float radius = r;
        if (i % 2 == 1) radius = r * 0.45f;                // inner points
        float angle = (-90.f + i * 36.f) * PI / 180.f;
        star.setPoint(i, Vector2f(cx + radius * cos(angle), cy + radius * sin(angle)));
    }
    star.setFillColor(fill);
    star.setOutlineColor(outline);
    star.setOutlineThickness(thick);
    gWindow.draw(star);
}

void drawtxt(string s, float size, float x, float y, Color col,int align = AL_CENTER, float outline = 2.f) {
    Text t(gFont, s, (unsigned)size);
    t.setFillColor(col);
    if (outline > 0.f)
    {
        t.setOutlineColor(Color(20, 6, 44, 170));
        t.setOutlineThickness(outline);
    }
    FloatRect b = t.getLocalBounds();
    float originX = b.position.x + b.size.x * 0.5f;
    if (align == AL_LEFT)  originX = b.position.x;
    if (align == AL_RIGHT) originX = b.position.x + b.size.x;
    t.setOrigin({ originX, size * 0.64f });
    t.setPosition({ x, y });
    gWindow.draw(t);
}

float textWidth(string s, float size)
{
    Text t(gFont, s, (unsigned)size);
    return t.getLocalBounds().size.x;
}

void drawcoin(float cx, float cy, float r) {
    drawcircle(cx, cy, r, Color(255, 205, 50), Color(190, 120, 20), r * 0.14f);
    drawcircle(cx, cy, r * 0.62f, Color(255, 226, 110), Color(225, 160, 30), r * 0.08f);
}

void drawlock(float cx, float cy, float s, Color col) {
    drawcircle(cx, cy - s * 0.16f, s * 0.24f, COL_CLEAR, col, s * 0.1f);
    drawbox(cx - s * 0.36f, cy - s * 0.04f, s * 0.72f, s * 0.54f, col);
}

void drawtick(float cx, float cy, float s, Color col) {
    drawbar(cx - s * 0.45f, cy, cx - s * 0.12f, cy + s * 0.34f, s * 0.2f, col);
    drawbar(cx - s * 0.12f, cy + s * 0.34f, cx + s * 0.5f, cy - s * 0.36f, s * 0.2f, col);
}


void drawCandy(int type, float cx, float cy) {
    if (type < 0 || type >= max_color) return;
    float radius = CELL * 0.33f;
    Color candyColor = CANDY_MAIN[type];

    drawcircle(cx + 2.f, cy + 3.f, radius, Color(10, 5, 25, 75));
    drawcircle(cx, cy, radius, candyColor, darken(candyColor, 0.42f), 2.f);
    drawcircle(cx - radius * 0.28f, cy - radius * 0.32f, radius * 0.18f, Color(255, 255, 255, 190));
}

void drawjelly(float x, float y) {
    drawbox(x + 3.f, y + 3.f, CELL - 6.f, CELL - 6.f, Color(255, 150, 205, 160),
        Color(255, 205, 235, 220), 2.f);
}

void drawbackground(Color top, Color bottom, bool withCandies)
{
    drawgradient(0.f, 0.f, (float)WIN_W, (float)WIN_H, top, bottom);
    if (!withCandies) return;
    drawCandy(0, 110.f, 150.f);
    drawCandy(2, 950.f, 170.f);
    drawCandy(3, 160.f, 570.f);
    drawCandy(4, 900.f, 570.f);
    drawbox(0.f, 0.f, (float)WIN_W, (float)WIN_H, Color(40, 10, 80, 150));   // fade the candis
}

bool mouseIn(float x, float y, float w, float h) {
    return mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h;
}

bool uibutton(float x, float y, float w, float h, string label, int style = 0, bool enabled = true, float fontSize = 26.f)
{
    const Color styles[5] = {
        Color(255, 86, 160), Color(120, 84, 220), Color(66, 200, 110),
        Color(236, 72, 84), Color(255, 190, 40)
    };
    bool hover = enabled && mouseIn(x, y, w, h);

    Color fill = styles[clampInt(style, 0, 4)];
    if (!enabled)    fill = Color(110, 96, 140);
    else if (hover)  fill = lighten(fill, 0.18f);

    float grow = 0.f;
    if (hover) grow = 3.f;

    drawbox(x - grow, y - grow + 5.f, w + 2.f * grow, h + 2.f * grow, Color(0, 0, 0, 95));
    drawbox(x - grow, y - grow, w + 2.f * grow, h + 2.f * grow, fill, darken(fill, 0.42f), 3.f);
    drawtxt(label, fontSize, x + w * 0.5f, y + h * 0.5f, COL_WHITE);

    if (hover && clicked){
        clicked = false;
        return true;
    }
    return false;
}

void drawPanel(float x, float y, float w, float h){
    drawbox(x, y + 6.f, w, h, Color(0, 0, 0, 90));
    drawbox(x, y, w, h, COL_PANEL, Color(255, 150, 210, 120), 3.f);
}

void drawCoinsPill(float x, float y, float w, float h) {
    drawbox(x, y, w, h, Color(24, 10, 52, 220), Color(255, 206, 62, 170), 2.f);
    drawcoin(x + h * 0.5f, y + h * 0.5f, h * 0.32f);
    drawtxt(to_string(playerCoins), h * 0.52f, x + h + (w - h) * 0.4f, y + h * 0.5f, COL_GOLD);
}

void drawToast() {
    if (toastT <= 0.f) return;
    float a = clampf(toastT / 0.5f, 0.f, 1.f);              // fades out at the end
    float w = textWidth(toastMsg, 24.f) + 56.f;
    drawbox(WIN_W * 0.5f - w * 0.5f, WIN_H - 78.f, w, 46.f,
        Color(20, 8, 44, toByte(225 * a)), withAlpha(toastCol, a), 2.f);
    drawtxt(toastMsg, 24.f, WIN_W * 0.5f, WIN_H - 55.f, withAlpha(toastCol, a), AL_CENTER, 0.f);
}

Color diffColor(int d){           
    if (d == 0) return COL_GOLD;
    return COL_RED;
}

// the goal of a level as a sentence
string goaltext(int number) {
    if (number < 1 || number > max_level) return "Score as high as you can";
    int index = number - 1;
    if (levelGoals[index] == GOAL_SCORE)
        return "Reach " + to_string(levelStars[index][0]) + " points";
    if (levelGoals[index] == GOAL_COLLECT)
        return "Collect " + to_string(levelGoalCounts[index]) + " " +
        COLOR_NAME[levelGoalColors[index]] + " candies";
    return "Clear all the jelly";
}

// call when a new scren open
void onEnterScreen(int s){
    if (s == SCR_NAME){
        nameBuf = playerName;
        nameMsg = "";
    }
}

void goTo(int s) {
    screen = s;
    onEnterScreen(s);
}

const int PH_IDLE = 0;       
const int PH_SWAP = 1;       
const int PH_SWAPBACK = 2;   
const int PH_FALL = 3;       

int boardPhase = PH_IDLE;
float boardTime = 0.f, boardDuration = 0.2f, fallTotal = 0.f;
int selectedRow = -1, selectedCol = -1;
int fromRow = 0, fromCol = 0, toRow = 0, toCol = 0;
int mouseRow = 0, mouseCol = 0;
bool boardMarked[rows][colms];
int fallRows[rows][colms];

float cellCenterX(int c) { return BOARD_X + c * CELL + CELL * 0.5f; }
float cellCenterY(int r) { return BOARD_Y + r * CELL + CELL * 0.5f; }

bool cellfrom_mouse(float mx, float my) {
    float fx = (mx - BOARD_X) / CELL;
    float fy = (my - BOARD_Y) / CELL;
    if (fx < 0.f || fy < 0.f) return false;
    mouseCol = (int)fx;
    mouseRow = (int)fy;
    return inBoard(mouseRow, mouseCol);
}

void resetboardview() {
    boardPhase = PH_IDLE;
    boardTime = fallTotal = 0.f;
    selectedRow = selectedCol = -1;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < colms; c++){
            boardMarked[r][c] = false;
            fallRows[r][c] = 0;
        }
}

float fallTime(int distance){
    return 0.07f * distance;
    // a candy take 0.07 seconds for each row it fall
}

void startfalls() {
    boardPhase = PH_FALL;
    boardTime = 0.f;
    fallTotal = 0.12f;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < colms; c++)
            if (fallRows[r][c] > 0 && fallTime(fallRows[r][c]) > fallTotal)
                fallTotal = fallTime(fallRows[r][c]);
}

float fallOffsetY(int r, int c) {
    int distance = fallRows[r][c];
    if (distance <= 0) return 0.f;
    float progress = clampf(boardTime / fallTime(distance), 0.f, 1.f);
    return -distance * CELL * (1.f - progress);
}

//sve progres and open the result box
void onLevelEnd(int st){
    boardPhase = PH_IDLE;
    if (st == LS_WON){
        endStars = calcstars();
        endCoins = calccoins(endStars);

        int li = levelNumber - 1;
        if (endStars > playerStars[li]) playerStars[li] = endStars;
        if (levelNumber < max_level && levelsUnlocked < levelNumber + 1)
            levelsUnlocked = levelNumber + 1;
        playerCoins += endCoins;
        saveprofiles();
        overlay = OV_WIN;
    }
    else {
        overlay = OV_LOSE;
    }
}

void settleBoard(){
    boardPhase = PH_IDLE;
    int status = checklevelstatus();
    if (status != LS_PLAYING){
        onLevelEnd(status);
        return;
    }
    if (!findvalid_moves()){
        shuffleboard();
        showToast("No moves left - the board was shuffled!", COL_GOLD);
    }
 }

// remove matche candi at once then animate falling
void beginPop() {
    removedcandies(boardMarked, combo);
    dropcandies(fallRows);
    startfalls();
}

void cascades(){
    if (lookmatches(boardMarked))
    {
        combo++;
        beginPop();
    }
    else
    {
        combo = 0;
        settleBoard();
    }
}

void beginswaping(int r1, int c1, int r2, int c2){
    fromRow = r1;  fromCol = c1;
    toRow = r2;    toCol = c2;
    boardPhase = PH_SWAP;
    boardTime = 0.f;
    boardDuration = 0.2f;
    selectedRow = selectedCol = -1;
}

// the swap animation is over does it swapped
void finishSwap(){
    if (swapCandies(fromRow, fromCol, toRow, toCol, boardMarked)){
        movesLeft--;
        combo = 1;
        beginPop();
    }
    else  {                                      // no match so slide back & no move is lost
        boardPhase = PH_SWAPBACK;
        boardTime = 0.f;
    }
}

void startLevel(int n){
    currentLevel = n;
    locallevels(n, playerDifficulty);          
    initializeboard();                       
    resetboardview();
    overlay = OV_NONE;

    for (int r = 0; r < rows; r++)
        for (int c = 0; c < colms; c++)
            fallRows[r][c] = r + 2;
    startfalls();

    if (screen != SCR_GAME) goTo(SCR_GAME);
}

// first click sellect candy, second click on a neighbour swaps them
void clickCell(int r, int c) {
    if (selectedRow < 0)
    {
        selectedRow = r;
        selectedCol = c;
        return;
    }
    if (selectedRow == r && selectedCol == c)          // same square so unselect
    {
        selectedRow = selectedCol = -1;
        return;
    }
    if (canswap(selectedRow, selectedCol, r, c))
    {
        beginswaping(selectedRow, selectedCol, r, c);
    }
    else                                               // far away candy select the new one
{
        selectedRow = r;
        selectedCol = c;
    }
}

void updateBoard(float dt) {
    boardTime += dt;

    if (boardPhase == PH_SWAP && boardTime >= boardDuration)
        finishSwap();
    else if (boardPhase == PH_SWAPBACK && boardTime >= boardDuration)
        boardPhase = PH_IDLE;
    else if (boardPhase == PH_FALL && boardTime >= fallTotal)
        cascades();
}

void drawOneCell(int r, int c) {
    if (boardColor[r][c] == EMPTY) return;

    float ox = 0.f, oy = 0.f;
    if (boardPhase == PH_SWAP || boardPhase == PH_SWAPBACK){
        float k = clampf(boardTime / boardDuration, 0.f, 1.f);
        float p = k;
        if (boardPhase == PH_SWAPBACK) p = 1.f - k;

        if (r == fromRow && c == fromCol)
        {
            ox = (toCol - fromCol) * CELL * p;
            oy = (toRow - fromRow) * CELL * p;
        }
        else if (r == toRow && c == toCol)
        {
            ox = (fromCol - toCol) * CELL * p;
            oy = (fromRow - toRow) * CELL * p;
        }
    }
    else if (boardPhase == PH_FALL) {
        oy = fallOffsetY(r, c);
    }
    drawCandy(boardColor[r][c], cellCenterX(c) + ox, cellCenterY(r) + oy);
}

void drawBoard() {
    drawbox(BOARD_X - 12.f, BOARD_Y - 12.f, colms * CELL + 24.f, rows * CELL + 24.f,
        Color(24, 10, 52, 235), Color(255, 150, 210, 140), 4.f);

    // checkerboard tile with  jely
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < colms; c++)
        {
            float x = BOARD_X + c * CELL;
            float y = BOARD_Y + r * CELL;
            Color tile(88, 54, 150, 170);
            if ((r + c) % 2 == 0) tile = Color(70, 40, 128, 170);
            drawbox(x + 2.f, y + 2.f, CELL - 4.f, CELL - 4.f, tile);
            if (boardJelly[r][c] > 0) drawjelly(x, y);
        }
    }

    bool swapping = (boardPhase == PH_SWAP || boardPhase == PH_SWAPBACK);
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < colms; c++)
        {
            bool isSwapCandy = (r == fromRow && c == fromCol) || (r == toRow && c == toCol);
            if (swapping && isSwapCandy) continue;
            drawOneCell(r, c);
        }
    }
    if (swapping)
    {
        drawOneCell(fromRow, fromCol);
        drawOneCell(toRow, toCol);
    }

    // white frame around the selected candy
    if (boardPhase == PH_IDLE && selectedRow >= 0)
        drawbox(BOARD_X + selectedCol * CELL + 2.f, BOARD_Y + selectedRow * CELL + 2.f,
            CELL - 4.f, CELL - 4.f, COL_CLEAR, Color(255, 255, 255, 235), 3.5f);
}


//  SCREEN
void drawMenu(){
    drawbackground(Color(78, 26, 140), Color(220, 72, 150), true);
    drawtxt("CANDY", 84.f, WIN_W * 0.5f, 70.f, COL_GOLD, AL_CENTER, 5.f);
    drawtxt("CRUSH", 84.f, WIN_W * 0.5f, 150.f, COL_PINK, AL_CENTER, 5.f);

    float bx = WIN_W * 0.5f - 190.f;
    float bw = 380.f, bh = 52.f;
    float y = 232.f;                       
    float step = 62.f;

    if (uibutton(bx, y, bw, bh, "Play", 0)){
        afterName = NA_LEVELS;
        goTo(SCR_NAME);
    }
    y += step;

    if (uibutton(bx, y, bw, bh, "Level Select", 1)){
        if (playerName.empty()) {
            afterName = NA_LEVELS;
            goTo(SCR_NAME);
         }
        else{
            goTo(SCR_LEVELS);
        }
      }
    y += step;

    int diffStyle = 4;
    if (playerDifficulty == 1) diffStyle = 3;
    if (uibutton(bx, y, bw, bh, "Difficulty: " + difficulityname[playerDifficulty], diffStyle))
    {
        playerDifficulty = (playerDifficulty + 1) % 2;
        saveprofiles();
    }
    y += step;

    if (uibutton(bx, y, bw, bh, "Exit", 3))
        gWindow.close();

    drawCoinsPill(24.f, WIN_H - 66.f, 150.f, 44.f);
    string who = "Welcome!";
    if (!playerName.empty()) who = "Player: " + playerName;
    drawtxt(who, 24.f, WIN_W - 28.f, WIN_H - 44.f, COL_WHITE, AL_RIGHT);
}

//name entry 
void submitName(){
    if (!isValidName(nameBuf))
    {
        nameMsg = "Enter 1 to 12 letters or numbers";
        return;
    }
    playerName = nameBuf;
    saveprofiles();

    goTo(SCR_LEVELS);
}

void drawNameEntry(){
    drawbackground(Color(78, 26, 140), Color(220, 72, 150), true);

    // typing only letter and digits are accepted maximum 12
    for (size_t i = 0; i < typedText.size(); i++) {
        char ch = typedText[i];
        bool isLetter = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
        bool isDigit = (ch >= '0' && ch <= '9');
        if (!isLetter && !isDigit) continue;

        if ((int)nameBuf.size() < NAME_MAX_LEN) {
            nameBuf += ch;
            nameMsg = "";
        }
        else {
            nameMsg = "Maximum 12 characters";
        }
    }
    if (keyBackspace && !nameBuf.empty()){
        nameBuf.erase(nameBuf.size() - 1);
        nameMsg = "";
    }
    if (keyEnter) submitName();
    if (keyEsc)   goTo(SCR_MENU);

    float px = WIN_W * 0.5f - 270.f, py = 170.f, pw = 540.f, ph = 340.f;
    drawPanel(px, py, pw, ph);
    drawtxt("ENTER YOUR NAME", 40.f, px + pw * 0.5f, py + 58.f, COL_GOLD, AL_CENTER, 3.f);
    drawtxt("1 to 12 letters or numbers", 21.f, px + pw * 0.5f, py + 100.f, COL_SUB, AL_CENTER, 0.f);

    drawbox(px + 40.f, py + 130.f, pw - 80.f, 66.f, Color(255, 255, 255, 238), COL_PINK, 4.f);
    drawtxt(nameBuf, 38.f, px + pw * 0.5f, py + 163.f, Color(60, 24, 110), AL_CENTER, 0.f);

    bool caretOn = ((int)(gTime * 2.f) % 2) == 0;          // blinking cursor
    if (caretOn) {
        float w = 0.f;
        if (!nameBuf.empty()) w = textWidth(nameBuf, 38.f);
        drawbox(px + pw * 0.5f + w * 0.5f + 4.f, py + 143.f, 3.f, 42.f, COL_PINK);
    }

    drawtxt(to_string(nameBuf.size()) + " / " + to_string(NAME_MAX_LEN), 17.f,
        px + pw - 52.f, py + 212.f, COL_SUB, AL_RIGHT, 0.f);
    drawtxt(nameMsg, 23.f, px + pw * 0.5f, py + 240.f, COL_RED);

    if (uibutton(px + 40.f, py + 262.f, 220.f, 58.f, "Continue", 0)) submitName();
    if (uibutton(px + pw - 260.f, py + 262.f, 220.f, 58.f, "Back", 1)) goTo(SCR_MENU);
}

//level select
void drawlevelselect() {
    drawbackground(Color(40, 70, 160), Color(160, 80, 200), true);
    if (keyEsc) goTo(SCR_MENU);

    drawtxt("SELECT LEVEL", 50.f, WIN_W * 0.5f, 66.f, COL_WHITE, AL_CENTER, 5.f);
    drawtxt("Difficulty: " + difficulityname[playerDifficulty], 24.f, WIN_W * 0.5f, 112.f,
        diffColor(playerDifficulty));
    drawCoinsPill(WIN_W - 190.f, 30.f, 160.f, 44.f);

    float nx[max_level], ny = 380.f;
    for (int i = 0; i < max_level; i++)
        nx[i] = 260.f + i * 290.f;

    // the path between the circles pink = open grey = locked
    for (int i = 0; i + 1 < max_level; i++) {
        Color pathColor(120, 100, 160);
        if (i + 1 < levelsUnlocked) pathColor = Color(255, 190, 220);
        drawbar(nx[i], ny, nx[i + 1], ny, 10.f, pathColor);
    }

    int hovered = -1;
    for (int i = 0; i < max_level; i++){
        bool unlocked = (i < levelsUnlocked);
        bool current = unlocked && playerStars[i] == 0;       //next level to play

        float dx = mouseX - nx[i];
        float dy = mouseY - ny;
        bool hover = (dx * dx + dy * dy <= 52.f * 52.f);
        if (hover) hovered = i;

        float r = 46.f;
        if (hover) r = 51.f;

        Color fill(255, 90, 160);
        if (!unlocked)     fill = Color(120, 108, 150);
        else if (current)  fill = Color(255, 190, 40);
        if (unlocked && hover) fill = lighten(fill, 0.2f);

        drawcircle(nx[i], ny + 6.f, r, Color(0, 0, 0, 100));
        drawcircle(nx[i], ny, r, fill, darken(fill, 0.45f), 5.f);

        if (unlocked) drawtxt(to_string(i + 1), 44.f, nx[i], ny + 2.f, COL_WHITE, AL_CENTER, 3.f);
        else          drawlock(nx[i], ny, 40.f, Color(235, 228, 250));

        for (int k = 0; k < 3; k++){                           //stors won in each level
            Color starColor(40, 20, 80, 160);
            if (playerStars[i] > k) starColor = COL_GOLD;
            drawstar(nx[i] - 36.f + 36.f * k, ny + r + 28.f, 16.f, starColor, Color(20, 8, 44), 2.f);
        }

        if (hover && unlocked && clicked) {
            clicked = false;
            startLevel(i + 1);
        }
    }

    //information about level under mous
    if (hovered >= 0){
        float px = WIN_W * 0.5f - 300.f, py = 560.f;
        drawPanel(px, py, 600.f, 104.f);

        if (hovered < levelsUnlocked){
            float moveFactor = 1.f;
            if (playerDifficulty == 1) moveFactor = 0.8f;
            int previewMoves = (int)(levelMoves[hovered] * moveFactor + 0.5f);

            drawtxt("Level " + to_string(hovered + 1), 30.f, px + 28.f, py + 32.f, COL_GOLD, AL_LEFT);
            drawtxt(goaltext(hovered + 1), 26.f, px + 28.f, py + 72.f, COL_WHITE, AL_LEFT);
            drawtxt(to_string(previewMoves) + " moves", 26.f, px + 572.f, py + 32.f, COL_SUB, AL_RIGHT);
            drawtxt("Click to play", 20.f, px + 572.f, py + 72.f, COL_SUB, AL_RIGHT, 0.f);
        }
        else
        {
            drawlock(px + 52.f, py + 52.f, 44.f, Color(235, 228, 250));
            drawtxt("Locked", 30.f, px + 100.f, py + 32.f, COL_GOLD, AL_LEFT);
            drawtxt("Complete Level " + to_string(hovered) + " to unlock this level", 24.f,
                px + 100.f, py + 72.f, COL_WHITE, AL_LEFT);
      }
    } 

    if (uibutton(30.f, WIN_H - 76.f, 150.f, 50.f, "Back", 1)) goTo(SCR_MENU);
}

void drawcard(float x, float y, float w, float h, string label){
    drawbox(x, y, w, h, Color(30, 12, 64, 225), Color(255, 150, 210, 100), 2.f);
    drawtxt(label, 19.f, x + 18.f, y + 24.f, COL_SUB, AL_LEFT, 0.f);
}

void drawhud() {
    const float px = 680.f;                  
    const float pw = 380.f;
    bool endless = (goalType == GOAL_ENDLESS);

    drawgradient(0.f, 0.f, (float)WIN_W, 130.f, Color(46, 18, 96), Color(30, 12, 64));
    drawbox(0.f, 128.f, (float)WIN_W, 4.f, COL_PINK);

    string title = "LEVEL " + to_string(levelNumber);
    if (endless) title = "ENDLESS MODE";
    drawtxt(title, 42.f, 30.f, 54.f, COL_WHITE, AL_LEFT, 4.f);
    drawtxt(difficulityname[gameDifficulty] + " difficulty", 21.f, 32.f, 96.f,
        diffColor(gameDifficulty), AL_LEFT, 0.f);
    drawCoinsPill(WIN_W - 330.f, 38.f, 150.f, 46.f);

    bool uiOn = (overlay == OV_NONE);      
    if (uibutton(WIN_W - 164.f, 38.f, 140.f, 46.f, "Pause", 1, uiOn, 22.f))
        overlay = OV_PAUSE;

    float y = 150.f;
    drawcard(px, y, pw, 124.f, "SCORE");
    drawtxt(to_string(gameScore), 52.f, px + 18.f, y + 66.f, COL_WHITE, AL_LEFT, 3.f);

    if (!endless){
        float bx = px + 20.f, bw = pw - 40.f, by = y + 98.f;
        float maxScore = (float)starScore[2];              

        drawbox(bx, by, bw, 14.f, Color(0, 0, 0, 130));
        float fill = clampf(gameScore / maxScore, 0.f, 1.f) * bw;
        if (fill > 0.f) drawbox(bx, by, fill, 14.f, COL_PINK);

        for (int k = 0; k < 3; k++)                         
        {
            float markX = bx + bw * (starScore[k] / maxScore);
            Color starColor(60, 40, 110);
            if (gameScore >= starScore[k]) starColor = COL_GOLD;
            drawstar(markX, by - 12.f, 11.f, starColor, Color(20, 8, 44), 2.f);
        }
    }

    
    y = 290.f;
    drawcard(px, y, 180.f, 112.f, "MOVES");
    Color movesColor = COL_WHITE;
    if (movesLeft <= 5) movesColor = COL_RED;
    int shownMoves = movesLeft;
    if (shownMoves < 0) shownMoves = 0;
    drawtxt(to_string(shownMoves), 58.f, px + 90.f, y + 70.f, movesColor, AL_CENTER, 3.f);

    if (!endless){
        drawcard(px + 200.f, y, 180.f, 112.f, "STARS");
        for (int k = 0; k < 3; k++){
            Color starColor(60, 40, 110);
            if (gameScore >= starScore[k]) starColor = COL_GOLD;
            drawstar(px + 200.f + 34.f + 56.f * k, y + 70.f, 24.f, starColor, Color(20, 8, 44), 3.f);
        }
    }

    
    y = 418.f;
    drawcard(px, y, pw, 106.f, "GOAL");

    bool done = false;                     
    float progress = 0.f;                    
    string detail = "Play until the moves run out";

    if (goalType == GOAL_SCORE){
        progress = clampf((float)gameScore / starScore[0], 0.f, 1.f);
        detail = to_string(gameScore) + " / " + to_string(starScore[0]);
        done = (gameScore >= starScore[0]);
    }
    else if (goalType == GOAL_COLLECT)
    {
        progress = clampf((float)collected / goalCount, 0.f, 1.f);
        detail = to_string(collected) + " / " + to_string(goalCount);
        done = (collected >= goalCount);
    }
    else if (goalType == GOAL_JELLY)
    {
        int jellyLeft = countJelly();
        detail = to_string(jellyLeft) + " jelly left";
        done = (jellyLeft == 0);
    }

    float textX = px + 18.f;
    if (goalType == GOAL_COLLECT)            // small picture of the candy to collect
    {
        drawCandy(goalColor, px + 38.f, y + 62.f);
        textX = px + 72.f;
    }
    if (goalType == GOAL_JELLY)              // small picture of the jelly
{
        drawjelly(px + 4.f, y + 28.f);
        textX = px + 72.f;
     }
    Color goalColorText = COL_WHITE;
    if (done) goalColorText = COL_GREEN;
    drawtxt(goaltext(levelNumber), 24.f, textX, y + 56.f, goalColorText, AL_LEFT);
    drawtxt(detail, 20.f, textX, y + 86.f, COL_SUB, AL_LEFT, 0.f);

    if (goalType == GOAL_SCORE || goalType == GOAL_COLLECT) {
        drawbox(px + pw - 120.f, y + 78.f, 100.f, 10.f, Color(0, 0, 0, 130));
        if (progress > 0.f) drawbox(px + pw - 120.f, y + 78.f, 100.f * progress, 10.f, COL_GREEN);
    }
    if (done) drawtick(px + pw - 34.f, y + 40.f, 26.f, COL_GREEN);
    drawtxt("Click a candy, then a neighbour to swap them.    P = pause", 17.f,
        px + pw * 0.5f, 600.f, Color(190, 175, 225), AL_CENTER, 0.f);
}

void drawOverlay() {
    if (overlay == OV_NONE) return;

    drawbox(0.f, 0.f, (float)WIN_W, (float)WIN_H, Color(10, 4, 24, 160));    // dark layer
    float cx = WIN_W * 0.5f;

    if (overlay == OV_PAUSE) {
        float px = cx - 220.f, py = 170.f;
        drawPanel(px, py, 440.f, 400.f);
        drawtxt("PAUSED", 52.f, cx, py + 64.f, COL_GOLD, AL_CENTER, 3.f);

        if (uibutton(px + 50.f, py + 120.f, 340.f, 60.f, "Resume", 2))    overlay = OV_NONE;
        if (uibutton(px + 50.f, py + 200.f, 340.f, 60.f, "Restart", 1))   startLevel(currentLevel);
        if (uibutton(px + 50.f, py + 280.f, 340.f, 60.f, "Main Menu", 3)) goTo(SCR_MENU);
      }
    else if (overlay == OV_WIN){
        float px = cx - 290.f, py = 130.f;
        drawPanel(px, py, 580.f, 440.f);
        drawtxt("LEVEL COMPLETE!", 50.f, cx, py + 60.f, COL_GOLD, AL_CENTER, 3.f);

        for (int k = 0; k < 3; k++) {                         
            Color starColor(40, 20, 80, 190);
            if (k < endStars) starColor = COL_GOLD;
            float sy = py + 165.f;
            if (k == 1) sy -= 14.f;
            drawstar(cx + (k - 1) * 120.f, sy, 52.f, starColor, Color(20, 8, 44), 3.f);
        }

        drawtxt("Score: " + to_string(gameScore), 36.f, cx, py + 262.f, COL_WHITE, AL_CENTER, 3.f);
        drawcoin(cx - 78.f, py + 314.f, 17.f);
        drawtxt("+" + to_string(endCoins) + " coins", 32.f, cx - 48.f, py + 314.f, COL_GOLD, AL_LEFT, 3.f);

        float by = py + 360.f;
        if (levelNumber < max_level){
            if (uibutton(px + 24.f, by, 170.f, 60.f, "Next Level", 0, true, 24.f)) {
                startLevel(levelNumber + 1);
            }
        }
        else {
            if (uibutton(px + 24.f, by, 170.f, 60.f, "Levels", 0, true, 24.f))
                goTo(SCR_LEVELS);
        }
        if (uibutton(px + 205.f, by, 170.f, 60.f, "Replay", 1, true, 24.f)) startLevel(levelNumber);
        if (uibutton(px + 386.f, by, 170.f, 60.f, "Menu", 1, true, 24.f))   goTo(SCR_MENU);
    }
    else if (overlay == OV_LOSE){
        float px = cx - 270.f, py = 160.f;
        float panelH = 370.f;
        drawPanel(px, py, 540.f, panelH);
        drawtxt("OUT OF MOVES", 50.f, cx, py + 62.f, COL_RED, AL_CENTER, 3.f);

        string line = "Jelly left: " + to_string(countJelly());
        if (goalType == GOAL_SCORE)
            line = "Score: " + to_string(gameScore) + " / " + to_string(starScore[0]);
        else if (goalType == GOAL_COLLECT)
            line = "Collected: " + to_string(collected) + " / " + to_string(goalCount);
        drawtxt(line, 34.f, cx, py + 128.f, COL_WHITE, AL_CENTER, 3.f);
        drawtxt("So close! Try again.", 23.f, cx, py + 170.f, COL_SUB, AL_CENTER, 0.f);

        float by = py + 214.f;
        if (uibutton(px + 50.f, by, 210.f, 62.f, "Try Again", 0)) startLevel(levelNumber);
        if (uibutton(px + 280.f, by, 210.f, 62.f, "Menu", 1))     goTo(SCR_MENU);
   }
 }

void drawGameScreen(){
    drawbackground(Color(56, 24, 120), Color(26, 10, 66), false);
    drawBoard();
    drawhud();
    drawOverlay();
}

void updateGame(float dt){
    if (overlay == OV_NONE) {
        if (keyP || keyEsc) {
            overlay = OV_PAUSE;
        }
        else{
            updateBoard(dt);
            if (overlay == OV_NONE && boardPhase == PH_IDLE && clicked && cellfrom_mouse(mouseX, mouseY))
                clickCell(mouseRow, mouseCol);
        }
    }
    else if (overlay == OV_PAUSE)
    {
        if (keyP || keyEsc) overlay = OV_NONE;
    }
    else if (keyEsc)                                   
    {
        goTo(SCR_MENU);
   }
  }
bool loadFonts() {
    const string paths[4] = { "arialbd.ttf", "C:/Windows/Fonts/arialbd.ttf",
                              "C:/Windows/Fonts/segoeuib.ttf", "C:/Windows/Fonts/tahomabd.ttf" };
    for (int i = 0; i < 4; i++)
        if (gFont.openFromFile(paths[i])) return true;
    return false;
 }

void readEvents(){
    clicked = false;
    keyEsc = keyEnter = keyBackspace = false;
    keyP = keyNum1 = keyNum2 = false;
    typedText.clear();

    while (auto event = gWindow.pollEvent()){
        if (event->is<Event::Closed>())  {
            gWindow.close();
        }
        else if (const auto* key = event->getIf<Event::KeyPressed>()) {
            if (key->code == Keyboard::Key::Escape)    keyEsc = true;
            if (key->code == Keyboard::Key::Enter)     keyEnter = true;
            if (key->code == Keyboard::Key::Backspace) keyBackspace = true;
            if (key->code == Keyboard::Key::P)         keyP = true;
            if (key->code == Keyboard::Key::Num1)      keyNum1 = true;
            if (key->code == Keyboard::Key::Num2)      keyNum2 = true;
        }
        else if (const auto* text = event->getIf<Event::TextEntered>()){
            if (text->unicode >= 32 && text->unicode < 127)
                typedText += (char)text->unicode;
        }
        else if (const auto* press = event->getIf<Event::MouseButtonPressed>()){
            if (press->button == Mouse::Button::Left) clicked = true;
        }
    }

    Vector2i mousePos = Mouse::getPosition(gWindow);
    mouseX = (float)mousePos.x;
    mouseY = (float)mousePos.y;
    mouseDown = Mouse::isButtonPressed(Mouse::Button::Left);
}

void updateAll(float dt){
    gTime += dt;
    if (toastT > 0.f) toastT -= dt;
    if (screen == SCR_GAME) updateGame(dt);
}

void drawAll(){
    if (screen == SCR_MENU)        drawMenu();
    else if (screen == SCR_NAME)   drawNameEntry();
    else if (screen == SCR_LEVELS) drawlevelselect();
    else if (screen == SCR_GAME)   drawGameScreen();
    drawToast();
}

int main(){
    srand((unsigned)time(0));          

    if (!loadFonts())
    {
        cerr << "ERROR: no font found. Copy arialbd.ttf into the project folder.\n";
        return 1;
    }
    gWindow.setFramerateLimit(60);

    loadprofile();                       
    locallevels(1, playerDifficulty);
    resetboardview();

    Clock clock;
    while (gWindow.isOpen()) {
        float dt = clampf(clock.restart().asSeconds(), 0.f, 0.05f);
        readEvents();
        updateAll(dt);

        gWindow.clear(Color(20, 8, 44));
        drawAll();
        gWindow.display();
    }

    saveprofiles();
    return 0;
}
