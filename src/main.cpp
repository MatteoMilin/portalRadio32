#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <TEA5767N.h>

#define PIN_A         25
#define PIN_B         26
#define PIN_SW        27

#define MIN_FREQUENCY 87.0F
#define MAX_FREQUENCY 107.0F

volatile float frequency = 99.4F;
volatile bool encoderUpdated = false;
volatile unsigned long lastInterruptTime = 0;

volatile uint8_t lastEncoded = 0;

LiquidCrystal_I2C lcd(0x25, 16, 2);
TEA5767N radio = TEA5767N();

void IRAM_ATTR handleEncoder()
{
    unsigned long currentTime = micros();

    if (currentTime - lastInterruptTime < 5000) {
        return;
    }
    lastInterruptTime = currentTime;

    uint8_t MSB = digitalRead(PIN_A);
    uint8_t LSB = digitalRead(PIN_B);

    uint8_t encoded = (MSB << 1) | LSB;
    uint8_t sum = (lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100) {
        frequency = min(frequency + 0.1F, MAX_FREQUENCY);
        encoderUpdated = true;
    } else if (sum == 0b1110 || sum == 0b0111) {
        frequency = max(frequency - 0.1F, MIN_FREQUENCY);
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
    lcd.print("Freq: ");
    lcd.print(frequency, 1);
    lcd.print(" MHz");

    // Encoder initialization
    pinMode(PIN_A, INPUT_PULLUP);
    pinMode(PIN_B, INPUT_PULLUP);
    pinMode(PIN_SW, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_A), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_B), handleEncoder, CHANGE);
    lastEncoded = (digitalRead(PIN_A) << 1) | digitalRead(PIN_B);

    // Radio initialization
    radio.setMonoReception();
    radio.setStereoNoiseCancellingOn();
    radio.setHighCutControlOn();
    radio.setSoftMuteOn();
    radio.setSearchHighStopLevel();
    radio.selectFrequency(frequency);
}

void loop()
{
    if (encoderUpdated) {
        encoderUpdated = false;
        lcd.setCursor(0, 0);
        lcd.print("Freq: ");
        lcd.print(frequency, 1);
        lcd.print(" MHz   ");
        radio.selectFrequency(frequency);
    }

    if (digitalRead(PIN_SW) == LOW) {
        // le bouton il est pressé
        delay(200);
    } else {
        // le bouton il est pas pressé
    }
    delay(50);
}
