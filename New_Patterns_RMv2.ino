#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE, RANDOM_PIXELS, 
                FLASH_COLOR, SCROLL_COLOR, SCROLL_TWO_COLOR, SCROLL_TWO_COLOR_REVERSE, 
                SCROLL_MIDDLE, SCROLL_MIDDLE_TWO_COLOR, SCROLL_ENDS, SCROLL_ENDS_TWO_COLOR, 
                COLOR_BREATHE, COLOR_RAIN, BLOCK_DROP, COLOR_SWITCH, RANDOM_EATER, RANDOM_FILL};
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
    public:

    // Member Variables:  
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    
    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern
    int i;
    int Counter;
    int Counter2;
    
    void (*OnComplete)();  // Callback on completion of pattern

    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
    :Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
    }

    // Update the pattern
    void Update()
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();
            switch(ActivePattern)
            {
                case RAINBOW_CYCLE:
                    RainbowCycleUpdate();
                    break;
                case THEATER_CHASE:
                    TheaterChaseUpdate();
                    break;
                case COLOR_WIPE:
                    ColorWipeUpdate();
                    break;
                case SCANNER:
                    ScannerUpdate();
                    break;
                case FADE:
                    FadeUpdate();
                    break;
                case RANDOM_PIXELS:
                    RandomPixelsUpdate();
                    break;
                case SCROLL_TWO_COLOR:
                    ScrollTwoColorUpdate();
                    break;
                case SCROLL_TWO_COLOR_REVERSE:
                    ScrollTwoColorReverseUpdate();
                    break;
                case SCROLL_MIDDLE:
                    ScrollMiddleUpdate();
                    break;
                case COLOR_BREATHE:
                    ColorBreatheUpdate();
                    break;
                case COLOR_RAIN:
                    ColorRainUpdate();
                    break;
                case BLOCK_DROP:
                    BlockDropUpdate();
                    break;
                case COLOR_SWITCH:
                    ColorSwitchUpdate();
                    break;
                case RANDOM_EATER:
                    RandomEaterUpdate();
                    break;
                case RANDOM_FILL:
                    RandomFillUpdate();
                    break;
             
//                case SCROLL_MIDDLE_TWO_COLOR:
//                    ScrollMiddleTwoColorUpdate();
//                    break;
//                case SCROLL_ENDS:
//                    ScrollEndsUpdate();
//                    break;
//                case SCROLL_ENDS_TWO_COLOR:
//                    ScrollEndsTwoColorUpdate();
//                    break;
//                case FLASH_COLOR:
//                    FlashColorUpdate();
//                    break;
//                case SCROLL_COLOR:
//                    ScrollColorUpdate();
//                    break;
                default:
                    break;
            }
        }
    }

    // Increment the Index and reset at the end
    void Increment()
    {
        if (Direction == FORWARD)
        {
           Index++;
           if (Index >= TotalSteps)
            {
                Index = 0;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
        else // Direction == REVERSE
        {
            --Index;
            if (Index <= 0)
            {
                Index = TotalSteps-1;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
    }

//////////////////// PATTERNS ////////////////////////////////////////////////////////////////////////

//------------------- Initialize for a RainbowCycle -----------------------------------------
    void RainbowCycle(uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = RAINBOW_CYCLE;
        Interval = interval;
        TotalSteps = 255;
        Index = 0;
        Direction = dir;
    }

    // Update the Rainbow Cycle Pattern
    void RainbowCycleUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
        }
        show();
        Increment();
    }

 //-------------------- Initialize for a ColorWipe----------------------------------------------
    void ColorWipe(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = COLOR_WIPE;
        Interval = interval;
        TotalSteps = numPixels()*2;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
    }

    // Update the Color Wipe Pattern
    void ColorWipeUpdate()
    {
        if(Index < numPixels()){
          setPixelColor(Index, Color1);
        }
        else{
          setPixelColor(TotalSteps-Index, Color2);
        } 
        show();
        Increment();
    }

//-------------------- THEATER CHASE ---------------------------------------------------  
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = THEATER_CHASE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
   }

   // Update the Theater Chase Pattern
    void TheaterChaseUpdate()
    {
      for(int i=0; i< numPixels(); i++)
        {
            if ((i + Index) % 3 == 0)
            {
                setPixelColor(i, Color1);
            }
            else
            {
                setPixelColor(i, Color2);
            }
        }
        show();
        Increment();
    }

//-------------------- SCANNNER PATTERN -------------------------------------------------
    void Scanner(uint32_t color1, uint8_t interval)
    {
        ActivePattern = SCANNER;
        Interval = interval;
        TotalSteps = (numPixels() - 1) * 2;
        Color1 = color1;
        Index = 0;
    }

    // Update the Scanner Pattern
    void ScannerUpdate()
    {   
      for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index) // first half of the scan
            {
                setPixelColor(i, Color1);
            }
            else if (i == TotalSteps - Index) // The return trip.
            {
                setPixelColor(i, Color1);
            }
            else  // fade to black
            {
                setPixelColor(i, DimColor(getPixelColor(i)));
            }
            
        }
        show();
        Increment();
    }

//-------------------- FADE -----------------------------------------
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = FADE;
        Interval = interval;
        TotalSteps = steps;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
    }

    // Update the Fade Pattern
    void FadeUpdate()
    {
        uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
        uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
        uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
        
        ColorSet(Color(red, green, blue));
        show();
        Increment();
    }
    


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Custom Patterns Begin              Custom Patterns Begin                        Custom Patterns Begin                Custom Patterns Begin
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------- Random Pixels -----------------------------------------
    void RandomPixels(int interval, int steps) {
      ActivePattern = RANDOM_PIXELS;
      Interval = interval;
      TotalSteps = steps;
    }

    // Update the Random Pixels pattern
    void RandomPixelsUpdate() {
      setPixelColor(random(numPixels()-1), Wheel(random(255)));
      setPixelColor(random(numPixels()-1), 0);
      show();
      Increment();
    }

//-------------------- Random Pixels -----------------------------------------++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  void RandomEater(uint32_t color1, uint32_t color2, int interval, direction dir = FORWARD){
    ActivePattern = RANDOM_EATER;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color1;
    Color2 = color2;
    Index = 0;
    Direction = dir;
  }
  
  void RandomEaterUpdate(){
    if(Index==0){
      CleanPixels();
    }
    else{
      //setPixelColor(random(TotalSteps-Index), Wheel(random(255)));
      setPixelColor(random(TotalSteps-Index), Color2);
      setPixelColor(TotalSteps-Index, Color1);
      setPixelColor(TotalSteps-Index+1, DimControl(Color1, 2));
      setPixelColor(TotalSteps-Index+2, DimControl(Color1, 4));
      setPixelColor(TotalSteps-Index+3, DimControl(Color1, 8));
      setPixelColor(TotalSteps-Index+4, DimControl(Color1, 15));
      setPixelColor(TotalSteps-Index+5, DimControl(Color1, 20));
      setPixelColor(TotalSteps-Index+6, DimControl(Color1, 30));
      setPixelColor(TotalSteps-Index+7, 0);
    }
    show();
    Increment();
  }
  

//-------------------- Two-color scroll (or ColorWipe) -----------------------------------------
    void ScrollTwoColor(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = SCROLL_TWO_COLOR;
        Interval = interval;
        TotalSteps = numPixels() + 3;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
    }

    // Update the Scroll Two Color Pattern
    void ScrollTwoColorUpdate()
    {
        if (Index == 0) {
          CleanPixels();
          setPixelColor(Index, Color1);
        }

        else if (Index == 1) {
          setPixelColor(Index, Color1);
          setPixelColor(Index-1, Color1);
        }
        else if (Index == 2) {
          setPixelColor(Index, Color1);
          setPixelColor(Index-1, Color1);
          setPixelColor(Index-2, Color1);
        }
        else if (Index == numPixels() - 1) {        //the last pixel
          setPixelColor(Index, Color1);
          setPixelColor(Index-1, Color1);
          setPixelColor(Index-2, Color1);
          setPixelColor(Index-3, Color2);
        }
        else if (Index == numPixels()) {
          setPixelColor(Index-1, Color1);
          setPixelColor(Index-2, Color1);
          setPixelColor(Index-3, Color2);
        }
        else if (Index == numPixels() + 1) {
          setPixelColor(Index-2, Color1);
          setPixelColor(Index-3, Color2);
        }
        else if (Index == numPixels() + 2) {
          setPixelColor(Index-3, Color2);
        }
        else {
          setPixelColor(Index, Color1);
          setPixelColor(Index-1, Color1);
          setPixelColor(Index-2, Color1);
          setPixelColor(Index-3, Color2);
        }
        show();
        Increment();
    }

//-------------------- Two color scroll in reverse -----------------------------------------
    void ScrollTwoColorReverse(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD) {
      ActivePattern = SCROLL_TWO_COLOR_REVERSE;
      Interval = interval;
      TotalSteps = numPixels() + 4;
      Color1 = color1;
      Color2 = color2;
      Index = 0;
      Direction = dir;
    }

    // Update the reverse two-color scroll
    void ScrollTwoColorReverseUpdate() {
      if (Index == 0) {
        CleanPixels();
      }
      setPixelColor(numPixels()-Index, Color1);
      setPixelColor(numPixels()-Index+1, Color1);
      setPixelColor(numPixels()-Index+2, Color1);
      setPixelColor(numPixels()-Index+3, Color2);
      
      show();
      Increment();
    }

//-------------------- Scroll out from middle to ends -----------------------------------------
    void ScrollMiddle(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD) {
      ActivePattern = SCROLL_MIDDLE;
      Interval = interval;
      TotalSteps = numPixels();
      Color1 = color1;
      Color2 = color2;
      Index = 0;
      Direction = dir;
    }

    // Update the scroll middle pattern
    void ScrollMiddleUpdate() {
      if (Index == 0) {
        CleanPixels();
      }
      else if(Index < numPixels()/2){
        setPixelColor((numPixels()/2)+Index, Color1);
        setPixelColor((numPixels()/2)-Index, Color1);
      }
      else{
        setPixelColor(Index-(numPixels()/2), Color2);
        setPixelColor(TotalSteps-(Index-(numPixels()/2)), Color2);
      }
      show();
      Increment();
    }
    
//-------------------- Single Color color dims from top to bottom ----------------------------
  void ColorRain(uint32_t color1, uint8_t interval, direction dir = FORWARD) {
      ActivePattern = COLOR_RAIN;
      Interval = interval;
      TotalSteps = numPixels()+5;
      Color1 = color1;
      Index = 0;
      Direction = dir;
  }
  
  void ColorRainUpdate(){
    int colorLess = .75;
    int colorLesser = .66;
    int colorLeast = .5; 
    
    for (int i = 0; i < numPixels(); i++) {
       setPixelColor(i, DimColorMore(Color1));
    }
    if (Index == 0) {
      setPixelColor(Index, Color1*colorLesser);
    }
    
    else if (Index == 1) {
      setPixelColor(Index+29, DimColorMore(Color1));
      setPixelColor(Index+30, DimColor(Color1));
      setPixelColor(Index+31, Color1);
      setPixelColor(Index+32, DimColor(Color1));
      setPixelColor(Index+33, DimColorMore(Color1));
      setPixelColor(Index, DimColorMore(Color1));
      setPixelColor(Index-1, DimColor(Color1));
    }
    else if (Index == 2) {
      setPixelColor(Index+29, DimColorMore(Color1));
      setPixelColor(Index+30, DimColor(Color1));
      setPixelColor(Index+31, Color1);
      setPixelColor(Index+32, DimColor(Color1));
      setPixelColor(Index+33, DimColorMore(Color1));
      setPixelColor(Index, DimColorMore(Color1));
      setPixelColor(Index-1, DimColor(Color1));
      setPixelColor(Index-2, Color1);
    }
    else if (Index == 3) {
      setPixelColor(Index+29, DimColorMore(Color1));
      setPixelColor(Index+30, DimColor(Color1));
      setPixelColor(Index+31, Color1);
      setPixelColor(Index+32, DimColor(Color1));
      setPixelColor(Index+33, DimColorMore(Color1));
      setPixelColor(Index, DimColorMore(Color1));
      setPixelColor(Index-1, DimColor(Color1));
      setPixelColor(Index-2, Color1);
      setPixelColor(Index-3, DimColor(Color1));
    }
    else if (Index == 4) {
      setPixelColor(Index+29, DimColorMore(Color1));
      setPixelColor(Index+30, DimColor(Color1));
      setPixelColor(Index+31, Color1);
      setPixelColor(Index+32, DimColor(Color1));
      setPixelColor(Index+33, DimColorMore(Color1));
      setPixelColor(Index, DimColorMore(Color1));
      setPixelColor(Index-1, DimColor(Color1));
      setPixelColor(Index-2, Color1);
      setPixelColor(Index-3, DimColor(Color1));
      setPixelColor(Index-5, DimColorMore(Color1));
    }
    else if (Index == 27) {
      setPixelColor(Index+29, DimColorMore(Color1));
      setPixelColor(Index+30, DimColor(Color1));
      setPixelColor(Index+31, Color1);
      setPixelColor(Index+32, DimColor(Color1));
      setPixelColor(Index+33, DimColorMore(Color1));
      setPixelColor(Index, DimColorMore(Color1));
      setPixelColor(Index-1, DimColor(Color1));
      setPixelColor(Index-2, Color1);
      setPixelColor(Index-3, DimColor(Color1));
      setPixelColor(Index-5, DimColorMore(Color1));
      setPixelColor(Index-6, DimColorMost(Color1));  
  }
    else {
      setPixelColor(Index-27, DimColorMore(Color1));
      setPixelColor(Index-28, DimColor(Color1));
      setPixelColor(Index-29, Color1);
      setPixelColor(Index-30, DimColor(Color1));
      setPixelColor(Index-31, DimColorMore(Color1));
      setPixelColor(Index, DimColorMore(Color1));
      setPixelColor(Index-1, DimColor(Color1));
      setPixelColor(Index-2, Color1);
      setPixelColor(Index-3, DimColor(Color1));
      setPixelColor(Index-5, DimColorMore(Color1));
      setPixelColor(Index-6, DimColorMost(Color1));
      setPixelColor(Index+29, DimColorMore(Color1));
      setPixelColor(Index+30, DimColor(Color1));
      setPixelColor(Index+31, Color1);
      setPixelColor(Index+32, DimColor(Color1));
      setPixelColor(Index+33, DimColorMore(Color1));
    }
    show();
    Increment();
    }
    
//-------------------- Color Breathe-------------------------------------------- 
  void ColorBreathe(uint32_t color1, uint8_t interval, direction dir = FORWARD) {
      ActivePattern = COLOR_BREATHE;
      Interval = interval;
      TotalSteps = numPixels();
      Color1 = color1;
      Index = 0;
      Direction = dir;
  }

  void ColorBreatheUpdate() {
   
    show();
    Increment();
}

//-------------------- Block Drop --------------------------------------------------
  void BlockDrop(uint32_t color1, uint8_t interval, direction dir = FORWARD){
      ActivePattern = BLOCK_DROP;
      Interval = interval;
      TotalSteps = (numPixels()*(numPixels()+1))/2;
      //TotalSteps = 1431;
      Color1 = color1;
      Index = 0;
      Direction = dir;
      i = 0; //keeps track of number which LEDs to light up
      Counter = 0; //keeps track of which particular LED is going to the bottom
      Counter2 = 0;
      
  }

  void BlockDropUpdate(){     
     if(Index==0){
       //CleanPixels();
       Counter=0;
       Counter2=0;
     }
    i = Index - Counter; 
    setPixelColor(i, Color1);
    setPixelColor(i-1, 0);
      
    if(i>numPixels()-2-Counter2){
      Counter = Index;
      Counter2++;
    }
    show();
    Increment();
       
  }
    

//-------------- COLOR SWITCH --------------------------

  void ColorSwitch(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD){
        ActivePattern = COLOR_SWITCH;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir; 
  }  

  void ColorSwitchUpdate(){
    setPixelColor(TotalSteps/2, Color2);
    
    if(Index == 0){
      CleanPixels();
    }
    
    else if(Index < TotalSteps/2){
      setPixelColor(Index, Color1);
      setPixelColor(TotalSteps-Index, Color1);
    }
    else{
      setPixelColor(Index, Color2);
      setPixelColor(TotalSteps-Index, Color2);
    }
    show();
    Increment();
  }

//-------------------- Random Pixels -----------------------------------------
    void RandomFill(uint32_t color1, int interval) {
      TotalSteps = numPixels();
      Index = 0; 
      ActivePattern = RANDOM_FILL;
      Interval = interval;
      Color1 = color1;
      i = 0;      // A random pixel to update
    }

    // Update the Random Pixels pattern
    void RandomFillUpdate() {
      if (Index == 0) {
        CleanPixels();
      }

      else {
        while (true) {
          i = random(numPixels()-1);
          if (getPixelColor(i) == 0) {
            setPixelColor(i, Color1);
            break;
          }
        }
      }
      show();
      Increment();
    }
    
//--------------- 


//------------------OTHER SHIT--------------------------------
    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
        return (color >> 16) & 0xFF;
    }

    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
        return (color >> 8) & 0xFF;
    }

    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
        return color & 0xFF;
    }

    // Return color, dimmed by 75% (used by scanner)
    uint32_t DimColor(uint32_t color)
    {
        uint32_t dimColor = Color(Red(color)*.75, Green(color)*.75, Blue(color)*.75);
        return dimColor;
    }
    
    uint32_t DimColorMore(uint32_t color)
    {
        uint32_t dimColor = Color(Red(color)/4, Green(color)/4, Blue(color)/4);
        return dimColor;
    }
    
    uint32_t DimColorMost(uint32_t color)
    {
        uint32_t dimColor = Color(Red(color)/9, Green(color)/9, Blue(color)/9);
        return dimColor;
    }
    
    uint32_t DimControl(uint32_t color, int i)
    {
      uint32_t dimColor = Color(Red(color)/i, Green(color)/i, Blue(color)/i);
        return dimColor;
    }

    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos)
    {
        WheelPos = 255 - WheelPos;
        if(WheelPos < 85)
        {
            return Color(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            return Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
    }

        // Reverse direction of the pattern
    void Reverse()
    {
        if (Direction == FORWARD)
        {
            Direction = REVERSE;
            Index = TotalSteps-1;
        }
        else
        {
            Direction = FORWARD;
            Index = 0;
        }
    }

        // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        show();
    }

    void CleanPixels() 
    {
      for (int i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, 0);
      }
      show();
    }
};

///////////////////// end of NeoPattern class /////////////////////////
///////////////////////////////////////////////////////////////////////

int buttonPattern = 7; //button #1 for pattern cycling
int buttonBrightness = 8; //button #2 for brightness
int buttonBank = 9; //button #3 for changing pattern bank

volatile int patternNumber = 0; // keeps track of pattern
volatile int bankNumber = 1; //keeps track of both buttons being pressed
//volatile int brightnessNumber = 0; //keeps track of brightness level
volatile int brightnessNumber = 255;

//variables to keep track of the timing of recent interrupts
unsigned long button_time_pattern = 0;  
unsigned long last_button_time_pattern = 0; 
unsigned long button_time_brightness = 0;  
unsigned long last_button_time_brightness = 0; 
unsigned long button_time_bank = 0;  
unsigned long last_button_time_bank = 0; 

int debounceTime = 250; //in milliseconds

const int NEO_PIXEL_PIN = 3;           // Output pin for neo pixels.
const int NEO_PIXEL_COUNT = 53;         // Number of neo pixels.  You should be able to increase this without
//const int POWER_LED_PIN = 13; 

void pixelsComplete(); 

// Define some NeoPatterns for the two rings and the stick
//  as well as some completion routines
NeoPatterns pixels(NEO_PIXEL_COUNT, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800, &pixelsComplete);

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEO_PIXEL_COUNT, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int wait = 80; 

////////////// Change these accordingly ////////////////////////////////////////////////////////////////////////////////

int numberBanks = 3; //set this to the number of banks
int numberBrightnessLevels = 9; //set this to number of bightness levels

int numberPatterns1 = 5;
int numberPatterns2 = 6;
int numberPatterns3 = 3;

int brightnessLevels[] = {255, 191, 127, 64, 40, 20, 10, 5, 2, 0}; //Set to maximum brightness

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {                
  Serial.begin(38400);  //turn on serial communication
  //pin setups
  pinMode(7, INPUT_PULLUP); //define pin 7 as input
  //digitalWrite(7,HIGH);  // enable pullup resistor
  pinMode(8, INPUT_PULLUP); //define pin 8 as input
  //digitalWrite(8,HIGH);  // enable pullup resistor
  pinMode(9, INPUT_PULLUP); //define pin 9 as input
  //digitalWrite(9,HIGH);  // enable pullup resistor
  
  // Turn on the power indicator LED.
  //pinMode(POWER_LED_PIN, OUTPUT);
  //digitalWrite(POWER_LED_PIN, HIGH);
  
  pixels.begin(); //start NeoPattern class
  pixels.setBrightness(brightnessNumber);
  pixels.show();

  Serial.print(brightnessNumber);
  //pixels.Scanner(pixels.Color(255, 0, 0), 55);
  pixels.RandomPixels(20, 500);
  //pixels.ScrollTwoColor(pixels.Color(255, 0, 0), pixels.Color(0, 0, 255), 40);
  //pixels.ScrollTwoColorReverse(pixels.Color(255, 0, 0), pixels.Color(0, 0, 255), 40);
  //pixels.ScrollMiddle(pixels.Wheel(random(255)), 40);
  
//  attachInterrupt(buttonPattern, patternControl, FALLING);  //jump to the increment function on falling edge
//  attachInterrupt(buttonBrightness, brightnessControl, FALLING);  //jump to the increment function on falling edge
//  attachInterrupt(buttonBank, bankControl, FALLING);  //jump to the increment function on falling edge
  
}

void loop() {
  pixels.Update();
  
  if (digitalRead(7) == LOW) {
    patternControl();
    bankChoice();  
  }

  else if (digitalRead(8) == LOW) {
    brightnessControl();
    pixels.setBrightness(brightnessLevels[brightnessNumber]);
  }

  else if (digitalRead(9) == LOW) {
    bankControl();
    patternControl();
    bankChoice();
  } 
}

void pixelsComplete() {
  pixels.Update(); //Color1 = pixels.Wheel(random(255));
  pixels.Color1 = pixels.Wheel(random(255));
  pixels.Color2 = pixels.Wheel(random(255));
}


////////////////////////////////////////////////////////////////////////////////
//// PATTERN CALLS 
///////////////////////////////////////////////////////////////////////////////
//
void bankChoice() {
  switch(bankNumber) {
    case 1: patternChoiceBank1();
            break;
    case 2: patternChoiceBank2();
            break;
    case 3: patternChoiceBank3();
            break;  
  }
}

void patternChoiceBank1() {
  pixels.CleanPixels();
  pixels.Index = 0;
  
  switch(patternNumber) {

    case 1:
            pixels.ColorSwitch(pixels.Wheel(random(255)), pixels.Wheel(random(255)), 40);
            break;   
    case 2: 
            pixels.ScrollTwoColor(pixels.Wheel(random(255)), pixels.Wheel(random(255)), 40);
            break;
    case 3: 
            pixels.ColorRain(pixels.Wheel(random(255)), 40);
            break;
    case 4:
            pixels.ColorBreathe(pixels.Wheel(random(255)), 40);
            break;
    case 5:
            pixels.Fade(pixels.Wheel(random(255)), pixels.Wheel(random(255)), 100, 100);
            break;
           
  }
}

void patternChoiceBank2() {
  pixels.CleanPixels();
  pixels.Index = 0;
  
  switch(patternNumber) {
    case 1: 
            pixels.RainbowCycle(40);
            break;
    case 2: 
            pixels.ColorWipe(pixels.Wheel(random(255)), pixels.Wheel(random(255)), 40);
            break;
    case 3: 
            pixels.ScrollMiddle(pixels.Wheel(random(255)), pixels.Wheel(random(255)), 40);
            break;
    case 4:
            pixels.BlockDrop(pixels.Wheel(random(255)), 20);
            break;
    case 5: 
            pixels.Scanner(pixels.Wheel(random(255)), 40);
            break;
    case 6:
            pixels.RandomEater(pixels.Wheel(random(255)), pixels.Wheel(random(255)), 100);
            break;
  }
}

void patternChoiceBank3() {
  switch(patternNumber) {
    case 1: pixels.TheaterChase(pixels.Wheel(random(255)), pixels.Wheel(random(255)), 100);
            break;
    case 2: pixels.RandomFill(pixels.Wheel(random(255)), 100);
            break;
    case 3: //colorAquaWalk();
            break;
  }
}

////////////////////////////////////////////////////////////////////////////////
//// BUTTON ROUTINES 
///////////////////////////////////////////////////////////////////////////////

////Pattern button interrupt
void patternControl() {
  button_time_pattern = millis();
  
  if (button_time_pattern - last_button_time_pattern > debounceTime) //check to see if ISR was called in the last 250 milliseconds
  {
    switch(bankNumber) { //add cases for each bank added
      case 1:
              if (patternNumber < numberPatterns1) {
                patternNumber++;
              }
              else {
                patternNumber = 1;
              }
              break;
      case 2:
              if (patternNumber < numberPatterns2) {
                patternNumber++;
              }
              else {
                patternNumber = 1;
              }
              break;
      case 3:
              if (patternNumber < numberPatterns3) {
                patternNumber++;
              }
              else {
                patternNumber = 1;
              }
              break;
    }  
   last_button_time_pattern = button_time_pattern; 
//    for (int i=0; i< NEO_PIXEL_COUNT; i++){
//      pixels.setPixelColor(i, 0, 0, 0);
//      pixels.show();
//    }
  }
}

//brightness button interrupt
void brightnessControl() {
  button_time_brightness = millis();

  if (button_time_brightness - last_button_time_brightness > debounceTime)
  {
    if (brightnessNumber < numberBrightnessLevels) {
      brightnessNumber++;
    }
    else {
      brightnessNumber = 0;
    }
   //EEPROM.write(0, brightnessNumber);
    last_button_time_brightness = button_time_brightness;
  }
}

//pattern bank button interrupt
void bankControl() {
  button_time_bank = millis();
 
  if (button_time_bank - last_button_time_bank > debounceTime)
  {
    if (bankNumber < numberBanks) {
      bankNumber++;
    }
    else {
      bankNumber = 1;
    }
    last_button_time_bank = button_time_bank;
    patternNumber = 0;
    
  }
}


//////////////////////////////////////////// EXTRA SHIT//////////////////////

 /* serial monitor shit
  delay(1000); 
  Serial.print("Pattern: ");
  Serial.print(patternNumber);
  Serial.print("  Brightness Level: "); 
  Serial.print(brightnessNumber);
  Serial.print("   Bank Number:  ");
  Serial.print(bankNumber);
  Serial.print('\n'); */
