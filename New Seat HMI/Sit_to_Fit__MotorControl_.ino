//Set pins for Dir/PWM
const int SlidePWMH=8;
const int SeatHeightPWMH=5;
const int SeatAnglePWMH=7;
const int UpBackAnglePWMH=4;
const int BackAnglePWMH=3;
const int HeadrestPWMH=2;
const int CushionLengthPWMH=6;
//const int pin 9

const int SlidePWML=38;
const int SeatHeightPWML=35;
const int SeatAnglePWML=37;
const int UpBackAnglePWML=36;
const int BackAnglePWML=33;
const int HeadrestPWML=32;
const int CushionLengthPWML=34;

const int SlideDir=42;
const int SeatHeightDir=43;
const int SeatAngleDir=41;
const int UpBackAngleDir=47;
const int BackAngleDir=45;
const int HeadrestDir=46;
const int CushionLengthDir=44;

const int PumpPWMH=13;
const int PumpPWML=12;
const int PumpDir=11;

const int LowFillValve=25;
const int MidFillValve=29;
const int UpFillValve=26;

const int LowVentValve=23;
const int MidVentValve=27;
const int UpVentValve=28;

//Potentiometers
const int SlidePot=A8;
const int SeatHeightPot=A9;
const int SeatAnglePot=A10;
const int UpBackAnglePot=A11;
const int BackAnglePot=A12;
const int HeadrestPot=A13;
const int CushionLengthPot=A14;

const int lowLumPressure=A0;
const int midLumPressure=A1;
const int upLumPressure=A2;

//const int buttonPin = 2;

char Pressures[6];

char ThighBP;
char PedalBP;
char MidBP;
char UpBP;

boolean ButtonPressed=false;

unsigned long Timer=millis();

void ReceivePressures();
void CheckButton();
void AdjustSeat();
void StopMotors();
void CorrelateHeight();
void CorrelateCLA();

void setup() 
{
  Serial.begin (9600);
  Serial1.begin(9600);
  
  pinMode (SlidePWMH, OUTPUT);
  pinMode (SeatHeightPWMH, OUTPUT);
  pinMode (SeatAnglePWMH, OUTPUT);
  pinMode (UpBackAnglePWMH, OUTPUT);
  pinMode (BackAnglePWMH, OUTPUT);
  pinMode (HeadrestPWMH, OUTPUT);
  pinMode (CushionLengthPWMH, OUTPUT);
     //PWML pins
  pinMode (SlidePWML, OUTPUT);
  pinMode (SeatHeightPWML, OUTPUT);
  pinMode (SeatAnglePWML, OUTPUT);
  pinMode (UpBackAnglePWML, OUTPUT);
  pinMode (BackAnglePWML, OUTPUT);
  pinMode (HeadrestPWML, OUTPUT);
  pinMode (CushionLengthPWML, OUTPUT);
  
  digitalWrite (SlidePWML, HIGH);
  digitalWrite (SeatHeightPWML, HIGH);
  digitalWrite (SeatAnglePWML, HIGH);
  digitalWrite (UpBackAnglePWML, HIGH);
  digitalWrite (BackAnglePWML, HIGH);
  digitalWrite (HeadrestPWML, HIGH);
  digitalWrite (CushionLengthPWML, HIGH);
  
      //DIR pins
  pinMode (SlideDir, OUTPUT);
  pinMode (SeatHeightDir, OUTPUT);
  pinMode (SeatAngleDir, OUTPUT);
  pinMode (UpBackAngleDir, OUTPUT);
  pinMode (BackAngleDir, OUTPUT);
  pinMode (HeadrestDir, OUTPUT);
  pinMode (CushionLengthDir, OUTPUT);

  //Potentiometer Setup
  pinMode (SlidePot, INPUT);
  pinMode (SeatHeightPot, INPUT);
  pinMode (SeatAnglePot, INPUT);
  pinMode (UpBackAnglePot, INPUT);
  pinMode (BackAnglePot, INPUT);
  pinMode (HeadrestPot, INPUT);
  pinMode (CushionLengthPot, INPUT);
  
  pinMode (LowFillValve, OUTPUT);
  pinMode (MidFillValve, OUTPUT);
  pinMode (UpFillValve, OUTPUT);
  
  pinMode (LowVentValve, OUTPUT);
  pinMode (MidVentValve, OUTPUT);
  pinMode (UpVentValve, OUTPUT);
  
  pinMode (lowLumPressure, INPUT);
  pinMode (midLumPressure, INPUT);
  pinMode (upLumPressure, INPUT);
  
  pinMode (PumpPWMH, OUTPUT);
  pinMode (PumpPWML, OUTPUT);
  pinMode (PumpDir, OUTPUT);
  
  digitalWrite (PumpDir, LOW);
  digitalWrite (PumpPWML, HIGH);
}

void loop() 
{
  ReceivePressures();//Takes in pressures over Serial1 from Pneumatic board
  if (ButtonPressed==true) // Capacitive switch (Info received from pneumatic board
  {
    AdjustSeat(); //Adjusts seat based on data/pressures received (High, low, or nominal
  }
  else if (ButtonPressed==false)
  { 
    StopMotors(); //Disables all motors
  }
  
}

void ReceivePressures()
{
  String readstring;
  
  if (Timer+200<millis()) //Shutoff after 100ms of no data
  {
    StopMotors();
    
    PedalBP = 'N';
    ThighBP = 'N';
    MidBP = 'N';
    UpBP = 'N';
  }
  
  while (Serial1.available()>0) //Only enters into setup if data is real (ASCII values>0)
  { 
    Timer = millis(); //Resets timer for shutoff
    
    readstring = Serial1.readStringUntil('\n'); //Reads in string of 5 values (Button on/off, thigh,pedal,upper,mid)
    readstring.toCharArray (Pressures,6); //Converts string to character array for manipulation
    
    if (Pressures[0] == '0') ButtonPressed = false; //Enables/disables the adjustment function based on touch of switch
    else if (Pressures[0] == '1') ButtonPressed = true;
    
    ThighBP = Pressures[1]; //Converts 4 remaining values to the correct area pressure
    PedalBP = Pressures[2];
    MidBP = Pressures[3];
    UpBP = Pressures[4];
    
    
    for (int i=0;i<5;i++) // Prints out data received on Serial line
    {
      Serial.print (Pressures[i]);
    }
    Serial.println("");
  }  
}


void AdjustSeat()
{ 
  int curSeatHeight=map(analogRead (SeatHeightPot),148,1020,100,0); // Checks all positions (Slide pots). All values calibrated to SmartFit A8 seat
  int curSlide=map(analogRead (SlidePot),204,704,0,100);
  int curSeatAngle=map(analogRead (SeatAnglePot),140,1010,0,100);
  int curBackAngle=map(analogRead (BackAnglePot),475,1000,0,100);
  int curUpBackAngle=map(analogRead (UpBackAnglePot),180,960,0,100);
  int curHeadrest=map(analogRead (HeadrestPot),35,835,0,100);
  int curCushionLength=map(analogRead (CushionLengthPot),0,800,0,100);
  
  if (PedalBP == 'H') // If pedal pressure is high and slide isnt at max position, slide back.
  {
    if (curSlide>0)
    {
      digitalWrite (SlidePWMH, HIGH);
      digitalWrite (SlideDir, HIGH);
    }
    else digitalWrite (SlidePWMH, LOW);
    
    CorrelateHeight(); //Function to correlate slide position to headrest and seat height positions based on SmartFit data
  }
  else if (PedalBP == 'L')// If pedal pressure is low and not at min position, slide forward
  {
    if (curSlide<100)
    {
      digitalWrite (SlidePWMH, HIGH);
      digitalWrite (SlideDir, LOW);
    }
    else digitalWrite (SlidePWMH, LOW);
    
    CorrelateHeight();
  }
  else if (PedalBP == 'N') //Turn off slide adjustment, pressure is nominal
  {
    digitalWrite (SlidePWMH, LOW);
    digitalWrite (SeatHeightPWMH, LOW);
    
    CorrelateHeight();
  }
  
            if (ThighBP == 'H') //If thigh pressure is high and adjustment is not bottomed out, adjust seat angle down
            {
              if (curSeatAngle>5)
              {
                digitalWrite (SeatAnglePWMH, HIGH);
                digitalWrite (SeatAngleDir, LOW);
              }
              else digitalWrite (SeatAnglePWMH, LOW);
              
              CorrelateCLA(); // Function to correlate CLA to seat angle position
            }
            else if (ThighBP == 'L') // Opposite of the above statement
            {
              if (curSeatAngle<95)
              {
                digitalWrite (SeatAnglePWMH, HIGH);
                digitalWrite (SeatAngleDir, HIGH);
              }
              else digitalWrite (SeatAnglePWMH, LOW);
              
              CorrelateCLA();
            }
            else if (ThighBP == 'N')// Nominal position, disable seat angle
            {
              digitalWrite (SeatAnglePWMH, LOW);
              digitalWrite (CushionLengthPWMH, LOW);
              
              CorrelateCLA();
            }
  
  if (MidBP=='H'&&UpBP=='H') // If mid and upper pressure is high, recline aft
  {
    if (curBackAngle<100) // Stop reclining if the recliner is maxed
    {
      digitalWrite (BackAnglePWMH, HIGH);
      digitalWrite (BackAngleDir, HIGH);
    }
    else digitalWrite (BackAnglePWMH, LOW);
    
    digitalWrite(LowFillValve, LOW); // Ensure other adjustments are disabled
    digitalWrite (MidFillValve, LOW);
    digitalWrite (UpFillValve, LOW);
    digitalWrite (LowVentValve, LOW);
    digitalWrite (MidVentValve, LOW);
    digitalWrite (UpVentValve, LOW);
    digitalWrite (PumpPWMH, LOW);
    
    digitalWrite (UpBackAnglePWMH, LOW);
  }
  else if (MidBP=='L'&&UpBP=='H') // If mid pressure is low and upper is high, fill lumbar bladders and move UBA aft
  {
    digitalWrite (BackAnglePWMH, LOW);    
    
    digitalWrite(LowFillValve, HIGH);
    digitalWrite (MidFillValve, HIGH);
    digitalWrite (UpFillValve, HIGH);
    digitalWrite (LowVentValve, LOW);
    digitalWrite (MidVentValve, LOW);
    digitalWrite (UpVentValve, LOW);
    digitalWrite (PumpPWMH, HIGH);
    
    if (curUpBackAngle>0) // Checks to ensure motor isnt maxed
    {
      digitalWrite (UpBackAnglePWMH, HIGH);
      digitalWrite (UpBackAngleDir, HIGH);
    }
    else digitalWrite (UpBackAnglePWMH, LOW);
  }
  else if (MidBP=='N'&&UpBP=='H') // If mid is nominal and upper is high, adjust upper back aft
  {
    digitalWrite (BackAnglePWMH, LOW);
    
    digitalWrite(LowFillValve, LOW);
    digitalWrite (MidFillValve, LOW);
    digitalWrite (UpFillValve, LOW);
    digitalWrite (LowVentValve, LOW);
    digitalWrite (MidVentValve, LOW);
    digitalWrite (UpVentValve, LOW);
    digitalWrite (PumpPWMH, LOW);
    
    if (curUpBackAngle>0)//checks to ensure motor isnt maxed
    {
      digitalWrite (UpBackAnglePWMH, HIGH);
      digitalWrite (UpBackAngleDir, HIGH);
    }
    else digitalWrite (UpBackAnglePWMH, LOW);
    
  }
  else if (MidBP=='H'&&UpBP=='L')// If mid is high and upper is low, vent lumbar and move UBA fore
  {
    digitalWrite (BackAnglePWMH, LOW);
    
    digitalWrite(LowFillValve, LOW);
    digitalWrite (MidFillValve, LOW);
    digitalWrite (UpFillValve, LOW);
    digitalWrite (LowVentValve, HIGH);
    digitalWrite (MidVentValve, HIGH);
    digitalWrite (UpVentValve, HIGH);
    digitalWrite (PumpPWMH, LOW);
    
    if (curUpBackAngle<100)//checks to ensure the motor isnt maxed
    {
      digitalWrite (UpBackAnglePWMH, HIGH);
      digitalWrite (UpBackAngleDir, LOW);
    }
    else digitalWrite (UpBackAnglePWMH, LOW);
  }
  else if (MidBP=='L'&&UpBP=='L')//If mid and upper are low pressure, move recliner fore
  {
    if (curBackAngle>0) //Ensures motor isnt maxed
    {
      digitalWrite (BackAnglePWMH, HIGH);
      digitalWrite (BackAngleDir, LOW);
    }
    else digitalWrite (BackAnglePWMH, LOW);
    
    digitalWrite(LowFillValve, LOW);
    digitalWrite (MidFillValve, LOW);
    digitalWrite (UpFillValve, LOW);
    digitalWrite (LowVentValve, LOW);
    digitalWrite (MidVentValve, LOW);
    digitalWrite (UpVentValve, LOW);
    digitalWrite (PumpPWMH, LOW);
    
    digitalWrite (UpBackAnglePWMH, LOW);
  }
  else if (MidBP=='N'&&UpBP=='L')// if mid is nominal and upper is low, move UBA fore
  {
    digitalWrite (BackAnglePWMH, LOW);
    
    digitalWrite(LowFillValve, LOW);
    digitalWrite (MidFillValve, LOW);
    digitalWrite (UpFillValve, LOW);
    digitalWrite (LowVentValve, LOW);
    digitalWrite (MidVentValve, LOW);
    digitalWrite (UpVentValve, LOW);
    digitalWrite (PumpPWMH, LOW);
    
    if (curUpBackAngle<100)//checks to ensure it isnt maxed
    {
      digitalWrite (UpBackAnglePWMH, HIGH);
      digitalWrite (UpBackAngleDir, LOW);
    }
    else digitalWrite (UpBackAnglePWMH, LOW);
  }
  else if (MidBP=='H'&&UpBP=='N')// If mid is high and upper is nominal, vent lumbar
  {
    digitalWrite (BackAnglePWMH, LOW);
    
    digitalWrite(LowFillValve, LOW);
    digitalWrite (MidFillValve, LOW);
    digitalWrite (UpFillValve, LOW);
    digitalWrite (LowVentValve, HIGH);
    digitalWrite (MidVentValve, HIGH);
    digitalWrite (UpVentValve, HIGH);
    digitalWrite (PumpPWMH, LOW);
    
    digitalWrite (UpBackAnglePWMH, LOW);
  }
  else if (MidBP=='L'&&UpBP=='N')// if mid is low and upper is nominal, fill lumbar
  {
    digitalWrite (BackAnglePWMH, LOW);
    
    digitalWrite(LowFillValve, HIGH);
    digitalWrite (MidFillValve, HIGH);
    digitalWrite (UpFillValve, HIGH);
    digitalWrite (LowVentValve, LOW);
    digitalWrite (MidVentValve, LOW);
    digitalWrite (UpVentValve, LOW);
    digitalWrite (PumpPWMH, HIGH);
    
    digitalWrite (UpBackAnglePWMH, LOW);
  }
  else if (MidBP=='N'&&UpBP=='N')// if both are nominal, disable recliner, uba and lumbar
  {
    digitalWrite (BackAnglePWMH, LOW);
    
    digitalWrite(LowFillValve, LOW);
    digitalWrite (MidFillValve, LOW);
    digitalWrite (UpFillValve, LOW);
    digitalWrite (LowVentValve, LOW);
    digitalWrite (MidVentValve, LOW);
    digitalWrite (UpVentValve, LOW);
    digitalWrite (PumpPWMH, LOW);
    
    digitalWrite (UpBackAnglePWMH, LOW);
  }
}

void StopMotors() //Disables all motors
{
  digitalWrite (SlidePWMH, LOW);
  digitalWrite (SeatHeightPWMH, LOW);
  digitalWrite (SeatAnglePWMH, LOW);
  digitalWrite (CushionLengthPWMH, LOW);
  digitalWrite (BackAnglePWMH, LOW);
  digitalWrite (UpBackAnglePWMH, LOW);
  digitalWrite (HeadrestPWMH, LOW);
  
  digitalWrite (LowFillValve, LOW);
  digitalWrite (MidFillValve, LOW);
  digitalWrite (UpFillValve, LOW);
  digitalWrite (LowVentValve, LOW);
  digitalWrite (MidVentValve, LOW);
  digitalWrite (UpVentValve, LOW);
  
  digitalWrite (PumpPWMH, LOW);
}

void CorrelateHeight() // Correlates height to slide and then makes the necessary adjustments to headrest and seat height
{
  int corSlide, curHeadrest, curSeatHeight, corHeadrest, corSeatHeight;
  int PersonHeight;
  
  corSlide=map(analogRead (SlidePot),204,704,0,100);
  curHeadrest=map(analogRead (HeadrestPot),35,835,0,100);
  curSeatHeight=map(analogRead (SeatHeightPot),148,1020,100,0);
  
   if (corSlide<=100&&corSlide>=98)PersonHeight = 150;
   else if (corSlide<98&&corSlide>=96) PersonHeight = 151;
   else if (corSlide<96&&corSlide>=94) PersonHeight = 152;
   else if (corSlide<94&&corSlide>=92) PersonHeight = 153;
   else if (corSlide<92&&corSlide>=90) PersonHeight = 154;
   else if (corSlide<90&&corSlide>=88) PersonHeight = 155;
   else if (corSlide<88&&corSlide>=86) PersonHeight = 156;
   else if (corSlide<86&&corSlide>=84) PersonHeight = 157;
   else if (corSlide<84&&corSlide>=82) PersonHeight = 158;
   else if (corSlide<82&&corSlide>=80) PersonHeight = 159;
   else if (corSlide<80&&corSlide>=78) PersonHeight = 160;
   else if (corSlide<78&&corSlide>=76) PersonHeight = 161;
   else if (corSlide<76&&corSlide>=74) PersonHeight = 162;
   else if (corSlide<74&&corSlide>=72) PersonHeight = 163;
   else if (corSlide<72&&corSlide>=70) PersonHeight = 164;
   else if (corSlide<70&&corSlide>=68) PersonHeight = 165;
   else if (corSlide<68&&corSlide>=66) PersonHeight = 166;
   else if (corSlide<66&&corSlide>=64) PersonHeight = 167;
   else if (corSlide<64&&corSlide>=62) PersonHeight = 168;
   else if (corSlide<62&&corSlide>=60) PersonHeight = 169;
   else if (corSlide<60&&corSlide>=58) PersonHeight = 170;
   else if (corSlide<58&&corSlide>=56) PersonHeight = 171;
   else if (corSlide<56&&corSlide>=54) PersonHeight = 172;
   else if (corSlide<54&&corSlide>=52) PersonHeight = 173;
   else if (corSlide<52&&corSlide>=50) PersonHeight = 174;
   else if (corSlide<50&&corSlide>=48) PersonHeight = 175;
   else if (corSlide<48&&corSlide>=46) PersonHeight = 176;
   else if (corSlide<46&&corSlide>=44) PersonHeight = 177;
   else if (corSlide<44&&corSlide>=42) PersonHeight = 178;
   else if (corSlide<42&&corSlide>=40) PersonHeight = 179;
   else if (corSlide<40&&corSlide>=38) PersonHeight = 180;
   else if (corSlide<38&&corSlide>=36) PersonHeight = 181;
   else if (corSlide<36&&corSlide>=34) PersonHeight = 182;
   else if (corSlide<34&&corSlide>=32) PersonHeight = 183;
   else if (corSlide<32&&corSlide>=30) PersonHeight = 184;
   else if (corSlide<30&&corSlide>=28) PersonHeight = 185;
   else if (corSlide<28&&corSlide>=26) PersonHeight = 186;
   else if (corSlide<26&&corSlide>=24) PersonHeight = 187;
   else if (corSlide<24&&corSlide>=22) PersonHeight = 188;
   else if (corSlide<22&&corSlide>=20) PersonHeight = 189;
   else if (corSlide<20&&corSlide>=18) PersonHeight = 190;
   else if (corSlide<18&&corSlide>=16) PersonHeight = 191;
   else if (corSlide<16&&corSlide>=14) PersonHeight = 192;
   else if (corSlide<14&&corSlide>=12) PersonHeight = 193;
   else if (corSlide<12&&corSlide>=10) PersonHeight = 194;
   else if (corSlide<10&&corSlide>=8) PersonHeight = 195;
   else if (corSlide<8&&corSlide>=6) PersonHeight = 196;
   else if (corSlide<6&&corSlide>=4) PersonHeight = 197;
   else if (corSlide<4&&corSlide>=2) PersonHeight = 198;
   else if (corSlide<2&&corSlide>=0) PersonHeight = 199;
   else if (corSlide==0) PersonHeight = 200;
   
   if (PersonHeight<=151&&PersonHeight>=150)corSeatHeight=3;
   else if (PersonHeight<=153&&PersonHeight>=152) corSeatHeight=6;
   else if (PersonHeight<=155&&PersonHeight>=154) corSeatHeight=9;
   else if (PersonHeight<=157&&PersonHeight>=156) corSeatHeight=11;
   else if (PersonHeight<=159&&PersonHeight>=158) corSeatHeight=14;
   else if (PersonHeight<=161&&PersonHeight>=160) corSeatHeight=17;
   else if (PersonHeight<=163&&PersonHeight>=162) corSeatHeight=20;
   else if (PersonHeight<=165&&PersonHeight>=164) corSeatHeight=22;
   else if (PersonHeight<=167&&PersonHeight>=166) corSeatHeight=25;
   else if (PersonHeight<=169&&PersonHeight>=168) corSeatHeight=28;
   else if (PersonHeight<=171&&PersonHeight>=170) corSeatHeight=31;
   else if (PersonHeight<=173&&PersonHeight>=172) corSeatHeight=34;
   else if (PersonHeight<=175&&PersonHeight>=174) corSeatHeight=36;
   else if (PersonHeight<=177&&PersonHeight>=176) corSeatHeight=39;
   else if (PersonHeight<=179&&PersonHeight>=178) corSeatHeight=41;
   else if (PersonHeight<=181&&PersonHeight>=180) corSeatHeight=44;
   else if (PersonHeight<=183&&PersonHeight>=182) corSeatHeight=46;
   else if (PersonHeight<=185&&PersonHeight>=184) corSeatHeight=49;
   else if (PersonHeight<=187&&PersonHeight>=186) corSeatHeight=52;
   else if (PersonHeight<=189&&PersonHeight>=188) corSeatHeight=54;
   else if (PersonHeight<=191&&PersonHeight>=190) corSeatHeight=57;
   else if (PersonHeight<=193&&PersonHeight>=192) corSeatHeight=60;
   else if (PersonHeight<=195&&PersonHeight>=194) corSeatHeight=63;
   else if (PersonHeight<=197&&PersonHeight>=196) corSeatHeight=65;
   else if (PersonHeight<=199&&PersonHeight>=198) corSeatHeight=67;
   else if (PersonHeight==200) corSeatHeight=70;
   
   if (PersonHeight<=151&&PersonHeight>=150)corHeadrest=0;
   else if (PersonHeight<=153&&PersonHeight>=152) corHeadrest=4;
   else if (PersonHeight<=155&&PersonHeight>=154) corHeadrest=8;
   else if (PersonHeight<=157&&PersonHeight>=156) corHeadrest=12;
   else if (PersonHeight<=159&&PersonHeight>=158) corHeadrest=16;
   else if (PersonHeight<=161&&PersonHeight>=160) corHeadrest=20;
   else if (PersonHeight<=163&&PersonHeight>=162) corHeadrest=24;
   else if (PersonHeight<=165&&PersonHeight>=164) corHeadrest=28;
   else if (PersonHeight<=167&&PersonHeight>=166) corHeadrest=32;
   else if (PersonHeight<=169&&PersonHeight>=168) corHeadrest=36;
   else if (PersonHeight<=171&&PersonHeight>=170) corHeadrest=40;
   else if (PersonHeight<=173&&PersonHeight>=172) corHeadrest=44;
   else if (PersonHeight<=175&&PersonHeight>=174) corHeadrest=48;
   else if (PersonHeight<=177&&PersonHeight>=176) corHeadrest=52;
   else if (PersonHeight<=179&&PersonHeight>=178) corHeadrest=56;
   else if (PersonHeight<=181&&PersonHeight>=180) corHeadrest=60;
   else if (PersonHeight<=183&&PersonHeight>=182) corHeadrest=64;
   else if (PersonHeight<=185&&PersonHeight>=184) corHeadrest=68;
   else if (PersonHeight<=187&&PersonHeight>=186) corHeadrest=72;
   else if (PersonHeight<=189&&PersonHeight>=188) corHeadrest=76;
   else if (PersonHeight<=191&&PersonHeight>=190) corHeadrest=80;
   else if (PersonHeight<=193&&PersonHeight>=192) corHeadrest=84;
   else if (PersonHeight<=195&&PersonHeight>=194) corHeadrest=88;
   else if (PersonHeight<=197&&PersonHeight>=196) corHeadrest=92;
   else if (PersonHeight<=199&&PersonHeight>=198) corHeadrest=96;
   else if (PersonHeight==200) corHeadrest=100;
   
   
    if (curSeatHeight>(corSeatHeight+2))digitalWrite (SeatHeightDir, LOW); //Above, decreasing
    else if (curSeatHeight<(corSeatHeight-2)) digitalWrite (SeatHeightDir, HIGH); //Below, increasing
    
    if (curHeadrest>(corHeadrest+2)) digitalWrite (HeadrestDir, HIGH);
    else if (curHeadrest<(corHeadrest-2)) digitalWrite (HeadrestDir, LOW);
    
    if (curSeatHeight<(corSeatHeight+2)&&curSeatHeight>(corSeatHeight-2)) // Moves motor within a small position of the desired range
    {
        digitalWrite (SeatHeightPWMH, LOW);
    }
    else analogWrite (SeatHeightPWMH, 137);
    
    if (curHeadrest<(corHeadrest+2)&&curHeadrest>(corHeadrest-2))
    {
        digitalWrite (HeadrestPWMH, LOW);
    }
    else analogWrite (HeadrestPWMH, 137);
   
   
  
  
}

void CorrelateCLA() // correlates seat angle to CLA position and makes necessary adjustments
{
  int corSeatAngle, curCushionLength, corCushionLength;
  int PersonHeight2;
  
  corSeatAngle=map(analogRead (SeatAnglePot),140,980,0,100);
  curCushionLength=map(analogRead (CushionLengthPot),0,800,0,100);
  
   if (corSeatAngle<4&&corSeatAngle>=0)PersonHeight2=150;
   else if (corSeatAngle<8&&corSeatAngle>=4) PersonHeight2=152;
   else if (corSeatAngle<12&&corSeatAngle>=8) PersonHeight2=154;
   else if (corSeatAngle<16&&corSeatAngle>=12) PersonHeight2=156;
   else if (corSeatAngle<20&&corSeatAngle>=16) PersonHeight2=158;
   else if (corSeatAngle<24&&corSeatAngle>=20) PersonHeight2=160;
   else if (corSeatAngle<28&&corSeatAngle>=24) PersonHeight2=162;
   else if (corSeatAngle<32&&corSeatAngle>=28) PersonHeight2=164;
   else if (corSeatAngle<36&&corSeatAngle>=32) PersonHeight2=166;
   else if (corSeatAngle<40&&corSeatAngle>=36) PersonHeight2=168;
   else if (corSeatAngle<44&&corSeatAngle>=40) PersonHeight2=170;
   else if (corSeatAngle<48&&corSeatAngle>=44) PersonHeight2=172;
   else if (corSeatAngle<52&&corSeatAngle>=48) PersonHeight2=174;
   else if (corSeatAngle<56&&corSeatAngle>=52) PersonHeight2=176;
   else if (corSeatAngle<60&&corSeatAngle>=56) PersonHeight2=178;
   else if (corSeatAngle<64&&corSeatAngle>=60) PersonHeight2=180;
   else if (corSeatAngle<68&&corSeatAngle>=64) PersonHeight2=182;
   else if (corSeatAngle<72&&corSeatAngle>=68) PersonHeight2=184;
   else if (corSeatAngle<76&&corSeatAngle>=72) PersonHeight2=186;
   else if (corSeatAngle<80&&corSeatAngle>=76) PersonHeight2=188;
   else if (corSeatAngle<84&&corSeatAngle>=80) PersonHeight2=190;
   else if (corSeatAngle<88&&corSeatAngle>=84) PersonHeight2=192;
   else if (corSeatAngle<92&&corSeatAngle>=88) PersonHeight2=194;
   else if (corSeatAngle<96&&corSeatAngle>=92) PersonHeight2=196;
   else if (corSeatAngle<100&&corSeatAngle>=96) PersonHeight2=198;
   
   if (PersonHeight2<=151&&PersonHeight2>=150)corCushionLength=90;
   else if (PersonHeight2<=153&&PersonHeight2>=152) corCushionLength=87;
   else if (PersonHeight2<=155&&PersonHeight2>=154) corCushionLength=85;
   else if (PersonHeight2<=157&&PersonHeight2>=156) corCushionLength=82;
   else if (PersonHeight2<=159&&PersonHeight2>=158) corCushionLength=80;
   else if (PersonHeight2<=161&&PersonHeight2>=160) corCushionLength=77;
   else if (PersonHeight2<=163&&PersonHeight2>=162) corCushionLength=75;
   else if (PersonHeight2<=165&&PersonHeight2>=164) corCushionLength=73;
   else if (PersonHeight2<=167&&PersonHeight2>=166) corCushionLength=70;
   else if (PersonHeight2<=169&&PersonHeight2>=168) corCushionLength=67;
   else if (PersonHeight2<=171&&PersonHeight2>=170) corCushionLength=65;
   else if (PersonHeight2<=173&&PersonHeight2>=172) corCushionLength=63;
   else if (PersonHeight2<=175&&PersonHeight2>=174) corCushionLength=60;
   else if (PersonHeight2<=177&&PersonHeight2>=176) corCushionLength=57;
   else if (PersonHeight2<=179&&PersonHeight2>=178) corCushionLength=55;
   else if (PersonHeight2<=181&&PersonHeight2>=180) corCushionLength=52;
   else if (PersonHeight2<=183&&PersonHeight2>=182) corCushionLength=50;
   else if (PersonHeight2<=185&&PersonHeight2>=184) corCushionLength=47;
   else if (PersonHeight2<=187&&PersonHeight2>=186) corCushionLength=45;
   else if (PersonHeight2<=189&&PersonHeight2>=188) corCushionLength=42;
   else if (PersonHeight2<=191&&PersonHeight2>=190) corCushionLength=40;
   else if (PersonHeight2<=193&&PersonHeight2>=192) corCushionLength=37;
   else if (PersonHeight2<=195&&PersonHeight2>=194) corCushionLength=35;
   else if (PersonHeight2<=197&&PersonHeight2>=196) corCushionLength=33;
   else if (PersonHeight2<=200&&PersonHeight2>=198) corCushionLength=30;
   if (PersonHeight2==200) corCushionLength=27;
   
   if (curCushionLength>(corCushionLength+2)) digitalWrite (CushionLengthDir, LOW); 
   else if (curCushionLength<(corCushionLength-2)) digitalWrite (CushionLengthDir, HIGH);
    
   if (curCushionLength<(corCushionLength+2)&&curCushionLength>(corCushionLength-2))
   {
       digitalWrite (CushionLengthPWMH, LOW);
   }
   else digitalWrite (CushionLengthPWMH, HIGH);
}
