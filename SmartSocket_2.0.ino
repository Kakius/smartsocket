
#include <GyverEncoder.h>
#include <LiquidCrystal_I2C.h>

/**
   Alekseenko D.V
   Программа реализации умной розетки
   Работаем через энкодер

   По мере появления идей, что ещё можно добавить список реализации будет поплняться

   Реализовано:
   1. Таймер

   cursor(column, row)
*/

// Дигитал пины 2,3,4
#define CLK 2
#define DT 3
#define SW 4

LiquidCrystal_I2C lcd(0x3F, 16, 2); // Создание перменной дисплея
Encoder enc(CLK, DT, SW);           // Создание переменной энкодера

// String menu[] = {"On", "Off", "00:00"};
// int menuSize = sizeof(menu) / sizeof(menu[0]);

// int val = 0;        //Переменная в которую пишется значение при вращение энкодера
// int innerValue = 0; //Переменная для отслеживания нахождения курсора на дисплее
int hour = 0;
int minuts = 0;
unsigned long timer = 0;
unsigned long millsecFromStart = 0;

boolean editH = false;
boolean editM = false;
boolean waitRunTimer = true;
boolean done = false;

void setup()
{
  Serial.begin(9600);

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  lcd.init();
  lcd.backlight();

  // tone(5, 300, 1000); работа с  пасивным зумером

  // Отрисовка на LCD

  lcd.setCursor(3, 0);
  lcd.print("SET TIMER");
  printTimeChooserLCD();
}

void printTimeChooserLCD()
{
  lcd.setCursor(3, 0);
  lcd.print("SET TIMER");
  if (hour < 10)
  {
    lcd.setCursor(5, 1);
    lcd.print("0");
    lcd.setCursor(6, 1);
  }
  else
  {
    lcd.setCursor(5, 1);
  }
  lcd.print(hour);

  lcd.setCursor(7, 1);
  lcd.print(":");

  if (minuts < 10)
  {
    lcd.setCursor(8, 1);
    lcd.print("0");
    lcd.setCursor(9, 1);
  }
  else
  {
    lcd.setCursor(8, 1);
  }
  lcd.print(minuts);
}

void loop()
{
  enc.tick();
  // Считаем сколько с момента старта программы прошло времени
  if (timer == 0)
  {
    millsecFromStart = millis();
  }
  if (done)
  {
    if (!enc.isClick())
    {
      return;
    }
    done = false;
    digitalWrite(7, HIGH);
  }

  if (!waitRunTimer) // Тамер запущен
  {
    timerWorker();
  }
  else
  {
    setTimer();
  }
}

void setTimer()
{
  if (enc.isTurn())
  {
    // Врщение в право
    if (enc.isRight() || enc.isFastR())
    {
      if (editH && !editM)
      {
        if (hour < 24)
        {
          hour++;
          if (hour >= 10)
          {
            lcd.setCursor(5, 1);
          }
          else
          {
            lcd.setCursor(6, 1);
          }
          lcd.print(hour);
        }
      }
      else if (editM)
      {
        if (minuts < 59)
        {
          minuts++;
          if (minuts >= 10)
          {
            lcd.setCursor(8, 1);
          }
          else
          {
            lcd.setCursor(9, 1);
          }
          lcd.print(minuts);
        }
      }
    }
    // Вращение в лево
    if (enc.isLeft() || enc.isFastL())
    {
      if (editH && !editM)
      {
        if (hour > 0)
        {
          hour--;
          if (hour < 10)
          {
            lcd.setCursor(5, 1);
            lcd.print("0");
            lcd.setCursor(6, 1);
          }
          else
          {
            lcd.setCursor(5, 1);
          }
          lcd.print(hour);
        }
      }
      else if (editM)
      {
        if (minuts > 0)
        {
          minuts--;
          if (minuts < 10)
          {
            lcd.setCursor(8, 1);
            lcd.print("0");
            lcd.setCursor(9, 1);
          }
          else
          {
            lcd.setCursor(8, 1);
          }
          lcd.print(minuts);
        }
      }
    }
  }

  if (enc.isClick())
  {
    if (!editH)
    {
      editH = true;
    }
    else if (!editM)
    {
      editM = true;
    }
    else
    {
      editH = false;
      editM = false;
      convert_To_Mills(hour, minuts);
      if (timer != 0)
      {
        waitRunTimer = false;
        Serial.print("Timer = ");
        Serial.println(timer);
      }

      // lcd.setCursor(7, 1);

      // digitalWrite(7, HIGH);
    }
  }
}

void timerWorker()
{

  // Расчитываем сколько прошло времени после старта таймера
  // Если прошло больше, чем выставлен таймер, то выключаем его
  // Думаю, тут будет погрешности где от 1 - 5 сек -- тестирование показала, что погрешности нет
  unsigned long time = millis() - millsecFromStart;

  if (timer < time)
  {
    // Serial.println("Завершение таймера");
    lcd.setCursor(7, 1);
    lcd.blink_off();
    waitRunTimer = true;
    done = true;

    timer = 0;
    digitalWrite(7, LOW);

    tone(5, 1000, 1000);
    delay(2000);
    tone(5, 1000, 1000);
    delay(2000);
    tone(5, 1000, 1000);

    lcd.clear();
    printTimeChooserLCD();
  }
  else
  {
    // Serial.println("Отрисовка обратного отсчета");
    unsigned long t = timer - time;
    uint32_t sec = t / 1000ul;            // полное количество секунд
    int timeHours = (sec / 3600ul);       // часы
    int timeMins = (sec % 3600ul) / 60ul; // минуты
    int timeSecs = (sec % 3600ul) % 60ul; // секунды

    if (timeHours < 10)
    {
      lcd.setCursor(5, 1);
      lcd.print("0");
      lcd.setCursor(6, 1);
    }
    else
    {
      lcd.setCursor(5, 1);
    }
    lcd.print(timeHours);

    if (timeMins < 10)
    {
      lcd.setCursor(8, 1);
      lcd.print("0");
      lcd.setCursor(9, 1);
    }
    else
    {
      lcd.setCursor(8, 1);
    }
    lcd.print(timeMins);

    if (timeSecs < 10)
    {
      lcd.setCursor(14, 1);
      lcd.print("0");
      lcd.setCursor(15, 1);
    }
    else
    {
      lcd.setCursor(14, 1);
    }
    lcd.print(timeSecs);
  }

  // Прерываем таймер
  if (enc.isClick())
  {
    Serial.println("Прерываем работу таймера");
    lcd.setCursor(7, 1);
    lcd.blink_off();
    waitRunTimer = true;
    timer = 0;

    lcd.clear();
    printTimeChooserLCD();
  }
}

// Метод который конвертит часы и минуты в миллисекунды
// Для справки в 1 секунде - 1000 миллисекунд
void convert_To_Mills(int hour, int minuts)
{
  Serial.print("Часы: ");
  Serial.println(hour);
  Serial.print("Минуты: ");
  Serial.println(minuts);

  unsigned long hourToMinuts = hour * 60 + minuts;
  timer = 60 * hourToMinuts * 1000;
  Serial.print("Итого врмени в миллисекундах: ");
  Serial.println(timer);
}
