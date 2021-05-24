#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

const char MSG_USERNAME_EXISTS[] = "NAZWA ISTNIEJE\0",
           MSG_YOUR_NUMBER[] = "TWOJ NUMER TO\n",
           MSG_NO_SIGNED_USER[] = "BRAK KONTA\0",
           MSG_WRONG_CODE[] = "ZLY KOD!\0",
           MSG_START_WALK[] = "MILEGO SPACERU!\0",
           MSG_END_WALK[] = "DZIEKUJEMY!\0";

const char WALKS_DIR[] = "/doggo/records/";
const char USERBASE[] = "usrs_v2.csv";

const uint8_t CHIP_SELECT = 10;

//            BUTTON LAYOUT
//            =============
//                BTN_1
//        
//        BTN_0           BTN_3    BTN_4
//                
//                BTN_2
//

const uint8_t BUTTONS[5] = {7,5,4,6,3};

const size_t SIZE_INPUT_STR = 13,
             SIZE_INPUT_NUM = 5,
             SIZE_USER_ID = 6,
             SIZE_RECORD = 23;

unsigned new_user_id = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup()
{

    for(uint8_t i = 0; i < 5; ++i) { pinMode(BUTTONS[i], INPUT); }

    Serial.begin(115200);

    if(!SD.begin(CHIP_SELECT))
    {
        Serial.println(F("Initialization failed!"));
        while(1);
    }

    Serial.println(F("Initialization successful!"));

    new_user_id = count_users();

    lcd.init();                   
    lcd.backlight();
}

void loop()
{
    if(digitalRead(BUTTONS[3]))
    {
        delay(200);
        add_new_user();
    }
    else if(digitalRead(BUTTONS[1]))
    {
        delay(200);
        log_in(true);
    }
    else if(digitalRead(BUTTONS[4]))
    {
        delay(200);

        File f = SD.open("/");
        DEBUG_dump_sd(SD.open("/"), 0);
        f.close();

        Serial.print("\ndone\n");
    }
}


void add_new_user()
{
    char username[SIZE_INPUT_STR],
         pin_code[SIZE_INPUT_NUM],
         user_id[SIZE_INPUT_NUM],
         record[SIZE_RECORD];
    
    read_input(username, SIZE_INPUT_STR, false);

    if(get_record_by_field(USERBASE, username, 
                           record, SIZE_RECORD))
    {
        lcd_prompt(MSG_USERNAME_EXISTS, "\0", 500);
        return;
    }

    read_input(pin_code, SIZE_INPUT_NUM, true);
  
    sprintf(user_id, "#%d\0", new_user_id);

    sprintf(record, "%s,%s,%s\n\0", 
            user_id, pin_code, username);

    append_record(USERBASE, record);
    ++new_user_id;

    lcd_prompt(MSG_YOUR_NUMBER, user_id, 1000);
}


void log_in(bool start)
{
    char input[SIZE_INPUT_NUM],
         user_id[SIZE_INPUT_NUM],
         record[SIZE_RECORD];
   
    read_input(input, SIZE_INPUT_NUM, true);

    sprintf(user_id, "#%s", input);

    if(!get_record_by_field(USERBASE, user_id, 
                           record, SIZE_RECORD))
    {
        lcd_prompt(MSG_NO_SIGNED_USER, "\0", 500);
        return;
    }

    read_input(input, SIZE_INPUT_NUM, true);

    if(!find_field(input, record))
    {
        lcd_prompt(MSG_WRONG_CODE, "\0", 500);
        return;
    }
    
    lcd_prompt((start) ? MSG_START_WALK : MSG_END_WALK, NULL, 500);
}


void read_input(char buffer_input[],
                size_t len,
                bool read_num)
{
    char c_start = read_num ? '0' : 'A',
         c_rollback = read_num ? '9' : 'Z',
         c_current = c_start;
    bool input_read = false;
    uint8_t i = 0;

    do
    {
        lcd.cursor();
        lcd.setCursor(i,1);
        lcd.write(c_current);

        if(digitalRead(BUTTONS[1]))         // GO CHARACTER UP
        {
            delay(200);
            c_current = (c_current < c_rollback)
                ? c_current + 1 
                : c_start;
        }
        else if(digitalRead(BUTTONS[3]))    // GO CHARACTER DOWN
        {
            delay(200);
            c_current = (c_current > c_start)
                ? c_current - 1 
                : c_rollback;
        }
        else if(digitalRead(BUTTONS[0]))    // BACKSPACE
        {
            delay(200);
            lcd.write(' ');

            c_current = ' ';
            if(i > 0) i--;
        }
        else if(digitalRead(BUTTONS[2]))    // ACCEPT CHARACTER
        {
            delay(200);

            buffer_input[i] = c_current;
            c_current = c_start;

            if(i < len) ++i;

            lcd.setCursor(i, 1);
        }
        else if(digitalRead(BUTTONS[4]))    // ACCEPT INPUT
        {
            delay(200);
            
            buffer_input[i] = '\0';
            input_read = true;
            lcd.clear();
        }
    } while(!input_read);
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


inline void append_record(char filepath[],
                   char buffer_record[])
{
    File f = SD.open(filepath, FILE_WRITE);
    f.print(buffer_record);
    f.close();
}


unsigned count_users()
{
    unsigned number = 0;
    char c;
    File f = SD.open(USERBASE, FILE_READ);

    while((c = f.read()) != EOF)
    {
        if(c == '\n') number++;
    }
    
    return number;
}

void lcd_prompt(char row_upper[], 
                       char row_lower[], 
                       unsigned time)
{
    lcd.setCursor(0,0);
    lcd.print(row_upper);

    lcd.setCursor(0,1);
    lcd.print(row_lower);

    delay(time);
    lcd.clear();
}

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
