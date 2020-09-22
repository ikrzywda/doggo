#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

#define BUFFER_SIZE 17

#define CRAWL_BUTTON 3
#define BACKSPACE_BUTTON 4
#define DWN_BUTTON 5
#define UP_BUTTON 6
#define CR_BUTTON 7     // carriage return button
#define IN_POT A3       // input potentiometer

typedef struct{
    int up;
    int down;
    int backspace;
    int carriage_return;
    char num;               /* 1 - numerical, 0 - alphabet */
}kbl;

typedef struct{
    char* login;
    char* code;
}user;

/* seperate struct for data record */

char* get_input(kbl* in, int but_crawl);
char add_user(user* usr);

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(){
    Serial.begin(115200);
    pinMode(CRAWL_BUTTON, INPUT);
    pinMode(BACKSPACE_BUTTON, INPUT);
    pinMode(CR_BUTTON, INPUT);
    pinMode(UP_BUTTON, INPUT);
    pinMode(DWN_BUTTON, INPUT);

    while(!Serial);

    lcd.init();                   
    lcd.backlight();
}

char* get_input(kbl* in, int but_crawl){
    char* buffer = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    char current_char, current_pos = 0, i = 0;
    current_char = in -> num ? '0' : 'a';
    while(1){
        lcd.setCursor(current_pos, 1);
        if(digitalRead(in -> up)){
            delay(200);
            if(in -> num)
                current_char = current_char == '9' ? '0' : current_char + 1;
            else
                current_char = current_char == 'z' ? 'a' : current_char + 1;
            lcd.cursor();
            lcd.write(current_char);
        }else if(digitalRead(in -> down)){
            delay(200);
            if(in -> num)
                current_char = current_char == '0' ? '9' : current_char - 1;
            else
                current_char = current_char == 'a' ? 'z' : current_char - 1;
            lcd.cursor();
            lcd.write(current_char);
        }else if(digitalRead(in -> backspace)){
            delay(200);
            lcd.write(' ');
            --i;
            *(buffer + (i * sizeof(char))) = ' ';
            current_pos = current_pos > 0 ? current_pos - 1 : 0;
            lcd.setCursor(current_pos, 1);
            lcd.write(' ');
        }else if(digitalRead(in -> carriage_return)){
            delay(200);
            *(buffer + (i * sizeof(char))) = current_char;
            current_char = in -> num ? '0' : 'a';
            current_pos = current_pos < (BUFFER_SIZE - 1) ? current_pos + 1 : current_pos;
            i = i < (BUFFER_SIZE - 1) ? i + 1 : i;;
        }else if(digitalRead(but_crawl)){
            delay(200);
            *(buffer + (i * sizeof(char))) = '\0';
            return buffer;
        }
    }
}

char add_user(user* usr){
    Serial.print("\n\rLOGIN: ");
    Serial.print(usr -> login);
    Serial.print("\n\rCODE: ");
    Serial.print(usr -> code);
}

void loop(){
    kbl input;
    input.up = UP_BUTTON;
    input.down = DWN_BUTTON;
    input.backspace = BACKSPACE_BUTTON;
    input.carriage_return = CR_BUTTON;
    input.num = 0;

    user usr;
    usr.login = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    usr.code = (char*)malloc(sizeof(char) * BUFFER_SIZE);

    if(digitalRead(DWN_BUTTON)){
        delay(200);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("LOGIN:");
        input.num = 0;
        usr.login = get_input(&input, CRAWL_BUTTON);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("CODE:");
        input.num = 1;
        usr.code = get_input(&input, CRAWL_BUTTON);
        add_user(&usr);
    }else{
        lcd.setCursor(0,0);
        lcd.print("DOGGO");
    }
}



