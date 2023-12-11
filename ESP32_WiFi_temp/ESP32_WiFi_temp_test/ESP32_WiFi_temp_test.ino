// WiFi temperature on Wemos ESP32 D1 R32 WiFi and Bluetooth
// This Program will collect temperature and humidity data from multiple DS18B20 temperature sensors and DHT sensor.
// And it'll send this data to ftp-server().

//LIBRARIES TO USE
#include <WiFi.h>               // WiFi's library
#include <time.h>               // time's library
#include <OneWire.h>            // OneWire's library for temperature sensor DS18B20
// #include <DallasTemperature.h>  // DS18B20's library
// #include <iarduino_DHT.h> // DHT sensor's library

OneWire  ds(16);       // Создаём объект с именем ds, и указанием пина, к которому подключен датчик
// DallasTemperature sensors(&ds_fr);

// DS18B20  ds_vol(3);      // Создаём объект с именем ds, и указанием пина, к которому подключен датчик
// DS18B20  ds_fe(5);       // Создаём объект с именем ds, и указанием пина, к которому подключен датчик
// DS18B20  ds_comp(6);     // Создаём объект с именем ds, и указанием пина, к которому подключен датчик
// iarduino_DHT sensor(4);  // объявляем  переменную для работы с датчиком DHT, указывая номер цифрового вывода к которому подключён датчик (сейчас 2pin)

// Create constant

// Wi-Fi login 
const char* ssid     = "GregAir5G";     
const char* password = "Gtheybzhbkj777!";
// Pin set
// int ledPin = 12;          // select the pin for the LED
// int speakerPin = 11;      // select the pin for the Speaker

// Time's constants
unsigned long myTime;
long day = 86400000;      // 86400000 milliseconds in a day
long hour = 3600000;      // 3600000 milliseconds in an hour
long minute = 60000;      // 60000 milliseconds in a minute
long second =  1000;      // 1000 milliseconds in a second


// Initializing the start mode
void setup() {

  // Wi-Fi set

  Serial.begin(115200);         
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }
  // digitalWrite(LED_BUILTIN, HIGH);
  Serial.println('\n');
  Serial.println("Connection established");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Print head;
  // Serial.println((String) "Time;Humidity;T room;T fridge;T vol;T mass;T compr");

  // DS18B20 test
  
  byte i;                                     // задаём переменную для счётчика
  byte present = 0;                           // Флаг сброса шины
  byte type_s;                                // Флаг модели датчика
  byte data[12];                              // массив под значения температуры, считанные с датчика
  byte addr[8];                               // массив под значение адреса датчика
  
  Serial.println(ds.search(addr));
  if ( !ds.search(addr)) {                    // если функция принимает значение false, значит
    Serial.println("No more addresses.");     // больше устройств не найдено. Выводит в монитор порта сообщение об этом.
    Serial.println();                         // добавляем символ новой строки
    ds.reset_search();                        // функция начинает новый поиск устройства.
    delay(250);                               // задержка 250 мсек
    return;
  }
  Serial.println(ds.search(addr));
  Serial.print("ROM =");                      // выводим текст в монитор порта. "ROM" - это адрес устройства
  for ( i = 0; i < 8; i++) {                  // запускаем цикл
    Serial.write(' ');                        // между значениями добавляем пробел
    Serial.print(addr[i], HEX);               // в монитор порта побайтово выводим адрес устройства в 16-ричной кодировке
  }
  Serial.println(ds.search(addr));
  if (OneWire::crc8(addr, 7) != addr[7]) {    // выполяем проверка контрольной суммы CRC
    Serial.println("CRC is not valid!");      // если контрольная сумма не верна - отправляем в монитор порта сообщение об этом
    return;
  }
  Serial.println(ds.search(addr));
  Serial.println();                           // добавляем символ новой строки
  switch (addr[0]) {// Модель датчика температуры                                       Флаг типа модуля                В данном блоке проверяем значение старшего байта полученного адреса и по нему определяем модель модуля
    case 0x10:          Serial.println("  Chip = DS18S20");                             type_s = 1;        break;
    case 0x28:          Serial.println("  Chip = DS18B20");                             type_s = 0;        break;
    case 0x22:          Serial.println("  Chip = DS1822");                              type_s = 0;        break;
    default:            Serial.println("Device is not a DS18x20 family device.");                          return;
  }

}

void loop() {

  outputTime();
  delay(1000);

}

void outputTime(){
  // get time
  long timeNow = millis();
  int days = timeNow / day ;  
  int hours = (timeNow % day) / hour;
  int minutes = ((timeNow % day) % hour) / minute ;
  int seconds = (((timeNow % day) % hour) % minute) / second;

  // Output the curent time {hh:mm:ss}
  printDigits(hours, ":");  // print hours    {00}
  printDigits(minutes, ":");     // print minuts   {00} 
  printDigits(seconds, ";");     // print seconds  {00}
  Serial.println();
}

void printDigits(byte digits, String delimetr){
  // utility function for digital clock display: prints colon and leading 0
  if(digits < 10)
    Serial.print("0");
  Serial.print(digits,DEC);
  Serial.print(delimetr);   
}


// void loop(void) {

//   ds.reset();                                 // производим сброс шины перед началом работы с модулем
//   ds.select(addr);                            // указываем устройство, с которым далее будет работать код
//   ds.write(0x44, 0);                          // отправка команды выполнить конвертацию
//   delay(1000);                                // задержка в 1 сек (для выполнения конвертации)
//   present = ds.reset();                       // флаг сброса шины перед приёмом данных с модуля
//   ds.select(addr);                            // указываем устройство, с которым далее будет работать код
//   ds.write(0xBE);                             // считываем значение из scratchpad-памяти модуля
//   Serial.print("  Data = ");                  // в монитор порта выводим текст "Значение"
//   Serial.print(present, HEX);                 // выводим в монитор порта значение с модуля, в 16-ричной кодировке
//   Serial.print(" ");                          // добавляем символ новой строки
//   for ( i = 0; i < 9; i++) {                  // запускаем цикл считывания 9 байт информации с модуля
//     data[i] = ds.read();                      // побайтовое чтение значений с модуля
//     Serial.print(data[i], HEX);               // выводим в монитор порта значение байта, считанного с модуля
//     Serial.print(" ");                        // между значениями добавляем пробел
//   }
//   Serial.print(" CRC=");                      // выводим в монитор порта текст CRC (контрольная сумма)
//   Serial.print(OneWire::crc8(data, 8), HEX);  // выводим в монитор порта значение CRC
//   Serial.println();                           // добавляем символ новой строки
//   int16_t raw = (data[1] << 8) | data[0];     // конвертация значения температуры в 16-битный тип данных
//   if (type_s) {                               // если подключена модель DS18S20
//     raw = raw << 3;                           // тогда сдвигаем значение на 3 влево для получения 12-битного разрешения точности температуры
//     if (data[7] == 0x10) {                    // если указанный бит совпадает с условием, то
//       raw = (raw & 0xFFF0) + 12 - data[6];    // пересчитываем сырое значение
//     }
//   } else {                                    // для всех остальных моделей
//     byte cfg = (data[4] & 0x60);              // для задания разрешения вводится переменная (по умолчанию установлено 12-битное разрешение)
//     if (cfg == 0x00) raw = raw & ~7;          // если значение переменной и указанного бита совпадают, тогда устанавливается 9-битное разрешение точности
//     else if (cfg == 0x20) raw = raw & ~3;     // если значение переменной и указанного бита совпадают, тогда устанавливается 10-битное разрешение точности
//     else if (cfg == 0x40) raw = raw & ~1;     // если значение переменной и указанного бита совпадают, тогда устанавливается 11-битное разрешение точности
//   }
//   celsius = (float)raw / 16.0;                // переводим значения в градусы Цельсия
//   fahrenheit = celsius * 1.8 + 32.0;          // переводим значения в градусы Фаренгейта
//   Serial.println("  Temperature = ");         // выводим в монитор порта текста Температура
//   Serial.print(celsius);                      // выводим значения в градусах Цельсия
//   Serial.println(" Celsius, ");               // выводим в монитор порта текста "градусы Цельсия"
//   Serial.print(fahrenheit);                   // выводим значения в градусах Фаренгейта
//   Serial.println(" Fahrenheit");              // выводим в монитор порта текста "градусы Цельсия"
// }