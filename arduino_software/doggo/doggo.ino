#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

const char MSG_LOGIN[] PROGMEM = "LOGIN:",
           MSG_CODE[] PROGMEM = "KOD:",
           MSG_ADD_USR[] PROGMEM = "NOWY PROFIL",
           MSG_USR_ADDED[] PROGMEM = "PROFIL DODANY",
           MSG_USR_EXISTS[] PROGMEM = "LOGIN ISNIEJE",
           MSG_WRONG_CRED[] PROGMEM = "ZLE DANE",
           MSG_RUN_STARTED[] PROGMEM = "MIŁEGO SPACERU!",
           MSG_RUN_ENDED[] PROGMEM = "DOBRA ROBOTA!",
           MSG_DEFAULT[] PROGMEM = "dogGo WITA!",
           MSG_ERR_0[] PROGMEM = "ERR:NO SD";

const char WALKS_DIR[] PROGMEM = "/doggo/records/";
const char USERBASE[] = "usrs.csv";

const uint8_t CHIP_SELECT = 10;

/*          BUTTON LAYOUT
            =============
                BTN_1
        
        BTN_0           BTN_3    BTN_4
                
                BTN_2
*/

const uint8_t BUTTONS[5] = {7,5,4,6,3};

const size_t SIZE_BUFFER_USERNAME = 13,
             SIZE_BUFFER_PIN = 5,
             SIZE_BUFFER_RECORD = 23;

unsigned new_user_id = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup()
{

    for(uint8_t i = 0; i < 5; ++i) pinMode(BUTTONS[i], INPUT);

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
        new_user_add();
    }
    else if(digitalRead(BUTTONS[1]))
    {
        delay(200);
        
        test_get_record();
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

void test_get_record()
{
    char record_buffer[SIZE_BUFFER_RECORD],
         saught_field[SIZE_BUFFER_USERNAME];

    read_input(saught_field, SIZE_BUFFER_USERNAME, false);

    File f = SD.open(USERBASE, FILE_READ);

    while(get_record(f, record_buffer, SIZE_BUFFER_RECORD))
    {
        if(find_field(saught_field, record_buffer))
        {
            Serial.print(F("Field found!\n Record: "));
            Serial.println(record_buffer);
            Serial.print(F("Saught field: "));
            Serial.println(saught_field);
            
            break;
        }
    }

    f.close();
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

            *(buffer_input + i) = c_current;
            c_current = c_start;

            if(i < len) ++i;

            lcd.setCursor(i, 1);
        }
        else if(digitalRead(BUTTONS[4]))    // ACCEPT INPUT
        {
            delay(200);
            
            *(buffer_input + i) = '\0';
            input_read = true;
            lcd.clear();
        }
    } while(!input_read);
}

void new_user_add()
{
    char buffer_username[SIZE_BUFFER_USERNAME],
         buffer_pin_code[SIZE_BUFFER_PIN],
         buffer_record[SIZE_BUFFER_RECORD];

    read_input(buffer_username, SIZE_BUFFER_USERNAME, false);

    Serial.println("record_buffer:");
    Serial.print(buffer_record);
    read_input(buffer_pin_code, SIZE_BUFFER_PIN, true);
    
    Serial.println(buffer_username);
    Serial.println(buffer_pin_code);

    sprintf(buffer_record, "%d,%s,%s\n\0", 
            new_user_id, buffer_pin_code, buffer_username);

    append_record(USERBASE, buffer_record);
    ++new_user_id;
    Serial.print(buffer_record);
}

void append_record(char filename[],
                   char buffer_record[])
{
    File f = SD.open(filename, FILE_WRITE);
    f.print(buffer_record);
    f.close();
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

inline bool find_field(char field[],
                       char record[])
{
    char temp_buffer[SIZE_BUFFER_USERNAME];
    uint8_t i = 0, 
            j = 0; 
    
    for(char c = record[i]; 
        c != '\0' && j < SIZE_BUFFER_USERNAME;
        c = record[i], ++i)
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


inline unsigned count_users()
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

void DEBUG_dump_sd(File dir, 
                   uint8_t tabs)
{
    File f = dir.openNextFile();
    while(f)
    {
        for (uint8_t i = 0; i < tabs; i++) { Serial.print('\t'); }

        Serial.print(f.name());
        Serial.print(F(" : "));
        Serial.print(f.size());
        Serial.print(F("\n\r"));

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

