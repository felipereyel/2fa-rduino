# 2fa-rduino

## Requirements

- Arduino CLI
    - m5stack core (`arduino-cli core install m5stack:esp32`)
    - M5AtomS3 lib (`arduino-cli lib install M5AtomS3`)
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