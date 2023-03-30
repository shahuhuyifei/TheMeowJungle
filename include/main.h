#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Define pins for RFID reader
#define RST_PIN 25
#define SS_PIN 26

// Declaration of the RFID reader
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Create a struct for all players to store their information
#define NUM_SPOTS 9
#define MAX_DIG 15
#define FOOD_TYPE 9
#define ALL_LEVELS 3

struct player
{
  String board[NUM_SPOTS];
  String detect_uid;
  String dig_uid;
  String collect_uid;
  String player_A_uid;
  String player_B_uid;
  String player_C_uid;
  int myFood[MAX_DIG];
  int digAmount;
};

/*
  Code and amount for food treasures
  ------------------------------------------------
  |           NAME               | CODE | AMOUNT |
  | Chicken Flavour Dry Food     |   0  |    3   |
  | Canned Chicken               |   1  |    5   |
  | Freeze-dried Chicken         |   2  |    6   |
  | Chicken Flavour Creamy Treat |   3  |    6   |
  | Canned Beef                  |   4  |    6   |
  | Beef Flavour Dry Food        |   5  |    7   |
  | Seafood Flavour Dry Food     |   6  |    7   |
  | Canned Seafood               |   7  |    8   |
  | Cucumber                     |   8  |    6   |
  ------------------------------------------------
*/

struct gameBoard
{
  int level[NUM_SPOTS];
  int food[NUM_SPOTS];
  int totalFoodAmount[FOOD_TYPE];
  int myTurn;  // 0 means it is not this player's turn, 1 means it is this player's turn
  int endGame; // 0 means the game is not ending, 1 means the game is ending after one more round

  void initValues(int *levels, int *foods, int *totalFoodAmounts, int initMyTurn, int initEndGame)
  {
    for (int i = 0; i < NUM_SPOTS; i++)
    {
      level[i] = levels[i];
      food[i] = foods[i];
    }
    for (int i = 0; i < FOOD_TYPE; i++)
    {
      totalFoodAmount[i] = totalFoodAmounts[i];
    }
    myTurn = initMyTurn;
    endGame = initEndGame;
  }
};

// Define a constant to identify the current player
#define CURRENT_PLAYER "A"

player player_A = {{"63 3A 51 42", "73 32 5B 42", "E3 BD 5A 42", "D3 A9 51 42", "D3 AF 51 42", "23 1E 52 42", "73 87 52 42", "93 EB 52 42", "93 F0 52 42"},
                   "00 7B 29 4A",
                   "10 7B 29 4A",
                   "20 7B 29 4A",
                   "",
                   "30 7B 29 4A",
                   "40 7B 29 4A",
                   {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
                   MAX_DIG};

player player_B = {{"F3 B8 5A 42", "33 3C 5A 42", "63 B9 59 42", "A3 37 59 42", "53 58 53 42", "93 D2 53 42", "B3 31 59 42", "63 B2 58 42", "43 32 58 42"},
                   "F0 79 29 4A",
                   "00 7A 29 4A",
                   "10 7A 29 4A",
                   "20 7A 29 4A",
                   "",
                   "30 7A 29 4A",
                   {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
                   MAX_DIG};

player player_C = {{"E3 41 54 42", "E3 47 54 42", "83 C8 54 42", "C3 39 55 42", "A3 BC 55 42", "A3 C2 55 42", "C3 35 56 42", "93 B3 56 42", "93 B9 56 42"},
                   "40 7A 29 4A",
                   "50 7A 29 4A",
                   "60 7A 29 4A",
                   "70 7A 29 4A",
                   "80 7A 29 4A",
                   "",
                   {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
                   MAX_DIG};

int initialFoodAmount[FOOD_TYPE] = {3, 5, 6, 6, 6, 7, 7, 8, 6};
#endif