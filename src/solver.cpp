#include <iostream>
#include <stdlib.h>
#include <queue>
#include <fstream>
#include <sstream>
#include "board.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iomanip>

void findSolutionBfs(uint16_t initialState, const PossibleJumps lookupTable[15], std::unordered_map<uint16_t,std::vector<uint16_t>> &map);
inline void backtrace(std::vector<uint16_t> finalState, const std::unordered_map<uint16_t,std::vector<uint16_t>> &map);
void backtrace(std::vector<uint16_t> finalStates, const std::unordered_map<uint16_t,std::vector<uint16_t>> &map,std:: ofstream &file, std::string currentStr);
inline std::string toString(uint16_t state);

// Return back the indexes of positions with pegs as we should only consider spots with pegs.
// state: the 16 bit state of the board.
std::vector<unsigned char> getPegs(uint16_t state){
    std::vector<unsigned char> empty_pegs;
    for(int i=0;state;i++, state >>= 1){
        if((state & 1)){
            empty_pegs.push_back(i);
        }
    }

    return empty_pegs;
}


static int count;
//This function will start from the final states and backtrace its way back by recursively writing all parents up until the initial state to a file.
// finalStates: the states that you want to find the parents for.
// map: the map of parent states. Should be of the form <currentState, vector<parentState>>
inline void backtrace(std::vector<uint16_t> finalStates, const std::unordered_map<uint16_t,std::vector<uint16_t>> &map){
    std::ofstream file;
    file.open("output.txt");

    backtrace(finalStates,map,file,"");
    file << "Total number of solutions are: " << count << std::endl;
    
    file.close();
}

// The actual recursive method that finds the parents and writes to file.
void backtrace(std::vector<uint16_t> finalStates, const std::unordered_map<uint16_t,std::vector<uint16_t>> &map, std::ofstream &file, std::string currentStr){
    for(uint16_t state : finalStates){
        if(map.count(state)){
            std::vector<uint16_t> parents = map.at(state);
            backtrace(parents,map,file,toString(state)+" "+currentStr);
        }else{
            count++;
            file << toString(state)<<" "<< currentStr << std::endl;
        }
    }
}

//This function converts a unsigned 16 bit state to a hex string of the form 0xXXXX.
inline std::string toString(uint16_t state){
    std::stringstream sstream;
    sstream << "0x" <<std::setfill('0') << std::setw(4) << std::hex << state;
    return sstream.str();
}

//This method will compute every single solution using BFS.
// initialState: the state of the board we want to start at.
// lookupTable: the lookup table containing information for every peg about possible jumps in that position. It does this with an array of tuples, (expected,mask)
// map: A pointer to an empty map. (Will be filled with parent states).
inline void findSolutionBfs(uint16_t initialState, const PossibleJumps lookupTable[15], std::unordered_map<uint16_t,std::vector<uint16_t>> &map){
    std::queue<uint16_t> queue;
    queue.push(initialState);
    //Create a visited set so that we NEVER look back once we pop a node to prevent repeated calculation.
    std::unordered_set<uint16_t> visited;


    std::vector<uint16_t> finalStates;

    while(!queue.empty()){
        uint16_t currentState =  queue.front();
        queue.pop();
        //Skip iteration if we already computed this path.
        if (visited.find(currentState) != visited.end()){
            continue;
        }

        visited.insert(currentState);

        if(numSetBits(currentState) == 1){
            //Does not have to be a set because the visited clause as if we already visited this final state,
            // we will skip it next time.
            finalStates.push_back(currentState);
            continue;
        }

        for (int i : getPegs(currentState)){
            PossibleJumps possibleJumps = lookupTable[i];
            JumpInfo* jumpArr = possibleJumps.arr;
            for(int j=0;j<possibleJumps.length;j++){
                JumpInfo info = jumpArr[j];
                if(check(currentState,info.expected,info.mask)){
                    uint16_t nextState = performJump(currentState,info.expected,info.mask);
                    //TODO: Is it still possible to cycle? Prove or disprove
                    if(std::find(map[nextState].begin(), map[nextState].end(), currentState) == map[nextState].end()){
                        map[nextState].push_back(currentState);
                    }                     
                    queue.push(nextState);
                }
            }
        }
    }

    backtrace(finalStates,map);
}

// We are storing the board state in a 16 bit number!
/*
We see that the board will look something like this:
             0   
           1   2   
         3   4   5      
       6   7   8   9   
    10  11  12   13  14

Note that the board has 15 possible spots which can represented by a 0 if empty,
1 if occupied.
Now we can flatten this to a linear representation
             0   
           1   2   
         3   4   5        =>  14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
       6   7   8   9   
    10  11  12   13  14

But then we notice that this is just a 15 bit number, and hence, we may represent the board state using
just 2 bytes!
This not only saves a lot of space, but lets us use masks to quickly compute if a move is possible.
We can tell a move is possible if we AND the mask and get back the mask.
*/
int main(void){
    uint16_t initialState;
    std::cout << "Please enter the initialState (default is 7FFE): ";
    std::cin >> std::hex >> initialState;
    std::cout << "The initial state you chose was: 0x"<<std::hex << initialState;
    std::unordered_map<uint16_t,std::vector<uint16_t>> map;
    PossibleJumps* lookupTable = createLookupTable();
    findSolutionBfs(initialState,lookupTable,map);
    freeLookupTable(lookupTable);
    return 0;
}
