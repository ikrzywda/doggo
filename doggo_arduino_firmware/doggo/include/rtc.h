#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <Wire.h>
#include <Rtc_Pcf8563.h>

const size_t SIZE_FILENAME = 13,
             SIZE_TIMESTAMP = 6;
             

void get_log_filename(Rtc_Pcf8563 *rtc,
                      char filename[SIZE_FILENAME])
{
    uint8_t day = rtc->getDay(),
            month = rtc->getMonth(),
            year = rtc->getYear();

    sprintf(filename, "%02d%02d%02d.csv\0", 
            day, month, year);
}

void get_timestamp(Rtc_Pcf8563 *rtc, 
                   char timestamp[SIZE_TIMESTAMP])
{
    uint8_t hour = rtc->getHour(),
            minute = rtc->getMinute();

    sprintf(timestamp, "%02d:%02d\0", hour, minute);
}

#endif 
