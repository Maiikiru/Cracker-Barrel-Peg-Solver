#pragma once
#include <cstdint>
#include <stdlib.h>
#include <stdio.h>


const unsigned char NUM_PINS = 15;

typedef struct jumpInfo{
    uint16_t expected;
    uint16_t mask;
} JumpInfo;

typedef struct possibleJumps{
    // This will be an array of jumpInfo containing information on how
    // to quickly compute if a jump is possible.
    JumpInfo* arr;
    int length;
} PossibleJumps;

// This lookup table will store all possible combinations of bytes that can be set as well as the
// number of bits set in that case.
static const unsigned char BitsSetTable256[256] = 
{
#   define B2(n) n,     n+1,     n+1,     n+2
#   define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#   define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
    B6(0), B6(1), B6(1), B6(2)
};

//This function fill find the number of set bits in O(1) time using a LUT.
inline unsigned char numSetBits(uint16_t value){
    return BitsSetTable256[value & 0xff] + BitsSetTable256[(value >> 8) & 0xff];
}




// Creates the lookup table.
// The length of the lookup table will be 15 elements.
// Be very careful here as we're hyper optimizing, this leads to some pretty bad
// code practices being employed here.
// We generated this using createLUTable.java
PossibleJumps* createLookupTable(){
    //Order of checks is: L R TL TR BL BR
    //JumpInfo takes expected, mask in that order.

    PossibleJumps* lookupTable = new PossibleJumps[15];

    JumpInfo* data0 = new JumpInfo[2]  {{0x0003,0x000B},{0x0005,0x0025}};
    JumpInfo* data1 = new JumpInfo[2]  {{0x000A,0x004A},{0x0012,0x0112}};
    JumpInfo* data2 = new JumpInfo[2]  {{0x0014,0x0094},{0x0024,0x0224}};
    JumpInfo* data3 = new JumpInfo[4] {{0x0018,0x0038},{0x000A,0x000B},{0x0048,0x0448},{0x0088,0x1088}};
    JumpInfo* data4 = new JumpInfo[2]  {{0x0090,0x0890},{0x0110,0x2110}};
    JumpInfo* data5 = new JumpInfo[4] {{0x0030,0x0038},{0x0024,0x0025},{0x0120,0x1120},{0x0220,0x4220}};
    JumpInfo* data6 = new JumpInfo[2]  {{0x00C0,0x01C0},{0x0048,0x004A}};
    JumpInfo* data7 = new JumpInfo[2]  {{0x0180,0x0380},{0x0090,0x0094}};
    JumpInfo* data8 = new JumpInfo[2]  {{0x0180,0x01C0},{0x0110,0x0112}};
    JumpInfo* data9 = new JumpInfo[2]  {{0x0300,0x0380},{0x0220,0x0224}};
    JumpInfo* data10 = new JumpInfo[2] {{0x0C00,0x1C00},{0x0440,0x0448}};
    JumpInfo* data11 = new JumpInfo[2] {{0x1800,0x3800},{0x0880,0x0890}};
    JumpInfo* data12 = new JumpInfo[4] {{0x1800,0x1C00},{0x3000,0x7000},{0x1080,0x1088},{0x1100,0x1120}};
    JumpInfo* data13 = new JumpInfo[2] {{0x3000,0x3800},{0x2100,0x2110}};
    JumpInfo* data14 = new JumpInfo[2] {{0x6000,0x7000},{0x4200,0x4220}};
    PossibleJumps row0 = {data0,2};
    PossibleJumps row1 = {data1,2};
    PossibleJumps row2 = {data2,2};
    PossibleJumps row3 = {data3,4};
    PossibleJumps row4 = {data4,2};
    PossibleJumps row5 = {data5,4};
    PossibleJumps row6 = {data6,2};
    PossibleJumps row7 = {data7,2};
    PossibleJumps row8 = {data8,2};
    PossibleJumps row9 = {data9,2};
    PossibleJumps row10 = {data10,2};
    PossibleJumps row11 = {data11,2};
    PossibleJumps row12 = {data12,4};
    PossibleJumps row13 = {data13,2};
    PossibleJumps row14 = {data14,2};

    lookupTable[0] = row0;
    lookupTable[1] = row1;
    lookupTable[2] = row2;
    lookupTable[3] = row3;
    lookupTable[4] = row4;
    lookupTable[5] = row5;
    lookupTable[6] = row6;
    lookupTable[7] = row7;
    lookupTable[8] = row8;
    lookupTable[9] = row9;
    lookupTable[10] = row10;
    lookupTable[11] = row11;
    lookupTable[12] = row12;
    lookupTable[13] = row13;
    lookupTable[14] = row14;

    return lookupTable;
}

// Free the lookup table that we allocated on the heap.
void freeLookupTable(PossibleJumps* lookupTable){
    for(int i=0;i<15;i++){
        delete[] lookupTable[i].arr;
    }
    delete[] lookupTable;
}



// Crash the program if bad things happened.
inline void panic(const char* message){
    if (message != NULL){
        printf("panic: %s\n", message);
    }   
    exit(1);
}

// This function will return the new state of the board once a jump occurs with expected, mask.
inline uint16_t performJump(uint16_t board, uint16_t expected,uint16_t mask){
    return ((board & ~mask) | (mask - expected));
}


/*
Checks if a jump is possible.
*/
inline bool check(uint16_t board, uint16_t expected,uint16_t mask){
    return (board & mask) == expected;
}

// This function will find the row of the triangle that we are on right now.
inline int findRow(int number){
    if(number < 1){
        return 0;
    }else if(number < 3){
        return 1;
    }else if(number < 6){
        return 2;
    }else if(number < 9){
        return 3;
    }else if(number < 15){
        return 4;
    }

    //Should not happen for a default triangle.
    panic("Invalid row found! Did you modify the board to be bigger??");
}
