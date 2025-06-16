#ifndef STATUSLED_H
#define STATUSLED_H

#ifdef STATUS_LED_PIN
#include <Arduino.h>

class {
        const uint8_t CHANNEL = 7;
        const uint32_t FREQ = 5000;
        const uint8_t RESOLUTION = 8;
        const unsigned long BLINK_TIME = 40;      // ms on-time per blink
        const unsigned long BLINK_GAP = 100;      // limit to 10 Hz
        const uint8_t MAX_BREATH = 64;            // idle brightness
        const unsigned long BREATH_PERIOD = 2000; // ms for a full breath cycle

        unsigned long activeUntil = 0;
        unsigned long lastBlink = 0;

    public:
        void init()
        {
                ledcSetup(CHANNEL, FREQ, RESOLUTION);
                ledcAttachPin(STATUS_LED_PIN, CHANNEL);
                ledcWrite(CHANNEL, 0);
        }

        void update(bool hasActivity)
        {
                unsigned long now = millis();

                if (hasActivity && (now - lastBlink >= BLINK_GAP))
                {
                        lastBlink = now;
                        activeUntil = now + BLINK_TIME;
                }

                if (now < activeUntil)
                {
                        ledcWrite(CHANNEL, 255);
                }
                else
                {
                        unsigned long phase = now % BREATH_PERIOD;
                        uint16_t value;
                        if (phase < BREATH_PERIOD / 2)
                                value = (phase * MAX_BREATH * 2) / BREATH_PERIOD;
                        else
                                value = MAX_BREATH - ((phase - BREATH_PERIOD / 2) * MAX_BREATH * 2) / BREATH_PERIOD;
                        ledcWrite(CHANNEL, value);
                }
        }
} statusLed;

#endif // STATUS_LED_PIN
#endif // STATUSLED_H
