#ifndef STATUSLED_H
#define STATUSLED_H

#ifdef STATUS_LED_PIN

#include <Arduino.h>

class
{
        const uint8_t CHANNEL = 7;
        const uint32_t FREQ = 5000;
        const uint8_t RESOLUTION = 8;
        const unsigned long PULSE_TIME = 100;     // ms rise time for activity pulse
        const unsigned long PULSE_GAP  = 100;     // ~5 Hz limit between pulses
        const unsigned long WAIT_GAP   = 1000;    // 1s between wait patterns
        const unsigned long BREATH_STEP = 4;      // ms per brightness step (~2 s cycle)

        unsigned long blinkUntil = 0;
        unsigned long lastBlink = 0;
        uint8_t blinkState = 0;  // 0: idle, 1: on, 2: off
        uint8_t blinkCount = 0;
        unsigned long pulseStart = 0;
        unsigned long lastPulse = 0;
        bool pulseActive = false;
        unsigned long lastBreath = 0;
        int8_t breatheDir = 1;
        uint8_t breatheValue = 0;
        bool waiting = true;

        public:
                void init(bool waitHandshake)
                {
                        ledcSetup(CHANNEL, FREQ, RESOLUTION);
                        ledcAttachPin(STATUS_LED_PIN, CHANNEL);
                        breatheValue = 0;
                        breatheDir = 1;
                        waiting = waitHandshake;
                }

                void handshakeComplete()
                {
                        waiting = false;
                }

                void startBlink(uint8_t count)
                {
                        blinkCount = (count > 0) ? count - 1 : 0;
                        blinkState = 1;
                        blinkUntil = millis() + PULSE_TIME;
                        lastBlink = millis();
                }

                void startPulse()
                {
                        pulseStart = millis();
                        lastPulse = pulseStart;
                        pulseActive = true;
                }

                void error()
                {
                        startBlink(3);
                }

                void update(bool hasActivity)
                {
                        unsigned long now = millis();

                        if (waiting)
                        {
                                if (blinkState == 0 && blinkCount == 0 && (now - lastBlink >= WAIT_GAP))
                                        startBlink(2);
                        }
                        else if (hasActivity && !pulseActive && (now - lastPulse >= PULSE_GAP))
                        {
                                startPulse();
                        }

                        if (blinkState == 1)
                        {
                                if (now >= blinkUntil)
                                {
                                        blinkState = 2;
                                        blinkUntil = now + PULSE_TIME;
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
                                                blinkUntil = now + PULSE_TIME;
                                        }
                                        else
                                        {
                                                blinkState = 0;
                                                lastBlink = now;
                                        }
                                }
                                ledcWrite(CHANNEL, 0);
                                return;
                        }

                        if (pulseActive)
                        {
                                unsigned long elapsed = now - pulseStart;
                                if (elapsed >= PULSE_TIME)
                                {
                                        pulseActive = false;
                                        ledcWrite(CHANNEL, 0);
                                }
                                else
                                {
                                        uint8_t brightness = (elapsed * 255) / PULSE_TIME;
                                        ledcWrite(CHANNEL, brightness);
                                }
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
