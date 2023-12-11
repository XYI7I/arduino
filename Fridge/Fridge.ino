#include <iarduino_DHT.h> // подключаем библиотеку для работы с датчиком DHT
#include <OneWire.h>      // подключаем библиотеку для работы с интерфейсом OneWire

OneWire  ds_fr(2);        // Создаём объект с именем ds, и указанием пина, к которому подключен датчик
OneWire  ds_vol(3);       // Создаём объект с именем ds, и указанием пина, к которому подключен датчик
OneWire  ds_fe(5);        // Создаём объект с именем ds, и указанием пина, к которому подключен датчик
OneWire  ds_comp(6);     // Создаём объект с именем ds, и указанием пина, к которому подключен датчик
iarduino_DHT sensor(4);   // объявляем  переменную для работы с датчиком DHT, указывая номер цифрового вывода к которому подключён датчик (сейчас 2pin)

int ledPin = 12;          // select the pin for the LED
int speakerPin = 11;      // select the pin for the Speaker
unsigned long myTime;
long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second =  1000; // 1000 milliseconds in a second

float temp, celsius, fahrenheit;                  // переменные под значения температуры в градусах Цельсия и Фаренгейта

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println((String) "Time;Humidity;T room;T fridge;T vol;T mass;T compr");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  long timeNow = millis();
  // put your main code here, to run repeatedly:
  int days = timeNow / day ;  
  int hours = (timeNow % day) / hour;
  int minutes = ((timeNow % day) % hour) / minute ;
  int seconds = (((timeNow % day) % hour) % minute) / second;

  Serial.print(hours);  
  printDigits(minutes);
  printDigits(seconds);
  Serial.print("; "); 
  

  // Serial.println(time); // prints time since program started
  // Выводим результат на последовательный порт
  
  switch(sensor.read()){    // читаем показания датчика
    case DHT_OK:               Serial.print((String) sensor.hum + "; " + sensor.tem + "; ");  break;
    case DHT_ERROR_CHECKSUM:   Serial.print("Error;Checksum;");                              break;
    case DHT_ERROR_DATA:       Serial.print("Error;Data;");                                  break;
    case DHT_ERROR_NO_REPLY:   Serial.print("Error;No replay;");                             break;
    default:                   Serial.print("ERROR; System;");                                       break;
  }

  sensorTemp(ds_fr);
  sensorTemp(ds_vol);
  sensorTemp(ds_fe);
  sensorTemp(ds_comp);
  attentionTemp(temp);
  Serial.println();

  digitalWrite(LED_BUILTIN, LOW);  
  
  delay(56000);

}

void printDigits(byte digits){
  // utility function for digital clock display: prints colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits,DEC);   
}

void sensorTemp(OneWire ds){

    byte i;                                     // задаём переменную для счётчика
    byte present = 0;                           // Флаг сброса шины
    byte type_s;                                // Флаг модели датчика
    byte data[12];                              // массив под значения температуры, считанные с датчика
    byte addr[] = { 0x28, 0x3D, 0x11, 0x3F, 0x0E, 0x00, 0x00, 0x2E }; // (0x2E 00 00 0E 3F 11 3D 28)
    
    ds.reset();                                 // производим сброс шины перед началом работы с модулем
    ds.select(addr);                            // указываем устройство, с которым далее будет работать код
    ds.write(0x44, 0);                          // отправка команды выполнить конвертацию
    delay(1000);                                // задержка в 1 сек (для выполнения конвертации)
    present = ds.reset();                       // флаг сброса шины перед приёмом данных с модуля
    ds.select(addr);                            // указываем устройство, с которым далее будет работать код
    ds.write(0xBE);                             // считываем значение из scratchpad-памяти модуля
  
    for ( i = 0; i < 9; i++) {                  // запускаем цикл считывания 9 байт информации с модуля
      data[i] = ds.read();                      // побайтовое чтение значений с модуля
    }
    
    int16_t raw = (data[1] << 8) | data[0];     // конвертация значения температуры в 16-битный тип данных
    
    if (type_s) {                               // если подключена модель DS18S20
      raw = raw << 3;                           // тогда сдвигаем значение на 3 влево для получения 12-битного разрешения точности температуры
      if (data[7] == 0x10) {                    // если указанный бит совпадает с условием, то
        raw = (raw & 0xFFF0) + 12 - data[6];    // пересчитываем сырое значение
      }
    } else {                                    // для всех остальных моделей
      byte cfg = (data[4] & 0x60);              // для задания разрешения вводится переменная (по умолчанию установлено 12-битное разрешение)
      if (cfg == 0x00) raw = raw & ~7;          // если значение переменной и указанного бита совпадают, тогда устанавливается 9-битное разрешение точности
      else if (cfg == 0x20) raw = raw & ~3;     // если значение переменной и указанного бита совпадают, тогда устанавливается 10-битное разрешение точности
      else if (cfg == 0x40) raw = raw & ~1;     // если значение переменной и указанного бита совпадают, тогда устанавливается 11-битное разрешение точности
    }
    
    celsius = (float)raw / 16.0;                // переводим значения в градусы Цельсия
    fahrenheit = celsius * 1.8 + 32.0;          // переводим значения в градусы Фаренгейта
    
    Serial.print((String) celsius + ";");         // выводим в монитор порта текста Температура 
    temp = celsius;
    return temp;
}

void attentionTemp(float temp){

    if  (temp >= 90 ) {
      digitalWrite(ledPin, HIGH);
      tone(speakerPin, 2960);    
      }

    else if (temp > 80) {
      digitalWrite(ledPin, HIGH);
      noTone(speakerPin);
        }

    else {
      digitalWrite(ledPin, LOW);
      noTone(speakerPin);
      }

}