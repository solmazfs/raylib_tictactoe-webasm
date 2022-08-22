/*******************************************************************************************
*
*   raylib demo [sm_simple.c] - tic-tac-toe
*
*   Demo originally created with raylib 4.2, last time updated with raylib 4.2
*
*   Demo licensed under MIT.
*
*   Copyright (c) 2022 Fatih S. Solmaz (@solmazfs)
*
********************************************************************************************/

#include <stdio.h>
// rand
#include <time.h>
#include <stdlib.h>
#include "raylib.h"
#include "reasings.h"

#include "sm_simple.h"

#define for_ij for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++)

// todo: timer

struct Screen {int w; int h;} screen;
struct Board {
    int array[3][3];
    Color color;
    Color alpha;
    Color font_color;
    int size;
    float th;
    float box;
    float box_len;
    float font_large;
    float font_xl;
    char *win_text;
    char *tie_text;
    int anim;
    int anim_speed;
    float ease;
    float ease_yy;
    float ease_alpha;
    int ease_time;
    float ease_counter;
    float fade;
    int turn;
    int winner;
} board;

struct Human {
    Color color;
    Color alpha;
    int score;
    float th;
    bool play;
    float anim_x;
    float anim_y;
    float anim_xx;
    int anim_speed;
    Vector2 anim_final_x;
    Vector2 anim_final_y;
    Rectangle current;
} human;
struct Computer { Color color;
    Color alpha;
    int best_i, best_j;
    int score;
    float th;
    bool play;
    float anim;
    int anim_speed;
    Rectangle current;
} computer;

enum Player {NONE,HUMAN,COMPUTER=-1};
enum State {start,play,computer_win,human_win,tie,gameover};
int currentPlayer;
int gameState;
Rectangle boxIndex[3][3];

int minmax_ai(int val, int depth);
bool check_winner();

void _Init() {
    srand(time(NULL)); // seed
    // color palette
    board.color = (Color){32,54,51,255};
    board.alpha = (Color){64,53,33,10};
    board.font_color = (Color){32, 54, 51,255};
    human.color = (Color){146,80,63,255};
    computer.color = (Color){70,126,115,255};
    // screen
    screen.w = GetScreenWidth();
    screen.h = GetScreenHeight();
    // board
    board.size = ( (screen.w - screen.w/4) < (screen.h - screen.h/4) ? (screen.w - screen.w/4) : (screen.h - screen.h/4) );
    board.box = (float)board.size/3;
    board.box_len = (board.box - (board.box/8)*2);
    board.th = (board.box_len/2)/4;
    board.win_text = "WON!";
    board.tie_text = "DRAW!";
    board.font_large = (float)board.size/18;
    board.font_xl = (float)board.size/6;
    board.anim = 0;
    board.anim_speed = board.size/48;
    // game state
    gameState = change_state(start);
    // human/computer
    human.score = 0;
    computer.score = 0;
}

void _Process() {
    switch (gameState) {
        case 0:
            // STATE --> Game Start
            board.anim += board.anim_speed;
            if (board.anim >= (float)board.size/2) {
                board.anim = board.size/2;
                gameState = change_state(play);
            }
            break;
        case 1:
            // STATE --> Play
            // check if mouse entered box --> human plays
            for_ij {
                if (CheckCollisionPointRec(GetMousePosition(),boxIndex[i][j])) {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && board.array[i][j] == NONE && currentPlayer == HUMAN && (!human.play && !computer.play)) {
                        board.array[i][j] = HUMAN;
                        human.current = boxIndex[i][j];
                        human.anim_x = human.current.x + board.box/12;
                        human.anim_y = human.current.y + board.box/12;
                        human.anim_xx = human.current.x + human.current.width - board.box/12;
                        human.play = true;
                        board.turn++;
                        currentPlayer = COMPUTER;
                    }
                    // DEBUG --> computer
                    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && board.array[i][j] == NONE && currentPlayer == COMPUTER && (!human.play && !computer.play)) {
                        board.array[i][j] = COMPUTER;
                        computer.current = boxIndex[i][j];
                        computer.anim = 180;
                        computer.play = true;
                        currentPlayer = HUMAN;
                        board.turn++;
                    }
                }
            }
            // DEBUG --> minmax ai
            if (currentPlayer == COMPUTER && !human.play) {
                minmax_ai(-1,0);
                board.array[computer.best_i][computer.best_j] = COMPUTER;
                computer.current = boxIndex[computer.best_i][computer.best_j];
                computer.anim = 180;
                computer.play = true;
                board.turn++;
                currentPlayer = HUMAN;
            }

            // play anim --> human ? maybe another options
            if (human.play) {
                human.anim_x += human.anim_speed;
                human.anim_y += human.anim_speed;
                human.anim_xx -= human.anim_speed;
                if (human.anim_x >= (human.current.x + human.current.width - human.th)) {
                    human.anim_x = human.current.x + human.current.width - human.th;
                    human.anim_y = human.current.y + human.current.height - human.th;
                    human.anim_xx = human.current.x + human.th;
                    human.play = false;
                }
            }
            // play anim --> computer
            if (computer.play) {
                computer.anim -= computer.anim_speed;
                if (computer.anim <= -180) {
                    computer.anim = -180;
                    computer.play = false;
                }
            }
            // ? we have winner
            if (check_winner()) {
                if (board.winner == COMPUTER) {
                    computer.score++;
                    gameState = change_state(computer_win);
                } else if (board.winner == HUMAN) {
                    human.score++;
                    gameState = change_state(human_win);
                } else {
                    gameState = change_state(tie);
                }
            }

            break;
        case 2:
            // STATE (Process) --> Computer Win
            board.ease_counter++;
            board.ease = EaseBackIn(board.ease_counter,0,(float)board.size/2,board.ease_time);
            board.ease_yy = EaseCircOut(board.ease_counter,0,(float)board.size/2,board.ease_time);
            board.ease_alpha -= 0.01f;
            if (board.ease_counter >= board.ease_time) {
                board.ease_counter = board.ease_time;
                board.ease_alpha = 0;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    gameState = change_state(start);
                }
            }
            break;
        case 3:
            // STATE (Process) --> Human Win
            board.ease_counter++;
            board.ease = EaseBackIn(board.ease_counter,0,(float)board.size/2,board.ease_time);
            board.ease_yy = EaseCircOut(board.ease_counter,0,(float)board.size/2,board.ease_time);
            board.ease_alpha -= 0.01f;
            if (board.ease_counter >= board.ease_time) {
                board.ease_counter = board.ease_time;
                board.ease_alpha = 0;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    gameState = change_state(start);
                }
            }

            break;
        case 4:
            // STATE (Process) --> TIE!
            board.ease_counter++;
            board.ease = EaseBackIn(board.ease_counter,0,(float)board.size/2,board.ease_time);
            board.ease_yy = EaseCircOut(board.ease_counter,0,(float)board.size/2,board.ease_time);
            board.ease_alpha -= 0.01f;
            if (board.ease_counter >= board.ease_time) {
                board.ease_counter = board.ease_time;
                board.ease_alpha = 0;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    gameState = change_state(start);
                }
            }

            break;
    }
}

void _Draw() {
    switch (gameState) {
        case 0:
            // STATE (Draw) --> Game Start
            // board animation --> starts from center: left/top & right/bottom
            for (int i=1; i<3; i++) {
                Vector2 xx = (Vector2){((float)screen.w/2 + board.anim), ((float)screen.h/2 - (float)board.size/2)+(float)board.size/3*i};
                Vector2 xy = (Vector2){((float)screen.w/2 - board.anim), ((float)screen.h/2 - (float)board.size/2)+(float)board.size/3*i};
                Vector2 yx = (Vector2){((float)screen.w/2 - (float)board.size/2)+(float)board.size/3*i,((float)screen.h/2 - board.anim)};
                Vector2 yy = (Vector2){((float)screen.w/2 - (float)board.size/2)+(float)board.size/3*i,((float)screen.h/2 + board.anim)};
                // horizontal
                DrawLineEx(xx,xy,board.th,board.color);
                // vertical
                DrawLineEx(yx,yy,board.th,board.color);
            }

            break;
        case 1:
            // STATE (Draw) --> Play
            for (int i=1; i<3; i++) {
                // board: lines
                Vector2 xx = (Vector2){((float)screen.w/2 + (float)board.size/2),
                    ((float)screen.h/2 - (float)board.size/2)+(float)board.size/3*i};
                Vector2 xy = (Vector2){((float)screen.w/2 - (float)board.size/2),
                    ((float)screen.h/2 - (float)board.size/2)+(float)board.size/3*i};
                Vector2 yx = (Vector2){((float)screen.w/2 - (float)board.size/2)+(float)board.size/3*i,
                    ((float)screen.h/2 - (float)board.size/2)};
                Vector2 yy = (Vector2){((float)screen.w/2 - (float)board.size/2)+(float)board.size/3*i,
                    ((float)screen.h/2 + (float)board.size/2)};
                // horizontal
                DrawLineEx(xx,xy,board.th,board.color);
                // vertical
                DrawLineEx(yx,yy,board.th,board.color);
            }
            // board: collision box
            for (int i=0; i<3; i++) {
                for (int j=0; j<3; j++) {
                    DrawRectangleRec(boxIndex[i][j],board.alpha);
                }
            }
            // draw score
            DrawText(TextFormat("YOU: %d", human.score),
                    ((float)screen.w/2-(float)board.size/2)+board.font_large,board.font_large,board.font_large,board.color);
            DrawText(TextFormat("AI: %d", computer.score),
                    ((float)screen.w/2)+board.font_large,board.font_large,board.font_large,board.color);

            DrawLineEx((Vector2){((float)screen.w/2),((float)screen.h/2-(float)board.size/2)- (board.th*3.5)},
                    (Vector2){(float)screen.w/2,((float)screen.h/2-(float)board.size/2)-(board.th*2)},board.th/6,board.color);

            Rectangle rec = {((float)screen.w/2-(float)board.size/2),(float)board.size/20,(board.size),(float)board.size/15};
            DrawRectangleRoundedLines(rec,4,8,board.th/6,board.color);


            // draw player's marks
            for_ij {
                if (board.array[i][j] == HUMAN) {
                    // draw human player
                    if (boxIndex[i][j].x == human.current.x && boxIndex[i][j].y == human.current.y) {
                        float aa = human.current.x + human.th;
                        float ab = human.current.y + human.th;
                        float ba = human.current.x + board.box_len - human.th;

                        DrawLineEx((Vector2){aa, ab},
                                (Vector2){human.anim_x, human.anim_y}, human.th, human.color);
                        DrawLineEx((Vector2){ba, ab},
                                (Vector2){human.anim_xx, human.anim_y}, human.th, human.color);

                    } else {
                        float xx = boxIndex[i][j].x + human.th;
                        float xy = boxIndex[i][j].y + human.th;
                        float yx = boxIndex[i][j].x + board.box_len - human.th;
                        float yy = boxIndex[i][j].y + board.box_len - human.th;

                        DrawLineEx((Vector2){xx,xy},(Vector2){yx,yy},human.th,human.color);
                        DrawLineEx((Vector2){yx,xy},(Vector2){xx,yy},human.th,human.color);
                    }
                } else if (board.array[i][j] == COMPUTER) {
                    // draw computer player
                    float x = (boxIndex[i][j].x + board.box_len/2);
                    float y = (boxIndex[i][j].y + board.box_len/2);
                    int tx = board.box_len/2 - computer.th/2*3;
                    int ty = board.box_len/2 - computer.th/2;
                    if (boxIndex[i][j].x == computer.current.x && boxIndex[i][j].y == computer.current.y) {
                        DrawRing((Vector2){x,y},tx,ty,180,computer.anim,0,computer.color);
                    } else {
                        DrawRing((Vector2){x,y},tx,ty,180,-180,0,computer.color);
                    }
                }
            }

            break;
        case 2:
            // STATE (Draw) --> Computer Win
            for (int i=1; i<3; i++) {
                // board lines anim
                Vector2 xx = (Vector2){((float)screen.w/2 + (float)board.size/2) - board.ease,
                    (((float)screen.h/2 - (float)board.size/2)+(float)board.size/3*i) - board.ease*2};
                Vector2 xy = (Vector2){((float)screen.w/2 - (float)board.size/2) + board.ease,
                    (((float)screen.h/2 - (float)board.size/2)+(float)board.size/3*i) - board.ease*2};
                Vector2 yx = (Vector2){(((float)screen.w/2 - (float)board.size/2) + (float)board.size/3*i),
                    (((float)screen.h/2 - (float)board.size/2)-board.ease*1.5f)};
                Vector2 yy = (Vector2){(((float)screen.w/2 - (float)board.size/2) + (float)board.size/3*i),
                    (((float)screen.h/2 + (float)board.size/2)-board.ease*3)};

                // horizontal
                DrawLineEx(xx,xy,board.th,Fade(board.color,board.ease_alpha));
                // vertical
                DrawLineEx(yx,yy,board.th,Fade(board.color,board.ease_alpha));
            }
            float ring_x = (float)screen.w/2;
            float ring_y = ((float)screen.h/2 - (float)board.size/2)+board.size - board.ease_yy;
            float ring_text = ((float)screen.h/2 - (float)board.size/2)+board.size+board.font_xl - board.ease_yy*0.8;
            int tx = board.ease_yy/3;
            int ty = board.ease_yy/3 + computer.th*1.5f;

            DrawRing((Vector2){ring_x,ring_y}, tx,ty,180,-180,0,computer.color);
            DrawText(board.win_text, (screen.w - MeasureText(board.win_text,board.font_xl)) / 2,
                    ring_text, board.font_xl, board.font_color);
            break;
        case 3:
            // STATE (Draw) --> Human Win
            for (int i=1; i<3; i++) {
                // board lines anim
                Vector2 xx = (Vector2){((float)screen.w/2 + (float)board.size/2) - board.ease,
                    (((float)screen.h/2 - (float)board.size/2)+(float)board.size/3*i) - board.ease*2};
                Vector2 xy = (Vector2){((float)screen.w/2 - (float)board.size/2) + board.ease,
                    (((float)screen.h/2 - (float)board.size/2)+(float)board.size/3*i) - board.ease*2};
                Vector2 yx = (Vector2){(((float)screen.w/2 - (float)board.size/2) + (float)board.size/3*i),
                    (((float)screen.h/2 - (float)board.size/2)-board.ease*1.5f)};
                Vector2 yy = (Vector2){(((float)screen.w/2 - (float)board.size/2) + (float)board.size/3*i),
                    (((float)screen.h/2 + (float)board.size/2)-board.ease*3)};

                // horizontal
                DrawLineEx(xx,xy,board.th,Fade(board.color,board.ease_alpha));
                // vertical
                DrawLineEx(yx,yy,board.th,Fade(board.color,board.ease_alpha));
            }
            float cx = ((float)screen.w/2 - (float)board.ease_yy/4) - human.th*2;
            float cy = ((float)screen.h/2 - (float)board.ease_yy/4)+ (float)board.size/2 - board.ease_yy - human.th*2;
            float dx = ((float)screen.w/2 + (float)board.ease_yy/4) + human.th*2;
            float dy = ((float)screen.h/2 + (float)board.ease_yy/4)+ (float)board.size/2 - board.ease_yy + human.th*2;

            float line_text = ((float)screen.h/2 - (float)board.size/2)+board.size+board.font_xl - board.ease_yy*0.8;

            DrawLineEx((Vector2){cx,cy},(Vector2){dx,dy},human.th*1.5f,human.color);
            DrawLineEx((Vector2){dx,cy},(Vector2){cx,dy},human.th*1.5f,human.color);

            DrawText(board.win_text, (screen.w - MeasureText(board.win_text,board.font_xl)) / 2, line_text, board.font_xl, board.font_color);
            break;
        case 4:
            // STATE (Draw) --> TIE!
            for (int i=1; i<3; i++) {
                // board lines anim
                Vector2 xx = (Vector2){((float)screen.w/2 + (float)board.size/2) - board.ease,
                    (((float)screen.h/2 - (float)board.size/2)+(float)board.size/3*i) - board.ease*2};
                Vector2 xy = (Vector2){((float)screen.w/2 - (float)board.size/2) + board.ease,
                    (((float)screen.h/2 - (float)board.size/2)+(float)board.size/3*i) - board.ease*2};
                Vector2 yx = (Vector2){(((float)screen.w/2 - (float)board.size/2) + (float)board.size/3*i),
                    (((float)screen.h/2 - (float)board.size/2)-board.ease*1.5f)};
                Vector2 yy = (Vector2){(((float)screen.w/2 - (float)board.size/2) + (float)board.size/3*i),
                    (((float)screen.h/2 + (float)board.size/2)-board.ease*3)};

                // horizontal
                DrawLineEx(xx,xy,board.th,Fade(board.color,board.ease_alpha));
                // vertical
                DrawLineEx(yx,yy,board.th,Fade(board.color,board.ease_alpha));
            }
            // computer
            float rx = (float)screen.w/2 - (float)board.size/4;
            float ry = ((float)screen.h/2 - (float)board.size/2)+board.size - board.ease_yy;
            int vxx = board.ease_yy/3;
            int vyy = board.ease_yy/3 + computer.th*1.5f;
            // human
            float kx = (((float)screen.w/2 - (float)board.ease_yy/4) - human.th*2) + (float)board.size/4;
            float ky = ((float)screen.h/2 - (float)board.ease_yy/4)+ (float)board.size/2 - board.ease_yy - human.th*2;
            float lx = (((float)screen.w/2 + (float)board.ease_yy/4) + human.th*2) + (float)board.size/4;
            float ly = ((float)screen.h/2 + (float)board.ease_yy/4)+ (float)board.size/2 - board.ease_yy + human.th*2;
            float textt = ((float)screen.h/2 - (float)board.size/2)+board.size+board.font_xl - board.ease_yy*0.8;

            DrawRing((Vector2){rx,ry}, vxx,vyy,180,-180,0,computer.color);

            DrawLineEx((Vector2){kx,ky},(Vector2){lx,ly},human.th*1.5f,human.color);
            DrawLineEx((Vector2){lx,ky},(Vector2){kx,ly},human.th*1.5f,human.color);

            DrawText(board.tie_text, (screen.w - MeasureText(board.tie_text,board.font_xl)) / 2,
                    textt, board.font_xl, board.font_color);
            break;
    }
}

int change_state(int _new_state) {
    int _state = _new_state;
    switch(_state) {
        case 0:
            // STATE --> Game Start
            board.anim = 0;
            board.ease = 0;
            board.ease_yy = 0;
            board.ease_alpha = 1.0f;
            board.ease_counter = 0;
            board.ease_time = 68;
            board.fade = 1.0f;
            board.turn = 1;
            board.winner = NONE;
            // players --> human
            currentPlayer = HUMAN;
            human.th = (board.box_len/2)/4;
            human.anim_x = 0;
            human.anim_y = 0;
            human.anim_xx = 0;
            human.anim_speed = board.size/32;
            human.play = false;
            human.score = (human.score > 999) ? 0 : human.score;
            // players --> computer
            computer.th = (board.box_len/2)/4;
            computer.anim = 0;
            computer.anim_speed = 32;
            board.ease_yy = 0;
            computer.play = false;
            computer.score = (computer.score > 999) ? 0 : computer.score;
            // init box
            for (int i=0; i<3; i++) {
                for (int j=0; j<3; j++) {
                    float x = ((float)screen.w/2 - (float)board.size/2) + board.box*i;
                    float y = ((float)screen.h/2 - (float)board.size/2) + board.box*j;
                    float xr = board.box/8;
                    boxIndex[i][j] = (Rectangle){x+xr,y+xr,board.box-xr*2,board.box-xr*2};
                }
            }
            // reset board.array
            for (int i=0; i<3; i++) {
                for (int j=0; j<3; j++) {
                    board.array[i][j] = NONE;
                }
            }
            break;
        case 1:
            // STATE --> Play
            break;
        case 2:
            // STATE --> Computer Win
            board.ease_counter = 0;
            break;
        case 3:
            // STATE --> Human Win
            board.ease_counter = 0;
            break;
        case 4:
            // STATE --> TIE!
            board.ease_counter = 0;
            break;

    }
    return _state;
}

int minmax_check_winner() {
    for (int i=0; i<3; i++) {
        if (board.array[i][0] && board.array[i][1] == board.array[i][0] && board.array[i][2] == board.array[i][0]) {
            return board.array[i][0];
        }
        if (board.array[0][i] && board.array[1][i] == board.array[0][i] && board.array[2][i] == board.array[0][i]) {
            return board.array[0][i];
        }
    }
    if (!board.array[1][1]) return 0;
    if (board.array[1][1] == board.array[0][0] && board.array[2][2] == board.array[0][0]) {
        return board.array[0][0];
    }
    if (board.array[1][1] == board.array[2][0] && board.array[0][2] == board.array[1][1]) {
        return board.array[1][1];
    }
    if (board.turn > 9) return 1;
    return 0;
}
// rosettacode.org -> tic-tac-toe c code example
int minmax_ai(int val, int depth)
{
    int score;
    int best = -1, changed = 0;

    if ((score = minmax_check_winner())) return (score == val) ? 1 : -1;

    for_ij {
        // not zero continue
        if (board.array[i][j]) continue;

        changed = board.array[i][j] = val;
        score = -minmax_ai(-val, depth+1);
        board.array[i][j] = NONE;

        if (score <= best) continue;
        if (!depth) {
            computer.best_i = i;
            computer.best_j = j;
        }
        best = score;
    }
    return changed ? best : 0;
}

bool check_winner() {
    for (int i=0; i<3; i++) {
        if (board.array[i][0] && board.array[i][1] == board.array[i][0] && board.array[i][2] == board.array[i][0]) {
            board.winner = board.array[i][0];
            return true;
        }
        if (board.array[0][i] && board.array[1][i] == board.array[0][i] && board.array[2][i] == board.array[0][i]) {
            board.winner = board.array[0][i];
            return true;
        }
    }
    if (!board.array[1][1]) return false;
    if (board.array[1][1] == board.array[0][0] && board.array[2][2] == board.array[0][0]) {
        board.winner = board.array[0][0];
        return true;
    }
    if (board.array[1][1] == board.array[2][0] && board.array[0][2] == board.array[1][1]) {
        board.winner = board.array[1][1];
        return true;
    }
    if (board.turn > 9) return true;
    return false;
}
