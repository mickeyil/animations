// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            10

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      20

float hsv_arr[NUMPIXELS * 3] = {0};

int idx_group[] = {4, 8, 12, 16};

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 10; // delay for half a second

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  // Serial.begin(9600);      // open the serial port at 9600 bps:    
  pixels.begin(); // This initializes the NeoPixel library.
}



/// Color helpers: hsv2rgb
float fract(float x) { return x - int(x); }

float mix(float a, float b, float t) { return a + (b - a) * t; }

float step(float e, float x) { return x < e ? 0.0 : 1.0; }

float* hsv2rgb(float h, float s, float b, float* rgb) {
  rgb[0] = b * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s) * 255;
  rgb[1] = b * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s) * 255;
  rgb[2] = b * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s) * 255;
  return rgb;
}


/// Animations
void constant(float *hsv_arr, int sz, float h, float s, float v)
{
  for (int i = 0; i < sz; i++) {
    hsv_arr[3*i] = h;
    hsv_arr[3*i + 1] = s;
    hsv_arr[3*i + 2] = v;
  }
}

void toggle(float *hsv_arr, int sz, unsigned long ms_count, int duration_ms, int duty_cyc_ms, int *indices, int indices_sz)
{
  int t = ms_count % duration_ms;
  //Serial.print("t = ");
  //Serial.print(t);
  //Serial.print("\n");
  
  if (t <= duty_cyc_ms) {
    for (int i = 0; i < indices_sz; i++) {
      int gidx = indices[i];
      hsv_arr[3*gidx] = 0.0;
      hsv_arr[3*gidx+1] = 0.0;
      hsv_arr[3*gidx+2] = 1.0;
    }
  }
}

void draw_arr(float * hsv_arr, int sz)
{
  for (int i = 0; i < sz; i++) {
    float rgb[3];
    hsv2rgb(hsv_arr[3*i], hsv_arr[3*i+1], hsv_arr[3*i+2], rgb);
    pixels.setPixelColor(i, pixels.Color((int)rgb[0], (int)rgb[1], (int)rgb[2])); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
  }  
}

void animation_flight_mode(unsigned long t_ms, int duration_ms, int *indices, int indices_sz)
{
    // draw background
    constant(hsv_arr, NUMPIXELS, 0.6, 1.0, 0.1);

    // activate animation
    toggle(hsv_arr, NUMPIXELS, t_ms, duration_ms, 20, indices, indices_sz);
}

void loop() {
    unsigned long t_ms = millis();

    animation_flight_mode(t_ms, 2000, idx_group, sizeof(idx_group)/sizeof(idx_group[0]));
    
    // draw result
    draw_arr(hsv_arr, NUMPIXELS);
}

