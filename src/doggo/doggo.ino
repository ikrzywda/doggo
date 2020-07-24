#include <SPI.h>
#include <SD.h>

#define CHIP_SELECT 4
#define BUFFER_SIZE 50
#define CR 0x0D

#define READING 0x01
#define WB1 0x02		/* Written Buffer 1*/
#define WB2 0x03		/* Written Buffer 2 End */

#define TRANS_TRUE 0x10
#define TRANS_FALSE 0x11

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

byte add_user(char input_c, byte status){
	static char local_input_buffer[BUFFER_SIZE];
	static byte i = 0, local_status = 0;
	
	if(status == TRANS_TRUE){
		local_input_buffer[i] = input_c;
		++i;
		return READING;
	} else if(status == TRANS_FALSE){
		++local_status;
		switch(local_status){
			case 1:
				Serial.print("entered username:\n");
				Serial.print(local_input_buffer);
				memset(local_input_buffer, 0, BUFFER_SIZE);
				i = 0;
				return WB1;
			case 2:
				Serial.print("entered code:\n");
				Serial.print(local_input_buffer);
				memset(local_input_buffer, 0, BUFFER_SIZE);
				local_status = i = 0;
				return WB2;
		}
	}
}

void loop(){	
	char input_buffer[BUFFER_SIZE];
	char code_buffer[4];
	char input_c;
	static byte i = 0;
	static byte status = 0;
	while(Serial.available()){
		input_c = Serial.read();
		switch(input_c){
			case CR:
				Serial.print('\n');
				Serial.print(add_user('\0', TRANS_FALSE)); 
				i = 0;
				break;
			case '1':
				Serial.print("Adding new user\n");
				status = TRANS_TRUE;
			default:
				Serial.print(input_c);
				add_user(input_c, status);
				++i;
				break;
		}
	} 
}	

