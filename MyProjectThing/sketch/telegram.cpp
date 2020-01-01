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
  //bot.getUpdates gets any pending messages and stores them in bot.messages
  return bot.getUpdates(bot.last_message_received + 1);
}



void handleNewMessages(int numNewMessages)
{
    for (int i=0; i < numNewMessages; i++)
    {
      String text = bot.messages[i].text;
      
      if (text == "2_ON")
      {
        
      }
      else if (text == "2_OFF")
      {
      
      }
      else if (text == "3_ON")
      {
      
      }
      else if (text == "3_OFF")
      {
      
      }
      else
      {
        //respond that the command is not recog
      }
        
    }
}
