# 2fa-rduino

## Requirements

- M5 Board
    - M5Core

- Arduino CLI
    - m5stack core (`arduino-cli core install m5stack:esp32`)
    - M5 grafics lib (`arduino-cli lib install M5GFX`)
    - M5 lib (`arduino-cli lib install M5Unified`)
    - TOTP lib (`arduino-cli lib install TOTP-RC6236-generator`)

## Compile and upload

```
    make firmware-compile
    make firmware-upload

    # Or both
    make firmware
```

## TODOs

- Fix time (sync from host)