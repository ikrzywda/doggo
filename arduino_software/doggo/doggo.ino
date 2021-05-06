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
char record_buffer[MAX_RECORD_SIZE];
char field_buffer[BUFFER_SIZE];
char path_buffer[MAX_PATH_SIZE];

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
}

/*void print_message_to_lcd(char* text, int display_time){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text);
    if(display_time > 0)
        delay(display_time);
}*/

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

        if(digitalRead(BUTTONS[1]))     // GO CHARACTER UP
        {
            delay(200);
            c_current = c_current < c_rollback ? c_current + 1 : c_start;
            Serial.println("up");
        }
        else if(digitalRead(BUTTONS[3]))    // GO CHARACTER DOWN
        {
            delay(200);
            c_current = c_current > c_start ? c_current - 1 : c_rollback;
            Serial.println("down");
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
            Serial.println((byte)i);
        }
        else if(digitalRead(BUTTONS[4]))    // ACCEPT INPUT
        {
            delay(200);
            
            i++;
            input_buffer[i] = '0';
            input_read = true;
            lcd.clear();
        }
    }while(!input_read);

}

/*void read_login_info(){
    print_message_to_lcd(MSG_LOGIN, 0);
    strcpy(usr.login, read_input(&input, 0));
    print_message_to_lcd(MSG_CODE, 0);
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
        load_field_to_buffer(0);                  
        Serial.println(field_buffer);
        if(!strcmp(usr.login, field_buffer)){
            load_field_to_buffer(1);             
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
    f.close();
}

byte add_user(){
    File f;
    char record[2 * BUFFER_SIZE + 1];
    print_message_to_lcd(MSG_ADD_USR, 2000);
    read_login_info(); 
    if(!find_login()){
        f = SD.open(USERS_FILE, FILE_WRITE);
        sprintf(record, "%s,%s\n\0", usr.login, usr.code);
        f.print(record);
        f.close();
        make_record_path();
        add_user_file(path_buffer);
        print_file(USERS_FILE);
        print_message_to_lcd(MSG_USR_ADDED, 2000);
        return 1;
    }else{
        print_message_to_lcd(MSG_USR_EXISTS, 2000);
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
    print_message_to_lcd(MSG_RUN_STARTED, 2000);
}

void log_time_end(){
    File f = SD.open(path_buffer, FILE_WRITE);
    sprintf(record_buffer, "%ld\n\0", millis()); 
    f.print(record_buffer);
    f.close();
    print_file(path_buffer);
    print_message_to_lcd(MSG_RUN_ENDED, 2000);
}

void login(){
    read_login_info();
    if(check_login_info()){
        make_record_path();
        if(check_walking_state())
            log_time_start();
        else
            log_time_end();
    }else{
        print_message_to_lcd(MSG_WRONG_CRED, 2000); 
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
}*/
