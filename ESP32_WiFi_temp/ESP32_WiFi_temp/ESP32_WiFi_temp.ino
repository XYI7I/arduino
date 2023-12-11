#include <WiFi.h>
#include <time.h>
#include <iarduino_DHT.h> // подключаем библиотеку для работы с датчиком DHT
#include <OneWire.h>      // подключаем библиотеку для работы с интерфейсом OneWire

// объявляем  переменную для работы с датчиком DHT, 
// указывая номер цифрового вывода к которому подключён датчик (сейчас 2pin)

const char* ssid     = "GregAir5G";     
const char* password = "Gtheybzhbkj777!";

int ledPin = 12;          // select the pin for the LED
int speakerPin = 11;      // select the pin for the Speaker
unsigned long myTime;
long day = 86400000;      // 86400000 milliseconds in a day
long hour = 3600000;      // 3600000 milliseconds in an hour
long minute = 60000;      // 60000 milliseconds in a minute
long second =  1000;      // 1000 milliseconds in a second

// переменные под значения температуры в градусах Цельсия и Фаренгейта
float temp, celsius, fahrenheit;

void setup() {

  pinMode(ledPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Wi-Fi set

  Serial.begin(115200);         
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.println("Connection established");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Print head;
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
  
  delay(1000);

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
    byte type_s;                               // Флаг модели датчика
    byte data[12];

    ds.reset();                                 // производим сброс шины перед началом работы с модулем
    ds.write(0x44, 0);                          // отправка команды выполнить конвертацию
    delay(1000);                                // задержка в 1 сек (для выполнения конвертации)
    present = ds.reset();                       // флаг сброса шины перед приёмом данных с модуля
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
    // fahrenheit = celsius * 1.8 + 32.0;          // переводим значения в градусы Фаренгейта
    
    Serial.print((String) celsius + ";");         // выводим в монитор порта текста Температура 
    temp = celsius;
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