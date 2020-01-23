/**
 * For handling messages via Telegram API bot.
 * Requires to have Telegram bot api key, that is inserted in private.h.
 * Uses Universal Telegram Bot lib:  https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot 
 * */
#include "telegram.h"
String telegramApiKey = _TELEGRAM_API_KEY;
WiFiClientSecure netSSL;
UniversalTelegramBot bot(telegramApiKey, netSSL);

/**
 * called from main::loop() every BOT_INTERVAL ms. 
 * @return number of new messages. The number is used in handleNewMessages.
 * */
int checkMessages()
{
  return bot.getUpdates(bot.last_message_received + 1);
}

/**
 * Checks every message and calls an appropriate plug toggling function 
 * if input message is recognised. 
 * */
void handleNewMessages(int numNewMessages)
{
    for (int i=0; i < numNewMessages; i++)
    {
      String text = bot.messages[i].text;
      if(text[0] == '/') text.remove(0, 1);
      Serial.printf("Telegram: ");
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
        else if (text == "start")
        {
            bot.sendSimpleMessage(bot.messages[i].chat_id, (String)"Hi!", "");
        }
      else
      {
          bot.sendSimpleMessage(bot.messages[i].chat_id, (String)"Command Not Recognised", "");
        //respond that the command is not recognised
      }
        
    }
}

void setTelegramApiKey(String newTelegramApiKey){
    try {
        telegramApiKey = newTelegramApiKey;
    }
    catch (...){
        resetTelegramApiKey();
    };
}

void resetTelegramApiKey(){
  telegramApiKey = _TELEGRAM_API_KEY;
}
