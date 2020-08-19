compile:
	arduino-cli compile --fqbn arduino:avr:uno doggo --verbose
upload:
	arduino-cli upload --port /dev/ttyACM0 --fqbn arduino:avr:uno doggo --verbose
clean:
	rm -r doggo/build
screen:
	screen /dev/ttyACM0 115200
