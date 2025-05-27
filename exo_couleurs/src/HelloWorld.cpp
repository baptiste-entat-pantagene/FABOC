#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const char *color[4] = {"RED", "GREEN", "YELLOW", "BLUE"};
const int BUTTON_i[] = {26, 12, 14, 27};
const int BUTTON_RED = 26;
const int BUTTON_GREEN = 12;
const int BUTTON_YELLOW = 14;
const int BUTTON_BLUE = 27;

const int YES = 18;
const int NO = 19;

void print_color(int color_index)
{
  lcd.setCursor(0, 0);
  const char *text_color = color[color_index];
  lcd.print(text_color);
}

void print_counter(int i)
{
  lcd.setCursor(i < 10 ? 12 : 11, 1);
  lcd.print(i);

  lcd.setCursor(13, 1);
  const char *on_twenty = "/20";
  lcd.print(on_twenty);
}

void show_color(int index_color)
{

  // Clear LED
  digitalWrite(BUTTON_RED, LOW);
  digitalWrite(BUTTON_YELLOW, LOW);
  digitalWrite(BUTTON_BLUE, LOW);
  digitalWrite(BUTTON_GREEN, LOW);

  digitalWrite(BUTTON_i[index_color], HIGH);
}

int check_color(int is_same)
{
  const int start = millis();
  int current = start;
  while (current - start < 5000)
  {
    current = millis();

    if (digitalRead(YES) == LOW)
    {
      while (digitalRead(YES) == LOW)
      {
      }
      return is_same == 1;
    }
    if (digitalRead(NO) == LOW)
    {
      while (digitalRead(NO) == LOW)
      {
      }
      return is_same == 0;
    }
  }
  return 0;
}

void setup()
{
  pinMode(BUTTON_RED, OUTPUT);
  pinMode(BUTTON_GREEN, OUTPUT);
  pinMode(BUTTON_YELLOW, OUTPUT);
  pinMode(BUTTON_BLUE, OUTPUT);

  pinMode(YES, INPUT_PULLUP);
  pinMode(NO, INPUT_PULLUP);

  // Clean
  digitalWrite(BUTTON_RED, LOW);
  digitalWrite(BUTTON_YELLOW, LOW);
  digitalWrite(BUTTON_BLUE, LOW);
  digitalWrite(BUTTON_GREEN, LOW);

  lcd.init();
  lcd.backlight();
}

void loop()
{

  for (int i = 1; i <= 20; ++i)
  {
    const int color_LCD_index = random(0, 4);
    const int color_LED_index = random(0, 4);
    print_color(color_LCD_index);
    print_counter(i);
    show_color(color_LED_index);

    if (check_color(color_LCD_index == color_LED_index) == 0)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      const char *text_color = "END GAME";
      lcd.print(text_color);
      print_counter(i);
      lcd.setCursor(0, 1);
      lcd.print("RESET 3 SEC");

      delay(3000);
      lcd.clear();
      break;
    }

    delay(500);
    lcd.clear();
  }

  // while (true) {}
}
