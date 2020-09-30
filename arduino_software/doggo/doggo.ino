#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

#define USERS_FILE "/doggo/users.csv"
#define CHIP_SELECT 10
#define BUFFER_SIZE 17
#define MAX_RECORD_SIZE 50
#define FIELD_SIZE 3
#define CRAWL_BUTTON 3
#define BACKSPACE_BUTTON 4
#define DWN_BUTTON 5
#define UP_BUTTON 6
#define CR_BUTTON 7     

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

char* read_input(kbl* in, int but_crawl);
char* get_record(File f);
char* get_field(File f, char* record, byte record_index);
user* read_login_info();
byte add_user();
void test_routine();
void print_message_to_lcd(char* text);

LiquidCrystal_I2C lcd(0x27, 16, 2);

kbl input;

void setup(){
    Serial.begin(115200);
    while(!Serial);

    if(!SD.begin(CHIP_SELECT)){
        Serial.println("Initialization failed!");
        while(1);
    }

    Serial.println("Initialization successful!");
    Serial.println("Testing!");

    test_routine();
    Serial.println("Test done!");

    pinMode(CRAWL_BUTTON, INPUT);
    pinMode(BACKSPACE_BUTTON, INPUT);
    pinMode(CR_BUTTON, INPUT);
    pinMode(UP_BUTTON, INPUT);
    pinMode(DWN_BUTTON, INPUT);

    input.up = UP_BUTTON;
    input.down = DWN_BUTTON;
    input.backspace = BACKSPACE_BUTTON;
    input.carriage_return = CR_BUTTON;
    input.num = 0;

    lcd.init();                   
    lcd.backlight();

}

void loop(){
    if(digitalRead(DWN_BUTTON)){
        delay(200);
        add_user();
    }else{
        lcd.setCursor(0,0);
        lcd.print("DOGGO");
    }
}

char* get_record(File f){
    char *record = malloc(sizeof(char) * MAX_RECORD_SIZE);
    int c, i = 0;
    while((c = f.read()) != '\n'){
        if(c == EOF) return NULL;
        *(record + i) = c;
        ++i;
    }
    *(record + i) = '\0';
    return record;
}

char* get_field(char *record, char delimiter, unsigned field_index){
    char *field = malloc(sizeof(char) * FIELD_SIZE);
    char c, *ptr = field;
    
    while((c = *record++) != '\0'){
        if(c == delimiter){
            field_index--; 
        }

        if(field_index == 0){
            if(c != ',' && c != ' ') *field++ = c;
        }
    }

    *field = '\0';
    return ptr;
}

void test_routine(){
    File f = SD.open(USERS_FILE, FILE_READ);
    char *record, *field;
    record = get_record(f);
    field = get_field(record, ',',  0);
    Serial.println(record);
    Serial.println(field);
    f.close();
    free(record);
    free(field);
}

char* read_input(kbl* in, int but_crawl){
    char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
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
            *(buffer + i) = ' ';
            current_pos = current_pos > 0 ? current_pos - 1 : 0;
            lcd.setCursor(current_pos, 1);
            lcd.write(' ');
        }else if(digitalRead(in -> carriage_return)){
            delay(200);
            *(buffer + i) = current_char;
            current_char = in -> num ? '0' : 'a';
            current_pos = current_pos < (BUFFER_SIZE - 1) ? current_pos + 1 : current_pos;
            i = i < (BUFFER_SIZE - 1) ? i + 1 : i;;
        }else if(digitalRead(but_crawl)){
            delay(200);
            *(buffer + i) = '\0';
            return buffer;
        }
    }
}

void print_message_to_lcd(char* text){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text);
}

user* read_login_info(){
    user *usr = malloc(sizeof(usr));  
    print_message_to_lcd("LOGIN:");
    usr -> login = read_input(&input, CRAWL_BUTTON);
    input.num = 1;
    print_message_to_lcd("CODE:");
    usr -> code = read_input(&input, CRAWL_BUTTON);
    input.num = 0;
    return usr;
}

byte find_field(char* search_field){
    char *record = malloc(sizeof(char) * MAX_RECORD_SIZE);
    char *field = malloc(sizeof(char) * BUFFER_SIZE);

    free(record);
    free(field);
    return 1;
}

byte add_user(){
    user *usr = read_login_info();
    
    free(usr);
    return 1;
}

