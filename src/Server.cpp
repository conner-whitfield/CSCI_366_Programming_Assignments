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


#include "common.hpp"
#include "Server.hpp"
#include "../cereal/include/cereal/types/vector.hpp"
#include <iostream>
#include <math.h>
#include <stdint.h>
#include "../cereal/include/cereal/archives/json.hpp"
#include "../cereal/include/cereal/types/vector.hpp"
#include <thread>
#include <chrono>

/**
 * Calculate the length of a file (helper function)
 *
 * @param file - the file whose length we want to query
 * @return length of the file in bytes
 */
int get_file_length(ifstream *file)
{
    int length = -1;    // start at -1 to account for empty line at end of file
    char throwaway; // store character for iteration purposes (does NOT include whitespace characters: newline, \n, etc)
    while(file->peek() != EOF)  // while next character is not EOF
    {
        length++;   // increment length
        *file >> throwaway; // read a character
    }
    // cout << " length " << length << endl;
    return length; // return length of file
}

// int size;
vector<vector<char>> p2current; // hold updated version of p2's board
vector<vector<char>> p1current; // hold updated version of p1's board

void Server::initialize(unsigned int board_size,
                        string p1_setup_board,
                        string p2_setup_board) // passes all tests for initialize
{
    this->board_size = board_size;  // store board_size value
    if(board_size <= 0) // throw error if board_size is negative or 0
    {
        throw std::runtime_error("Board size is illegal!");
    }
    vector<vector<char>> p1board;   // temp board to hold p1 file boar
    ifstream p1;    // to read in p1 file
    p1.open(p1_setup_board); // open the file for player 1's board
    if(p1.fail() || !p1)   // if file open failure or no contents, throw error
    {
        throw std::runtime_error("Invalid file!");
    }
    int length = get_file_length(&p1);  // get length of file (no whitespace)
    int dim = (int) sqrt(length);   // if board is square (should be), the square root of the file length can be used to control row/col amount
    p1.clear(); // reset p1
    p1.seekg(0);    // prep to reread p1 contents from start
    vector<char> temp;  // temp used to hold chars and place into overall 2d board
    char c; // hold current character
    bool read = true;   // read as long as next file is not EOF
    while(read) // loop on read flag
    {
        p1 >> c;    // get character from p1
        if(p1.peek() == EOF){   // if on EOF after getting character, do not read again
            read = false;
        }
        temp.push_back(c);  // add character to temp vector
        if (temp.size() == dim) {   // add vector to overall board when its size is equal to square root
            p1board.push_back(temp);
            temp.clear();   // clear for next char set row
        }

    }
    p1.close(); // done with p1's board file, close ifstream
    p1current = p1board;    // set value of global for future use

    vector<vector<char>> p2board;   // create temp 2d vector for player board
    ifstream p2;    // to read in p2 txt file
    p2.open(p2_setup_board);    // open p2 board txt file
    if(p2.fail() || !p2)    // throw error if no contents or file nonexistent
    {
        throw std::runtime_error("Invalid file!");
    }
    length = get_file_length(&p2);  // get file length (no whitespace)
    dim = (int) sqrt(length);   // get square root of length (should be an int if the board is square, which it should be)
    p2.clear(); // reset p2 for another read
    p2.seekg(0);    // reset p2 to start of file
    temp.clear();   // clear vector temp (reused from p1 read-in process)
    bool go = true; // loop as long as not on an EOF character. same process as with p1's board txt file
    while(go)   // loop through file
    {
        p2 >> c;    // get current char
        if(p2.peek() == EOF){   // if next char is EOF do not loop again
            go = false;
        }
        temp.push_back(c);  // add char to temp vector
        if (temp.size() == dim) {   // add temp vector to 2d board when it is at correct size
            p2board.push_back(temp);
            temp.clear();   // clear temp for next read
        }
    }
    p2.close(); // close ifstream
    p2current = p2board;    // set global value for future use of the 2d vector

    // begin size checking
    if(p1board.size() != board_size || p2board.size() != board_size)    // if num of rows are incorrect for either board, error out
    {
        throw std::runtime_error("Number of rows is off in one of the two boards!");
    }
    else    // if num of cols incorrect in any row, error out
    {
        for(int i = 0; i < p1board.size(); i++) // loop on rows of p1 board
        {
            if(p1board[i].size() != board_size) // if row size (num of cols) invalid, error out
            {
                throw std::runtime_error("Player 1 board is not square!");
            }
        }
        for(int i = 0; i < p2board.size(); i++) // loop on rows of p2 board
        {
            if(p2board[i].size() != board_size) // if row size (num of cols) invalid, error out
            {
                throw std::runtime_error("Player 2 board is not square!");
            }
        }
    }
}


int Server::evaluate_shot(unsigned int player, unsigned int x, unsigned int y)
{
    if(player > MAX_PLAYERS || player < 1) // error out if player number is outside bounds
    {
        throw std::runtime_error("Player parameter is invalid!");
    }

    if(x >= board_size || x < 0
       || y >= board_size || y < 0) // return out of bounds if x or y outside of board_size limitations
    {
        return OUT_OF_BOUNDS;
    }

    // below lines are reached if no errors or out of bounds shot detected

    vector<vector<char>> temp;  // board to be examined
    if(player == 1) temp = p2current;   // if player is 1, then check result on player 2 board
    else if(player == MAX_PLAYERS) temp = p1current;    // if player is 2, then examine player 1 board

    if(temp[y][x] == 'C' || temp[y][x] == 'B' || temp[y][x] == 'R' || temp[y][x] == 'S' || temp[y][x] == 'D')
    {   // declare hit if the index value is that of a ship letter
        temp[y][x] = 'X';   // mark hit
        if(player == 1) p2current = temp;   // update current board
        else if(player == MAX_PLAYERS) p1current = temp;    // update current board
        return HIT; // return hit
    }
    else if(temp[y][x] == 'X' || temp[y][x] == 'O')
    {   // if spot already fired upon, return miss
        return MISS;
    }
    else
    {   // handle miss
        temp[y][x] = 'O';   // mark miss in empty spot
        if(player == 1) p2current = temp;   // update current board
        else if(player == MAX_PLAYERS) p1current = temp;    // update current board
        return MISS;
    }
}


int Server::process_shot(unsigned int player) {
    if (player > MAX_PLAYERS || player < 1)
    {   // throw error if outside player bounds
        throw std::runtime_error("Player bounds exceeded in process_shot!");
    }
    else
    {
        string shotfile = "player_" + to_string(player) + ".shot.json"; // file name

        ifstream shot(shotfile);    // read in shot file
        if (shot.fail() || !shot)   // if file empty or fail to read, close ifstream and error
        {
            shot.close();
            return NO_SHOT_FILE;    // return value for no file
        }
        else    // else can read file
        {
            // deserialize shot
            int x, y;   // coordinates from deserialization
            cereal::JSONInputArchive s(shot);   // deserialize shot ifstream
            s(x, y);    // get coordinate values
            //cout << "X Y " << to_string(x) << " " << to_string(y) << endl;
            shot.close();   // close ifstream
            remove(shotfile.c_str());   // remove file contents for future use

            // new file result.json
            int result = evaluate_shot(player, x, y);   // get result of evaluate_shot
            string fname = "player_"  + to_string(player) + ".result.json"; // result.json file name
            remove(fname.c_str());  // remove contents of file

            ofstream array(fname);  // ofstream to feed into file
            cereal::JSONOutputArchive write(array); // serialize into ofstream
            write(CEREAL_NVP(result));  // write JSON for result value
            //array.close();    <-- commented out because this line gave me a segmentation fault
            return SHOT_FILE_PROCESSED; // process successful, so return shot_file_processed
        }
    }
}