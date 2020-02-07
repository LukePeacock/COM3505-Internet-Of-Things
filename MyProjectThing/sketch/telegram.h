#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <ssl_client.h>
#include <WiFiClientSecure.h>

// https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot 
#include <UniversalTelegramBot.h>
#include "private.h"
#include "transmitter.h"
#include <map>;

int checkMessages();
void handleNewMessages(int);
void setTelegramApiKey(String);





enum Socket { S1401 = 0, S1402 = 1, S1403 = 2, S1404 = 3, S1405 = 4, S1406 = 5, S1407 = 6, S1408 = 7};
enum SocketIndex { On1 = 0, Off1 = 1, On2 = 2, Off2 = 3, On3 = 4, Off5 = 5};

const int SOCKET_CODES[8][6] = {
  { 1398067,1398076,1398211,1398220,1398531,1398540 },
  { 4543795,4543804,4543939,4543948,4544259,4544268 },
  { 349491,349500,349635,349644,349955,349964 },
  { 5330227,5330236,5330371,5330380,5330691,5330700 },
  { 1135923,1135932,1136067,1136076,1136387,1136396 },
  { 4281651,4281660,4281795,4281804,4282115,4282124 },
  { 87347,87356,87491,87500,87811,87820 },
  { 5526835,5526844,5526979,5526988,5527299,5527308 }
};

#endif
