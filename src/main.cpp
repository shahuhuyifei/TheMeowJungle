#include "main.h"

player this_player;

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

// Identify the player
void identifyPlayer(String playerIdentifier)
{
  if (playerIdentifier = "A")
  {
    this_player = player_A;
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
}

void loop()
{
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  // Detect
  Serial.println(uidToString());
  if (uidToString() == this_player.detect_uid)
  {
    while (true)
    {
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
    }
  }
}