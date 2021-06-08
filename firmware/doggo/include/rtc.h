#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <Wire.h>
#include <Rtc_Pcf8563.h>

const size_t SIZE_FILENAME = 19,
             SIZE_TIMESTAMP = 6;

void get_log_filename(Rtc_Pcf8563 *rtc,
                      char filename[SIZE_FILENAME])
{
    uint8_t day = rtc->getDay(),
            month = rtc->getMonth(),
            year = rtc->getYear();

    sprintf(filename, "%s%02d%02d%02d.csv\0", 
            "doggo/", day, month, year);
}

void get_timestamp(Rtc_Pcf8563 *rtc, 
                   char timestamp[SIZE_TIMESTAMP])
{
    uint8_t hour = rtc->getHour(),
            minute = rtc->getMinute();

    sprintf(timestamp, "%02d:%02d\0", hour, minute);
}

void update_time(Rtc_Pcf8563 *rtc)
{
    // buf: hour, min, sec, day, mon, year 
    uint8_t buf[6];
    Serial.readBytes(buf, 6);

    rtc->setTime(buf[0], buf[1], buf[2]);
    // day, weekday(not used), mon, century (not used), year
    rtc->setDate(buf[3], 1, buf[4], false, buf[5]);

    Serial.println(rtc->formatTime());
    Serial.println(rtc->formatDate());
}

#endif 
