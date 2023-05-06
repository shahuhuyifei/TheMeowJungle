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

// Print the gameboard information for debugging
void printGameBoard()
{
  Serial.print("Levels: ");
  for (int i = 0; i < NUM_SPOTS; i++)
  {
    Serial.print(boardPlaying.level[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Foods: ");
  for (int i = 0; i < NUM_SPOTS; i++)
  {
    Serial.print(boardPlaying.food[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Total Food Amount: ");
  for (int i = 0; i < NUM_SPOTS; i++)
  {
    Serial.print(boardPlaying.totalFoodAmount[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Myturn: ");
  Serial.println(boardPlaying.myTurn);
  Serial.print("endGame: ");
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
    // Generate initial food types and adjust total amount of each type of food
    int initialFood[NUM_SPOTS];
    for (int i = 0; i < NUM_SPOTS; i++)
    {
      bool fillFood = true;
      while (fillFood)
      {
        int tempFood = random(FOOD_TYPE);
        if (initialFoodAmount[tempFood] > 0)
        {
          initialFood[i] = tempFood;
          initialFoodAmount[initialFood[i]] -= 1;
          fillFood = false;
        }
      }
    }
    // Initialize the board
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

// Draw single digit on the LED Matrix with an input color
void drawDigit(int digit, uint16_t color)
{
  matrix.setTextSize(1);
  matrix.setTextColor(color);
  matrix.clear();
  matrix.setCursor(1, 1);
  matrix.print(digit);
  matrix.writeDisplay();
}

// Draw single digit on the LED Matrix with an input color
void drawDigitWithBlink(int digit, uint16_t color)
{
  matrix.setTextSize(1);
  matrix.setTextColor(color);
  matrix.blinkRate(1);
  matrix.clear();
  matrix.setCursor(1, 1);
  matrix.print(digit);
  matrix.writeDisplay();
  delay(2000);
  matrix.blinkRate(0);
}

// Draw words on the LED Matrix with an input color
void drawWord(String word, uint16_t color)
{
  matrix.setTextWrap(false);
  matrix.setTextSize(1);
  matrix.setTextColor(color);
  for (int8_t x = 7; x >= -36; x--)
  {
    matrix.clear();
    matrix.setCursor(x, 0);
    matrix.print(word);
    matrix.writeDisplay();
    delay(50);
  }
}

// Draw a smile face to indicate successful action
void drawSmileFace()
{
  matrix.clear();
  matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(1000);
}

// Draw a sad face to indicate unsuccessful action
void drawSadFace()
{
  matrix.clear();
  matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_RED);
  matrix.writeDisplay();
  delay(1000);
}

void drawProgressBar(int num)
{
  for (int x = 7; x >= 0; x--)
  {
    if (x > 4)
    {
      matrix.clear();
      matrix.drawLine(0, 0, x, 0, LED_GREEN);
      matrix.setTextSize(1);
      matrix.setTextColor(LED_GREEN);
      matrix.setCursor(1, 1);
      matrix.print(num);
      matrix.writeDisplay();
      delay(500);
    }
    else if (x > 1)
    {
      matrix.clear();
      matrix.drawLine(0, 0, x, 0, LED_YELLOW);
      matrix.setTextSize(1);
      matrix.setTextColor(LED_GREEN);
      matrix.setCursor(1, 1);
      matrix.print(num);
      matrix.writeDisplay();
      matrix.writeDisplay();
      delay(500);
    }
    else
    {
      matrix.clear();
      matrix.drawLine(0, 0, x, 0, LED_RED);
      matrix.setTextSize(1);
      matrix.setTextColor(LED_GREEN);
      matrix.setCursor(1, 1);
      matrix.print(num);
      matrix.writeDisplay();
      matrix.writeDisplay();
      delay(500);
    }
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

  matrix.begin(0x70);

  printGameBoard();
}

void loop()
{
  drawDigit(this_player.digStrength, LED_YELLOW);

  // Start this player's turn
  if (boardPlaying.myTurn == 1)
  {
    String action;
    // Choose an action
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
    // Detect
    if (action == this_player.detect_uid)
    {
      drawWord("Detect", LED_GREEN);
      bool stopDetect = true;
      while (stopDetect)
      {
        String detectingSpot;
        if (!mfrc522.PICC_IsNewCardPresent())
        {
          continue;
        }
        if (!mfrc522.PICC_ReadCardSerial())
        {
          continue;
        }
        detectingSpot = uidToString();
        for (int i = 0; i < NUM_SPOTS; i++)
        {
          if (detectingSpot == this_player.board[i])
          {
            drawProgressBar(boardPlaying.level[i]);
            Serial.println(boardPlaying.level[i]);
            // Roll a 6 sided dice to have the chance to know food type
            for (int i = 0; i < 30; i++)
            {
              drawDigit(random(1, 7), LED_RED);
              delay(60);
            }
            int chance = random(1, 7);
            // Display the result of rolling and print the food when success
            if (chance <= this_player.chanceToKnowType[boardPlaying.level[i]])
            {
              drawDigitWithBlink(chance, LED_GREEN);
            }
            else
            {
              drawDigitWithBlink(chance, LED_RED);
            }
            stopDetect = false;
            boardPlaying.myTurn = 0;
          }
        }
      }
    }
    // Collect digging Strength
    else if (action == this_player.collect_uid)
    {
      if (this_player.digStrength < 3)
      {
        this_player.digStrength += 1;
        drawSmileFace();
        boardPlaying.myTurn = 0;
      }
      else
      {
        drawSadFace();
      }
    }
    else if (action == this_player.dig_uid)
    {
      matrix.setTextWrap(false);
      matrix.setTextSize(1);
      matrix.setTextColor(LED_GREEN);
      for (int8_t x = 7; x >= -36; x--)
      {
        matrix.clear();
        matrix.setCursor(x, 0);
        matrix.print("Dig");
        matrix.writeDisplay();
        delay(100);
      }
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
            matrix.clear();
            matrix.drawLine(0, 0, 7, 7, LED_RED);
            matrix.drawLine(0, 7, 7, 0, LED_RED);
            matrix.writeDisplay();
            delay(500);
          }
        }
      }
    }
  }
}