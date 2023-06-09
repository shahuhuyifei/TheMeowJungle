#include "main.h"

player this_player;     // Store the current player's information
gameBoard boardPlaying; // Store the current gameboard information
int finalScore = 0;     // Store the final score

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

// Print the player information for debugging
void printPlayer()
{
  Serial.print("myFood: ");
  for (int i = 0; i < MAX_DIG; i++)
  {
    Serial.print(this_player.myFood[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("digStrength: ");
  Serial.print(this_player.digStrength);
  Serial.println();
  Serial.print("cucumber: ");
  Serial.print(this_player.cucumber);
  Serial.println();
  Serial.print("digAmount: ");
  Serial.print(this_player.digAmount);
  Serial.println();
  Serial.print("ChanceToKnowType: ");
  for (int i = 0; i < FOOD_TYPE; i++)
  {
    Serial.print(this_player.chanceToKnowType[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Myturn: ");
  Serial.print(this_player.myTurn);
  Serial.println();
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
  Serial.print("endGame: ");
  Serial.println(boardPlaying.endGame);
  Serial.print("playerCucumber: ");
  for (int i = 0; i < 3; i++)
  {
    Serial.print(boardPlaying.playerCucumber[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println();
}

// Identify the player and initialize the gameboard at player A
void identifyPlayer(String playerIdentifier)
{
  if (playerIdentifier == "A")
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
    int initPlayerCucumber[3] = {0, 0, 0};
    boardPlaying.initValues(initialLevel, initialFood, initialFoodAmount, 0, initPlayerCucumber);
  }
  else if (playerIdentifier == "B")
  {
    this_player = player_B;
  }
  else if (playerIdentifier == "C")
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
void drawWord(String word, uint16_t color, int length)
{
  matrix.setTextWrap(false);
  matrix.setTextSize(1);
  matrix.setTextColor(color);
  for (int x = 7; x >= (-7 * length); x--)
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

// Draw a digit with a progress bar
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
      delay(500);
    }
  }
}

// Find if a combo of two food exist
void findComboOfTwoFood(int foodOne, int foodTwo, int score)
{
  int combo[2] = {MAX_DIG, MAX_DIG};
  bool findingCombo = true;
  while (findingCombo)
  {
    // Find the combo and note the position of the food ID
    for (int i = 0; i < MAX_DIG; i++)
    {
      if (this_player.myFood[i] == foodOne)
      {
        combo[0] = i;
      }
      else if (this_player.myFood[i] == foodTwo)
      {
        combo[1] = i;
      }
    }
    // Check if a combo exist
    for (int i = 0; i < 2; i++)
    {
      if (combo[i] == MAX_DIG)
      {
        findingCombo = false;
      }
    }
    if (findingCombo == true)
    {
      finalScore += score;
      // Reset the value of calculated food in player's food list and temporary value
      for (int i = 0; i < 2; i++)
      {
        this_player.myFood[combo[i]] = MAX_DIG;
        combo[i] = MAX_DIG;
      }
    }
  }
}

// Play the last turn and calculate the scores
void lastTurnAndCalculateScores()
{
  // Start calculating scores when players played their last turns
  if (boardPlaying.endGame == 1)
  {
    ESPNow.send_message(this_player.peer_mac, (uint8_t *)&boardPlaying, sizeof(boardPlaying));
    int comboOne[4] = {MAX_DIG, MAX_DIG, MAX_DIG, MAX_DIG};
    bool findingComboOne = true;
    while (findingComboOne)
    {
      // Find the combo and note the position of the food ID
      for (int i = 0; i < MAX_DIG; i++)
      {
        if (this_player.myFood[i] == 0)
        {
          comboOne[0] = i;
        }
        else if (this_player.myFood[i] == 1)
        {
          comboOne[1] = i;
        }
        else if (this_player.myFood[i] == 2)
        {
          comboOne[2] = i;
        }
        else if (this_player.myFood[i] == 3)
        {
          comboOne[3] = i;
        }
      }
      // Check if a combo exist
      for (int i = 0; i < 4; i++)
      {
        if (comboOne[i] == MAX_DIG)
        {
          findingComboOne = false;
        }
      }
      if (findingComboOne == true)
      {
        finalScore += 20;
        // Reset the value of calculated food in player's food list and temporary value
        for (int i = 0; i < 4; i++)
        {
          this_player.myFood[comboOne[i]] = MAX_DIG;
          comboOne[i] = MAX_DIG;
        }
      }
    }

    int comboTwo[3] = {MAX_DIG, MAX_DIG, MAX_DIG};
    bool findingComboTwo = true;
    while (findingComboTwo)
    {
      // Find the combo and note the position of the food ID
      for (int i = 0; i < MAX_DIG; i++)
      {
        if (this_player.myFood[i] == 0)
        {
          comboTwo[0] = i;
        }
        else if (this_player.myFood[i] == 1)
        {
          comboTwo[1] = i;
        }
        else if (this_player.myFood[i] == 2)
        {
          comboTwo[2] = i;
        }
      }
      // Check if a combo exist
      for (int i = 0; i < 3; i++)
      {
        if (comboTwo[i] == MAX_DIG)
        {
          findingComboTwo = false;
        }
      }
      if (findingComboTwo == true)
      {
        finalScore += 12;
        // Reset the value of calculated food in player's food list and temporary value
        for (int i = 0; i < 3; i++)
        {
          this_player.myFood[comboTwo[i]] = MAX_DIG;
          comboTwo[i] = MAX_DIG;
        }
      }
    }

    findComboOfTwoFood(1, 3, 7);
    findComboOfTwoFood(4, 3, 6);
    findComboOfTwoFood(5, 3, 6);
    findComboOfTwoFood(6, 3, 6);
    findComboOfTwoFood(7, 3, 6);

    // Add the individual food scores
    for (int i = 0; i < MAX_DIG; i++)
    {
      int foodID = this_player.myFood[i];
      if (foodID == 0)
      {
        finalScore += 5;
      }
      else if (foodID == 1 || foodID == 2 || foodID == 3)
      {
        finalScore += 3;
      }
      else if (foodID == 4 || foodID == 5 || foodID == 6 || foodID == 7)
      {
        finalScore += 2;
      }
    }

    // Deduct the cucumber score
    finalScore -= 2 * this_player.cucumber;

    // Draw the final score on the LED matrix
    while (true)
    {
      drawWord(String(finalScore), LED_GREEN, 2);
    }
  }

  // Counting down for players play their last turns
  if (this_player.myTurn == 1 && (boardPlaying.endGame == 3 || boardPlaying.endGame == 2))
  {
    drawWord("Last Round!", LED_RED, 11);
    boardPlaying.endGame--;
  }
}

// Receive gameboard information from the other board
void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  memcpy(&boardPlaying, data, sizeof(boardPlaying));
  if (boardPlaying.endGame != 1)
  {
    this_player.myTurn = 1;
  }

  // Record the cucumber that the current player owns
  if (CURRENT_PLAYER == "A")
  {
    this_player.cucumber = boardPlaying.playerCucumber[0];
  }
  else if (CURRENT_PLAYER == "B")
  {
    this_player.cucumber = boardPlaying.playerCucumber[1];
  }
  else if (CURRENT_PLAYER == "C")
  {
    this_player.cucumber = boardPlaying.playerCucumber[2];
  }

  printGameBoard();
  Serial.print("Bytes received: ");
  Serial.println(data_len);
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
  ESPNow.reg_recv_cb(onRecv);

  matrix.begin(0x70);

  printGameBoard();
}

void loop()
{
  // Display Digging Strength
  drawDigit(this_player.digStrength, LED_YELLOW);

  lastTurnAndCalculateScores();

  // Start this player's turn
  if (this_player.myTurn == 1)
  {
    // Draw a graphic to remind players to start their turn
    matrix.clear();
    matrix.drawRect(0, 0, 8, 8, LED_RED);
    delay(200);
    matrix.writeDisplay();
    matrix.drawRect(1, 1, 6, 6, LED_GREEN);
    delay(200);
    matrix.writeDisplay();
    matrix.drawRect(2, 2, 4, 4, LED_YELLOW);
    delay(200);
    matrix.writeDisplay();
    matrix.drawRect(3, 3, 2, 2, LED_GREEN);
    delay(200);
    matrix.writeDisplay();

    // Choose an action
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

    // Detect Action
    if (action == this_player.detect_uid)
    {
      drawWord("Detect", LED_GREEN, 6);
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
        // Loop to see which spot is detected
        for (int i = 0; i < NUM_SPOTS; i++)
        {
          if (detectingSpot == this_player.board[i])
          {
            drawProgressBar(boardPlaying.level[i]);
            // Roll a 6 sided dice to have the chance to know food type
            for (int j = 0; j < 30; j++)
            {
              drawDigit(random(1, 7), LED_RED);
              delay(60);
            }
            int chance = random(1, 7);
            int foodID = boardPlaying.food[i];
            // Display the result of rolling and print the food when success
            if (chance <= this_player.chanceToKnowType[foodID])
            {
              drawDigitWithBlink(chance, LED_GREEN);
              drawWord(foodName[foodID], LED_YELLOW, foodName[foodID].length());
            }
            else
            {
              drawDigitWithBlink(chance, LED_RED);
            }
            stopDetect = false;
          }
        }
      }
      this_player.myTurn = 0;
      ESPNow.send_message(this_player.peer_mac, (uint8_t *)&boardPlaying, sizeof(boardPlaying));
    }

    // Collect digging Strength Ation
    else if (action == this_player.collect_uid)
    {
      if (this_player.digStrength < 3)
      {
        this_player.digStrength += 1;
        drawSmileFace();
        this_player.myTurn = 0;
        ESPNow.send_message(this_player.peer_mac, (uint8_t *)&boardPlaying, sizeof(boardPlaying));
      }
      else
      {
        drawSadFace();
      }
    }

    // Dig Action
    else if (action == this_player.dig_uid)
    {
      drawWord("Dig", LED_GREEN, 3);
      bool stopDig = true;
      while (stopDig)
      {
        String digSpot;
        if (!mfrc522.PICC_IsNewCardPresent())
        {
          continue;
        }
        if (!mfrc522.PICC_ReadCardSerial())
        {
          continue;
        }
        digSpot = uidToString();
        // Loop to see which spot is digged
        for (int i = 0; i < NUM_SPOTS; i++)
        {
          if (digSpot == this_player.board[i])
          {
            // Decide whether player performed a successful dig
            if (boardPlaying.level[i] == this_player.digStrength)
            {
              int foodID = boardPlaying.food[i];
              // Deduct the required digging strength
              this_player.digStrength -= boardPlaying.level[i];
              // Store the food digged
              this_player.myFood[this_player.digAmount] = foodID;
              // Increase the change of knowing the food when detect
              if (this_player.chanceToKnowType[foodID] < 3)
              {
                if (foodID >= 0 && foodID < 4)
                {
                  for (int k = 0; k < 4; k++)
                  {
                    this_player.chanceToKnowType[k] += 1;
                  }
                }
                else if (foodID == 4 || foodID == 5)
                {
                  this_player.chanceToKnowType[4] += 1;
                  this_player.chanceToKnowType[5] += 1;
                }
                else if (foodID == 6 || foodID == 7)
                {
                  this_player.chanceToKnowType[6] += 1;
                  this_player.chanceToKnowType[7] += 1;
                }
                else
                {
                  this_player.chanceToKnowType[8] += 1;
                }
              }
              // Throw cucumber to other players
              if (foodID == 8)
              {
                drawWord("Cucumber Time!", LED_YELLOW, 14);
                bool stopFindPlayer = true;
                // Look for player ID
                while (stopFindPlayer)
                {
                  String playerID;
                  if (!mfrc522.PICC_IsNewCardPresent())
                  {
                    continue;
                  }
                  if (!mfrc522.PICC_ReadCardSerial())
                  {
                    continue;
                  }
                  playerID = uidToString();
                  if (playerID == this_player.player_A_uid)
                  {
                    boardPlaying.playerCucumber[0]++;
                    drawWord("Player A - 2", LED_GREEN, 12);
                    stopFindPlayer = false;
                  }
                  if (playerID == this_player.player_B_uid)
                  {
                    boardPlaying.playerCucumber[1]++;
                    drawWord("Player B - 2", LED_GREEN, 12);
                    stopFindPlayer = false;
                  }
                  if (playerID == this_player.player_C_uid)
                  {
                    boardPlaying.playerCucumber[2]++;
                    drawWord("Player C - 2", LED_GREEN, 12);
                    stopFindPlayer = false;
                  }
                }
              }
              // Refill a food on a different level at the spot digged
              bool fillFood = true;
              while (fillFood)
              {
                int tempFood = random(FOOD_TYPE);
                int tempLevel = random(1, ALL_LEVELS + 1);
                if (tempLevel != boardPlaying.level[i] && boardPlaying.totalFoodAmount[tempFood] > 0)
                {
                  boardPlaying.totalFoodAmount[boardPlaying.food[i]] -= 1;
                  boardPlaying.food[i] = tempFood;
                  boardPlaying.level[i] = tempLevel;
                  fillFood = false;
                }
              }
              drawSmileFace();
              // Draw the food name when it is not cucumber
              if (foodID != 8)
              {
                drawWord(foodName[foodID], LED_YELLOW, foodName[foodID].length());
              }
            }
            else
            {
              this_player.digStrength = 0;
              drawSadFace();
            }
            // Increase player's total digging amount
            this_player.digAmount++;
            // End the game when a player used MAX_DIG dig actions
            if (this_player.digAmount == MAX_DIG && boardPlaying.endGame == 0)
            {
              boardPlaying.endGame = 3;
            }
            Serial.println();
            printGameBoard();
            printPlayer();
            stopDig = false;
          }
        }
      }
      this_player.myTurn = 0;
      ESPNow.send_message(this_player.peer_mac, (uint8_t *)&boardPlaying, sizeof(boardPlaying));
    }
  }
}