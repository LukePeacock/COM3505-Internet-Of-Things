/**
 * Used for sending signals to plugs. Included in telegram.h and main.cpp
 */
#include "transmitter.h"


RCSwitch mySwitch = RCSwitch();
/**
 * initial transmitter setup. Called in main::setup() once.
 * */
void setupSwitch()
{
  mySwitch.enableTransmit(12);    // Transmitter is connected to esp32 Pin #12
  mySwitch.setPulseLength(175);   // We need to set pulse length as it's different from default
}
/**
 * methods for toggling plugs using mySwitch. Will not function properly if setupSwitch() is 
 * not called beforehand.
 * */
void sendSignal(int signalNum){
  mySwitch.send(signalNum, 24);
}
 
void plug2On()
{
  mySwitch.send(1398211, 24);
}

void plug2Off()
{
  mySwitch.send(1398220, 24);      
}

void plug3On()
{
  mySwitch.send(5527299, 24);
}

void plug3Off()
{
  mySwitch.send(5527308, 24);
}
//end of toggling plug methods
