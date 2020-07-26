#include <SPI.h>
#include <SD.h>

#define CHIP_SELECT 4
#define BUFFER_SIZE 50
#define CR 0x0D

#define INPUT_1 0x02
#define INPUT_2 0x01

#define SPLIT '+'
#define TERMINATE '|'

void setup(){
	Serial.begin(9600);
	
	while(!Serial){
		;
	}

	Serial.print("Initializing SD...\n");
	
	if(!SD.begin(CHIP_SELECT)){
		Serial.print("Initialization failed\n");
		while(1);
	} 

	Serial.print("Card initialization successful\n");
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
	root.write('\n');
	root.close();
}

void loop(){	
	char input_c;
	static char input_buffer[BUFFER_SIZE];
	byte mode_index;
	static byte i = 0;
	static byte mode[3];		/*0 - ADD_USR, 1 - START_RUN, 2 - STOP_RUN*/
	while(Serial.available()){
		input_c = Serial.read();
		switch(input_c){
			case '1':
				Serial.print("\nadd user\nusername:\t");
				mode[0] = INPUT_1;	
				break;
			case '2':
				Serial.print("\nstart run\nusername:\t");
				mode[1] = INPUT_1;
				break;
			case '3':
				Serial.print("\nstop run\nusername:\t");
				mode[2] = INPUT_1;
				break;
			case CR:
				for(mode_index = 0; mode[mode_index] == 0; ++mode_index)
					;
				switch(mode_index){
					case 0:
						if(mode[0] == INPUT_1){
							input_buffer[i] = SPLIT;
							++i;
							Serial.print(input_buffer);	
							--mode[0];
							Serial.print("\ncode:\t");
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
							Serial.print("input 1!");	
							--mode[1];
						} else {
							Serial.print("input 2!");	
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

