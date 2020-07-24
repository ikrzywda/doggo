compile:
	arduino-cli compile --fqbn arduino:avr:uno doggo --verbose | grep "Sketch uses"
upload:
	arduino-cli upload --port /dev/ttyACM0 --fqbn arduino:avr:uno doggo --verbose
