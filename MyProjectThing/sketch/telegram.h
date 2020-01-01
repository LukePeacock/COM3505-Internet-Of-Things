#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <ssl_client.h>
#include <WiFiClientSecure.h>

// https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot 
#include <UniversalTelegramBot.h>
#include "private.h"



int checkMessages();
void handleNewMessages(int);


#endif
