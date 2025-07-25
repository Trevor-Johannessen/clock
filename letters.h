#ifndef LETTERS_H
#define LETTERS_H

#include <MD_MAX72xx.h>

#define BLUEPRINT_FOOTER {-1,-1}

typedef const int Coord[2];
typedef struct {
    Coord *blueprint;
} Blueprint;

extern MD_MAX72XX matrix;
extern const int screen_width;

const Coord blueprint_space[] = {BLUEPRINT_FOOTER};
const Coord blueprint_zero[] = {{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{1,7},{2,7},{3,7},{4,7},{5,1},{5,2},{5,3},{5,4},{5,5},{5,6},{1,0},{2,0},{3,0},{4,0},{1,1},{2,2},{2,3},{3,4},{3,5},{4,6},BLUEPRINT_FOOTER};
const Coord blueprint_one[] = {{0,0},{1,0},{2,0},{3,0},{2,1},{2,2},{2,3},{2,4},{2,5},{2,6},{2,7},{0,5},{1,6},{1,5},BLUEPRINT_FOOTER};
const Coord blueprint_two[] = {{0,0},{1,0},{2,0},{3,0},{4,0},{1,1},{2,2},{3,3},{4,4},{4,5},{4,6},{3,7},{2,7},{1,7},{0,6},{0,5},BLUEPRINT_FOOTER};
const Coord blueprint_three[] = {{0,0},{1,0},{2,0},{3,1},{3,2},{3,3},{2,4},{1,4},{3,5},{3,6},{0,7},{1,7},{2,7},BLUEPRINT_FOOTER};
const Coord blueprint_four[] = {{3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7},{0,4},{0,5},{0,6},{0,7},{1,4},{2,4},BLUEPRINT_FOOTER};
const Coord blueprint_five[] = {{0,0},{1,0},{2,0},{3,0},{4,1},{4,2},{4,3},{3,4},{2,4},{1,4},{0,4},{0,5},{0,6},{0,7},{1,7},{2,7},{3,7},{4,7},BLUEPRINT_FOOTER};
const Coord blueprint_six[] = {{1,0},{2,0},{3,0},{4,1},{4,2},{4,3},{3,4},{2,4},{1,4},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{1,7},{2,7},{3,7},{4,6},BLUEPRINT_FOOTER};
const Coord blueprint_seven[] = {{1,0},{1,1},{2,2},{2,3},{3,4},{3,5},{4,6},{4,7},{0,6},{0,7},{1,7},{2,7},{3,7},BLUEPRINT_FOOTER};
const Coord blueprint_eight[] = {{1,0},{2,0},{3,0},{0,1},{0,2},{0,3},{1,4},{2,4},{3,4},{4,1},{4,2},{4,3},{1,7},{2,7},{3,7},{4,5},{4,6},{0,5},{0,6},BLUEPRINT_FOOTER};
const Coord blueprint_nine[] = {{0,1},{1,0},{2,0},{3,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{1,4},{2,4},{3,4},{0,5},{0,6},{1,7},{2,7},{3,7},BLUEPRINT_FOOTER};
const Coord blueprint_A[] = {{0,0},{1,0},{2,0},{4,0},{5,0},{6,0},{1,1},{1,2},{1,3},{5,1},{5,2},{5,3},{2,2},{3,2},{4,2},{2,4},{2,5},{4,4},{4,5},{3,6},BLUEPRINT_FOOTER};
const Coord blueprint_M[] = {{0,0},{1,0},{2,0},{4,0},{5,0},{6,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{5,1},{5,2},{5,3},{5,4},{5,5},{5,6},{2,5},{4,5},{3,4},{3,3},BLUEPRINT_FOOTER};
const Coord blueprint_P[] = {{0,0},{1,0},{2,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{0,6},{2,6},{3,6},{2,3},{3,3},{4,4},{4,5},BLUEPRINT_FOOTER};
const Coord blueprint_colon[] = {{0,1},{0,2},{0,5},{0,6},BLUEPRINT_FOOTER};
const Blueprint letter_blueprint[] = {
    0x0, // 0
    0x0, // 1
    0x0, // 2
    0x0, // 3
    0x0, // 4
    0x0, // 5
    0x0, // 6
    0x0, // 7
    0x0, // 8
    0x0, // 9
    0x0, // 10
    0x0, // 11
    0x0, // 12
    0x0, // 13
    0x0, // 14
    0x0, // 15
    0x0, // 16
    0x0, // 17
    0x0, // 18
    0x0, // 19
    0x0, // 20
    0x0, // 21
    0x0, // 22
    0x0, // 23
    0x0, // 24
    0x0, // 25
    0x0, // 26
    0x0, // 27
    0x0, // 28
    0x0, // 29
    0x0, // 30
    0x0, // 31
    {blueprint_space}, // ' '
    0x0, // !
    0x0, // "
    0x0, // #
    0x0, // $
    0x0, // %
    0x0, // &
    0x0, // '
    0x0, // (
    0x0, // )
    0x0, // *
    0x0, // +
    0x0, // ,
    0x0, // -
    0x0, // .
    0x0, // /
    {blueprint_zero},
    {blueprint_one},
    {blueprint_two},
    {blueprint_three},
    {blueprint_four},
    {blueprint_five},
    {blueprint_six},
    {blueprint_seven},
    {blueprint_eight},
    {blueprint_nine},
    {blueprint_colon},
    0x0, // ;
    0x0, // <
    0x0, // =
    0x0, // >
    0x0, // ?
    0x0, // @
    {blueprint_A},
    0x0, // B
    0x0, // C
    0x0, // D
    0x0, // E
    0x0, // F
    0x0, // G
    0x0, // H
    0x0, // I
    0x0, // J
    0x0, // K
    0x0, // L
    {blueprint_M},
    0x0, // N
    0x0, // O
    {blueprint_P},
    0x0, // Q
    0x0, // R
    0x0, // S
    0x0, // T
    0x0, // U
    0x0, // V
    0x0, // W
    0x0, // X
    0x0, // Y
    0x0  // Z
};

void sentence_write(char *str, int x, int y);
void character_write(char c, int x, int y);
int character_width(char c);
#endif