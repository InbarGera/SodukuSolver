class Solution {
public:
    void solveSudoku(vector<vector<char>>& board) {
        recursiveSudokuSolver(board);
    }
    
private:
    /* Main solving function, same as the public one, except it returning if i succedded or not.
       If receiving an ilegal or unsolveable board - returning it as is */
    bool recursiveSudokuSolver(vector<vector<char>>& refBoard) {
        if (!isLegal(refBoard)) {
            return false;
        }
        
        /* Modifing its own version - in case the board is unsolveable, and then applying ruls over it will be hard to revert*/
        vector<vector<char>> board(refBoard);
        
        /* Keeping track over valid options for every blank spot */
        vector<vector<vector<bool>>> available(9, vector<vector<bool>>(9, vector<bool>(9, true)));
        
        applyPropagationRools(board, available);
        if (isSolved(board)) {
            refBoard = std::move(board);
            return true;
        }

        // Doing the guessing part in a clever way:
        // Choosing accourding to the number of possible slots
        std::pair<int, int> nextIndexToGuess = getNextIndexToGuess(available);
        int i = nextIndexToGuess.first;
        int j = nextIndexToGuess.second;
        for (char c = '1'; c <= '9'; ++c) {
            if (available[i][j][c - '1']) {
                board[i][j] = c;
                if (recursiveSudokuSolver(board)) {
                    refBoard = std::move(board);
                    return true;
                }
                board[i][j] = '.';
            }
        }
        
        /* Should not get here */
        // assert(0);
        return false;
    }
    
    /* State checking methods */
    
    bool isSolved(vector<vector<char>>& board) {
        if(!isLegal(board)) {
            return false;
        }

         for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (board[i][j] == '.') {
                    return false;
                }
            }
        }

        return true;
    }
    
    bool isLegal(vector<vector<char>>& board) {
        for (int i = 0; i < 9; ++i) {
            if(!isRowLegal(i, board)) {
                return false;
            }
            if(!isCulLegal(i, board)) {
                return false;
            }
        }
        
        for (int i = 0; i < 9; i+= 3) {
            for (int j = 0; j < 9; j += 3) {
                if (!isSquareLegal(i, j, board)) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    bool isRowLegal(int i, vector<vector<char>>& board) {
        bool hist[9];
        for (int i=0; i < 9; ++i) {
            hist[i] = false;
        }
        
        for (int j = 0; j < 9; ++j) {
            const char& c = board[i][j];
            if (c == '.') {
                continue;
            }
            if (hist[c - '1']) {
                return false;
            }
            
            hist[c - '1'] = true;
        }
        
        return true;
    }
    
    bool isCulLegal(int j, vector<vector<char>>& board) {
        bool hist[9];
        for (int i=0; i < 9; ++i) {
            hist[i] = false;
        }
        
        for (int i = 0; i < 9; ++i) {
            const char& c = board[i][j];
            if (c == '.') {
                continue;
            }
            if (hist[c - '1']) {
                return false;
            }
            
            hist[c - '1'] = true;
        }
        
        return true;
    }
    
    bool isSquareLegal(int i, int j, vector<vector<char>>& board) {
        bool hist[9];
        for (int i=0; i < 9; ++i) {
            hist[i] = false;
        }
        
        for (int ii = i; ii < i+3; ++ii) {
            for (int jj = j; jj < j+3; ++jj) {
                const char& c = board[ii][jj];
                if (c == '.') {
                    continue;
                }
                if (hist[c - '1']) {
                    return false;
                }

                hist[c - '1'] = true;
            }
        }
        
        return true;
    }
    
    /* Methods for propagating the solution */
    
    bool applyPropagationRools(vector<vector<char>>& board , vector<vector<vector<bool>>>& available) {        
        bool res = false;
        bool changed = true;
        while(changed) {
            changed = false;
            applyRowCulRools(board, available);
            changed = changed || discoveryRestrictedNumbers(board, available);
            
            applySquareRool(board, available);
            changed = changed || discoveryRestrictedNumbers(board, available);
            
            res = res || changed;
        }
        
        return res;
    }
    
    bool discoveryRestrictedNumbers(vector<vector<char>>& board, vector<vector<vector<bool>>>&available) {
        bool changed = false;
        for(int i=0; i < 9; ++i) {
            for(int j=0; j < 9; ++j) {
                
                /* If there is only one valid option - this is the number in the slot */
                int numValids = 0;
                int validNumber;
                for(int k=0; k < 9; ++k) {
                    if(available[i][j][k]) {
                        numValids++;
                        validNumber = k;
                    }
                }
                
                if (numValids == 1) {
                    changed = true;
                    board[i][j] = '1' + validNumber;
                    available[i][j] = vector<bool>(9, true); // To ignore it next time, and to not take when choosing the next guess.

                }
            }
        }
        
        return changed;
    }
    
    void applyRowCulRools(vector<vector<char>>& board, vector<vector<vector<bool>>>&available) {
        for(int i=0; i < 9; ++i) {
            for(int j=0; j < 9; ++j) {
                const char& c = board[i][j];
                if (c == '.') {
                    continue;
                }
                
                for(int i=0; i < 9; ++i) {
                    if (board[i][j] == '.') {
                        available[i][j][c -'1'] = false;
                    }
                }
                
                for(int j=0; j < 9; ++j) {
                    if (board[i][j] == '.') {
                        available[i][j][c -'1'] = false;
                    }
                }
            }
        }
    }
                      
    void applySquareRool(vector<vector<char>>& board, vector<vector<vector<bool>>>&available) {
        for(int i=0; i < 9; i += 3) {
            for(int j=0; j < 9; j += 3) {
                
                for(int ii=i; ii < i+3; ++ii) {
                    for(int jj=j; jj < j+3; ++jj) {
                        const char& c = board[ii][jj];
                        if (c == '.') {
                            continue;
                        }
                        
                        for(int ii=i; ii < i+3; ++ii) {
                            for(int jj=j; jj < j+3; ++jj) {
                                if (board[ii][jj] == '.') {
                                    available[ii][jj][c -'1'] = false;
                                }
                            }
                        }
                    }
                }
                
            }
        }
    }

    /* Main algorithm helper */
    
    /* Getting the next index to guess and continue searchig - an index with the least amount of diferent possible numbers */
    pair<int, int> getNextIndexToGuess(vector<vector<vector<bool>>>& available) {
        pair<int, int> res;
        int maxLen = 10;
         for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                int numLeft = 0;
                for (int k = 0; k < 9; ++k) {
                    if (available[i][j][k]) {
                        numLeft++;
                    }
                }
                
                if(numLeft < maxLen) {
                    res = make_pair(i, j);
                    maxLen = numLeft;
                }
            }
        }
        
        return res;
    }

};