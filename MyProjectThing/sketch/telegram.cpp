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
      String chat_id = String(bot.messages[i].chat_id);
      if(text[0] == '/') text.remove(0, 1);
      Serial.printf("Telegram: ");
      Serial.println(text.c_str());


      
      //Legacy code ----------------
      /*
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
      else if (text == "help")
      {
        //keyboardResponse(chat_id);
        //respond that the command is not recognised
      }
        */
    }
}

/**
 * Plan:
 * 1. create a socket code map:
 *    key - socket main code
 *    values: 1on, 1off, 2on, 2off ... .
 *    Define this as enum with corresponding array index values 1on - 0, 1off - 1, 2on - 2..
 *    
 * 2. Rewrite command recognition system: given the command, try access map key and then value   
 *    Needs to parse command String: {socket}_{num}_{on/off}
 * 
 */
/*
void keyboardResponse(String chat_id){
  
  bot.sendMessageWithReplyKeyboard(String chat_id);
}
*/

//const std::map<std::string, std::map<std::string, int>> sample {"1401", {"1On", 1423434} } ;

void setTelegramApiKey(String newTelegramApiKey){
  telegramApiKey = newTelegramApiKey;
}

void resetTelegramApiKey(){
  telegramApiKey = _TELEGRAM_API_KEY;
}
