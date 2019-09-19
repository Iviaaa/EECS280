//
//  Map.h
//  280p2
//
//  Created by Hongting Zhu on 10/9/18.
//  Copyright © 2018 Hongting Zhu. All rights reserved.
//

#ifndef Map_h
#define Map_h

#include <vector>
#include <string>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <queue>

#include "P2random.h"

using myPair = std::pair<int, int>;

struct Tile{
    int row;
    int col;
    int val;
    bool used = false;
    bool cleared = false;
    friend class Map;
    friend class Prioritized;
    
    bool operator== (const Tile &b) const;
    bool operator!= (const Tile &b) const;
};//Tile

struct Prioritized{
    friend class Map;
    friend struct Tile;
    bool operator()(const Tile &a, const Tile &b) const;
};//Prioritized


class Map{
    friend struct Tile;
    friend class Prioritized;
    
public:
    //custom constructor
    Map(bool v, int f) {
        verbose = v;
        first = f;
    }
    
    void getMap(int argc, char** argv);
    void defaultPrint();
    //void printV();
    void printM();
    void printS();
    void escape();
    
    static std::vector<std::vector<Tile>> map;
private:
    std::string mode;
    int first;
    bool median = false;
    bool verbose = false;
    
    unsigned int floor_size = 0, seed = 0, max_rubble = 0, tnt_chance = 0;
    unsigned int gridcol = 0, gridrow = 0;
    
    int numTNT = 0;
    int numTiles = 0;
    int numRubs = 0;
    std::vector<int> meds;
    std::vector<Tile> cleared;
    std::priority_queue<Tile, std::vector<Tile>, Prioritized> tiles;
    
    Tile currentTile;
    unsigned int curCol = gridcol;
    unsigned int curRow = gridrow;
};

#endif /* Map_h */
