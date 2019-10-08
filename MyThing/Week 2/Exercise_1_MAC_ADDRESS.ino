// MyThing.ino
// COM3505 lab exercises sketch template

// initialisation entry point
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
}

// task loop entry point
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(2000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(2000);                       // wait for a second
  uint64_t chipID = ESP.getEfuseMac();
  

  Serial.print("MAC ADDRESS = \n");
  Serial.printf("Unsigned Integer = %u\n", chipID);   //Print as unsigned int
  Serial.printf("Decimal Integer = %d\n", chipID);    // Decimal Num
  Serial.printf("Hexadecimal = %X\n", chipID);        //Hex 
  Serial.printf("Integer (base 10) = %i\n", chipID);   //Integer, Base 10

  /* Decimal and Integer Base 10 are identical - both are incorrect.
   * HexaDecimal gives the last 4 bytes of the MAC Address
   * Unsigned Integer gives the mac in unsigned form - which is incorrect
   * The MAC ADDRESS of the chip is stored as 6 bytes, making up 12 Hex numbers, the above representation only gives the lower 4.
   * 
   * To retrive the first 2 bytes of the mac address we must to (chipID >> 32) to copy the upper 2 bytes into the smallest 4 bytes.
   * The following example, taken from https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ChipID/GetChipID/GetChipID.ino
   * Shows the chip MAC ADDRESS properly, displaying all 6 bytes
   */
  
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipID>>32));//print High 2 bytes
  Serial.printf("%08X\n",(uint32_t)chipID);//print Low 4bytes.
   
  /*
   * Alternatively, one may use the %llX formatting option to convert the chipID into a hex number 
   * and display the entire 6 bytes at once. This method uses the ll (el el) Long Long modifier 
   * on the unsigned integer storing the chipID. In fact, the ll modifier works for all integer types,
   * allowing decimal, unsigned int, and integer (base 10) to display the decimal representation
   */

  Serial.print("MAC ADDRESS (ll modifier)= \n");
  Serial.printf("Unsigned Integer = %llu\n", chipID);   //Print as unsigned int
  Serial.printf("Decimal Integer = %lld\n", chipID);    // Decimal Num
  Serial.printf("Hexadecimal = %llX\n", chipID);        //Hex 
  Serial.printf("Integer (base 10) = %lli\n", chipID);   //Integer, Base 10

 
  
}
