#include <Ticker.h>

Ticker flipper;

int count = 0;

void flip()
{
    int state = digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, !state);
    count = count + 1;
    if (count == 10)
    {
        flipper.detach();
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    flipper.attach(0.5, flip);
}

void loop()
{
}
