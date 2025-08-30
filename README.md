# 2fa-rduino

## Requirements

- M5 Board
    - M5Core

- Arduino CLI

    - m5stack core (`arduino-cli core install m5stack:esp32@2.1.4`)
    - M5 grafics lib (`arduino-cli lib install M5GFX`)
    - M5 lib (`arduino-cli lib install M5Unified`)
    - TOTP lib (`arduino-cli lib install TOTP-RC6236-generator`)
    - enable git libs (`arduino-cli config set library.enable_unsafe_install true`)
    - BLE lib (`arduino-cli lib install --git-url https://github.com/T-vK/ESP32-BLE-Keyboard.git`)

## Compile and upload

```
    make firmware-compile
    make firmware-upload

    # Or both
    make firmware
```

## TODOs

- Fix time (sync from host)