#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

#define USERS_FILE "/doggo/users.csv"
#define CHIP_SELECT 10
#define BUFFER_SIZE 17
#define MAX_RECORD_SIZE 50
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

char record_buffer[MAX_RECORD_SIZE];
char field_buffer[BUFFER_SIZE];
char input_buffer[BUFFER_SIZE];
kbl input;
user usr;

char* read_input(kbl* in, int but_crawl);
char* load_record_to_buffer(File f);
char* load_field_to_buffer(unsigned field_index, char delimiter);
void read_login_info();
byte add_user();
void test_routine();
void print_message_to_lcd(char* text);

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup(){
    Serial.begin(115200);
    while(!Serial);

    delay(500);

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

char* load_record_to_buffer(File f){
    char *buffer_ptr = record_buffer;
    int c;
    while((c = f.read()) != '\n'){
        if(c == EOF) return NULL;
        *buffer_ptr++ = c;
    }
    *buffer_ptr = '\0';
    return record_buffer;
}

char* load_field_to_buffer(unsigned field_index, char delimiter){
    char *record_buffer_ptr = record_buffer;
    char *field_buffer_ptr = field_buffer;
    char c;
    while((c = *record_buffer_ptr++) != '\0'){
        if(c == delimiter)
            field_index--; 
        if(field_index == 0)
            if(c != ',' && c != ' ') *field_buffer_ptr++ = c;
    }
    *field_buffer_ptr = '\0';
    return field_index > 0 ? NULL : field_buffer;
}

void test_routine(){
    File f = SD.open(USERS_FILE, FILE_READ);
    load_record_to_buffer(f);
    load_field_to_buffer(0, ',');
    Serial.println(record_buffer);
    Serial.println(field_buffer);
    f.close();
}

char* read_input(kbl* in, int but_crawl){
    char *input_buffer_ptr = input_buffer;
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
            *(input_buffer_ptr + i) = ' ';
            current_pos = current_pos > 0 ? current_pos - 1 : 0;
            lcd.setCursor(current_pos, 1);
            lcd.write(' ');
        }else if(digitalRead(in -> carriage_return)){
            delay(200);
            *(input_buffer_ptr + i) = current_char;
            current_char = in -> num ? '0' : 'a';
            current_pos = current_pos < (BUFFER_SIZE - 1) ? current_pos + 1 : current_pos;
            i = i < (BUFFER_SIZE - 1) ? i + 1 : i;;
        }else if(digitalRead(but_crawl)){
            delay(200);
            *(input_buffer_ptr + i) = '\0';
            return input_buffer;
        }
    }
}

void print_message_to_lcd(char* text){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text);
}

void read_login_info(){
    print_message_to_lcd("LOGIN:");
    usr.login = read_input(&input, CRAWL_BUTTON);
    input.num = 1;
    print_message_to_lcd("CODE:");
    usr.code = read_input(&input, CRAWL_BUTTON);
    input.num = 0;
}

byte find_field(char* search_field){
    char *record = malloc(sizeof(char) * MAX_RECORD_SIZE);
    char *field = malloc(sizeof(char) * BUFFER_SIZE);

    free(record);
    free(field);
    return 1;
}

byte add_user(){
    read_login_info(); 
    File f = SD.open(USERS_FILE, FILE_READ);
    return 1;
}

