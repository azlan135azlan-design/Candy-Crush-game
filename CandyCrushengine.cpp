bool canswap(int r1, int c1, int r2, int c2) {
    if (!inBoard(r1, c1) || !inBoard(r2, c2)) {
        return false;
    }
    int rowDifference = abs(r1 - r2);
    int colDifference = abs(c1 - c2);
    if (rowDifference + colDifference != 1) {
        return false;
    }
    if (boardColor[r1][c1] == EMPTY || boardColor[r2][c2] == EMPTY) {
        return false;
    }
    return true;
}

void swapPositions(int r1, int c1, int r2, int c2){
    int temp = boardColor[r1][c1];
    boardColor[r1][c1] = boardColor[r2][c2];
    boardColor[r2][c2] = temp;
}


bool lookmatches(bool marked[rows][colms]) {
    bool foundanymatches = false;

    for (int r = 0; r < rows; r++)
        for (int c = 0; c < colms; c++)
            marked[r][c] = false;

    // horizontal lines
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < colms - 2; c++)
        {
            int candy = boardColor[r][c];
            if (candy != EMPTY && boardColor[r][c + 1] == candy && boardColor[r][c + 2] == candy)
            {
                marked[r][c] = true;
                marked[r][c + 1] = true;
                marked[r][c + 2] = true;
                foundanymatches = true;
            }
        }
    }

    // vertical lines
    for (int r = 0; r < rows - 2; r++)
    {
        for (int c = 0; c < colms; c++)
        {
            int candy = boardColor[r][c];
            if (candy != EMPTY && boardColor[r + 1][c] == candy && boardColor[r + 2][c] == candy)
            {
                marked[r][c] = true;
                marked[r + 1][c] = true;
                marked[r + 2][c] = true;
                foundanymatches = true;
            }
        }
    }
    return foundanymatches;
}

// swap two candies; if they do not make a match, put them back
bool swapCandies(int r1, int c1, int r2, int c2, bool marked[rows][colms]) {
    if (!canswap(r1, c1, r2, c2)) 
    {
        return false;
    }
    swapPositions(r1, c1, r2, c2);
    if (lookmatches(marked)) {
        return true;
    }
    swapPositions(r1, c1, r2, c2);
    return false;
}

void gravity(int fall[rows][colms]) {
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < colms; c++)
            fall[r][c] = 0;

    for (int c = 0; c < colms; c++) {
        for (int r = rows - 1; r >= 0; r--)
        {
            if (boardColor[r][c] == EMPTY)
            {
                for (int i = r - 1; i >= 0; i--)
                {
                    if (boardColor[i][c] != EMPTY){
                        boardColor[r][c] = boardColor[i][c];
                        boardColor[i][c] = EMPTY;
                        fall[r][c] = r - i;
                        break;
                    }
                }
            }
        }
    }
}

void refillboard(int fall[rows][colms]) {
    for (int c = 0; c < colms; c++) {
        int emptyCount = 0;
        for (int r = 0; r < rows; r++)
            if (boardColor[r][c] == EMPTY) emptyCount++;

        for (int r = 0; r < rows; r++) {
            if (boardColor[r][c] == EMPTY){
                boardColor[r][c] = randInt(0, numColors - 1);
                fall[r][c] = emptyCount;
          }
      }
  }
}

void dropcandies(int fall[rows][colms]) {
    gravity(fall);
    refillboard(fall);
}

bool findvalid_moves(){
    bool tempMarked[rows][colms];

    for (int r = 0; r < rows; r++)  {
        for (int c = 0; c < colms; c++) {
            if (c + 1 < colms && canswap(r, c, r, c + 1)){
                swapPositions(r, c, r, c + 1);
                bool found = lookmatches(tempMarked);
                swapPositions(r, c, r, c + 1);
                if (found) return true;
            }
            if (r + 1 < rows && canswap(r, c, r + 1, c)){
                swapPositions(r, c, r + 1, c);
                bool found = lookmatches(tempMarked);
                swapPositions(r, c, r + 1, c);
                if (found) return true;
            }
         }
    }
     return false;
}

void initializeboard() {
    for (int attempt = 0; attempt < 200; attempt++){
        for (int r = 0; r < rows; r++){
            for (int c = 0; c < colms; c++) {
                int candy;
                bool makesMatch;
                do{
                    makesMatch = false;
                    candy = randInt(0, numColors - 1);
                    if (c >= 2 && boardColor[r][c - 1] == candy && boardColor[r][c - 2] == candy)
                        makesMatch = true;
                    if (r >= 2 && boardColor[r - 1][c] == candy && boardColor[r - 2][c] == candy)
                        makesMatch = true;
                } while (makesMatch);

                boardColor[r][c] = candy;
           }
        }
        if (findvalid_moves()) return;
     }
}

void shuffleboard(){
    bool tempMarked[rows][colms];

    for (int attempt = 0; attempt < 400; attempt++){
        for (int i = 0; i < rows * colms * 3; i++)
        {
            int r1 = randInt(0, rows - 1);
            int c1 = randInt(0, colms - 1);
            int r2 = randInt(0, rows - 1);
            int c2 = randInt(0, colms - 1);
            swapPositions(r1, c1, r2, c2);
        }
        if (!lookmatches(tempMarked) && findvalid_moves()) return;
    }
    initializeboard();
}
