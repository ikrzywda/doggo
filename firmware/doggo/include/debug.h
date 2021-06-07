#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <Rtc_Pcf8563.h>

void DEBUG_dump_sd(File dir, 
                   uint8_t tabs)
{
    File f = dir.openNextFile();
    while(f)
    {
        for (uint8_t i = 0; i < tabs; i++) { Serial.print('\t'); }

        Serial.print(f.name());
        Serial.print(F(" : "));
        Serial.println(f.size());

        if(f.isDirectory())
        {
            DEBUG_dump_sd(f, tabs+1);
        }
        else
        {
            while(f.available())
                Serial.print((char)f.read());
        }

        f = dir.openNextFile();
    }
    f.close();
}

#endif
