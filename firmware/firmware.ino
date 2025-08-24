#include "M5Unified.h"
#include <Preferences.h>
#include <TOTP-RC6236-generator.hpp>

#define FONT_SIZE_SMALL 1
#define FONT_SIZE_MEDIUM 2
#define FONT_SIZE_LARGE 3
#define FONT_SIZE_XLARGE 4
#define PROGRESS_BAR_MARGIN 10
#define PROGRESS_BAR_HEIGHT 10
#define SECRET_SEPARATOR ':'
#define MAX_KEYS 5
#define STEP 30
#define DIGITS 6

// Global Variables

time_t ltime = 0;
time_t time_delta;

Preferences preferences;
String current_provider;
String current_seed;
u8_t current_index;

//// computed
String current_key;
String current_totp;

// TOTP

time_t now()
{
    return time(NULL) + time_delta;
}

void compute_totp()
{
    if (current_seed.isEmpty())
    {
        current_totp.clear();
        return;
    }

    String *otp = TOTP::currentOTP(current_seed, ltime, STEP, DIGITS, 0);
    current_totp = *otp;
    delete otp;
}

// Secrets Storage

bool sync_index_key_secret()
{
    current_provider.clear();
    current_seed.clear();
    current_totp.clear();
    current_key.clear();

    current_key.concat("key-");
    current_key.concat(String(current_index));

    String raw_secret = preferences.getString(current_key.c_str());
    if (raw_secret.isEmpty())
    {
        Serial.printf("No secret found for key: %s\n", current_key);
        return false;
    }

    for (size_t i = 0; i < raw_secret.length(); i++)
    {
        if (raw_secret[i] == SECRET_SEPARATOR)
        {
            current_provider.concat(raw_secret.substring(0, i));
            current_seed.concat(raw_secret.substring(i + 1));

            return true;
        }
    }

    return false;
}

void drop_secret()
{
    if (preferences.isKey(current_key.c_str()))
    {
        preferences.remove(current_key.c_str());
    }
}

void put_secret(String raw_secret)
{
    int sep_index = raw_secret.indexOf(SECRET_SEPARATOR);
    if (sep_index > 0)
    {
        Serial.printf("Setting secret for key: >%s<\n", current_key.c_str());
        preferences.putString(current_key.c_str(), raw_secret);
    }
}

// Grafics

void draw_totp_screen()
{
    if (current_seed.isEmpty() || current_provider.isEmpty() || current_totp.isEmpty())
    {
        return;
    }

    int current = (ltime) % STEP;
    float progress = (float)current / STEP;

    int width = M5.Display.width();
    int height = M5.Display.height();
    int bar_height = PROGRESS_BAR_HEIGHT;
    int bar_width = width - 2 * PROGRESS_BAR_MARGIN;

    int bg_color = M5.Display.color565(200, 200, 200);
    int bar_color = M5.Display.color565(0, 255, 0);
    if (progress > 0.8)
    {
        bar_color = M5.Display.color565(255, 0, 0);
    }
    else if (progress > 0.5)
    {
        bar_color = M5.Display.color565(255, 165, 0);
    }

    M5.Display.fillRect(10, height - bar_height - 10, bar_width, bar_height, bg_color);
    int filled_width = (current * bar_width) / STEP;
    M5.Display.fillRect(10, height - bar_height - 10, filled_width, bar_height, bar_color);

    M5.Display.setTextSize(FONT_SIZE_XLARGE);
    M5.Display.setCursor(85, 100);
    M5.Display.printf("%s\n", current_totp.c_str());
}

void draw_text_ui()
{
    M5.Display.clear();
    M5.Display.setCursor(0, 0);

    M5.Display.setTextSize(FONT_SIZE_MEDIUM);
    M5.Display.printf("slot %d/%d: ", current_index + 1, MAX_KEYS);

    auto isEmpty = current_provider.isEmpty() || current_seed.isEmpty();
    auto slotname = isEmpty ? "EMPTY" : current_provider;
    M5.Display.setTextSize(FONT_SIZE_MEDIUM);
    M5.Display.printf("%s\n", slotname);
}

// Arduino Setup and Loop

void setup()
{
    ltime = now();
    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);
    preferences.begin("M5", false);

    // Secrets
    current_index = 0;
    sync_index_key_secret();

    // Draw initial UI
    M5.Display.init();
    M5.Display.setBrightness(100);
    M5.Display.fillScreen(TFT_BLACK);
    draw_text_ui();

    Serial.println("M5 Initialized");
}

void loop()
{
    time_t ctime = now();

    if (Serial.available())
    {
        Serial.println("Serial Input Detected");
        String input = Serial.readStringUntil('\n');

        if (input.startsWith("sync "))
        {
            input.remove(0, 5);
            input.trim();
            time_delta = (time_t)input.toInt() - time(NULL);
        }
        else if (input.startsWith("set "))
        {
            input.remove(0, 4);
            input.trim();
            put_secret(input);
            sync_index_key_secret();
            draw_text_ui();
        }
        else if (input.startsWith("drop"))
        {
            drop_secret();
            sync_index_key_secret();
            draw_text_ui();
        }
    }

    if (M5.BtnA.wasClicked())
    {
        current_index = (current_index + 1) % MAX_KEYS;
        sync_index_key_secret();
        draw_text_ui();
    }

    if (M5.BtnA.isHolding())
    {
        if (current_totp.isEmpty())
        {
            return;
        }

        Serial.printf("%s: %s\n", current_provider.c_str(), current_totp.c_str());
        while (M5.BtnA.isHolding())
        {
            M5.delay(10);
            M5.update();
        }
    }

    if (ctime > ltime)
    {
        ltime = ctime;
        compute_totp();
        draw_totp_screen();
    }

    M5.delay(10);
    M5.update();
}