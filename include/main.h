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
struct player
{
  String board[NUM_SPOTS];
  String detect_uid;
  String dig_uid;
  String collect_uid;
  String player_A_uid;
  String player_B_uid;
  String player_C_uid;
};
// 3 * Chicken Flavour Dry Food
// 5 * Canned Chicken
// 6 * Freeze-dried Chicken
// 6 * Chicken Flavour Creamy Treat
// 6 * Canned Beef
// 7 * Beef Flavour Dry Food
// 7 * Seafood Flavour Dry Food
// 8 * Canned Seafood
// 6 * Cucumber

struct board
{
  int Level[NUM_SPOTS];
};

// Define a constant to identify the current player
#define CURRENT_PLAYER "A"

player player_A = {{"63 3A 51 42", "73 32 5B 42", "E3 BD 5A 42", "D3 A9 51 42", "D3 AF 51 42", "23 1E 52 42", "73 87 52 42", "93 EB 52 42", "93 F0 52 42"},
                   "00 7B 29 4A",
                   "10 7B 29 4A",
                   "20 7B 29 4A",
                   "",
                   "30 7B 29 4A",
                   "40 7B 29 4A"};
player player_B = {{"F3 B8 5A 42", "33 3C 5A 42", "63 B9 59 42", "A3 37 59 42", "53 58 53 42", "93 D2 53 42", "B3 31 59 42", "63 B2 58 42", "43 32 58 42"},
                   "F0 79 29 4A",
                   "00 7A 29 4A",
                   "10 7A 29 4A",
                   "20 7A 29 4A",
                   "",
                   "30 7A 29 4A"};
player player_C = {{"E3 41 54 42", "E3 47 54 42", "83 C8 54 42", "C3 39 55 42", "A3 BC 55 42", "A3 C2 55 42", "C3 35 56 42", "93 B3 56 42", "93 B9 56 42"},
                   "40 7A 29 4A",
                   "50 7A 29 4A",
                   "60 7A 29 4A",
                   "80 7A 29 4A",
                   ""};

#endif