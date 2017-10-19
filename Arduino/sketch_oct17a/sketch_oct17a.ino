#include <EEPROM.h> //Needed to access the eeprom read write functions

// чтение
int EEPROM_int_read(int addr) {
  byte raw[2];
  for (byte i = 0; i < 2; i++) raw[i] = EEPROM.read(addr + i);
  int &num = (int&)raw;
  return num;
}

// запись
void EEPROM_int_write(int addr, int num) {
  byte raw[2];
  (int&)raw = num;
  for (byte i = 0; i < 2; i++) EEPROM.write(addr + i, raw[i]);
}

// чтение
long EEPROM_long_read(int addr) {    
  byte raw[4];
  for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
  long &num = (long&)raw;
  return num;
}

// запись
void EEPROM_long_write(int addr, long num) {
  byte raw[4];
  (long&)raw = num;
  for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}

void setup()
  {
  Serial.begin(9600);
  //EEPROM_float_write(0, 20.34);
  EEPROM_int_write(0, 0);
  EEPROM_int_write(2, 0);
  EEPROM_int_write(4, 40);
  EEPROM_int_write(6, 35);
  EEPROM_int_write(8, 0);
  EEPROM_long_write(10, 900000);
  Serial.print("Read the following int at the eeprom address 0: ");
  Serial.println(EEPROM_int_read(0));
  Serial.println(EEPROM_int_read(2));
  Serial.println(EEPROM_int_read(4));
  Serial.println(EEPROM_int_read(6));
  Serial.println(EEPROM_int_read(8));
  Serial.println(EEPROM_long_read(10));
  }

void loop()
  {
  }
