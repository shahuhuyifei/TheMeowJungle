#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include "ESPNowW.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// Define pins for RFID reader
#define RST_PIN 25
#define SS_PIN 26

// Declaration of the RFID reader
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Declaration of the LED matrix
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

#define NUM_SPOTS 9        // Total number of food spots on the map
#define MAX_DIG 15         // Maximum number of digging action a player can perform
#define FOOD_TYPE 9        // Total number of food types
#define ALL_LEVELS 3       // Total number of levels of each spot
#define CURRENT_PLAYER "A" // Define a constant to identify the current player

// Create a struct for all players to store their information
struct player
{
  String board[NUM_SPOTS]; // uid for food spots
  String detect_uid;       // uid for Detect Action Card
  String dig_uid;          // uid for the Dig Action Cad
  String collect_uid;      // uid for Collecting Digging Strength Card
  String player_A_uid;     // uid for the card repersenting player A
  String player_B_uid;     // uid for the card repersenting player B
  String player_C_uid;     // uid for the card repersenting player C
  int myFood[MAX_DIG];     // Store the food that a player owns
  int digStrength;
  int cucumber;                    // Store the number of cucumbers a player owns
  int digAmount;                   // Amount of digging actions used
  int chanceToKnowType[FOOD_TYPE]; // The chance to know the food type by rolling a digital dice after detect
  uint8_t my_mac[6];               // mac address of the current player
  uint8_t peer_mac[6];             // mac address of the next player
  int myTurn;                      // 0 - not this player's turn, 1 - this player's turn
};

// Create a struct for the game board information to share between players
struct gameBoard
{
  int level[NUM_SPOTS];           // Depth of food at each spot
  int food[NUM_SPOTS];            // Type of food at each spot
  int totalFoodAmount[FOOD_TYPE]; // Amount of each type of food left in the pool
  int endGame;                    // 0 - game is not ending, 1 - game is ending after one more round
  int playerCucumber[3];          // Cucumbers each player own. 0 - Play A, 1 - Player B, 2 - Player C

  // A function to initialise the values of a gameboard
  void initValues(int *levels, int *foods, int *totalFoodAmounts, int initEndGame, int *initPlayerCucumber)
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
    endGame = initEndGame;
    for (int i = 0; i < 3; i++)
    {
      playerCucumber[i] = initPlayerCucumber[i];
    }
  }
};

// Create initial properties for each player
player player_A = {{"63 3A 51 42", "73 32 5B 42", "E3 BD 5A 42", "D3 A9 51 42", "D3 AF 51 42", "23 1E 52 42", "73 87 52 42", "93 EB 52 42", "93 F0 52 42"},
                   "00 7B 29 4A",
                   "10 7B 29 4A",
                   "20 7B 29 4A",
                   "",
                   "30 7B 29 4A",
                   "40 7B 29 4A",
                   {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
                   0,
                   0,
                   0,
                   {1, 1, 1, 1, 1, 1, 1, 1, 1},
                   {0xE8, 0x31, 0xCD, 0x63, 0x5F, 0xD0},
                   {0x3C, 0x61, 0x05, 0x4B, 0x05, 0x6C},
                   1};

player player_B = {{"F3 B8 5A 42", "33 3C 5A 42", "63 B9 59 42", "A3 37 59 42", "53 58 53 42", "93 D2 53 42", "B3 31 59 42", "63 B2 58 42", "43 32 58 42"},
                   "F0 79 29 4A",
                   "00 7A 29 4A",
                   "10 7A 29 4A",
                   "20 7A 29 4A",
                   "",
                   "30 7A 29 4A",
                   {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
                   0,
                   0,
                   0,
                   {1, 1, 1, 1, 1, 1, 1, 1, 1},
                   {0x3C, 0x61, 0x05, 0x4B, 0x05, 0x6C},
                   {0x94, 0xB5, 0x55, 0x6B, 0x25, 0x48},
                   0};

player player_C = {{"E3 41 54 42", "E3 47 54 42", "83 C8 54 42", "C3 39 55 42", "A3 BC 55 42", "A3 C2 55 42", "C3 35 56 42", "93 B3 56 42", "93 B9 56 42"},
                   "40 7A 29 4A",
                   "50 7A 29 4A",
                   "60 7A 29 4A",
                   "70 7A 29 4A",
                   "80 7A 29 4A",
                   "",
                   {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
                   0,
                   0,
                   0,
                   {1, 1, 1, 1, 1, 1, 1, 1, 1},
                   {0x94, 0xB5, 0x55, 0x6B, 0x25, 0x48},
                   {0xE8, 0x31, 0xCD, 0x63, 0x5F, 0xD0},
                   0};

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
int initialFoodAmount[FOOD_TYPE] = {3, 5, 6, 6, 6, 7, 7, 8, 6};
String foodName[FOOD_TYPE] = {"Chicken Dry Food", "Canned Chicken", "Freeze-dried Chicken", "Chicken Creamy Treat",
                              "Canned Beef", "Beef Dry Food", "Seafood Dry Food", "Canned Seafood", "Cucumber"};

// LED Matrix graphics
static const uint8_t PROGMEM
    smile_bmp[] =
        {B00111100,
         B01000010,
         B10100101,
         B10000001,
         B10100101,
         B10011001,
         B01000010,
         B00111100},
    neutral_bmp[] =
        {B00111100,
         B01000010,
         B10100101,
         B10000001,
         B10111101,
         B10000001,
         B01000010,
         B00111100},
    frown_bmp[] =
        {B00111100,
         B01000010,
         B10100101,
         B10000001,
         B10011001,
         B10100101,
         B01000010,
         B00111100};

#endif