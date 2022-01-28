/**
 * Copyright (c) 2022 André Gewert <agewert@ubergeek.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <Arduboy2.h>

//#define COLS 21
//#define ROWS 10
//#define CELLSIZE 6

//#define COLS 25
//#define ROWS 12
//#define CELLSIZE 5

#define COLS 32
#define ROWS 16
#define CELLSIZE 4

//#define COLS 42
//#define ROWS 21
//#define CELLSIZE 3

Arduboy2 arduboy;
bool drawMode = true;
byte cellState[COLS *ROWS];

void setup() {
    arduboy.begin();
    arduboy.setFrameRate(10);
    arduboy.initRandomSeed();
    arduboy.setTextSize(1);
    fillCellsRandomly();
}

void loop() {
    if (!arduboy.nextFrame()) return;
    arduboy.clear();
    arduboy.pollButtons();
    
    calculateNextCellState();
    drawCells();
    advanceCellState();
    
    if (arduboy.justPressed(B_BUTTON)) {
        fillCellsRandomly();
    } else if (arduboy.pressed(A_BUTTON)) {
        addRandomCell();
    }
    if (arduboy.justPressed(UP_BUTTON)) {
        drawMode = !drawMode;
    }
       
    arduboy.display();
}

void drawCells() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            int idx = getCellIndexAt(x, y);

            if (drawMode) {
                int oldState = cellState[idx] & 1;
                int currentState = (cellState[idx] >> 1) & 1;
                int nextState = (cellState[idx] >> 2) & 1;

                // Born
                if (currentState == 1 && oldState == 0) {
                    arduboy.fillRect(x *CELLSIZE +1, y *CELLSIZE +1, CELLSIZE -2, CELLSIZE -2);
                }

                // Living
                else if (currentState == 1 && nextState == 1) {
                    arduboy.fillRect(x *CELLSIZE, y *CELLSIZE, CELLSIZE, CELLSIZE);
                }
    
                // Dying
                else if (currentState == 1 && nextState == 0) {
                    arduboy.drawRect(x *CELLSIZE +1, y *CELLSIZE +1, CELLSIZE -2, CELLSIZE -2);
                }                
            } else {
                int currentState = (cellState[idx] >> 1) & 1;
                if (currentState == 1) {
                    arduboy.fillRect(x *CELLSIZE, y *CELLSIZE, CELLSIZE, CELLSIZE);
                }
            }
        }
    }
}

void advanceCellState() {
    for (int idx = 0; idx < COLS *ROWS; idx++) {
        cellState[idx] = cellState[idx] >> 1;
    }
}

void calculateNextCellState() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            int idx = getCellIndexAt(x, y);
            byte currentState = (cellState[idx] >> 1) & 1;
            byte count = countLivingNeighborsAt(x, y);

            if (currentState == 0 && count == 3) {
                cellState[idx] |= 4;
            } else if (currentState == 1 && (count == 2 || count == 3)) {
                cellState[idx] |= 4;
            } else {
                cellState[idx] &= ~4;
            }
        }
    }
}

/**
 * @brief Fills all cells with a random state
 */
void fillCellsRandomly() {
    for (int i = 0; i < COLS *ROWS; i++) {
        cellState[i] = 0;
        if (random(0, 5) == 0) {
            cellState[i] = 2;
        }
    }
}

void addRandomCell() {
    cellState[random(0, COLS *ROWS)] |= 2;
}

/**
 * @brief Get the array index for the cell with the given coordinates
 * 
 * @param x Horizontal coordinate
 * @param y Vertical coordinate
 * @return int Array index
 */
int getCellIndexAt(int x, int y) {

    if (x < 0) {
        x = COLS +x;
    } else if (x >= COLS) {
        x = COLS -x;
    }
    if (y < 0) {
        y = ROWS +y;
    } else if (y >= ROWS) {
        y = ROWS -y;
    }
    
    x = x %COLS;
    y = y %ROWS;
    return (y *COLS) +x;
}

int getCurrentCellStateAt(int x, int y) {
    //if (x < 0 || y < 0 || x >= COLS || y >= ROWS) return 0;
    return (cellState[getCellIndexAt(x, y)] >> 1) & 1;
}


int countLivingNeighborsAt(int x, int y) {
    return getCurrentCellStateAt(x -1, y -1)
        + getCurrentCellStateAt(x, y -1)
        + getCurrentCellStateAt(x +1, y -1)
        
        + getCurrentCellStateAt(x -1, y)
        + getCurrentCellStateAt(x +1, y)
        
        + getCurrentCellStateAt(x -1, y +1)
        + getCurrentCellStateAt(x, y +1)
        + getCurrentCellStateAt(x +1, y +1);
}
