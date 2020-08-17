#include <SPI.h>
#include <SD.h>

#define CHIP_SELECT 4
#define BUFFER_SIZE 50

#define INPUT_1 0x02
#define INPUT_2 0x01

#define SPLIT '+'
#define TERMINATE '|'

byte str_cmp(char* str1, char* str2);
unsigned long is_unique(char* in_str);
byte log_in(char* usr_name);
void add_user(char* buffer);
void print_file();

void setup(){
	Serial.begin(115200);

	while(!Serial)
		;
	
	Serial.print("Initializing SD...\n\r");
	
	if(!SD.begin(CHIP_SELECT)){
		Serial.print("Initialization failed\n\r");
		while(1);
	} 

	Serial.print("Card initialization successful\n\r");
}

void print_file(){
	File f = SD.open("/doggo/users.csv", FILE_READ);
	do{
		Serial.print((char)f.read());
	} while(f.peek() != EOF);
	f.close();
}

byte str_cmp(char* str1, char* str2){
	Serial.println("input strings:");
	Serial.println(str1);
	Serial.println(str2);
	static byte len = 0;
	byte i = 0;
	while(*(str1 + i) != 0)
		if(*(str2 + i) != 0){
			++len;
			++i;
		} else {
			return 0;
		} 
	
	for(; i <= len ; ++i, --len)
		if(*(str1 + i) != *(str2 + i) || *(str1 + len) != *(str2 + len))
			return 0;
	
		
	return 1;
}

unsigned long is_unique(char* in_str){
	File root = SD.open("/doggo/users.csv", FILE_READ);
	unsigned long pos;
	char cmp[BUFFER_SIZE - 6];
	char c;
	byte i = 0;
	while((c = root.read()) != EOF){
		switch(c){
			case ',':
				if(str_cmp(in_str, cmp) == 0){
				memset(cmp, 0, i);
				i = 0;
				} else {
					root.close();
					return pos;
				}
				break;
			case '\n':
				memset(cmp, 0, i);	
				i = 0;
				break;
			default:
				cmp[i] = c;
				pos = root.position();
				++i;
				break;	
		}
	}
	root.close();
	return 0;
}

byte log_in(char* code, unsigned long pos){
	File f = SD.open("/doggo/users.csv", FILE_READ);
	byte i = 0;
	char code_buff[10];
	char c;
	f.seek(pos);
	
	Serial.print("\n\rpos = ");
	Serial.print((char)f.read());

	while((c = f.read()) != '\n'){
		code_buff[i] = c;
		++i;		
	}
	Serial.print("\r\nread code: ");
	Serial.print(code_buff);
	
	if(str_cmp(code_buff, code) == 0){
		f.close();
		return 0;
	}

	f.close();
	return 1;
}

void add_user(char* buffer){
	byte i = 0;
	char c;
	File root = SD.open("/doggo/users.csv", FILE_WRITE);	
	
	while((c = *(buffer + i)) != TERMINATE){
		if(c == SPLIT){
			root.write(',');
			++i;
		} else {
			root.write(c);
			++i;
		} 
	}
	root.write("\n");
	root.close();
}

void loop(){	
	char input_c;
	static char input_buffer[BUFFER_SIZE];
	byte mode_index;
	static byte i = 0;
	static byte mode[3];		/*0 - ADD_USR, 1 - START_RUN, 2 - STOP_RUN*/
	unsigned long pos;			/* position in file stream */
	while(Serial.available()){
		input_c = Serial.read();
		switch(input_c){
			case '1':
				Serial.print("\r\nadd user\r\nusername:\t");
				mode[0] = INPUT_1;	
				break;
			case '2':
				Serial.print("\r\nstart run\r\nusername:\t");
				mode[1] = INPUT_1;
				break;
			case '3':
				Serial.print("\r\nstop run\r\nusername:\t");
				mode[2] = INPUT_1;
				break;
			case '4':
				print_file();
				break;
			case '\r':
				for(mode_index = 0; mode[mode_index] == 0; ++mode_index)
					;
				switch(mode_index){
					case 0:
						if(mode[0] == INPUT_1){
							if(is_unique(input_buffer) > 0){
								input_buffer[i] = SPLIT;
								++i;
								Serial.print(input_buffer);	
								--mode[0];
								Serial.print("\r\ncode:\t");
							} else {
								mode[0] = 0;
								Serial.print("username taken!\n\r");
							}
						} else {
							input_buffer[i] = TERMINATE;
							++i;
							Serial.print(input_buffer);
							add_user(input_buffer);
							--mode[0];
							memset(input_buffer, 0, i);
							i = 0;
						}
						break;
					case 1:
						if(mode[1] == INPUT_1){
							Serial.print("input: ");
							Serial.print(input_buffer);
							Serial.print("\n\r");
							if(is_unique(input_buffer) > 0){
								pos = is_unique(input_buffer);
								memset(input_buffer, 0, i);
								i = 0;
								--mode[1];
								Serial.print("\r\ncode: ");
							} else {
								mode[1] = 0;
								memset(input_buffer, 0, i);
								i = 0;
								Serial.print("\r\nusername not found!\n");
							}
						} else {
							if(log_in(input_buffer, pos) == 1){
								memset(input_buffer, 0, i);
								Serial.print("\r\nlogged in!");
							} else {
								memset(input_buffer, 0, i);
								Serial.print("\r\nwrong code!");
							}
							--mode[1];
						}
						break;
					case 2:
						if(mode[2] == INPUT_1){
							Serial.print("input 1!");	
							--mode[2];
						} else {
							Serial.print("input 2!");	
							--mode[2];
						}
						break;
				}
				break;
			default:
				Serial.print(input_c);
				input_buffer[i] = input_c;
				++i;
				break;
		}
	} 
}	
