//
//  Map.cpp
//  280p2
//
//  Created by Hongting Zhu on 10/9/18.
//  Copyright © 2018 Hongting Zhu. All rights reserved.
//
//

#include <stdio.h>
#include "Map.h"
bool Prioritized::operator()(const Tile &a, const Tile &b) const {
    int aVal = a.val;
    int bVal = b.val;
    //The expression comp(a,b), where comp is an object of this type and a
    //and b are elements in the container, shall return true if a is considered
    //to go before b in the strict weak ordering the function defines.
    //which means b would be the top!
    if (aVal == bVal) {
        if (a.col != b.col) return a.row > b.row;
        else return a.col > b.col;
    }
    else return aVal > bVal;
}

bool Tile::operator== (const Tile &b) const{
    if (this->col == b.col && this->row == b.row) return true;
    else return false;
}

bool Tile::operator!= (const Tile &b) const{
    if (this->col != b.col || this->row != b.row) return true;
    else return false;
}

void Map::getMap(int argc, char** argv){
    std::cin >> mode;
    std::stringstream ss;
    
    std::string junk;
    //set up istream
    if (mode == "R") {
        // Read some variables from cin
        std::cin >> junk;
        std::cin >> floor_size;
        std::cin >> junk;
        std::cin >> gridrow >> gridcol;
        std::cin >> junk;
        std::cin >> seed;
        std::cin >> junk;
        std::cin >> max_rubble;
        std::cin >> junk;
        std::cin >> tnt_chance;
        P2random::PR_init(ss, floor_size, seed, max_rubble, tnt_chance);
    }//if
    else if (mode == "M"){
        std::cin >> junk >> floor_size;
        std::cin >> junk >> gridrow >> gridcol;
    }//else if
    else {
        std::cerr << "Error: Invalid mode. \n";
        exit(1);
    }//else
    
    if (gridcol > floor_size || gridrow > floor_size){
        std::cerr << "Error: Starting point not in the grid.\n";
    }//if
    
    map.resize(floor_size);
    //if map mode is on, read from cin; otherwise read from the stringstream
    std::istream &inputsteam = (mode == "M") ? std::cin : ss;
    for(int i = 0; i < floor_size; ++i) {
        map[i].resize(floor_size);
        for (int j = 0; j < floor_size; ++i){
            Tile temp;
            temp.row = i;
            temp.col = j;
            inputsteam >> temp.val;
            map[i][j] = temp;
        }
    }//for
}

void Map::escape(){
    std::priority_queue<Tile, std::vector<Tile>, Prioritized> denotation;
    while (currentTile.row != floor_size - 1 && currentTile.row != 0
       && currentTile.col != floor_size - 1 && currentTile.col != 0){
        
        map[curRow][curCol].used = true;
        if (currentTile.val == -1) {
            denotation.push(currentTile);
            while (!denotation.empty()){
                Tile next = denotation.top();
                if (next.val == -1) {
                    if(verbose) std::cout << "TNT explostion at [" << next.row << "," << next.col << "]!\n" ;
                    numTNT++;
                    map[next.row][next.col].val = 0;

                    if(next.row - 1 >= 0 && !map[next.row- 1][next.col].used) denotation.push(map[next.row - 1][next.col]);
                    if(next.row + 1 < floor_size && !map[next.row + 1][next.col].used) denotation.push(map[next.row + 1][next.col]);
                    if(next.col - 1 >= 0 && !map[next.row][next.col - 1].used) denotation.push(map[next.row][next.col - 1]);
                    if(next.col + 1 < floor_size && !map[next.row][next.col + 1].used) denotation.push(map[next.row][next.col + 1]);
                    cleared.push_back(denotation.top());
                    next.val = 0;
                    if (next != currentTile) tiles.push(next);
                }
                else if (next.val > 0) {
                    if(verbose) std::cout << "Cleared by TNT: " << map[next.row][next.col].val << " at " << "[" << next.row << "," << next.col << "]\n";
                    
                    numRubs += next.val;
                    numTiles++;
                    cleared.push_back(next);
                    meds.push_back(next.val);
                    map[next.row][next.col].val = 0;
                    next.val = 0;
                    //prioritized
                    tiles.push(next);
                    if(median) printM();
                }
                next = denotation.top();
                denotation.pop();
            }
        }
        else if (currentTile.val >= 0) {
            if (!map[curRow - 1][curCol].used) tiles.push(map[curRow - 1][curCol]);
            if (!map[curRow + 1][curCol].used) tiles.push(map[curRow + 1][curCol]);
            if (!map[curRow][curCol - 1].used) tiles.push(map[curRow][curCol - 1]);
            if (!map[curRow][curCol + 1].used) tiles.push(map[curRow][curCol + 1]);
            //...TODO
            if (map[curRow][curCol].val > 0) {
                if(verbose) std::cout << "Cleared: " << map[curRow][curCol].val << " at " << "[" << curRow << "," << curCol << "]\n";
                //Clear the current tile
                cleared.push_back(map[curRow][curCol]);
                numTiles++;
                //if(map[curRow][curCol].cleared) map[curRow][curCol].val = 0;
                numRubs += map[curRow][curCol].val;
                meds.push_back(map[curRow][curCol].val);
                if(median) printM();
            }
        }
        //and keep cleaning TOOOOOOODOOOOOOO
        currentTile = tiles.top();
        curRow = currentTile.row;
        curCol = currentTile.col;
        tiles.pop();
    }
    //if it is at the edge
}

//Cleared ​<NUM>​ tiles containing ​<AMOUNT>​ rubble and escaped.
void Map::defaultPrint(){
    std::cout << "Cleared " << numTiles << " tiles containing "
            << numRubs << " rubble and escaped.\n";
}

void Map::printM(){
    //Median difficulty of clearing rubble is ​<MEDIAN>
    std::nth_element(meds.begin(), meds.begin() + meds.size()/2, meds.end());
    std::cout << "Median difficulty of clearing rubble is " << meds[meds.size()/2] << '\n';
}

void Map::printS(){
    std::cout << "First tiles cleared:\n";
    int content;
    int i = 0;
    int size = int(cleared.size());
    while (i < first || i == size){
        content = cleared[i].val;
        if (content == -1){
            std::cout << "TNT at [" << cleared[i].row << "," << cleared[i].col << "]\n";
        }
        else if (content > 0){
            std::cout <<  content << " at [" << cleared[i].row << "," << cleared[i].col << "]\n";
        }
        ++i;
    }//first N
    
    std::cout << "Last tiles cleared:\n";
    i = size;
    while (i > size - first || i < 0){
        content = cleared[i].val;
        if (content == -1){
            std::cout << "TNT at [" << cleared[i].row << "," << cleared[i].col << "]\n";
        }
        else if (content > 0){
            std::cout << content << " at [" << cleared[i].row << "," << cleared[i].col << "]\n";
        }
        --i;
    }//last N
    
    Prioritized prior;
    std::sort(cleared.begin(), cleared.end(), prior);
    std::cout << "Easiest tiles cleared:\n";
    i = 0;
    while (i < first || i == size){
        content = cleared[i].val;
        if (content == -1){
            std::cout << "TNT at [" << cleared[i].row << "," << cleared[i].col << "]\n";
        }
        else if (content > 0){
            std::cout <<  content << " at [" << cleared[i].row << "," << cleared[i].col << "]\n";
        }
        ++i;
    }//easiest N
    
    std::cout << "Hardest tiles cleared:\n";
    i = size;
    while (i > size - first || i < 0){
        content = cleared[i].val;
        if (content == -1){
            std::cout << "TNT at [" << cleared[i].row << "," << cleared[i].col << "]\n";
        }
        else if (content > 0){
            std::cout << content << " at [" << cleared[i].row << "," << cleared[i].col << "]\n";
        }
        --i;
    }//hardest N
}

/*void Map::printV(){
    //Cleared: ​<RUBBLE>​ at [​<ROW>​,​<COL>​]
    //TNT explosion at ​[<ROW>,<COL>]​!
    //Cleared by TNT: ​<RUBBLE>​ at [​<ROW>​,​<COL>​]
    if (map[curRow][curCol].val == -1)    std::cout << "TNT explostion at [" << curRow << "," << curCol << "]!\n" ;
    else if (map[curRow][curCol].val > 0)
        std::cout << "Cleared: " << map[curRow][curCol].val << " at " << "[" << curRow << "," << curCol << "]\n";
}
 */
