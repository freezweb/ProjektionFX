#if !defined(MQTTStreamFarben_H__)
#define MQTTStreamFarben_H__

#include <Arduino.h>


class MQTTStreamFarben
{
private:
    CRGB Farbe;

public:
    void setStreamColor(CRGB color)
    {
        Farbe = color;
    }

    CRGB getStreamColor()
    {
        return Farbe;
        // return CRGB::Red;
    }
};

#endif // MQTTStreamFarben_H__
