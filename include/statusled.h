#ifndef STATUSLED_H
#define STATUSLED_H

#ifdef STATUS_LED_PIN

#include <Arduino.h>

class
{
        const uint8_t CHANNEL = 7;
        const uint32_t FREQ = 5000;
        const uint8_t RESOLUTION = 8;
        const unsigned long BLINK_TIME = 100;     // ms
        const unsigned long BLINK_GAP = 100;      // limit to 10 Hz
        const unsigned long BREATH_STEP = 4;      // ms per brightness step (~2 s cycle)

        unsigned long blinkUntil = 0;
        unsigned long lastBlink = 0;
        uint8_t blinkState = 0;  // 0: idle, 1: on, 2: off
        uint8_t blinkCount = 0;
        unsigned long lastBreath = 0;
        int8_t breatheDir = 1;
        uint8_t breatheValue = 0;

        public:
                void init()
                {
                        ledcSetup(CHANNEL, FREQ, RESOLUTION);
                        ledcAttachPin(STATUS_LED_PIN, CHANNEL);
                        breatheValue = 0;
                        breatheDir = 1;
                }

                void startBlink(uint8_t count)
                {
                        blinkCount = (count > 0) ? count - 1 : 0;
                        blinkState = 1;
                        blinkUntil = millis() + BLINK_TIME;
                        lastBlink = millis();
                }

                void error()
                {
                        startBlink(3);
                }

                void update(bool hasActivity)
                {
                        unsigned long now = millis();

                        if (hasActivity && blinkState == 0 && blinkCount == 0 && (now - lastBlink >= BLINK_GAP))
                        {
                                startBlink(1);
                        }

                        if (blinkState == 1)
                        {
                                if (now >= blinkUntil)
                                {
                                        blinkState = 2;
                                        blinkUntil = now + BLINK_TIME;
                                }
                                ledcWrite(CHANNEL, 255);
                                return;
                        }
                        else if (blinkState == 2)
                        {
                                if (now >= blinkUntil)
                                {
                                        if (blinkCount > 0)
                                        {
                                                blinkCount--;
                                                blinkState = 1;
                                                blinkUntil = now + BLINK_TIME;
                                        }
                                        else
                                                blinkState = 0;
                                }
                                ledcWrite(CHANNEL, 0);
                                return;
                        }

                        if (now - lastBreath >= BREATH_STEP)
                        {
                                lastBreath = now;
                                breatheValue += breatheDir;
                                if (breatheValue == 0 || breatheValue == 255)
                                        breatheDir = -breatheDir;
                        }
                        ledcWrite(CHANNEL, breatheValue);
                }
} statusLed;

#endif // STATUS_LED_PIN
#endif // STATUSLED_H
