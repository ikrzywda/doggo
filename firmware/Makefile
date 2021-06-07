# PREREQUISITES: 
#  - arduino-cli
#  - screen 


compile:
	arduino-cli compile --fqbn arduino:avr:uno doggo --verbose
upload:
	arduino-cli upload --port /dev/ttyACM0 --fqbn arduino:avr:uno doggo --verbose
screen:
	screen /dev/ttyACM0 115200
test:
	arduino-cli compile --fqbn arduino:avr:uno doggo --verbose
	arduino-cli upload --port /dev/ttyACM0 --fqbn arduino:avr:uno doggo --verbose
	screen /dev/ttyACM0 115200
