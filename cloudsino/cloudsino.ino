#include <FastLED.h>
#define NUM_LEDS 300
#define DATA_PIN 6
#define MAX_DIVIDER 16
#define INIT 10
#define SPEED 1
#define COLORSWITCH 4

const int pinSelect=10;
const int pinUp=7;
const int pinDown=8;
const int pinBack=9;
const int patternNum=2;

int selectState;
int previousUp=LOW;
int previousDown=LOW;
unsigned long tUp = 0;         // the last time the output pin was toggled
unsigned long tDown = 0;         // the last time the output pin was toggled
unsigned long debounce = 200;   // the debounce time, increase if the output flickers


int currentUp;
int currentDown;

unsigned int central[]={0,0,0,0};
const unsigned int cloudIndices[]={0,69,140,218};
const unsigned int skyIndices[]={42,108,180,262};
unsigned int stripLength=0;
unsigned int widthPattern=8;
unsigned int divider=1;
unsigned int turncnt=0;
int faders[]={0,0,0,0};
unsigned int pattern=0;
unsigned int readingPot;
unsigned int switchDelay=1;
unsigned int cloudColorIndex=0;
unsigned int skyColorIndex=2;
CRGB leds[NUM_LEDS];

void fadeinandout(CRGB *leds,int cloudIndices[], int skyIndices[], int faders[])
{
  unsigned int lower;
  unsigned int upper;
  Serial.println("-----------");
  
  for (int k=0; k<4; k++) 
  {
    for(int i=cloudIndices[k];i<skyIndices[k];i++)
      {
        leds[i]=CRGB::Red;
        leds[i].fadeToBlackBy(abs(255-faders[k]));
        leds[i]/=2;
      }
    Serial.println(256-faders[k]);

    lower=skyIndices[k];
    if(k==3)
    {
      upper=NUM_LEDS;  
    }
    else{
      upper=cloudIndices[k+1];
    }
    for(int i=lower;i<upper;i++)
      {
        leds[i]=CRGB::Purple;
        leds[i].fadeToBlackBy(abs(255-faders[k]));
        leds[i]/=2;
      }
      if(faders[k]!=0){faders[k]=(faders[k]+2)%510;}
      else if (random(0,1020)==101) {
        faders[k]=1;
      }
      if((faders[0]+faders[1]+faders[2]+faders[3])==0)
      { faders[int(round(random(0,4)))]=1;}
      
  }
  FastLED.show();
  for(int i=0;i<=NUM_LEDS;i++)
  {
    leds[i]=CRGB::Black;
  }
  
}

void passingBand(CRGB *leds, CRGB *predefColors,int cloudIndices[], int skyIndices[], int widthPattern, int *central, int *turncnt,int *cloudColorIndex,int *skyColorIndex)
{
  unsigned int lower;
  unsigned int upper;
  int colorNumber=sizeof(predefColors)/sizeof(CRGB);
  for (int k=0; k<4; k++) 
  {
    stripLength=skyIndices[k]-cloudIndices[k];
    for (int i=0; i<widthPattern; i++) 
    {
      leds[(central[k]+i)%stripLength+cloudIndices[k]]=predefColors[*cloudColorIndex];
      leds[(central[k]+i)%stripLength+cloudIndices[k]].maximizeBrightness();
      leds[(central[k]+i)%stripLength+cloudIndices[k]]/=2;
    }
    lower=skyIndices[k];
    if(k==3)
    {
      upper=NUM_LEDS;  
    }
    else{
      upper=cloudIndices[k+1];
    }
      
    for(int j=lower; j<upper;j++)
    {
      leds[j]=predefColors[*skyColorIndex];
      leds[j]/=4;
    }
    
  }
    FastLED.show();
  for (int k=0; k<4; k++) 
  {
    stripLength=skyIndices[k]-cloudIndices[k];
    for (int i=0; i<widthPattern; i++) 
    {
      leds[(central[k]+i)%stripLength+cloudIndices[k]]=CRGB::Black;
      leds[(central[k]-i)%stripLength+cloudIndices[k]]=CRGB::Black;
    }
    lower=skyIndices[k];
    if(k==3)
    {
      upper=NUM_LEDS;  
    }
    else{
      upper=cloudIndices[k+1];
    }
      
    for(int j=lower; j<upper;j++)
    {
      leds[j]=CRGB::Black;
    }
    
    
  }
  

  
  for (int k=0; k<4; k++) 
  {
    stripLength=skyIndices[k]-cloudIndices[k];
    central[k]=(central[k]+SPEED)%stripLength;
    
  }
 if(central[0]==0)
    {
      *turncnt++;
      if(*turncnt==COLORSWITCH)
      {
        *cloudColorIndex=(*cloudColorIndex+1)%(colorNumber);
      }
      if(*turncnt==2*COLORSWITCH)
      {
        *turncnt=0;
        *skyColorIndex=(*skyColorIndex+1)%(colorNumber);
      }
    }
}





CRGB predefColors[]={CRGB::MediumVioletRed,CRGB::MidnightBlue,CRGB::SpringGreen,CRGB::Salmon,CRGB::Tomato,CRGB::Yellow,CRGB::Crimson,CRGB::GreenYellow,CRGB::MediumBlue,CRGB::Teal};
int colorNumber=sizeof(predefColors)/sizeof(CRGB);



void setup() 
{ 
  FastLED.addLeds<NEOPIXEL, 6>(leds, NUM_LEDS); 
  pinMode(pinSelect,INPUT);
  pinMode(pinUp,INPUT);
  tUp=millis();
  tDown=millis();
  pinMode(pinDown,INPUT);
  selectState=digitalRead(pinSelect);
  Serial.begin(9600);
}


void loop() {
  switch(pattern){
    case 1:
      passingBand(leds,predefColors,cloudIndices,skyIndices,widthPattern,central,&turncnt,&cloudColorIndex,&skyColorIndex);
      break;
    case 0:
      fadeinandout(leds,cloudIndices,skyIndices,faders);
      break;
  }
  
  
  
  
  //Control section of the code
  currentUp=digitalRead(pinUp);
  currentDown=digitalRead(pinDown);

  if (currentUp == HIGH && previousUp == LOW) {
    pattern=(pattern+1)%patternNum;
    Serial.println("Up!");
  }
  tUp=millis();
  previousUp=currentUp;
  if (currentDown == HIGH && previousDown == LOW ) {
    pattern=(pattern-1)%patternNum;
    Serial.println("Down!");
  }
  tDown=millis();
  previousDown=currentDown;
  delay(20);
}
