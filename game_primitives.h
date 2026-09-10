#ifndef _GAME_PRIMITIVES__H
#define _GAME_PRIMITIVES__H


#define UP_MASK    0b1000
#define DOWN_MASK  0b0100
#define LEFT_MASK  0b0010
#define RIGHT_MASK 0b0001

typedef struct XY {
    int x;
    int y;
} XY;

#endif  // _GAME_PRIMITIVES__H