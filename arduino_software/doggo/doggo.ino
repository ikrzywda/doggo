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
              CODE_LENGTH = 4;

char input_buffer[BUFFER_SIZE],
     account_buffer[RECORD_SIZE];

unsigned new_user_id;

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
        read_input(true, CODE_LENGTH);
        search_field(1);
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

    if(search_field(0) == false)
    {
        append_field(input_buffer, false);
        read_input(true, CODE_LENGTH);
        append_field(input_buffer, false);
        sprintf(input_buffer, "%d", new_user_id);
        append_field(input_buffer, true);

        File f = SD.open(USERBASE, FILE_WRITE);
        f.print(account_buffer);
        f.close();

        new_user_id++;
    }

    account_buffer[0] = '\0';
}

void append_field(char field[],
                  bool last_field)
{
    uint8_t i = 0, 
            j = 0;
    
    for(; account_buffer[i] != '\0'; ++i);

    for(char c; (c = field[j]) != '\0'; ++j, ++i)
    {
        account_buffer[i] = c;
    }

    account_buffer[i++] = (last_field) ?
                        '\n' : ',';
    account_buffer[i] = '\0';
}

bool search_field(uint8_t col)
{
    File f = SD.open(USERBASE, FILE_READ);
    char buffer[BUFFER_SIZE],
         c;
    uint8_t i = 0,
            col_current = col;

    while((c = f.read()) != EOF)
    {
        switch(c)
        {
            case ',': col_current--;
                      break;
            
            case '\n': buffer[i] = '\0'; 
                       if(compare_to_input(buffer)) return true;
                       col_current = col;
                       i = 0;
                       break;

            default: if(col_current == 0) buffer[i++] = c;
                     break;
        }
    }

    f.close();
    return false;
}

bool compare_to_input(char buffer[])
{
    uint8_t i = 0;
    char c, d;

    while((c = buffer[i]) != '\0'
          && (d = input_buffer[i]) != '\0')
    {
        if(c != d) return false;
        ++i;
    }

    return true;
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

