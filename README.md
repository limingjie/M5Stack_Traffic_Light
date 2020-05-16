# M5Stack Traffic Light

A traffic light toy build by M5Stack.

![M5Stack Traffic Light Video](images/M5Stack_Traffic_Light.gif)

## Build

- Setup Arduino development environment follow [M5Stack Docs - Arduino IDE Development](https://docs.m5stack.com/#/en/arduino/arduino_development).
- Install Adafruit NeoPixel library, supports up to 1.2.5, it does not work with 1.3.0 or above. There was a bug of the m5stack's light strip control, I will try the latest version of m5stack library when I have time.

## Customize The Traffic Light

Find the traffic light sequence definition in `TrafficLight.ino`. Customize the sequence in `lights` array.

```c++
// Traffic light struct
typedef struct
{
    int color_index; // 1 - Red, 2 - Yellow, 3 - Green
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
```

