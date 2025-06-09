
  result Save1() {
   EEPROM.begin(512); 
   delay(20);
EEPROM.put(0, value);
EEPROM.put(8, bitlength);
EEPROM.put(12, protocol);
EEPROM.put(16, pulselength);
  EEPROM.commit();
    EEPROM.end();
  return proceed;
  } 

    result Save2() {
   EEPROM.begin(512); 
   delay(20);
EEPROM.put(20, value);
EEPROM.put(28, bitlength);
EEPROM.put(32, protocol);
EEPROM.put(36, pulselength);
  EEPROM.commit();
    EEPROM.end();
  return proceed;
  } 

    result Save3() {
   EEPROM.begin(512); 
   delay(20);
EEPROM.put(40, value);
EEPROM.put(48, bitlength);
EEPROM.put(52, protocol);
EEPROM.put(56, pulselength);
  EEPROM.commit();
    EEPROM.end();
  return proceed;
  } 

    result Save4() {
   EEPROM.begin(512); 
   delay(20);
EEPROM.put(60, value);
EEPROM.put(68, bitlength);
EEPROM.put(72, protocol);
EEPROM.put(76, pulselength);
  EEPROM.commit();
    EEPROM.end();
  return proceed;
  } 

    result Save5() {
   EEPROM.begin(512); 
   delay(20);
EEPROM.put(80, value);
EEPROM.put(88, bitlength);
EEPROM.put(92, protocol);
EEPROM.put(96, pulselength);
  EEPROM.commit();
    EEPROM.end();
  return proceed;
  } 



//Load

  result Load1() {
     EEPROM.begin(512);
     delay(20);
  EEPROM.get(0, value);
  EEPROM.get(8, bitlength);
  EEPROM.get(12, protocol);
  EEPROM.get(16, pulselength);
  oled.clear();
  oled.println(value);
  oled.println(bitlength);
  oled.println(protocol);
  oled.println(pulselength);
  return proceed;
  } 

    result Load2() {
     EEPROM.begin(512);
     delay(20);
  EEPROM.get(20, value);
  EEPROM.get(28, bitlength);
  EEPROM.get(32, protocol);
  EEPROM.get(36, pulselength);
  oled.clear();
  oled.println(value);
  oled.println(bitlength);
  oled.println(protocol);
  oled.println(pulselength);
  return proceed;
  } 

    result Load3() {
     EEPROM.begin(512);
     delay(20);
  EEPROM.get(40, value);
  EEPROM.get(48, bitlength);
  EEPROM.get(52, protocol);
  EEPROM.get(56, pulselength);
  oled.clear();
  oled.println(value);
  oled.println(bitlength);
  oled.println(protocol);
  oled.println(pulselength);
  return proceed;
  } 

    result Load4() {
     EEPROM.begin(512);
     delay(20);
  EEPROM.get(60, value);
  EEPROM.get(68, bitlength);
  EEPROM.get(72, protocol);
  EEPROM.get(76, pulselength);
  oled.clear();
  oled.println(value);
  oled.println(bitlength);
  oled.println(protocol);
  oled.println(pulselength);
  return proceed;
  } 

    result Load5() {
     EEPROM.begin(512);
     delay(20);
  EEPROM.get(80, value);
  EEPROM.get(88, bitlength);
  EEPROM.get(92, protocol);
  EEPROM.get(96, pulselength);
  oled.clear();
  oled.println(value);
  oled.println(bitlength);
  oled.println(protocol);
  oled.println(pulselength);
  
  return proceed;

  } 
