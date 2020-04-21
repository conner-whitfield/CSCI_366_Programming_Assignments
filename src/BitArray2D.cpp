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
using namespace std;

#include <math.h>
#include <cereal/types/vector.hpp>
#include <vector>
#include "BitArray2D.hpp"
#include "common.hpp"



char* array;
unsigned int size = BOARD_SIZE;

BitArray2D::BitArray2D(unsigned int rows, unsigned int columns) {
    if(rows < 1 || columns < 1)
    {
        throw BitArray2DException("Bad bounds 0 or negative. - BitArray2D");
    }
    else if(rows > size || columns > size)
    {
        throw BitArray2DException("Bad bounds above size limit. - BitArray2D");
    }
    array = (char*)calloc(rows*columns, sizeof(char));
}


BitArray2D::~BitArray2D() {

}


bool BitArray2D::get(unsigned int row, unsigned int column){
   // check array bounds
    if(row < 0 || column < 0 || row >= size || column >= size)
    {
        throw BitArray2DException("Bad bounds. - get");
    }
   // get the element
   return get_bit_elem(array, columns, row, column);
}



void BitArray2D::set(unsigned int row, unsigned int column){
   // check array bounds
    if(row < 0 || column < 0 || row >= size || column >= size)
    {
        cout << "row " << to_string(row) << " col " << to_string(column) << " size " << to_string(size);
        throw BitArray2DException("Bad bounds. - set");
    }
   // set the element
   set_bit_elem(array, columns, row, column);
}
