# Morphe Engine

Morphe is a modified version of chess.

Morphe Engine was built in C++.

# Play Morphe

```
./Scyther.bin
```
# More Details

```
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

   Each time a player moves, their piece "morphes" into a different piece.

   The ordering of this morphe is Knight -> Rook -> Bishop -> Knight -> Rook -> Bishop ... etc

   The computer only thinks about four moves deep, have fun and try to win! 
    
   If you want more of a challenge, remove showing where captures are.
```
