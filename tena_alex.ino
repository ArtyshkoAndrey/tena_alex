#include <iarduino_RTC.h> //Библиотека для времени
#include <TM1638.h> //Библиотека для экрана
#include <OneWire.h>
#include <EEPROM.h>//Библиотека для eeprom

iarduino_RTC time(RTC_DS1307); //инициализация часов
TM1638 module(7, 6, 5); //пины lcd
OneWire  ds(2);  // on pin 10 (a 4.7K resistor is necessary) 2 пин датчика температуры для бочки



/////////////////////////
//Работа с EEPROM Long// 4 байта для Long
/////////////////////////

// чтение
long EEPROM_long_read(int addr) {
  byte raw[4];
  for (byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr + i);
  long &num = (long&)raw;
  return num;
}

// запись
void EEPROM_long_write(int addr, long num) {
  if (EEPROM_long_read(addr) != num) { //если сохраняемое отличается
    byte raw[4];
    (long&)raw = num;
    for (byte i = 0; i < 4; i++) EEPROM.write(addr + i, raw[i]);
  }
}

///////////////////////
//Работа с EEPROM Int// 2 байта для int
///////////////////////

// чтение
int EEPROM_int_read(int addr) {
  byte raw[2];
  for (byte i = 0; i < 2; i++) raw[i] = EEPROM.read(addr + i);
  int &num = (int&)raw;
  return num;
}

// запись
void EEPROM_int_write(int addr, int num) {
  if (EEPROM_int_read(addr) != num) { //если сохраняемое отличается
    byte raw[2];
    (int&)raw = num;
    for (byte i = 0; i < 2; i++) EEPROM.write(addr + i, raw[i]);
  }
}

//0-1 байт для stateTena
//2-3 байт для disp
//4-5 байт для temperatureStop
//6-7 байт для temperatureStart
//8-9 байт для stresStop
//10-11--12--13 байт для tempTime

byte  button; //код нажатой кнопки на lcd
unsigned long prev_ms = 0; //переменная для создания отрезков, чтобы не использовать delay
int counterTena = 0; //счётчки для тен
int counterTenaPin = 8; //счётчик пинов тен
int stateTena = EEPROM_int_read(0); //работа тен
int N = 3; //Кол-во тен
int disp = EEPROM_int_read(2);//переменная для методы вывода на экран
byte i; //Для датчика температуры
byte present = 0;// для сбора данных температуры датчику
byte data[12]; //дата датчика темературы
byte addr[8]; //Адрес датчика температуры
float celsius; //температура с датчика
int temperatureStop = EEPROM_int_read(4); //температура для выключение алгоритма
int temperatureStart = EEPROM_int_read(6); //температура включения для  алгоритма
int stresStop = EEPROM_int_read(8); //переменная для полного выключение тен
long tempTime = EEPROM_long_read(10); //Периад включения тен

//////////////////////////
//Алгоритм включение тен//
//////////////////////////

void toggleTena(int currentTena, int counterTenaPin) {
  if (currentTena == 0) {
    digitalWrite(9, false);
  } else if (currentTena == 1) {
    digitalWrite(10, false);
  } else {
    digitalWrite(8, false);
  }
  digitalWrite(counterTenaPin, true);
}

void setup() {
  Serial.begin(9600);
  time.begin();//инициализация времени

  //////////////////////////////////////////
  //Инициализация пинов для вывода для тен//
  //////////////////////////////////////////

  for (int i = 8; i < 11; i++)
    pinMode(i, OUTPUT);
}
void loop() {
  
  //////////////////////////////////
  //Для работы датчика температуры//
  //////////////////////////////////

  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);

  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();
  }

  int16_t raw = (data[1] << 8) | data[0];
  byte cfg = (data[4] & 0x60);

  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms

  ////////////////////////
  //Запуск алгоритма тен//
  ////////////////////////

  celsius = (float)raw / 16.0; //Выщитывание температы
  if (stresStop == 0) {
    if ((time.Hours >= 23 && time.minutes >= 00) || (time.Hours < 7) || (stateTena == 1)) { // запускать тены после 23 и до 7
      if (celsius > temperatureStop) //Если привышает температуру то выкл тен
        goto stop;
      else if (celsius < temperatureStart) {
        module.setLED(TM1638_COLOR_RED, 7); //Включение светодиода для отсеживание работы тен
        toggleTena(counterTena, counterTenaPin); //Включение тен
        if ((millis() - prev_ms) > tempTime) { // период переключение тен
          prev_ms = millis(); //переменная для отсчёта времени
          counterTena++;
          counterTenaPin++;
          toggleTena(counterTena, counterTenaPin); //Включение тен
          if (counterTena == N) { //Если счётчик тен больше кол-во, то обнуляем
            counterTena = 0;
          }
          if (counterTenaPin == 11) //Зброс счётчик атен при переборе
            counterTenaPin = 8;
        }
      }
    }
    else { //Если нет подходящего времени, то выключаем все тены и обнуляем счётчик
stop:
      prev_ms = 0;
      counterTenaPin = 8;
      counterTena = 0;
      if (stateTena != 1)
        module.setLED(0, 7); //Включение светодиода для отсеживание работы тен
      for (int i = 8; i < 11; i++)
        digitalWrite(i, false);
    }
  }
  else {
    goto stop;
  }

  if (stresStop == 1)
    module.setLED(TM1638_COLOR_RED, 6);
  else
    module.setLED(0, 6);
  if (stateTena == 1)
    module.setLED(TM1638_COLOR_RED, 7);


  ///////////////////////////////
  /////Методы вывода на экран////
  ///////////////////////////////

  if (disp == 0) {
    module.setDisplayToString(time.gettime("H:i:s")); //Вывод времени на lcd
  }
  else if (disp == 1) {
    module.setDisplayToString("t", 0, false);
    module.setDisplayDigit(int(celsius) / 10, 1, false);
    module.setDisplayDigit(int(celsius) % 10, 2, false);
    module.setDisplayToString("t", 0, 4);
    module.setDisplayDigit(int(celsius) / 10, 5, false);
    module.setDisplayDigit(int(celsius) % 10, 6, false);
  }
  else if (disp == 2) {
    module.setDisplayToString("Ure", 0, false);
    module.setDisplayDigit(int((tempTime/60000)/10), 4, false);
    module.setDisplayDigit(int((tempTime/60000)%10), 5, false);
  }
  else if (disp == 3) {
    module.setDisplayToString("tStOP", 0, false);
    module.setDisplayDigit(int((tempTime/60000)/10), 6, false);
    module.setDisplayDigit(int((tempTime/60000)%10), 7, false);
    //module.setDisplayToString("teStart", 0, false);
  }
  else if (disp == 4) {
    module.setDisplayToString("tStArt", 0, false);
    module.setDisplayDigit(int((tempTime/60000)/10), 6, false);
    module.setDisplayDigit(int((tempTime/60000)%10), 7, false);
  }
  
  /////////////////////////////
  /////РАБОТЫ С ЭКРАНОМ////////
  /////////////////////////////

  button = module.getButtons(); //Узнаём код кнопки

  if (button == 1) {

    if (disp == 0) { //Прибовляем десятки часов
      if (time.Hours >= 20) {
        time.settime(-1, -1, time.Hours - 20);
      }
      else
        time.settime(-1, -1, time.Hours + 10);
    }
    else if (disp == 2) {
      tempTime += 600000;
      EEPROM_long_write(10, tempTime);
    }

  }
  if (button == 2) {

    if (disp == 0) { //Прибовляем единицы часов
      if (time.Hours == 23) {
        time.settime(-1, -1, time.Hours - 23);
      }
      else if (time.Hours % 10 >= 9) {
        time.settime(-1, -1, time.Hours - 9);
      }
      else
        time.settime(-1, -1, time.Hours + 1);
    }
    else if (disp == 2) {
      tempTime -= 600000;
      EEPROM_long_write(10, tempTime);
    }

  }
  if (button == 4) {
    if (disp == 2) {
      tempTime += 60000;
      EEPROM_long_write(10, tempTime);
    }
  }
  if (button == 8) {

    if (disp == 0) { //Прибовляем десятки минут
      if (time.minutes >= 50) {
        time.settime(-1, time.minutes - 50);
      }
      else
        time.settime(-1, time.minutes + 10);
    }
    if (disp == 2) {
      tempTime -= 60000;
      EEPROM_long_write(10, tempTime);
    }

  }
  if (button == 16) {

    if (disp == 0) { //Прибовляем единицы минут
      if (time.minutes % 10 >= 9) {
        time.settime(-1, time.minutes - 9);
      }
      else
        time.settime(-1, time.minutes + 1);
    }

  }
  if (button == 64) { //Меняем режим просмотра на экране

    if (disp == 0) {
      disp = 1;
      module.clearDisplay();
    }
    else if (disp == 1) {
      disp = 2;
      module.clearDisplay();
    }
    else if (disp == 2) {
      disp = 3;
      module.clearDisplay();
    }
    else if (disp == 3) {
      disp = 4;
      module.clearDisplay();
    }
    else if(disp == 4) {
      disp = 0;
      module.clearDisplay();
    }

    EEPROM_int_write(2, disp);
  }
  if (button == 128) { //Ручное включение и выключение алгоритма тен

    if (stateTena == 0) {
      stateTena = 1;
    }
    else {
      stateTena = 0;
      module.setLED(0, 7);
    }

    EEPROM_int_write(0, stateTena);
  }
  if (button == 192) { //Выключение всей системы

    if (stresStop == 0) {
      stresStop = 1;
    }
    else {
      stresStop = 0;
    }

    EEPROM_int_write(8, stresStop);
  }

  delay(10); // Задержка

} //end loop
