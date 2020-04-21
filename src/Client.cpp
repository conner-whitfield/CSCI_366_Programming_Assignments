//    Battleship game assignment for MSU CSCI 366
//    Copyright (C) 2020    Mike P. Wittie
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.


// https://github.com/msu-netlab/CSCI_366_examples/blob/master/serialization/main.cpp

using namespace std;

#include "common.hpp"
#include "Client.hpp"
#include <map>
#include <iterator>
#include <algorithm>
#include "../cereal/include/cereal/archives/json.hpp"
#include "../cereal/include/cereal/types/vector.hpp"

using namespace std;

int pnum;   // global to track player number
vector<vector<int>> board;  // global for board

Client::~Client()
{

}

void Client::initialize(unsigned int player, unsigned int board_size)
{
    pnum = player;  // store global value
    vector<int> row(board_size, 0); // init row vector with size and all 0s
    vector<vector<int>> newboard(board_size, row);  // create 2d vector from row vectors
    board = newboard;   // set global value
    string file = "player_" + to_string(player) + ".action_board.json"; // file name
    remove(file.c_str());   // clear file contents

    ofstream array(file);   // ofstream to create file
    cereal::JSONOutputArchive write(array); // JSON Output
    write(CEREAL_NVP(board));   // write JSON for board value
    // no ifstream .close() here due to segmentation faults
}


void Client::fire(unsigned int x, unsigned int y)
{
    string fname = "player_"  + to_string(pnum) + ".shot.json"; // make filename
    remove(fname.c_str());  // remove file contents

    ofstream array(fname);  // write to file
    cereal::JSONOutputArchive write(array); // write to file in JSON
    write(CEREAL_NVP(x));   // write x coord
    write(CEREAL_NVP(y));   // write y coord
    //array.close();    <-- removed due to segmentation fault SIGSEGV errors
}


bool Client::result_available()
{
    string f = "player_" + to_string(pnum) + ".result.json";    // file name for result for this player
    ifstream file(f);   // input the give file
    if(file.fail() || !file)    // if read error, assume no result available
    {
        return false;
    }
    else    // else true
    {
        return true;
    }
}


int Client::get_result()
{
    if(result_available())  // if result avialable then examine result.json
    {
        string f = "player_" + to_string(pnum) + ".result.json";    // get file name
        ifstream file(f);   // read in file
        int res;    // store result
        cereal::JSONInputArchive JIA(file); // deserialization
        JIA(res);   // get result value
        file.close();   // close ifstream
        remove(f.c_str());  // remove file contents
        if(res == 1 || res == -1 || res == 0)
        {   // if result is hit, miss, or out of bounds return the value
            return res;
        }
        else    // else the value is prohibited
        {
            throw std::runtime_error("Illegal result value!");
        }
    }
}



void Client::update_action_board(int result, unsigned int x, unsigned int y)
{
    if(result == HIT || result == MISS) // if hit or miss, then make appropriate changes to board
    {
        board[y][x] = result;   // set value in array

        // serialize the update player action board
        string file = "player_" + to_string(pnum) + ".action_board.json";
        remove(file.c_str());
        ofstream array(file);
        cereal::JSONOutputArchive write(array);
        write(CEREAL_NVP(board));
        // no ifstream close() due to segmentation faluts SIGSEGV
    }
}


string Client::render_action_board()
{   // simple double for loop to get 2d vector contents and concatenate them together
    string out = "";
    for(int i = 0; i < board.size(); i++)
    {
        for(int j = 0; j < board[i].size(); j++)
        {
            out = out + " " + to_string(board[i][j]);
        }
        out = out + "\n";
    }
    return out; // return string representation of board
}