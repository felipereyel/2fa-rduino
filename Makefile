firmware-compile:
	arduino-cli compile --fqbn m5stack:esp32:m5stack_core  firmware 

firmware-upload:
	arduino-cli upload -p /dev/cu.usbserial-02010754 --fqbn m5stack:esp32:m5stack_core  firmware 

firmware: firmware-compile firmware-upload