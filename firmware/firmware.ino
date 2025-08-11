#include "M5AtomS3.h"
#include <Preferences.h>

#define FONT_SIZE_SMALL 1
#define FONT_SIZE_MEDIUM 2
#define FONT_SIZE_LARGE 3
#define PROGRESS_BAR_MARGIN 10
#define PROGRESS_BAR_HEIGHT 10
#define SECRET_SEPARATOR ':'
#define MAX_KEYS 5

// Global Variables

u_int32_t last_time = 0;

// Secrets Storage

Preferences preferences;
String current_provider;
String current_seed;
u8_t current_index;
String current_key;

bool sync_index_key_secret()
{
    current_provider.clear();
    current_seed.clear();
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

void draw_progress_bar(LovyanGFX *gfx, int current, int total)
{
    int width = gfx->width();
    int height = gfx->height();
    int bar_height = PROGRESS_BAR_HEIGHT;
    int bar_width = width - 2 * PROGRESS_BAR_MARGIN;
    float progress = (float)current / total;

    int bg_color = gfx->color565(200, 200, 200);
    int bar_color = gfx->color565(0, 255, 0);
    if (progress > 0.8)
    {
        bar_color = gfx->color565(255, 0, 0);
    }
    else if (progress > 0.5)
    {
        bar_color = gfx->color565(255, 165, 0);
    }

    gfx->fillRect(10, height - bar_height - 10, bar_width, bar_height, bg_color);
    int filled_width = (current * bar_width) / total;
    gfx->fillRect(10, height - bar_height - 10, filled_width, bar_height, bar_color);
}

void draw_text_ui()
{
    AtomS3.Display.clear();
    AtomS3.Display.setCursor(0, 0);

    AtomS3.Display.setTextSize(FONT_SIZE_SMALL);
    AtomS3.Display.printf("slot %d/%d\n\n", current_index + 1, MAX_KEYS);

    auto isEmpty = current_provider.isEmpty() || current_seed.isEmpty();
    auto slotname = isEmpty ? "EMPTY" : current_provider;
    AtomS3.Display.setTextSize(FONT_SIZE_MEDIUM);
    AtomS3.Display.printf("%s\n", slotname);

    if (!isEmpty)
    {
        AtomS3.Display.setTextSize(FONT_SIZE_LARGE);
        AtomS3.Display.print("\nSeed");
    }
}

// Arduino Setup and Loop

void setup()
{
    last_time = millis();
    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    AtomS3.begin(cfg);
    preferences.begin("M5AtomS3", false);

    // Secrets
    current_index = 0;
    sync_index_key_secret();

    // Draw initial UI
    draw_text_ui();

    Serial.println("M5AtomS3 Initialized");
}

void loop()
{
    auto time = millis();

    if (Serial.available())
    {
        Serial.println("Serial Input Detected");
        String input = Serial.readStringUntil('\n');
        if (input.startsWith("set "))
        {
            input.remove(0, 4); // Remove "set "
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

    if (M5.BtnA.wasPressed())
    {
        current_index = (current_index + 1) % MAX_KEYS;
        sync_index_key_secret();
        draw_text_ui();
    }

    if (time - last_time > 1000)
    {
        last_time = time;
        int progress = (time / 1000) % 30;
        draw_progress_bar(&AtomS3.Display, progress, 30);
    }

    M5.update();
}