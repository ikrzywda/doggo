#ifndef CSV_H
#define CSV_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

const size_t SIZE_INPUT_STR = 13,
             SIZE_INPUT_NUM = 5,
             SIZE_USER_ID = 6,
             SIZE_RECORD = 23;

inline bool get_record(File f,
                       char record_buffer[],
                       size_t record_size)
{
    int8_t c;
    uint8_t i = 0; 

    for(c = f.read(); 
        c != '\n'; 
        c = f.read(), i = (i < record_size - 2) ? i + 1 : i)
    {
        if(c == EOF)
        {
            record_buffer[i] = '\n';
            record_buffer[i] = '\0';
            return false;
        }
        record_buffer[i] = c;
    }

    record_buffer[i] = '\n';
    record_buffer[i+1] = '\0';
    return true;
} 

bool find_field(char field[],
                char record[])
{
    char temp_buffer[SIZE_INPUT_STR];
    uint8_t i = 0, 
            j = 0; 
    
    for(char c = record[i]; 
        c != '\0' && j < SIZE_INPUT_STR;
        ++i, c = record[i])
    {
        if(c == ',' || c == '\n')
        {
            temp_buffer[j] = '\0';

            if(!strcmp(temp_buffer, field)) { return true; }
            j = 0;
        }
        else { temp_buffer[j++] = c; }
    }
    
    return false;
}

bool get_record_by_field(char filepath[], 
                         char field[],
                         char record[],
                         size_t record_size)
{
    File f = SD.open(filepath, FILE_READ);
    
    while(get_record(f, record, SIZE_RECORD))
    {
        if(find_field(field, record))
        {
            f.close();
            return true;
        }
    }   
    
    f.close();
    return false;
}

void append_record(char filepath[],
                   char buffer_record[])
{
    File f = SD.open(filepath, FILE_WRITE);
    f.print(buffer_record);
    f.close();
}

#endif
