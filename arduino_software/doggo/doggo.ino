#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>


            /* LCD PROMPTS */
const char MSG_LOGIN[] PROGMEM =        "LOGIN:";
const char MSG_CODE[] PROGMEM =         "KOD:";
const char MSG_ADD_USR[] PROGMEM =      "NOWY PROFIL";
const char MSG_USR_ADDED[] PROGMEM =    "PROFIL DODANY";
const char MSG_USR_EXISTS[] PROGMEM =   "LOGIN ISNIEJE";
const char MSG_WRONG_CRED[] PROGMEM =   "ZLE DANE";
const char MSG_RUN_STARTED[] PROGMEM =  "MIŁEGO SPACERU!";
const char MSG_RUN_ENDED[] PROGMEM =    "DOBRA ROBOTA!";
const char MSG_DEFAULT[] PROGMEM =      "dogGo WITA!";
const char MSG_ERR_0[] PROGMEM =        "ERR:NO SD" ;

            /* FILEPATHS */
#define USERS_RECORDS_DIR "/doggo/records/"
#define USERS_FILE "/doggo/users.csv"

            /* DIGITAL PINS */
const byte CHIP_SELECT =    10;
const byte MAX_PATH_SIZE =  30;


/*          BUTTON LAYOUT

                BTN_2
        BTN_1           BTN_3       BTN_5
                BTN_4
*/

const byte BUTTONS[5] = {7,5,4,6,3};

            /* OTHER CONSTANTS */
const byte BUFFER_SIZE =        17;
const byte MAX_RECORD_SIZE =    50;

            /* BUFFERS */
char input_buffer[BUFFER_SIZE];

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
    for(byte i = 0; i < 5; ++i) pinMode(BUTTONS[i], INPUT);

    Serial.begin(115200);

    if(!SD.begin(CHIP_SELECT))
    {
        Serial.println(F("Initialization failed!"));
        while(1);
    }

    Serial.println(F("Initialization successful!"));

    lcd.init();                   
    lcd.backlight();
}

void loop()
{
    if(digitalRead(BUTTONS[3]))
    {
        delay(200);
        read_input(false);
        Serial.println(input_buffer);
    }
    else if(digitalRead(BUTTONS[1]))
    {
        delay(200);
        read_input(true);
        Serial.println(input_buffer);
    }
    else if(digitalRead(BUTTONS[4]))
    {
        delay(200);
        DEBUG_dump_sd(SD.open("/"), 0);
        Serial.print("\ndone\n");
    }
}

void read_input(bool read_num)
{

    char c_start = read_num ? '0' : 'A',
         c_rollback = read_num ? '9' : 'Z';
    bool input_read = false;
    char c_current = c_start,
         i = 0;

    do{
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

            if(i < BUFFER_SIZE-1) i++;

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

void DEBUG_dump_sd(File dir, byte tabs)
{
    File f = dir.openNextFile();
    while(f)
    {
        for (byte i = 0; i < tabs; i++) 
            Serial.print('\t');

        Serial.println(f.name());


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

