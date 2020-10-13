#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

#define USERS_RECORDS_DIR "/doggo/records/"
#define USERS_FILE "/doggo/users.csv"
#define HEADER_RECORD "time_in,time_out\n\0"
#define CHIP_SELECT 10
#define BUFFER_SIZE 17
#define MAX_RECORD_SIZE 50
#define MAX_PATH_SIZE 30
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
    int crawl_button;
}kbl;

typedef struct{
    char* login;
    char* code;
}user;

kbl input;
user usr;

unsigned long time;

char input_buffer[BUFFER_SIZE];
char record_buffer[MAX_RECORD_SIZE];
char field_buffer[BUFFER_SIZE];
char path_buffer[MAX_PATH_SIZE];

void print_message_to_lcd(char* text);
void read_input(kbl* in, char *input_buffer_ptr, char is_num);
void read_login_info(user* usr);
byte load_record_to_buffer(File f);
byte load_field_to_buffer(unsigned field_index, char delimiter);
byte find_field(File f, char* search_field, unsigned field_index);
byte find_login();
byte make_record_path();
byte add_user();
byte start_run();
byte end_run();
void login();
void print_file(char* filename);
void test_routine();

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup(){
    pinMode(CRAWL_BUTTON, INPUT);
    pinMode(BACKSPACE_BUTTON, INPUT);
    pinMode(CR_BUTTON, INPUT);
    pinMode(UP_BUTTON, INPUT);
    pinMode(DWN_BUTTON, INPUT);
    pinMode(CHIP_SELECT, OUTPUT);

    Serial.begin(115200);
    while(!Serial);

    if(!SD.begin(CHIP_SELECT)){
        Serial.println("Initialization failed!");
        while(1);
    }

    Serial.println("Initialization successful!");

    test_routine();

    input.up = UP_BUTTON;
    input.down = DWN_BUTTON;
    input.backspace = BACKSPACE_BUTTON;
    input.carriage_return = CR_BUTTON;
    input.crawl_button = CRAWL_BUTTON;

    usr.login = malloc(BUFFER_SIZE * sizeof(char));
    usr.code = malloc(BUFFER_SIZE * sizeof(char));

    lcd.init();                   
    lcd.backlight();
}

void loop(){
    if(digitalRead(DWN_BUTTON)){
        delay(200);
        add_user();
    }else if(digitalRead(UP_BUTTON)){
        delay(200);
        login();
    }else{
        lcd.setCursor(0,0);
        lcd.print("DOGGO");
    }
}

void print_message_to_lcd(char* text){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text);
}

char* read_input(kbl* in, char is_num){
    char *input_buffer_ptr = input_buffer;
    char current_char, current_pos = 0, i = 0;
    current_char = is_num ? '0' : 'a';
    while(1){
        lcd.setCursor(current_pos, 1);
        if(digitalRead(in -> up)){
            delay(200);
            if(is_num)
                current_char = current_char == '9' ? '0' : current_char + 1;
            else
                current_char = current_char == 'z' ? 'a' : current_char + 1;
            lcd.cursor();
            lcd.write(current_char);
        }else if(digitalRead(in -> down)){
            delay(200);
            if(is_num)
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
            current_char = is_num ? '0' : 'a';
            current_pos = current_pos < (BUFFER_SIZE - 1) ? current_pos + 1 : current_pos;
            i = i < (BUFFER_SIZE - 1) ? i + 1 : i;
        }else if(digitalRead(in -> crawl_button)){
            delay(200);
            *(input_buffer_ptr + i) = '\0';
            return input_buffer_ptr;
        }
    }
}

void read_login_info(){
    print_message_to_lcd("LOGIN:");
    strcpy(usr.login, read_input(&input, 0));
    print_message_to_lcd("CODE:");
    strcpy(usr.code, read_input(&input, 1));
}

byte load_record_to_buffer(File f){
    char *buffer_ptr = record_buffer;
    int c;
    while((c = f.read()) != '\n'){
        if(c == EOF){ 
            *buffer_ptr++ = 'E';
            *buffer_ptr = '\0';
            return 0;
        }
        *buffer_ptr++ = c;
    }
    *buffer_ptr = '\0';
    return 1;
}

byte load_field_to_buffer(unsigned field_index){
    char *record_buffer_ptr = record_buffer;
    char *field_buffer_ptr = field_buffer;
    char c;
    while((c = *record_buffer_ptr++) != '\0'){
        if(c == ',')
            field_index--; 
        if(field_index == 0)
            if(c != ',' && c != ' ' && c != '\n' && c != '\r')
                *field_buffer_ptr++ = c;
    }
    *field_buffer_ptr = '\0';
    return field_index > 0 ? 0 : 1;
}

byte find_field(File f, char* search_field, unsigned field_index){
    while(load_record_to_buffer(f)){
        load_field_to_buffer(field_index);
        if(!strcmp(search_field, field_buffer))
            return 1;
    }
    return 0;
}

byte find_login(){
    byte is_found;
    File f = SD.open(USERS_FILE, FILE_READ);
    is_found = find_field(f, usr.login, 0);
    f.close();
    return is_found;
}

byte check_login_info(){
    File f = SD.open(USERS_FILE, FILE_READ);
    while(load_record_to_buffer(f)){
        load_field_to_buffer(0);                     /* load login field to buffer */ 
        Serial.println(field_buffer);
        if(!strcmp(usr.login, field_buffer)){
            load_field_to_buffer(1);                 /* load code field to buffer */
            Serial.println(field_buffer);
            if(!strcmp(usr.code, field_buffer)){
                f.close();
                return 1;
            }
        }
    }
    f.close();
    return 0;
}

byte make_record_path(){
    if(*(usr.login) == '\0'){
    Serial.println("No user selected");
    return 0;
    }
    sprintf(path_buffer, "%s%s.csv\0", USERS_RECORDS_DIR, usr.login);
    Serial.println(path_buffer);
    return 1;
}

void add_user_file(char* filepath){
    File f = SD.open(filepath, FILE_WRITE);
    f.print(HEADER_RECORD);
    f.close();
}

byte add_user(){
    File f;
    char record[2 * BUFFER_SIZE + 1];
    read_login_info(); 
    if(!find_login()){
        f = SD.open(USERS_FILE, FILE_WRITE);
        sprintf(record, "%s,%s\n\0", usr.login, usr.code);
        f.print(record);
        f.close();
        make_record_path();
        add_user_file(path_buffer);
        print_file(USERS_FILE);
        return 1;
    }else{
        print_file(USERS_FILE);
    }
    return 0;
}

byte check_walking_state(){
    File f = SD.open(path_buffer, FILE_READ);
    int c;
    byte walking_status = 1;
    Serial.println(path_buffer);
    while((c = f.read()) != EOF){
        if(c == ','){
            walking_status = 0;
        }else if(c == '\n'){
            walking_status = 1;
        }
    }
    f.close();
    return walking_status;
}

void log_time_start(){
    File f = SD.open(path_buffer, FILE_WRITE);
    sprintf(record_buffer, "%ld,\0", millis());
    f.print(record_buffer);
    f.close();
    print_file(path_buffer);
}

void log_time_end(){
    File f = SD.open(path_buffer, FILE_WRITE);
    sprintf(record_buffer, "%ld\n\0", millis()); 
    f.print(record_buffer);
    f.close();
    print_file(path_buffer);
}

void login(){
    read_login_info();
    if(check_login_info()){
        make_record_path();
        print_message_to_lcd("RUN STARTED"); 
        if(check_walking_state())
            log_time_start();
        else
            log_time_end();
    }else{
        print_message_to_lcd("WRONG INFO"); 
    }
}

void print_file(char* filename){
    Serial.print(filename);
    Serial.print(":\n\r");
    File f = SD.open(filename, FILE_READ);
    char c;
    while((c = f.read()) != EOF){
        Serial.write(c);
    }
    Serial.println("end");
    f.close();
}

void test_routine(){
    print_file("/doggo/test_2.csv");
    print_file("/doggo/test_file.txt");
}
