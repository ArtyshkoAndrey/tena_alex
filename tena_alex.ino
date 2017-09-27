#include <iarduino_RTC.h>
#include <TM1638.h>
#include <InvertedTM1638.h>
iarduino_RTC time(RTC_DS1307); //инициализация часов
TM1638 module(8, 7, 6); //пины lcd
byte  button; //код нажатой кнопки на lcd

unsigned long prev_ms = 0; //переменная для создания отрезков, чтобы не использовать delay
int counterTena = 0; //счётчки для тен
int counterTenaPin = 10; //счётчик пинов тен
int stateTena = 0; //работа тен
int N = 3; //Кол-во тен

void toggleTena(int currentTena, int counterTenaPin) { //функция с алгоритмом включение тен
  if (currentTena == 0) {
    digitalWrite(11, false);
  } else if (currentTena == 1) {
    digitalWrite(12, false);
  } else {
    digitalWrite(10, false);
  }
  digitalWrite(counterTenaPin, true);
}

void setup() {
  Serial.begin(9600);
  time.begin();//инициализация времени

  //////////////////////////////////////////
  //Инициализация пинов для вывода для тен//
  //////////////////////////////////////////
  for (int i = 10; i < 13; i++)
    pinMode(i, OUTPUT);
}
void loop() {
  if ((time.Hours >= 23 && time.minutes >= 00) || (time.Hours < 7) || (stateTena == 1)) { // запускать тены после 23 и до 7
    module.setLED(TM1638_COLOR_RED, 7); //Включение светодиода для отсеживание работы тен
    toggleTena(counterTena, counterTenaPin);//Первое включение что бы сначало не ждать 15 минут
    if ((millis() - prev_ms) >  900000) { //переключение тен каждые 15 минут
      prev_ms = millis(); //переменная для отсчёта времени
      counterTena++;
      counterTenaPin++;
      if (counterTena == N) { //Если счётчик тен больше кол-во, то обнуляем
        counterTena = 0;
      }
      if (counterTenaPin == 13) //Зброс счётчик атен при переборе
        counterTenaPin = 10;
      toggleTena(counterTena, counterTenaPin); //Включение тен
    }
  }
  else { //Если нет подходящего времени, то выключаем все тены и обнуляем счётчик
    prev_ms = 0;
    module.setLED(0, 7); //Включение светодиода для отсеживание работы тен
    for (int i = 10; i < 13; i++)
      digitalWrite(i, false);
  }
  module.setDisplayToString(time.gettime("H:i:s")); //Вывод времени на lcd
  button = module.getButtons(); //Узнаём код кнопки
  if (button == 1) { //Прибовляем десятки часов
    if (time.Hours >= 20) {
      time.settime(-1, -1, time.Hours - 20);
    }
    else
      time.settime(-1, -1, time.Hours + 10);
  }
  if (button == 2) { //Прибовляем единицы часов
    if (time.Hours == 23) {
      time.settime(-1, -1, time.Hours - 23);
    }
    else if (time.Hours % 10 >= 9) {
      time.settime(-1, -1, time.Hours - 9);
    }
    else
      time.settime(-1, -1, time.Hours + 1);
  }
  if (button == 8) { //Прибовляем десятки минут
    if (time.minutes >= 50) {
      time.settime(-1, time.minutes - 50);
    }
    else
      time.settime(-1, time.minutes + 10);
  }
  if (button == 16) { //Прибовляем единицы минут
    if (time.minutes % 10 >= 9) {
      time.settime(-1, time.minutes - 9);
    }
    else
      time.settime(-1, time.minutes + 1);
  }
  if (button == 128) { //Ручное включение и выключение алгоритма тен
    if (stateTena == 0) {
      stateTena = 1;
      module.setLED(TM1638_COLOR_RED, 7); //Включение светодиода для отсеживание работы тен
    }
    else {
      stateTena = 0;
      module.setLED(0, 7);
    }
  }
  delay(200);

} //end loop
