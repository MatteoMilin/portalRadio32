#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#define PIN_A  25
#define PIN_B  26
#define PIN_SW 27

volatile int encoderValue = 0;
volatile bool encoderUpdated = false;
volatile unsigned long lastInterruptTime = 0;

volatile uint8_t lastEncoded = 0;

LiquidCrystal_I2C lcd(0x25, 16, 2);

void IRAM_ATTR handleEncoder()
{
    unsigned long currentTime = micros();

    if (currentTime - lastInterruptTime < 1000) {
        return;
    }
    lastInterruptTime = currentTime;

    uint8_t MSB = digitalRead(PIN_A);
    uint8_t LSB = digitalRead(PIN_B);

    uint8_t encoded = (MSB << 1) | LSB;
    uint8_t sum = (lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100) {
        encoderValue++;
        encoderUpdated = true;
    } else if (sum == 0b1110 || sum == 0b0111) {
        encoderValue--;
        encoderUpdated = true;
    }

    lastEncoded = encoded;
}

void setup()
{
    // LCD screen initialization
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Valeur: ");
    lcd.print(encoderValue);

    // Encoder initialization
    pinMode(PIN_A, INPUT_PULLUP);
    pinMode(PIN_B, INPUT_PULLUP);
    pinMode(PIN_SW, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_A), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_B), handleEncoder, CHANGE);
    lastEncoded = (digitalRead(PIN_A) << 1) | digitalRead(PIN_B);
}

void loop()
{
    if (encoderUpdated) {
        encoderUpdated = false;
        lcd.setCursor(0, 0);
        lcd.print("Valeur: ");
        lcd.print(encoderValue);
        lcd.print("    ");
    }

    if (digitalRead(PIN_SW) == LOW) {
        lcd.setCursor(0, 1);
        lcd.print("Bouton appuye   ");
        delay(200);
    } else {
        lcd.setCursor(0, 1);
        lcd.print("                ");
    }
}
