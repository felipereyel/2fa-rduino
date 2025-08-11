firmware-compile:
	arduino-cli compile --fqbn m5stack:esp32:m5stack_atoms3  firmware 

firmware-upload:
	arduino-cli upload -p /dev/cu.usbmodem101 --fqbn m5stack:esp32:m5stack_atoms3  firmware 

firmware: firmware-compile firmware-upload