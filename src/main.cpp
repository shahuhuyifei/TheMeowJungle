#include "main.h"

player this_player;     // Store the current player's information
gameBoard boardPlaying; // Store the current gameboard information

// Convert uid to a string
String uidToString()
{
  String outputStringUid;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    outputStringUid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    outputStringUid.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  outputStringUid.toUpperCase();
  outputStringUid = outputStringUid.substring(1);
  return outputStringUid;
}

void printGameBoard()
{
  for (int i = 0; i < NUM_SPOTS; i++)
  {
    Serial.print(boardPlaying.level[i]);
    Serial.print(" ");
  }
  Serial.println();
  for (int i = 0; i < NUM_SPOTS; i++)
  {
    Serial.print(boardPlaying.food[i]);
    Serial.print(" ");
  }
  Serial.println();
  for (int i = 0; i < NUM_SPOTS; i++)
  {
    Serial.print(boardPlaying.totalFoodAmount[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println(boardPlaying.myTurn);
  Serial.println(boardPlaying.endGame);
}

// Identify the player and initialize the gameboard at player A
void identifyPlayer(String playerIdentifier)
{
  if (playerIdentifier = "A")
  {
    this_player = player_A;
    // Generate initial levels
    int initialLevel[NUM_SPOTS];
    for (int i = 0; i < NUM_SPOTS; i++)
    {
      initialLevel[i] = random(1, ALL_LEVELS + 1);
    }
    // Generate initial food types
    int initialFood[NUM_SPOTS];
    for (int i = 0; i < NUM_SPOTS; i++)
    {
      initialFood[i] = random(FOOD_TYPE);
    }
    // Adjust total amount of each type of food
    for (int i = 0; i < NUM_SPOTS; i++)
    {
      initialFoodAmount[initialFood[i]] -= 1;
    }
    boardPlaying.initValues(initialLevel, initialFood, initialFoodAmount, 1, 0);
  }
  else if (playerIdentifier = "B")
  {
    this_player = player_B;
  }
  else if (playerIdentifier = "C")
  {
    this_player = player_C;
  }
  else
  {
    Serial.println("Unknown player");
  }
}

void setup()
{
  Serial.begin(9600); // Initialize serial communications

  SPI.begin();                       // Init SPI bus
  mfrc522.PCD_Init();                // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details

  identifyPlayer(CURRENT_PLAYER); // Identify the player. Can be A, B or C.

  // Init ESPNow communication between two boards
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
  WiFi.disconnect();
  ESPNow.init();
  ESPNow.add_peer(this_player.peer_mac);
  // ESPNow.reg_recv_cb(onRecv);

  printGameBoard();
}

void loop()
{
  if (boardPlaying.myTurn == 1)
  {
    String action;
    while (action == NULL)
    {
      String cardRead;
      // Look for new cards
      if (!mfrc522.PICC_IsNewCardPresent())
      {
        continue;
      }
      // Select one of the cards
      if (!mfrc522.PICC_ReadCardSerial())
      {
        continue;
      }
      cardRead = uidToString();
      if (cardRead == this_player.detect_uid || cardRead == this_player.dig_uid || cardRead == this_player.collect_uid)
      {
        action = cardRead;
      }
    }
    Serial.println(action);
    // Detect
    if (uidToString() == this_player.detect_uid)
    {
      while (true)
      {
        // Look for new cards
        String detectingSpot;
        if (!mfrc522.PICC_IsNewCardPresent())
        {
          continue;
        }
        // Select one of the cards
        if (!mfrc522.PICC_ReadCardSerial())
        {
          continue;
        }
        detectingSpot = uidToString();
        for (int i = 0; i < NUM_SPOTS; i++)
        {
          if (detectingSpot == this_player.board[i])
          {
            Serial.println(boardPlaying.level[i]);
          }
        }
      }
    }
  }
}