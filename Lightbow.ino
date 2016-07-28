#include <Adafruit_NeoPixel.h>

#define PLUGIN_BRIGHTNESS
#define PLUGIN_FPS
#define PLUGIN_SWITCHER

#ifdef PLUGIN_BRIGHTNESS
  #define ANALOG_INPUT 15
  #define ANALOG_HIGH 6
  #define ANALOG_LOW 2
  #define BRIGHTNESS_RESOLUTION 1024/256
  #define BRIGHTNESS_UPDATE_INTERVAL 1000
  
  int lastBrightness;
  unsigned long lastBrightnessUpdate = 0;
#endif //PLUGIN_BRIGHTNESS

#ifdef PLUGIN_SWITCHER
  #define BUTTON_INPUT 10
  #define BUTTON_LOW 12
  
  const int debounceDelay = 50; //change to define
  int buttonState;
  int lastButtonState = LOW;
  unsigned long lastDebounceTime = 0;
#endif //PLUGIN_SWITCHER

#ifdef PLUGIN_FPS
  #define FPS 60
  #define UPDATE_INTERVAL (1000 / FPS)
  
  unsigned long lastUpdate = 0;
#endif //PLUGIN_FPS

#define STRIP_OUT 17
#define NUMBER_OF_PIXELS 30

#define FORWARD 0
#define BACKWARD 1

#define SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_OF_PIXELS, STRIP_OUT);

int actionCounter = 0;
int actionDirection = 1;
unsigned long lastActionTime = 0;

uint32_t baseColors[] {
  strip.Color(0, 0, 0),
  strip.Color(255, 0, 0), 
  strip.Color(0, 255, 0),
  strip.Color(0, 0, 255),
  strip.Color(255, 255, 0),
  strip.Color(0, 255, 255),
  strip.Color(255, 0, 255),
  strip.Color(255, 255, 255)
};

enum Colors {
  black = 0,
  red,
  green,
  blue,
  yellow,
  cyan,
  magenta,
  white
};

uint32_t rainbow[] {
   strip.Color( 128, 0,   0  ),
   strip.Color( 130, 40,  40 ),
   strip.Color( 141, 83,  59 ),
   strip.Color( 153, 102, 117),
   strip.Color( 153, 102, 169),
   strip.Color( 140, 51,  148),
   strip.Color( 128, 0,   128),
   strip.Color( 101, 0,   155),
   strip.Color( 72,  0,   255),
   strip.Color( 4,   0,   208),
   strip.Color( 0,   68,  220),
   strip.Color( 1,   114, 226),
   strip.Color( 1,   159, 232),
   strip.Color( 11,  175, 162),
   strip.Color( 23,  179, 77 ),
   strip.Color( 0,   212, 28 ),
   strip.Color( 0,   255, 0  ),
   strip.Color( 128, 255, 0  ),
   strip.Color( 200, 255, 0  ),
   strip.Color( 255, 255, 0  ),
   strip.Color( 255, 219, 0  ),
   strip.Color( 255, 182, 0  ),
   strip.Color( 255, 146, 0  ),
   strip.Color( 255, 109, 0  ),
   strip.Color( 255, 73,  0  ),
   strip.Color( 255, 0,   0  ),
   strip.Color( 255, 0,   128),
   strip.Color( 255, 105, 180),
   strip.Color( 255, 0,   255),
   strip.Color( 168, 0,   185)  
};

const int NUMBER_OF_FUNCTIONS = 10;
int functionCounter = 0;
typedef void (* functionPointer) ();
functionPointer functionPointerArray[NUMBER_OF_FUNCTIONS];

void setup() {
  strip.begin();

  functionPointerArray[0] = ClearStrip;
  functionPointerArray[1] = SetStripWhite;
  functionPointerArray[2] = Flag;
  functionPointerArray[3] = SlowKitt;
  functionPointerArray[4] = FastKitt;
  functionPointerArray[5] = FastDisco;
  functionPointerArray[6] = FastWhiteStrobe;
  functionPointerArray[7] = FastPulse;
  functionPointerArray[8] = Rainbow;
  functionPointerArray[9] = FastForwardPacman;
  
  PrepareBrightnessPlugin();  
  UpdateBrightness();
  
  PrepareSwitcherPlugin();
}

void PrepareBrightnessPlugin() {
  #ifdef PLUGIN_BRIGHTNESS
  pinMode(ANALOG_HIGH, OUTPUT);
  pinMode(ANALOG_LOW, OUTPUT);
  pinMode(ANALOG_INPUT, INPUT);
  digitalWrite(ANALOG_HIGH, HIGH);
  digitalWrite(ANALOG_LOW, LOW);
  #endif //PLUGIN_BRIGHTNESS
}

void PrepareSwitcherPlugin() {
  #ifdef PLUGIN_SWITCHER
  pinMode(BUTTON_INPUT, INPUT_PULLUP);
  pinMode(BUTTON_LOW, OUTPUT);
  digitalWrite(BUTTON_LOW, LOW);  
  #endif //PLUGIN_SWITCHER
}

void loop() {  
  UpdateBrightness();
  
  CheckButton();
  
  Update();
}

void Update() {
  #ifdef PLUGIN_FPS
  if(millis() >= lastUpdate + UPDATE_INTERVAL) {
  #endif //PLUGIN_FPS
  
    strip.show();
    (*functionPointerArray[functionCounter])();

  #ifdef PLUGIN_FPS
    lastUpdate = millis();
  }
  #endif //PLUGIN_FPS
}

void UpdateBrightness() {
  #ifdef PLUGIN_BRIGHTNESS
  if(millis() >= lastBrightnessUpdate + BRIGHTNESS_UPDATE_INTERVAL) {
    int brightness = analogRead(ANALOG_INPUT);
    brightness /= BRIGHTNESS_RESOLUTION;
    if(brightness != lastBrightness) {
      lastBrightness = brightness;
      strip.setBrightness(brightness);
    }
  
    lastBrightnessUpdate = millis();
  }  
  #endif //PLUGIN_BRIGHTNESS
}

void CheckButton() {
  #ifdef PLUGIN_SWITCHER
  int reading = digitalRead(BUTTON_INPUT);

  if(reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
        ResetActionCounter();
        ClearStrip();
        
        AdvanceFunctionCounter();
      } 
    }
  }

  lastButtonState = reading;
  
  #endif //PLUGIN_SWITCHER
}

void AdvanceFunctionCounter() {
  functionCounter++;
  if(functionCounter >= NUMBER_OF_FUNCTIONS) {
    functionCounter = 0;
  }
}

void ClearStrip() {
  SetStripColor(0, 0, 0);
}

void SetStripColor(uint8_t R, uint8_t G, uint8_t B) {
  SetStripColor(strip.Color(R, G, B));
}

void SetStripColor(uint32_t color) {
  for(int i = 0; i < NUMBER_OF_PIXELS; i++) {
    strip.setPixelColor( i, color );
  }
}

void SetStripWhite() {
  SetStripColor(255, 255, 255);
}

void Flag() {
  for(int i = 0; i < NUMBER_OF_PIXELS; i++) {
    if(i < NUMBER_OF_PIXELS / 3) {
      strip.setPixelColor( i, 255, 121, 0 );
    } else if( i >= NUMBER_OF_PIXELS / 3 && i < NUMBER_OF_PIXELS / 3 * 2) {
      strip.setPixelColor( i, 255, 255, 255 );
    } else {
      strip.setPixelColor( i, 0, 149, 67 );
    }
  }
}

void Kitt(unsigned long actionTimeDelay, uint8_t R, uint8_t G, uint8_t B) {
  if(millis() > lastActionTime + actionTimeDelay) {
    ClearStrip();
    
    if(actionCounter < 0) {
      actionDirection = 1;
    } else if (actionCounter >= NUMBER_OF_PIXELS) {
      actionDirection = -1;
    }
    
    strip.setPixelColor( actionCounter, R, G, B );
    actionCounter += actionDirection;
    lastActionTime = millis();
  }
}

void SlowKitt() {
  Kitt(50, 255, 0 , 0);
}

void FastKitt() {
  Kitt(10, 255, 0, 0);
}

void ResetActionCounter() {
  actionCounter = 0;
}

void Random(unsigned long actionTimeDelay, int lowerLimit, int upperLimit) {
  if(millis() > lastActionTime + actionTimeDelay) {
    int dice = random(0, 100);
    if(dice < lowerLimit) {
      
    } else if( dice >= lowerLimit && dice < upperLimit) {
      strip.setPixelColor( random(0, NUMBER_OF_PIXELS), baseColors[random(0, 8)]);
    } else {
      strip.setPixelColor( random(0, NUMBER_OF_PIXELS), 0, 0, 0 );
    }    
    
    lastActionTime = millis();
  }
}

void FastDisco() {
  Random(25, 33, 66);
}

void Strobe(unsigned long actionTimeDelay, uint8_t R, uint8_t G, uint8_t B) {
   Strobe(actionTimeDelay, strip.Color(R, G, B));
}

void Strobe(unsigned long actionTimeDelay, uint32_t color) {
  if(millis() > lastActionTime + actionTimeDelay) {
    if(actionDirection == 1) {
      ClearStrip();
      actionDirection = -1;
    } else {
      SetStripColor(color);
      actionDirection = 1;
    }
    
    lastActionTime = millis();
  }
}

void FastWhiteStrobe() {
  Strobe(100, baseColors[white]);
}

void Rainbow() {
  for(int i = 0; i < NUMBER_OF_PIXELS; i++) {
    strip.setPixelColor(i, rainbow[i]);
  }
}

void Pulse(unsigned long actionTimeDelay, unsigned int resolution, uint8_t R, uint8_t G, uint8_t B) {
  if(millis() > lastActionTime + actionTimeDelay) {
    
    SetStripColor(
      R / resolution * actionCounter * actionTimeDelay,
      G / resolution * actionCounter * actionTimeDelay,
      B / resolution * actionCounter * actionTimeDelay );
    
    actionCounter += actionDirection;
    lastActionTime = millis();
  }
}

void FastPulse() {
  Pulse(25, 100, 255, 255, 255);
}

void AdvancePixels() {
  AttractPixels(NUMBER_OF_PIXELS);
}

void RetreatPixels() {
  AttractPixels(0);
}

void AttractPixels(uint8_t center) {
  for(int i = NUMBER_OF_PIXELS - 1; i > 0; i--) {
    if(i < center) {
      strip.setPixelColor(i, strip.getPixelColor( i - 1) );
    }
  }
  for(int i = 0; i < NUMBER_OF_PIXELS - 1; i++) {
    if(i > center) {
      strip.setPixelColor(i, strip.getPixelColor( i + 1) );
    }
  }
}

void Pacman(unsigned long actionTimeDelay, uint8_t dir, uint8_t pitch, uint32_t color) {
  if(millis() > lastActionTime + actionTimeDelay) {
    AttractPixels((1 - dir) * NUMBER_OF_PIXELS);
    if(actionCounter % (pitch + 1) == 0) {
      strip.setPixelColor((dir * NUMBER_OF_PIXELS) - dir, color);
    } else {
      strip.setPixelColor((dir * NUMBER_OF_PIXELS) - dir, baseColors[black]);
    }
    
    actionCounter++;
    lastActionTime = millis();
  }
}

void FastForwardPacman() {
  Pacman(100, FORWARD, 5, baseColors[yellow]);
}
