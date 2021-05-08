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
const char USERBASE[] = "/doggo/userbase.csv";

const uint8_t CHIP_SELECT = 10;

/*          BUTTON LAYOUT
            =============
                BTN_1
        
        BTN_0           BTN_3    BTN_4
                
                BTN_2
*/

const uint8_t BUTTONS[5] = {7,5,4,6,3};

const uint8_t BUFFER_SIZE = 13,
              RECORD_SIZE = 23,
              USERNAME_LENGTH = 12,
              CODE_LENGTH = 4,
              ROW_NUMBER_LENGTH = 5,
              USERNAME_FIELD_OFFSET = 0,
              CODE_FIELD_OFFSET = 13,
              ROW_NUMBER_FIELD_OFFSET = 18;

char input_buffer[BUFFER_SIZE],
     account_buffer[RECORD_SIZE];

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
    Serial.println(USERBASE);

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
        Serial.println(F("USERNAMES:"));
        search_field(USERNAME_FIELD_OFFSET);
        Serial.println(F("CODES:"));
        search_field(CODE_FIELD_OFFSET);
        Serial.println(F("ROW_NUMBER_LENGTH:"));
        search_field(ROW_NUMBER_FIELD_OFFSET);
    }
    else if(digitalRead(BUTTONS[4]))
    {
        delay(200);
        DEBUG_dump_sd(SD.open("/"), 0);
        Serial.print("\ndone\n");
    }
}


void read_input(bool read_num,
                uint8_t len)
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

            input_buffer[i] = c_current;
            c_current = c_start;

            if(i < len) i++;

            lcd.setCursor(i, 1);
        }
        else if(digitalRead(BUTTONS[4]))    // ACCEPT INPUT
        {
            delay(200);
            
            input_buffer[i] = '\0';
            input_read = true;
            lcd.clear();
        }
    }while(!input_read);
}

void add_new_user()
{
    read_input(false, USERNAME_LENGTH);
    append_field(input_buffer, USERNAME_LENGTH, false);

    read_input(true, CODE_LENGTH);
    append_field(input_buffer, CODE_LENGTH, false);
    append_field("10\0", ROW_NUMBER_LENGTH, true);

    Serial.print(account_buffer);

    File f = SD.open(USERBASE, FILE_WRITE);

    f.print(account_buffer);
    f.close();

    account_buffer[0] = '\0';
}

void append_field(char field[],
                  uint8_t width,
                  bool last_field)
{
    uint8_t i = 0, 
            j = 0;
    bool field_copied = false;
    
    for(; account_buffer[i] != '\0'; ++i);

    for(; j < width; ++j, ++i)
    {
        if(field[j] == '\0') field_copied = true;

        account_buffer[i] = (!field_copied) ? 
                              field[j] : '-';
    }

    account_buffer[i++] = (last_field) ?
                        '\n' : ',';
    account_buffer[i] = '\0';
}

bool search_field(uint8_t offset)
{
    File f = SD.open(USERBASE, FILE_READ);
    long pos = offset;
    char field_buffer[13],
         c;
    uint8_t i = 0;
    
    while(f.seek(pos))
    {
        while((c = f.read()) != '-'
              && c != '\n'
              && c != ',')
        {
            Serial.print(c);
            field_buffer[i] = c;
            ++i;
        }

        field_buffer[i] = '\0';
        Serial.println(field_buffer);
        Serial.println(i);
        Serial.println(pos);

        i = 0;
        pos += RECORD_SIZE;
    }
    f.close();
}

void DEBUG_dump_sd(File dir, 
                   uint8_t tabs)
{
    File f = dir.openNextFile();
    while(f)
    {
        for (uint8_t i = 0; i < tabs; i++) 
            Serial.print('\t');

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

