#include <M5Stack.h>
#include <Adafruit_NeoPixel.h>

#define LED_COUNT 10
#define LED_PIN 15
#define BLINK_INTERVAL 200

// Declare LED strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// LED strip colors
static uint32_t strip_colors[] = {
    strip.Color(0, 0, 0),     // black
    strip.Color(255, 0, 0),   // red
    strip.Color(255, 100, 0), // yellow
    strip.Color(0, 255, 0)};  // green

// LCD colors
static uint32_t lcd_colors[] = {BLACK, RED, YELLOW, GREEN};

// Traffic light struct
typedef struct
{
    int color_index;
    bool blink;
    unsigned long interval; // milliseconds
} Traffic_Light;

// Traffic light sequences
Traffic_Light lights[] = {
    {3, false, 99999},
    {3, true, 10999},
    {2, true, 10999},
    {1, false, 30999},
    {1, true, 10999}};

void draw_traffic_light(int color_index)
{
    // LCD Display
    M5.Lcd.fillRoundRect(15, 15, 90, 90, 45, color_index == 1 ? RED : LIGHTGREY);
    M5.Lcd.fillRoundRect(115, 15, 90, 90, 45, color_index == 2 ? YELLOW : LIGHTGREY);
    M5.Lcd.fillRoundRect(215, 15, 90, 90, 45, color_index == 3 ? GREEN : LIGHTGREY);

    // LED Strip
    strip.setBrightness(10);
    for (int i = 0; i < LED_COUNT; i++)
    {
        strip.setPixelColor(i, strip_colors[color_index]);
        strip.show();
        delay(2);
    }
}

void draw_count_down(unsigned long count_down, int color_index)
{
    static unsigned long last = 0;
    static char buffer[2];

    // Convert milliseconds to seconds
    count_down /= 1000;
    if (count_down != last)
    {
        M5.Lcd.setTextSize(15);

        // Clear last number
        M5.Lcd.setTextColor(BLACK);
        M5.Lcd.setCursor(120, 130);
        sprintf(buffer, "%02ld", last);
        M5.Lcd.print(buffer);

        // Show new number
        M5.Lcd.setTextColor(lcd_colors[color_index]);
        M5.Lcd.setCursor(120, 130);
        sprintf(buffer, "%02ld", count_down);
        M5.Lcd.print(buffer);

        last = count_down;
    }
}

void setup()
{
    // Initialize LED strip
    strip.begin();
    strip.show();
    strip.setBrightness(10);

    // Initialize M5Stack
    M5.begin(true, false, true);

    // Initialize LCD and display button indicators
    M5.Lcd.clear(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setCursor(50, 220);
    M5.Lcd.print("Red");
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setCursor(125, 220);
    M5.Lcd.print("Yellow");
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setCursor(221, 220);
    M5.Lcd.print("Green");

    draw_traffic_light(lights[0].color_index);
}

void loop()
{
    // Keep states
    static unsigned long now = 0;
    static unsigned long light_start = 0;
    static unsigned long blink_start = 0;
    static bool blink_dark = false;
    static uint32_t sequence = 0;
    static uint32_t size = sizeof lights / sizeof lights[0];

    // Time for all calculations
    if (now == 0)
    {
        now = light_start = blink_start = millis();
    }
    else
    {
        now = millis();
    }

    // Detect button press
    uint32_t last_sequence = sequence;
    M5.update();
    if (M5.BtnA.wasReleased())
    {
        sequence = 3;
    }
    else if (M5.BtnB.wasReleased())
    {
        sequence = 2;
    }
    else if (M5.BtnC.wasReleased())
    {
        sequence = 0;
    }

    // Change traffic light color
    if (sequence != last_sequence)
    {
        light_start = now;
        draw_traffic_light(lights[sequence].color_index);
    }

    // Time for next sequence
    if (now - light_start >= lights[sequence].interval)
    {
        light_start = now;
        blink_start = now;
        blink_dark = false;
        sequence = (sequence + 1) % size;
        draw_traffic_light(lights[sequence].color_index);
    }

    // Blink
    if (lights[sequence].blink && now - blink_start >= BLINK_INTERVAL)
    {
        blink_start = now;
        draw_traffic_light(blink_dark ? 0 : lights[sequence].color_index);
        blink_dark = !blink_dark;
    }

    draw_count_down(lights[sequence].interval - (now - light_start),
                    lights[sequence].color_index);

    delay(100);
}
