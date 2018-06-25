//Stephen Blatt - Author
//Scyther - Program Name

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
using namespace std;

//can go from 0000 --> 8686
  //legal moves can be found here
    //move 4265 caused an aB prune? increment it's score

struct mData {
  int sx;
  int sy;
  int ex;
  int ey;
  int spid;
  int epid;
  int cpid;
};

//mData moveData; //declare mData object

vector<mData> moves;
vector<mData> cmoves;
vector<mData> captureMoves;
vector<mData> minimaxMoves[20];

int moveCount = 0;
int moveCountComputer = 0;
int captureMoveCount = 0;
int captureHumanCount = 0;

int movesLooked = 0;

int firstPlayer;

int humanKingFound = 0;

int win = -2; //win = -1 human wins, win = 1 computer;

//int ma[8888]; // total list of possible moves;

//board rows and columns
const int rows = 8;
const int cols = 6;

//human char pieces
const char king = 'k';
const char bishop = 'b';
const char rook = 'r';
const char knight = 'n';
const char pawn = 'p';

//computer char pieces
const char king_c = 'K';
const char bishop_c = 'B';
const char rook_c = 'R';
const char knight_c = 'N';
const char pawn_c = 'P';

//empty space
const char e = '-';

//human int values
const int king_val = -1;
const int bishop_val = -2;
const int rook_val = -3;
const int knight_val = -4;
const int pawn_val = -5;

//computer int values
const int king_val_c = 1;
const int bishop_val_c = 2;
const int rook_val_c = 3;
const int knight_val_c = 4;
const int pawn_val_c = 5;

//depth to search
const int depth = 5;

const int maxdepth = 4;

//game board array
int b[rows][cols]; //rows, cols

//temp int for bishop and rook checks
int temp;
int tempj;

void setup() {
  // 7 - K - - - -
  // 6 N B R R B N
  // 5 - - P P - -
  // 4 - - - - - -
  // 3 - - - - - -
  // 2 - - p p - -
  // 1 n b r r b n
  // 0 - - - - k -
  //   0 1 2 3 4 5

  //first fill the array with 0's so
  //we can test for NULL values without errors
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      b[i][j] = 0;
    }
  }

  //human pieces setup
  b[0][4] = king_val; //human king
  b[1][0] = knight_val; //human knight
  b[1][1] = bishop_val; //human bishop
  b[1][2] = rook_val; //human rook_val
  b[1][3] = rook_val; //human rook_val
  b[1][4] = bishop_val;
  b[1][5] = knight_val;
  b[2][2] = pawn_val;
  b[2][3] = pawn_val;

  //computer pieces initial setup
  b[7][1] = king_val_c;
  b[6][0] = knight_val_c;

  //*
  b[6][1] = bishop_val_c;
  b[6][2] = rook_val_c;
  b[6][3] = rook_val_c;
  b[6][4] = bishop_val_c;
  b[6][5] = knight_val_c;
  b[5][2] = pawn_val_c;
  b[5][3] = pawn_val_c;//*/

}

void printBoard() {
  for (int i = rows - 1; i >= 0; i--) { //need to print from the top down
  //for (int i = 0; i < rows; i++) {
    //cout << i << " " << i + 1 << " ";
    cout << i + 1 << " ";
    for (int j = 0; j < cols; j++) {    //because otherwise board is flipped
      if (b[i][j] == king_val_c) {
        cout << king_c;
      }
      else if (b[i][j] == king_val) {
        cout << king;
      }
      else if (b[i][j] == knight_val_c) {
        cout << knight_c;
      }
      else if (b[i][j] == knight_val) {
        cout << knight;
      }
      else if (b[i][j] == rook_val_c) {
        cout << rook_c;
      }
      else if (b[i][j] == rook_val) {
        cout << rook;
      }
      else if (b[i][j] == bishop_val_c) {
        cout << bishop_c;
      }
      else if (b[i][j] == bishop_val) {
        cout << bishop;
      }
      else if (b[i][j] == pawn_val_c) {
        cout << pawn_c;
      }
      else if (b[i][j] == pawn_val) {
        cout << pawn;
      }
      else if (b[i][j] == 0) {
        cout << e;
      }
      else {
        cout << b[i][j];
      }
      cout << " ";
    }
    cout << endl;
  }
  cout << "  A B C D E F\n";
  //cout << "    0 1 2 3 4 5\n\n";
}

void setupMove(int mc, int sx, int sy, int ex, int ey) {
  moves.push_back(mData());
  moves[mc].sx = sx;
  moves[mc].sy = sy;
  moves[mc].ex = ex;
  moves[mc].ey = ey;
  //data->startpieceid = sp;
  //data->endpieceid = ep;
  //data->capturepieceid = cp;
}

void setupMoveComputer(int mc, int sx, int sy, int ex, int ey) {
  cmoves.push_back(mData());
  cmoves[mc].sx = sx;
  cmoves[mc].sy = sy;
  cmoves[mc].ex = ex;
  cmoves[mc].ey = ey;
  int spid = b[sx][sy];
  int epid;
  cmoves[mc].spid = spid;
  cmoves[mc].cpid = b[ex][ey];
  switch (spid) {
    case 2: epid = 4; break;
    case 3: epid = 2; break;
    case 4: epid = 3; break;
    case -2: epid = -4; break;
    case -3: epid = -2; break;
    case -4: epid = -3; break;
    default: epid = spid;
  }
  cmoves[mc].epid = epid;
}

void setupMoveMiniMax(int sx, int sy, int ex, int ey, int depth) {

  //before adding and setting up the move, check to see if the move is legal
  if (b[ex][ey] < 1) {
    int mc = minimaxMoves[depth].size();
    minimaxMoves[depth].push_back(mData());
    minimaxMoves[depth][mc].sx = sx;
    minimaxMoves[depth][mc].sy = sy;
    minimaxMoves[depth][mc].ex = ex;
    minimaxMoves[depth][mc].ey = ey;
    int spid = b[sx][sy];
    int epid;
    minimaxMoves[depth][mc].spid = spid;
    minimaxMoves[depth][mc].cpid = b[ex][ey];
    switch (spid) {
      case 2: epid = 4; break;
      case 3: epid = 2; break;
      case 4: epid = 3; break;
      case -2: epid = -4; break;
      case -3: epid = -2; break;
      case -4: epid = -3; break;
      default: epid = spid;
    }
    minimaxMoves[depth][mc].epid = epid;
  }
  //print moves?
  /*
  for (int i = 0; i < depth - 1; i++) {
    for (int j = 0; j < mc; j++) {
      cout << "moves: " << minimaxMoves[i][j].sy << minimaxMoves[i][j].sx
           << minimaxMoves[i][j].ey << minimaxMoves[i][j].ex << endl;
    }
  }*/
}

void setupCaptureMoveComputer(int mc, int sx, int sy, int ex, int ey) {
  cmoves.push_back(mData());
  cmoves[mc].sx = sx;
  cmoves[mc].sy = sy;
  cmoves[mc].ex = ex;
  cmoves[mc].ey = ey;
  int spid = b[sx][sy];
  int epid;
  cmoves[mc].spid = spid;
  cmoves[mc].cpid = b[ex][ey];
  switch (spid) {
    case 2: epid = 4; break;
    case 3: epid = 2; break;
    case 4: epid = 3; break;
    case -2: epid = -4; break;
    case -3: epid = -2; break;
    case -4: epid = -3; break;
    default: epid = spid;
  }
  cmoves[mc].epid = epid;
}

void printMove(int sx, int sy, int ex, int ey, char c) {

  char colFrom;
  char colTo;
  switch (sy) {
    case 0: colFrom = 'A'; break;
    case 1: colFrom = 'B'; break;
    case 2: colFrom = 'C'; break;
    case 3: colFrom = 'D'; break;
    case 4: colFrom = 'E'; break;
    case 5: colFrom = 'F'; break;
    default: break;
  }
  switch (ey) {
    case 0: colTo = 'A'; break;
    case 1: colTo = 'B'; break;
    case 2: colTo = 'C'; break;
    case 3: colTo = 'D'; break;
    case 4: colTo = 'E'; break;
    case 5: colTo = 'F'; break;
    default: break;
  }

  if (b[ex][ey] > -1) {
    cout << c << " " << colFrom << sx + 1
         << colTo << ex + 1;
          if (b[ex][ey] > 0) {
            cout << " Capture";
            captureHumanCount++;
          }
    cout << endl;
    //add move to array of possible moves
    setupMove(moveCount, sx, sy, ex, ey); // 0s are piece info for start, end and captures
    moveCount++;
  }
}

void printMoveComputer(int sx, int sy, int ex, int ey, char c) {

  char colFrom;
  char colTo;
  switch (sy) {
    case 0: colFrom = 'A'; break;
    case 1: colFrom = 'B'; break;
    case 2: colFrom = 'C'; break;
    case 3: colFrom = 'D'; break;
    case 4: colFrom = 'E'; break;
    case 5: colFrom = 'F'; break;
    default: break;
  }
  switch (ey) {
    case 0: colTo = 'A'; break;
    case 1: colTo = 'B'; break;
    case 2: colTo = 'C'; break;
    case 3: colTo = 'D'; break;
    case 4: colTo = 'E'; break;
    case 5: colTo = 'F'; break;
    default: break;
  }

  if (b[ex][ey] < 1) {
    cout << c << " " << colFrom << sx + 1
         << colTo << ex + 1;
          if (b[ex][ey] < 0) {
            cout << " Capture";
            //setupCaptureMoveComputer(captureMoveCount, sx, sy, ex, ey);
            //captureMoveCount++;
          }
    cout << endl;
    //add move to array of possible moves
    setupMoveComputer(moveCountComputer, sx, sy, ex, ey); // 0s are piece info for start, end and captures
    moveCountComputer++;
  }
}

void getHumanLegalMoves() {

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {

      if (b[i][j] == king_val) {
        //2 total moves
        //LEFT ONLY IF NO PIECE TO CAPTURE TOWARDS THE RIGHT

        //2 cross moves
        if ((j + 1) < cols) {
          if (b[i][j + 1] > 0) { //CHECK IF CAPTURE, CAN ONLY DO THIS ON CAPTURE
            printMove(i, j, i, j + 1, king);
          }
        }
        if ((j - 1) >= 0) {
          printMove(i, j, i, j - 1, king);
        }

      }

      else if (b[i][j] == knight_val) {

        // 8 total possible moves
        if (((i + 2) < rows) && ((j + 1) < cols)) {
          printMove(i, j, i + 2, j + 1, knight);
        }
        if (((i + 1) < rows) && ((j + 2) < cols)) {
          printMove(i, j, i + 1, j + 2, knight);
        }
        if (((i + 1) < rows) && ((j - 2) >= 0)) {
          printMove(i, j, i + 1, j - 2, knight);
        }
        if (((i + 2) < rows) && ((j - 1) >= 0)) {
          printMove(i, j, i + 2, j - 1, knight);
        }

        if (((i - 1) >= 0) && ((j + 2) < cols)) {
          if (b[i - 1][j + 2] > 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            printMove(i, j, i - 1, j + 2, knight);
          }
        }
        if (((i - 2) >= 0) && ((j + 1) < cols)) {
          if (b[i - 2][j + 1] > 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            printMove(i, j, i - 2, j + 1, knight);
          }
        }
        if (((i - 2) >= 0) && ((j - 1) >= 0)) {
          if (b[i - 2][j - 1] > 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            printMove(i, j, i - 2, j - 1, knight);
          }
        }
        if (((i - 1) >= 0) && ((j - 2) >= 0)) {
          if (b[i - 1][j - 2] > 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            printMove(i, j, i - 1, j - 2, knight);
          }
        }
      }

      else if (b[i][j] == rook_val) {
        //ROOK------------------------------------------------

        //N
        temp = i;
        while ((temp + 1 < rows) && (b[temp + 1][j] >= 0)) {
          printMove(i, j, temp + 1, j, rook);
          temp++;
          if (b[temp][j] != 0) {
            break;
          }
        }

        //E
        temp = j;
        while ((temp + 1 < cols) && (b[i][temp + 1] >= 0)) {
          printMove(i, j, i, temp + 1, rook);
          temp++;
          if (b[i][temp] != 0) {
            break;
          }
        }

        //S
        temp = i;
        while ((temp - 1 >= 0) && (b[temp - 1][j] >= 0)) {
          if (b[temp - 1][j] > 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            printMove(i, j, temp - 1, j, rook);
          }
          temp--;
          if (b[temp][j] != 0) {
            break;
          }
        }

        //W
        temp = j;
        while ((temp - 1 >= 0) && (b[i][temp - 1] >= 0)) {
          printMove(i, j, i, temp - 1, rook);
          temp--;
          if (b[i][temp] != 0) {
            break;
          }
        }
        //------------------------------------------------

      }

      else if (b[i][j] == bishop_val) {
        //BISHOP--------------------------------------------------------------------------

        //NE i+1 j+1
        temp = i;
        tempj = j;
        while ((temp + 1 < rows) && (tempj + 1 < cols) && (b[temp + 1][tempj + 1] >= 0)) {
          printMove(i, j, temp + 1, tempj + 1, bishop);
          temp++;
          tempj++;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //NW i+1 j-1
        temp = i;
        tempj = j;
        while ((temp + 1 < rows) && (tempj - 1 >= 0) && (b[temp + 1][tempj - 1] >= 0)) {
          printMove(i, j, temp + 1, tempj - 1, bishop);
          temp++;
          tempj--;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //SE i-1 j+1
        temp = i;
        tempj = j;
        while ((temp - 1 >= 0) && (tempj + 1 < cols) && (b[temp - 1][tempj + 1] >= 0)) {
          if (b[temp - 1][tempj + 1] > 0) { //CANT MOVE BACKWARDS UNLESS CAPTURE
            printMove(i, j, temp - 1, tempj + 1, bishop);
          }
          temp--;
          tempj++;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //SW i-1 j-1
        temp = i;
        tempj = j;
        while ((temp - 1 >= 0) && (tempj - 1 >= 0) && (b[temp - 1][tempj - 1] >= 0)) {
          if (b[temp - 1][tempj - 1] > 0) { //CANT MOVE BACKWARDS UNLESS CAPTURE
            printMove(i, j, temp - 1, tempj - 1, bishop);
          }
          temp--;
          tempj--;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //-------------------------------------------------------------------

      }

      else if (b[i][j] == pawn_val) {
        //PAWN-----------------------------------------
        //if space above is empty...
        if (((i + 1) < rows) && (b[i+1][j] == 0)) {
          printMove(i, j, i + 1, j, pawn);
        }

        //NE capture
        if (((i + 1) < rows) && ((j + 1) < cols) && (b[i+1][j+1] > 0)) {
          printMove(i, j, i + 1, j + 1, pawn);
        }

        //NW capture
        if (((i + 1) < rows) && ((j - 1) >= 0) && (b[i+1][j-1] > 0)) {
          printMove(i, j, i + 1, j - 1, pawn);
        }
        //------------------------------------------------------------
      }
      else {

      }
    }
  }
}

void getHumanLegalMovesDepth(int depth) {

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {

      if (b[i][j] == king_val) {
        //2 total moves
        //LEFT ONLY IF NO PIECE TO CAPTURE TOWARDS THE RIGHT

        //2 cross moves
        if ((j + 1) < cols) {
          if (b[i][j + 1] > 0) { //CHECK IF CAPTURE, CAN ONLY DO THIS ON CAPTURE
            setupMoveMiniMax(i, j, i, j + 1, depth);
          }
        }
        if ((j - 1) >= 0) {
          setupMoveMiniMax(i, j, i, j - 1, depth);
        }

      }

      else if (b[i][j] == knight_val) {

        // 8 total possible moves
        if (((i + 2) < rows) && ((j + 1) < cols)) {
          setupMoveMiniMax(i, j, i + 2, j + 1, depth);
        }
        if (((i + 1) < rows) && ((j + 2) < cols)) {
          setupMoveMiniMax(i, j, i + 1, j + 2, depth);
        }
        if (((i + 1) < rows) && ((j - 2) >= 0)) {
          setupMoveMiniMax(i, j, i + 1, j - 2, depth);
        }
        if (((i + 2) < rows) && ((j - 1) >= 0)) {
          setupMoveMiniMax(i, j, i + 2, j - 1, depth);
        }

        if (((i - 1) >= 0) && ((j + 2) < cols)) {
          if (b[i - 1][j + 2] > 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            setupMoveMiniMax(i, j, i - 1, j + 2, depth);
          }
        }
        if (((i - 2) >= 0) && ((j + 1) < cols)) {
          if (b[i - 2][j + 1] > 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            setupMoveMiniMax(i, j, i - 2, j + 1, depth);
          }
        }
        if (((i - 2) >= 0) && ((j - 1) >= 0)) {
          if (b[i - 2][j - 1] > 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            setupMoveMiniMax(i, j, i - 2, j - 1, depth);
          }
        }
        if (((i - 1) >= 0) && ((j - 2) >= 0)) {
          if (b[i - 1][j - 2] > 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            setupMoveMiniMax(i, j, i - 1, j - 2, depth);
          }
        }
      }

      else if (b[i][j] == rook_val) {
        //ROOK------------------------------------------------

        //N
        temp = i;
        while ((temp + 1 < rows) && (b[temp + 1][j] >= 0)) {
          setupMoveMiniMax(i, j, temp + 1, j, depth);
          temp++;
          if (b[temp][j] != 0) {
            break;
          }
        }

        //E
        temp = j;
        while ((temp + 1 < cols) && (b[i][temp + 1] >= 0)) {
          setupMoveMiniMax(i, j, i, temp + 1, depth);
          temp++;
          if (b[i][temp] != 0) {
            break;
          }
        }

        //S
        temp = i;
        while ((temp - 1 >= 0) && (b[temp - 1][j] >= 0)) {
          if (b[temp - 1][j] > 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            setupMoveMiniMax(i, j, temp - 1, j, depth);
          }
          temp--;
          if (b[temp][j] != 0) {
            break;
          }
        }

        //W
        temp = j;
        while ((temp - 1 >= 0) && (b[i][temp - 1] >= 0)) {
          setupMoveMiniMax(i, j, i, temp - 1, depth);
          temp--;
          if (b[i][temp] != 0) {
            break;
          }
        }
        //------------------------------------------------

      }

      else if (b[i][j] == bishop_val) {
        //BISHOP--------------------------------------------------------------------------

        //NE i+1 j+1
        temp = i;
        tempj = j;
        while ((temp + 1 < rows) && (tempj + 1 < cols) && (b[temp + 1][tempj + 1] >= 0)) {
          setupMoveMiniMax(i, j, temp + 1, tempj + 1, depth);
          temp++;
          tempj++;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //NW i+1 j-1
        temp = i;
        tempj = j;
        while ((temp + 1 < rows) && (tempj - 1 >= 0) && (b[temp + 1][tempj - 1] >= 0)) {
          setupMoveMiniMax(i, j, temp + 1, tempj - 1, depth);
          temp++;
          tempj--;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //SE i-1 j+1
        temp = i;
        tempj = j;
        while ((temp - 1 >= 0) && (tempj + 1 < cols) && (b[temp - 1][tempj + 1] >= 0)) {
          if (b[temp - 1][tempj + 1] > 0) { //CANT MOVE BACKWARDS UNLESS CAPTURE
            setupMoveMiniMax(i, j, temp - 1, tempj + 1, depth);
          }
          temp--;
          tempj++;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //SW i-1 j-1
        temp = i;
        tempj = j;
        while ((temp - 1 >= 0) && (tempj - 1 >= 0) && (b[temp - 1][tempj - 1] >= 0)) {
          if (b[temp - 1][tempj - 1] > 0) { //CANT MOVE BACKWARDS UNLESS CAPTURE
            setupMoveMiniMax(i, j, temp - 1, tempj - 1, depth);
          }
          temp--;
          tempj--;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //-------------------------------------------------------------------

      }

      else if (b[i][j] == pawn_val) {
        //PAWN-----------------------------------------
        //if space above is empty...
        if (((i + 1) < rows) && (b[i+1][j] == 0)) {
          setupMoveMiniMax(i, j, i + 1, j, depth);
        }

        //NE capture
        if (((i + 1) < rows) && ((j + 1) < cols) && (b[i+1][j+1] > 0)) {
          setupMoveMiniMax(i, j, i + 1, j + 1, depth);
        }

        //NW capture
        if (((i + 1) < rows) && ((j - 1) >= 0) && (b[i+1][j-1] > 0)) {
          setupMoveMiniMax(i, j, i + 1, j - 1, depth);
        }
        //------------------------------------------------------------
      }
      else {

      }
    }
  }
}

void printHumanLegalMoves() {
  cout << "\nComputer format: \n";
  for (int i = 0; i < moveCount; i++) {
    cout << moves[i].sx << moves[i].sy << moves[i].ex << moves[i].ey << " ";
  }
  cout << endl;
}

void printComputerLegalMoves() {
  for (int i = 0; i < moveCountComputer; i++) {
    cout << cmoves[i].sx << cmoves[i].sy << cmoves[i].ex << cmoves[i].ey << " ";
  }
  cout << endl;
}

void makeHumanMove(int sx, int sy, int ex, int ey) {
  if (moveCount == 0) {
    win = 1;
  }

  if (b[ex][ey] == king_val_c) { //check human win
    win = -1;
  }
  temp = b[sx][sy];
  b[sx][sy] = 0;

  if (temp == -2) {
    b[ex][ey] = -4;
  }
  else if (temp == -4) {
    b[ex][ey] = -3;
  }
  else if (temp == -3) {
    b[ex][ey] = -2;
  }
  else {
    b[ex][ey] = temp;
  }

  for (int i = 0; i < moveCount; i++) {
    moves.pop_back();
  }
  moveCount = 0;
  captureHumanCount = 0;
}

void checkLegalMove(int sx, int sy, int ex, int ey, int& legalMove) {

  for (int i = 0; i < moveCount; i++) {
    if ((moves[i].sx == sx) && (moves[i].sy == sy) && (moves[i].ex == ex) && (moves[i].ey == ey)) {
      legalMove = 1;
    }
  }

  if (legalMove != 1) {
    cout << "\nIllegal Move\n";
    cin.clear();
  }

}

/*int main() { //TIPS ON HOW TO GET VALID INPUT

    int x;
    std::cin >> x;
    while(std::cin.fail()) {
        std::cout << "Error" << std::endl;
        std::cin.clear();
        std::cin.ignore(256,'\n');
        std::cin >> x;
    }
    std::cout << x << std::endl;

    return 0;
}*/

void getAndMakeHumanMove() {
  int legalMove = 0;

  int sx;
  char sy;
  int asy; //actual sy
  int ex;
  char ey;
  int aey; //actual ey

  while (legalMove == 0) {
    cout << "\nInput Move: ";
    cin.clear();
    cin.ignore(256, '\n');
    cin >> sy >> sx >> ey >> ex;

    switch (sy) {
      case 'a': asy = 0; break;
      case 'b': asy = 1; break;
      case 'c': asy = 2; break;
      case 'd': asy = 3; break;
      case 'e': asy = 4; break;
      case 'f': asy = 5; break;
      case 'A': asy = 0; break;
      case 'B': asy = 1; break;
      case 'C': asy = 2; break;
      case 'D': asy = 3; break;
      case 'E': asy = 4; break;
      case 'F': asy = 5; break;
    }
    switch (ey) {
      case 'a': aey = 0; break;
      case 'b': aey = 1; break;
      case 'c': aey = 2; break;
      case 'd': aey = 3; break;
      case 'e': aey = 4; break;
      case 'f': aey = 5; break;
      case 'A': aey = 0; break;
      case 'B': aey = 1; break;
      case 'C': aey = 2; break;
      case 'D': aey = 3; break;
      case 'E': aey = 4; break;
      case 'F': aey = 5; break;
    }

    sx--; //COMPENSATE FOR ADDED ROW
    ex--;

    checkLegalMove(sx, asy, ex, aey, legalMove);
  }
  makeHumanMove(sx, asy, ex, aey);

}

void getComputerLegalMoves() {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {

      if (b[i][j] == king_val_c) {
        //2 total moves
        //RIGHT ONLY IF NO PIECE TO CAPTURE TOWARDS THE LEFT

        //2 cross moves
        if ((j + 1) < cols) {
            printMoveComputer(i, j, i, j + 1, king_c);
        }
        if ((j - 1) >= 0) {
          if (b[i][j - 1] < 0) { //CHECK IF CAPTURE, CAN ONLY DO THIS ON CAPTURE
            printMoveComputer(i, j, i, j - 1, king_c);
          }
        }

      }

      else if (b[i][j] == knight_val_c) {

        // 8 total possible moves
        if (((i + 2) < rows) && ((j + 1) < cols)) {
          if (b[i + 2][j + 1] < 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            printMoveComputer(i, j, i + 2, j + 1, knight_c);
          }
        }
        if (((i + 1) < rows) && ((j + 2) < cols)) {
          if (b[i + 1][j + 2] < 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            printMoveComputer(i, j, i + 1, j + 2, knight_c);
          }
        }
        if (((i + 1) < rows) && ((j - 2) >= 0)) {
          if (b[i + 1][j - 2] < 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            printMoveComputer(i, j, i + 1, j - 2, knight_c);
          }
        }
        if (((i + 2) < rows) && ((j - 1) >= 0)) {
          if (b[i + 2][j - 1] < 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            printMoveComputer(i, j, i + 2, j - 1, knight_c);
          }
        }

        if (((i - 1) >= 0) && ((j + 2) < cols)) {
            printMoveComputer(i, j, i - 1, j + 2, knight_c);
        }
        if (((i - 2) >= 0) && ((j + 1) < cols)) {
            printMoveComputer(i, j, i - 2, j + 1, knight_c);
        }
        if (((i - 2) >= 0) && ((j - 1) >= 0)) {
            printMoveComputer(i, j, i - 2, j - 1, knight_c);
        }
        if (((i - 1) >= 0) && ((j - 2) >= 0)) {
            printMoveComputer(i, j, i - 1, j - 2, knight_c);
        }

      }

      else if (b[i][j] == rook_val_c) {
        //ROOK------------------------------------------------

        //N
        temp = i;
        while ((temp + 1 < rows) && (b[temp + 1][j] <= 0)) {
          if (b[temp + 1][j] < 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            printMoveComputer(i, j, temp + 1, j, rook_c);
          }
          temp++;
          if (b[temp][j] != 0) {
            break;
          }
        }

        //E
        temp = j;
        while ((temp + 1 < cols) && (b[i][temp + 1] <= 0)) {
          printMoveComputer(i, j, i, temp + 1, rook_c);
          temp++;
          if (b[i][temp] != 0) {
            break;
          }
        }

        //S
        temp = i;
        while ((temp - 1 >= 0) && (b[temp - 1][j] <= 0)) {
          printMoveComputer(i, j, temp - 1, j, rook_c);
          temp--;
          if (b[temp][j] != 0) {
            break;
          }
        }

        //W
        temp = j;
        while ((temp - 1 >= 0) && (b[i][temp - 1] <= 0)) {
          printMoveComputer(i, j, i, temp - 1, rook_c);
          temp--;
          if (b[i][temp] != 0) {
            break;
          }
        }
        //------------------------------------------------

      }

      else if (b[i][j] == bishop_val_c) {
        //BISHOP--------------------------------------------------------------------------

        //NE i+1 j+1
        temp = i;
        tempj = j;
        while ((temp + 1 < rows) && (tempj + 1 < cols) && (b[temp + 1][tempj + 1] <= 0)) {
          if (b[temp + 1][tempj + 1] < 0) { //CANT MOVE BACKWARDS UNLESS CAPTURE
            printMoveComputer(i, j, temp + 1, tempj + 1, bishop_c);
          }
          temp++;
          tempj++;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //NW i+1 j-1
        temp = i;
        tempj = j;
        while ((temp + 1 < rows) && (tempj - 1 >= 0) && (b[temp + 1][tempj - 1] <= 0)) {
          if (b[temp + 1][tempj - 1] < 0) { //CANT MOVE BACKWARDS UNLESS CAPTURE
            printMoveComputer(i, j, temp + 1, tempj - 1, bishop_c);
          }
          temp++;
          tempj--;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //SE i-1 j+1
        temp = i;
        tempj = j;
        while ((temp - 1 >= 0) && (tempj + 1 < cols) && (b[temp - 1][tempj + 1] <= 0)) {

          printMoveComputer(i, j, temp - 1, tempj + 1, bishop_c);

          temp--;
          tempj++;
          if (b[temp][tempj] != 0) {
            break;
          }

        }

        //SW i-1 j-1
        temp = i;
        tempj = j;
        while ((temp - 1 >= 0) && (tempj - 1 >= 0) && (b[temp - 1][tempj - 1] <= 0)) {
          printMoveComputer(i, j, temp - 1, tempj - 1, bishop_c);

          temp--;
          tempj--;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //-------------------------------------------------------------------

      }

      else if (b[i][j] == pawn_val_c) {

        //PAWN-----------------------------------------
        //if space below is empty...
        if (((i - 1) >= 0) && (b[i-1][j] == 0)) {
          printMoveComputer(i, j, i - 1, j, pawn_c);
        }

        //SE capture
        if (((i - 1) < rows) && ((j + 1) < cols) && (b[i-1][j+1] < 0)) {
          printMoveComputer(i, j, i - 1, j + 1, pawn_c);
        }

        //SW capture
        if (((i - 1) < rows) && ((j - 1) >= 0) && (b[i-1][j-1] < 0)) {
          printMoveComputer(i, j, i - 1, j - 1, pawn_c);
        }
        //--------------------------------------------------------------------

      }
      else {

      }
    }
  }
}

void getComputerLegalMovesDepth(int depth) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {

      if (b[i][j] == king_val_c) {
        //2 total moves
        //RIGHT ONLY IF NO PIECE TO CAPTURE TOWARDS THE LEFT

        //2 cross moves
        if ((j + 1) < cols) {
            setupMoveMiniMax(i, j, i, j + 1, depth);
        }
        if ((j - 1) >= 0) {
          if (b[i][j - 1] < 0) { //CHECK IF CAPTURE, CAN ONLY DO THIS ON CAPTURE
            setupMoveMiniMax(i, j, i, j - 1, depth);
          }
        }

      }

      else if (b[i][j] == knight_val_c) {

        // 8 total possible moves
        if (((i + 2) < rows) && ((j + 1) < cols)) {
          if (b[i + 2][j + 1] < 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            setupMoveMiniMax(i, j, i + 2, j + 1, depth);
          }
        }
        if (((i + 1) < rows) && ((j + 2) < cols)) {
          if (b[i + 1][j + 2] < 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            setupMoveMiniMax(i, j, i + 1, j + 2, depth);
          }
        }
        if (((i + 1) < rows) && ((j - 2) >= 0)) {
          if (b[i + 1][j - 2] < 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            setupMoveMiniMax(i, j, i + 1, j - 2, depth);
          }
        }
        if (((i + 2) < rows) && ((j - 1) >= 0)) {
          if (b[i + 2][j - 1] < 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            setupMoveMiniMax(i, j, i + 2, j - 1, depth);
          }
        }

        if (((i - 1) >= 0) && ((j + 2) < cols)) {
            setupMoveMiniMax(i, j, i - 1, j + 2, depth);
        }
        if (((i - 2) >= 0) && ((j + 1) < cols)) {
            setupMoveMiniMax(i, j, i - 2, j + 1, depth);
        }
        if (((i - 2) >= 0) && ((j - 1) >= 0)) {
            setupMoveMiniMax(i, j, i - 2, j - 1, depth);
        }
        if (((i - 1) >= 0) && ((j - 2) >= 0)) {
            setupMoveMiniMax(i, j, i - 1, j - 2, depth);
        }

      }

      else if (b[i][j] == rook_val_c) {
        //ROOK------------------------------------------------

        //N
        temp = i;
        while ((temp + 1 < rows) && (b[temp + 1][j] <= 0)) {
          if (b[temp + 1][j] < 0) { //ONLY CAN MOVE BACKWARDS ON CAPTURE
            setupMoveMiniMax(i, j, temp + 1, j, depth);
          }
          temp++;
          if (b[temp][j] != 0) {
            break;
          }
        }

        //E
        temp = j;
        while ((temp + 1 < cols) && (b[i][temp + 1] <= 0)) {
          setupMoveMiniMax(i, j, i, temp + 1, depth);
          temp++;
          if (b[i][temp] != 0) {
            break;
          }
        }

        //S
        temp = i;
        while ((temp - 1 >= 0) && (b[temp - 1][j] <= 0)) {
          setupMoveMiniMax(i, j, temp - 1, j, depth);
          temp--;
          if (b[temp][j] != 0) {
            break;
          }
        }

        //W
        temp = j;
        while ((temp - 1 >= 0) && (b[i][temp - 1] <= 0)) {
          setupMoveMiniMax(i, j, i, temp - 1, depth);
          temp--;
          if (b[i][temp] != 0) {
            break;
          }
        }
        //------------------------------------------------

      }

      else if (b[i][j] == bishop_val_c) {
        //BISHOP--------------------------------------------------------------------------

        //NE i+1 j+1
        temp = i;
        tempj = j;
        while ((temp + 1 < rows) && (tempj + 1 < cols) && (b[temp + 1][tempj + 1] <= 0)) {
          if (b[temp + 1][tempj + 1] < 0) { //CANT MOVE BACKWARDS UNLESS CAPTURE
            setupMoveMiniMax(i, j, temp + 1, tempj + 1, depth);
          }
          temp++;
          tempj++;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //NW i+1 j-1
        temp = i;
        tempj = j;
        while ((temp + 1 < rows) && (tempj - 1 >= 0) && (b[temp + 1][tempj - 1] <= 0)) {
          if (b[temp + 1][tempj - 1] < 0) { //CANT MOVE BACKWARDS UNLESS CAPTURE
            setupMoveMiniMax(i, j, temp + 1, tempj - 1, depth);
          }
          temp++;
          tempj--;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //SE i-1 j+1
        temp = i;
        tempj = j;
        while ((temp - 1 >= 0) && (tempj + 1 < cols) && (b[temp - 1][tempj + 1] <= 0)) {

          setupMoveMiniMax(i, j, temp - 1, tempj + 1, depth);

          temp--;
          tempj++;
          if (b[temp][tempj] != 0) {
            break;
          }

        }

        //SW i-1 j-1
        temp = i;
        tempj = j;
        while ((temp - 1 >= 0) && (tempj - 1 >= 0) && (b[temp - 1][tempj - 1] <= 0)) {
          setupMoveMiniMax(i, j, temp - 1, tempj - 1, depth);

          temp--;
          tempj--;
          if (b[temp][tempj] != 0) {
            break;
          }
        }

        //-------------------------------------------------------------------

      }

      else if (b[i][j] == pawn_val_c) {

        //PAWN-----------------------------------------
        //if space below is empty...
        if (((i - 1) >= 0) && (b[i-1][j] == 0)) {
          setupMoveMiniMax(i, j, i - 1, j, depth);
        }

        //SE capture
        if (((i - 1) < rows) && ((j + 1) < cols) && (b[i-1][j+1] < 0)) {
          setupMoveMiniMax(i, j, i - 1, j + 1, depth);
        }

        //SW capture
        if (((i - 1) < rows) && ((j - 1) >= 0) && (b[i-1][j-1] < 0)) {
          setupMoveMiniMax(i, j, i - 1, j - 1, depth);
        }
        //--------------------------------------------------------------------

      }
      else {

      }
    }
  }
}

void makeComputerMove(int sx, int sy, int ex, int ey) {
  if (moveCountComputer == 0) {
    win = -1;
  }

  if (b[ex][ey] == king_val) {
    win = 1;
  }

  temp = b[sx][sy];
  b[sx][sy] = 0;

  if (temp == 2) {
    b[ex][ey] = 4;
  }
  else if (temp == 4) {
    b[ex][ey] = 3;
  }
  else if (temp == 3) {
    b[ex][ey] = 2;
  }
  else {
    b[ex][ey] = temp;
  }

  cout << "\n made computer move: \n" << sx << sy << ex << ey << endl;

  if (moveCountComputer > 0) {
    for (int i = 0; i < moveCountComputer; i++) {
      cmoves.pop_back();
    }
    moveCountComputer = 0;
  }

  if (captureMoveCount > 0) {
    for (int i = 0; i < captureMoveCount; i++) {
      captureMoves.pop_back();
    }
    captureMoveCount = 0;
  }



}

void applyMove(mData md) {
  b[md.sx][md.sy] = 0;
  b[md.ex][md.ey] = md.epid;
}

void convertMoves(int sy, int sx, int ey, int ex) {
  char fromCol;
  char toCol;
  int fromRow;
  int toRow;

  switch (sy) {
    case 0: fromCol = 'F'; break;
    case 1: fromCol = 'E'; break;
    case 2: fromCol = 'D'; break;
    case 3: fromCol = 'C'; break;
    case 4: fromCol = 'B'; break;
    case 5: fromCol = 'A'; break;
  }

  switch (ey) {
    case 0: toCol = 'F'; break;
    case 1: toCol = 'E'; break;
    case 2: toCol = 'D'; break;
    case 3: toCol = 'C'; break;
    case 4: toCol = 'B'; break;
    case 5: toCol = 'A'; break;
  }

  switch (sx) {
    case 0: fromRow = 8; break;
    case 1: fromRow = 7; break;
    case 2: fromRow = 6; break;
    case 3: fromRow = 5; break;
    case 4: fromRow = 4; break;
    case 5: fromRow = 3; break;
    case 6: fromRow = 2; break;
    case 7: fromRow = 1; break;
  }

  switch (ex) {
    case 0: toRow = 8; break;
    case 1: toRow = 7; break;
    case 2: toRow = 6; break;
    case 3: toRow = 5; break;
    case 4: toRow = 4; break;
    case 5: toRow = 3; break;
    case 6: toRow = 2; break;
    case 7: toRow = 1; break;
  }

  cout << "\nTranslation: " << fromCol << fromRow << toCol << toRow << "\n\n";

}

void printComputerMoveWithTranslation(mData md) {
  char fromCol;
  char toCol;

  switch (md.sy) {
    case 0: fromCol = 'A'; break;
    case 1: fromCol = 'B'; break;
    case 2: fromCol = 'C'; break;
    case 3: fromCol = 'D'; break;
    case 4: fromCol = 'E'; break;
    case 5: fromCol = 'F'; break;
  }

  switch (md.ey) {
    case 0: toCol = 'A'; break;
    case 1: toCol = 'B'; break;
    case 2: toCol = 'C'; break;
    case 3: toCol = 'D'; break;
    case 4: toCol = 'E'; break;
    case 5: toCol = 'F'; break;
  }

  cout << "\nComputer Makes Move: " << fromCol << md.sx + 1 << toCol << md.ex + 1;

  convertMoves(md.sy, md.sx, md.ey, md.ex); //THESE ARE THE CONVERTED MOVES TO TELL THE OTHER PLAYER


}

void retractMove(mData md) {
  b[md.sx][md.sy] = md.spid;
  b[md.ex][md.ey] = md.cpid;
}

int eval() {
  int total = 0;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (b[i][j] == -1) {
        total -= 6000;
      }
      else if (b[i][j] == -2) {
        total -= 20;
      }
      else if (b[i][j] == -3) {
        total -= 20;
      }
      else if (b[i][j] == -4) {
        total -= 20;
      }
      else if (b[i][j] == -5) {
        total -= 5;
      }
      else if (b[i][j] == 1) {
        total += 6000;
      }
      else if (b[i][j] == 2) {
        total += 20;
      }
      else if (b[i][j] == 3) {
        total += 20;
      }
      else if (b[i][j] == 4) {
        total += 20;
      }
      else if (b[i][j] == 5) {
        total += 5;
      }
    }
  }
  movesLooked++;

  total -= moveCount;
  total -= captureHumanCount*2;

  total += moveCountComputer;
  total += captureMoveCount*2;

  return total;

}

int minimax_max(int depth); //PROTOTYPE


int minimax_min(int depth) {
  int min = 9999;
  int result;

  if (win == -1) {
    return -1000 - depth;
  }
  else if (win == 1) {
    return 1000 + depth;
  }
  else if (depth == 0) {
    return eval();
  }

  minimaxMoves[depth].clear();

  getHumanLegalMovesDepth(depth);

  for (int i = 0; i < minimaxMoves[depth].size(); i++) {
    applyMove(minimaxMoves[depth][i]);
    result = minimax_max(depth - 1);
    if (result < min) {
      min = result;
    }
    retractMove(minimaxMoves[depth][i]);
  }
  return min;
}

int minimax_max(int depth) {
  int max = -9999;
  int result;

  if (win == -1) {
    return -1000 - depth;
  }
  else if (win == 1) {
    return 1000 + depth;
  }
  else if (depth == 0) {
    return eval();
  }

  minimaxMoves[depth].clear();

  getComputerLegalMovesDepth(depth);

  for (int i = 0; i < minimaxMoves[depth].size(); i++) {
    applyMove(minimaxMoves[depth][i]);
    result = minimax_max(depth - 1);
    if (result > max) {
      max = result;
    }
    retractMove(minimaxMoves[depth][i]);
  }
  return max;
}

void makeRandomMoveComputer() {
  int r = 0;
  int p = 0;
  srand(time(NULL));
  r = rand() % moveCountComputer;
  //r2 = rand() % captureMoveCount;
  int sx = cmoves[r].sx;
  int sy = cmoves[r].sy;
  int ex = cmoves[r].ex;
  int ey = cmoves[r].ey;

  if (captureMoveCount == 0) {
    makeComputerMove(sx, sy, ex, ey);
  }
  else {
    //if (captureMoveCount > 1) {
      //for (int i = 0; i < captureMoveCount; i++) {

      //}
    //}
    sx = captureMoves[0].sx;
    sy = captureMoves[0].sy;
    ex = captureMoves[0].ex;
    ey = captureMoves[0].ey;
    makeComputerMove(sx, sy, ex, ey);
  }

}

void selectComputerMove() {

  if (moveCountComputer > 1) {
    int max = -9999;
    int bestMove;
    int mc; //move counter
    int result;
    minimaxMoves[maxdepth + 1].clear();
    getComputerLegalMovesDepth(maxdepth + 1);

    for (int i = 0; i < minimaxMoves[maxdepth + 1].size(); i++) {
      applyMove(minimaxMoves[maxdepth + 1][i]);
      //cout << "\n after apply move \n";
      result = minimax_min(maxdepth);
      //cout << "\n after minimax_min\n";
      if (result > max) {
        max = result;
        bestMove = i;
      }

      retractMove(minimaxMoves[maxdepth + 1][i]);
      //cout << "\n after retractMove\n";
    }
    applyMove(minimaxMoves[maxdepth + 1][bestMove]);
    cout << "\nMoves Considered: " << movesLooked << "\n";
    movesLooked = 0;
    printComputerMoveWithTranslation(minimaxMoves[maxdepth + 1][bestMove]);
  }
  else if (moveCountComputer == 1) {
    applyMove(moves[0]);
  }
  else {
    win = -1;
  }

  //CHECK IF WON
  for (int z = 0; z < rows; z++) {
    for (int x = 0; x < cols; x++) {
      if (b[z][x] == -1) {
        humanKingFound++;
      }
    }
  }
  if (humanKingFound == 0) {
    win = 1;
  }
  humanKingFound = 0;

}

int main() {
  setup();
  cout << "\n0: Computer goes first\n";
  cout << "1: Human goes first\n";

  cin >> firstPlayer;

  if (firstPlayer == 0) {
    setup();
    for (;;) {
      getComputerLegalMoves();
      //printComputerLegalMoves();
      //makeRandomMoveComputer();
      if (moveCountComputer == 0) {
        win = -1;
      }
      else {
        selectComputerMove();
      }
      if (win == 1) {
        printBoard();
        cout << "\nYOU LOSE\n";
        break;
      }
      printBoard();
      getHumanLegalMoves();
      //printHumanLegalMoves();
      if (moveCount == 0) {
        win = 1;
      }
      else {
        getAndMakeHumanMove();
      }
      if (win == -1) {
        printBoard();
        cout << "\nYOU WIN\n";
        break;
      }
      printBoard();
    }
    return 0;
  }
  else if (firstPlayer == 1) {
    for (;;) {
      printBoard();
      getHumanLegalMoves();
      //printHumanLegalMoves();
      getAndMakeHumanMove();
      if (win == -1) {
        printBoard();
        cout << "\nYOU WIN\n";
        break;
      }
      printBoard();
      getComputerLegalMoves();
      //printComputerLegalMoves();
      //makeRandomMoveComputer();
      selectComputerMove();
      if (win == 1) {
        printBoard();
        cout << "\nYOU LOSE\n";
        break;
      }
    }
  }
  else {
    cout << "Invalid Input\n";
  }
  return 0;
}

/* TeacherDocs!
int teacherDocs() {
The initial position is:

  8   - K - - - -  (COMPUTER)
  7   N B R R B N
  6   - - P P - -
  5   - - - - - -
  4   - - - - - -
  3   - - p p - -
  2   n b r r b n
  1   - - - - k -  (HUMAN)
      A B C D E F

   K,R,B,N,P are the computer's King, Rooks, Bishops, Knights, and Pawns
      (moving DOWN the board).

   k,r,b,n,p are the human's King, Rooks, Bishops, Knights, and Pawns
      (moving UP the board).
}
*/
