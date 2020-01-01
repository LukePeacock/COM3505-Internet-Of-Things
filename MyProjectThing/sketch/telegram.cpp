#include "telegram.h"
String telegramApiKey = _TELEGRAM_API_KEY;
WiFiClientSecure netSSL;
UniversalTelegramBot bot(telegramApiKey, netSSL);

/**
 * Plan
 * 1. poll for new messages in check messages
 * 1.1. If > 0, call handleNewMessages
 * 1.2 If == 0, do nothing
 * 
*/


// https://core.telegram.org/bots/api
int checkMessages()
{
  return bot.getUpdates(bot.last_message_received + 1);
}



void handleNewMessages(int numNewMessages)
{
    for (int i=0; i < numNewMessages; i++)
    {
      String text = bot.messages[i].text;
      Serial.printf("-- Telegram --");
      Serial.println(text.c_str());
      if (text == "2_ON")
      {
        plug2On();
        Serial.println("plug2On");
      }
      else if (text == "2_OFF")
      {
        plug2Off();
        Serial.println("plug2Off");
      }
      else if (text == "3_ON")
      {
        plug3On();
        Serial.println("plug3On");
      }
      else if (text == "3_OFF")
      {
        plug3Off();
        Serial.println("plug3Off");
      }
      else
      {
        //respond that the command is not recog
      }
        
    }
}
