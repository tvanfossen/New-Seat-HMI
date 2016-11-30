#include <eRCaGuy_NewAnalogRead.h>

#include <CapacitiveSensor.h>

CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2); //Creates new class for capacitive switch

ADC_prescaler_t ADCSpeed = ADC_FAST;

int ThighBP;
int PedalBP;
int MidBP;
int UpBP;

int ThighDefault=897;//Default value 
int PedalDefault=1200;
int MidDefault=1200;
int UpDefault=1100;

int DefaultThreshold = 50;// Threshold needed to cause a change in the seat - Higher=less sensitive, lower=more sensitive

boolean ThighOccupantFlag=false; // Flags used for occupant detection
boolean PedalOccupantFlag=false;
boolean MidOccupantFlag=false;
boolean UpOccupantFlag=false;

boolean Occupant=false;

boolean ButtonPressed=false;


//POSSIBLE IMPROVEMENT: Proportional increase in pressure from the default (Ex: thigh and upper ideal should increase proportionally, as should the mid and pedal - ~30% above the thigh/up increase)
int ThighIdeal=1200;//Ideal pressure levels for each region (Value is arbitrary, could be converted to actual PSI value given the right calculation
int PedalIdeal=2200;
int MidIdeal=1800;
int UpIdeal=1400;

char Pressures[6];

unsigned long Time;

/*
  Default value = analog value from Arduino where the seat has no occupant. Default value should be virtually the same on every startup
  Ideal value = Proportional increase from default value with an occupant. Thigh pressure and pelvic pressure should have a proportional increase from default as well as mid/lum/upper
*/
void OccupantDetection();
void ReadPressure();
void CompareToIdeal();

void setup() 
{
  Serial.begin (9600); // TX pin of Arduino Pro Mini connected to RX of Serial3 on Arduino Mega
  
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
 
  adc.setADCSpeed(ADCSpeed);
  adc.setBitsOfResolution(14);
  adc.setNumSamplesToAvg(1); 
  
  pinMode (A0, INPUT);
  pinMode (A1, INPUT);
  pinMode (A2, INPUT);
  pinMode (A3, INPUT);
}

void loop() 
{
  /*ReadPressure();
  
  Serial.print ("THIGH \t");
  Serial.print (ThighBP);
  Serial.print ("|  PEDAL \t");
   Serial.print (PedalBP);
   Serial.print ("|  MID \t");
    Serial.print (MidBP);
    Serial.print ("|  UP \t");
     Serial.println (UpBP);
 //long total1 =  cs_4_2.capacitiveSensor(30);
  //Serial.println(total1);
 */
  ReadPressure(); // Analog read of pressure values (10bit value (0-1023), with oversampling this could be increased to 14-16bit if necessary)
  OccupantDetection();  //Checks if occupant is in the seat, wont bother sending data if there is not
  if (Occupant==true) CompareToIdeal(); //Compares to ideal and converts value to high, low or nominal rather than numerical value (Easier to send over serial than multi digit data sets)
  if (Occupant==true&&Time+50<millis())// Sends data every 50ms (Shutoff on motor control if data not received at least every 100ms)
  {
    Time = millis(); //Timer reset for serial write
    
    for (int i=0;i<5;i++)// Writes each part of data to Arduino Mega for motor control
    {
      Serial.print (Pressures[i]);
    }
    Serial.println("");
  }
}

void ReadPressure()//From above
{
  ThighBP = adc.newAnalogRead (A0);
  PedalBP = adc.newAnalogRead (A3);
  MidBP = adc.newAnalogRead (A1);
  UpBP = adc.newAnalogRead (A2);
  /*
  ThighBP = analogRead (A0);
  PedalBP = analogRead (A3);
  MidBP = analogRead (A1);
  UpBP = analogRead (A2);*/
  //Conversion to voltage?
  //Converstion to KPa/PSI?
}

void OccupantDetection() //As any measureable increase in pressure will trigger the threshold, if someone sits in the seat an occupant will be detected and data will begin to read out
{
  if (ThighBP>ThighDefault+DefaultThreshold) ThighOccupantFlag = true;
  else ThighOccupantFlag = false;
  
  if (PedalBP>PedalDefault+DefaultThreshold) PedalOccupantFlag = true;
  else PedalOccupantFlag = false;
  
  if (MidBP>MidDefault+DefaultThreshold) MidOccupantFlag = true;
  else MidOccupantFlag = false;
  
  if (UpBP>UpDefault+DefaultThreshold) UpOccupantFlag = true;
  else UpOccupantFlag = false;
  
  if (ThighOccupantFlag==true||PedalOccupantFlag==true||MidOccupantFlag==true||UpOccupantFlag==true) Occupant = true;// If any flag is thrown, registers an occupant
  else Occupant = false;
}

void CompareToIdeal()
{
  if (cs_4_2.capacitiveSensor(30)>800)Pressures[0] = '1'; //Capacitive switch to enable/disable motor control
  else Pressures[0] = '0';
  
  if (ThighBP>ThighIdeal+DefaultThreshold*2) Pressures[1] = 'H';// Pressure increase over threshold enables movements. Registers change as H/L/N and sends data to move seat accordingly
  else if (ThighBP<ThighIdeal-DefaultThreshold*2) Pressures[1] = 'L';
  else if (ThighBP<ThighIdeal+DefaultThreshold*2&&ThighBP>ThighIdeal-DefaultThreshold*2) Pressures[1] = 'N';
  
  
    if (PedalBP>PedalIdeal+DefaultThreshold*3) Pressures[2] = 'H';
    else if (PedalBP<PedalIdeal-DefaultThreshold*3) Pressures[2] = 'L';
    else if (PedalBP<PedalIdeal+DefaultThreshold*3&&PedalBP>PedalIdeal-DefaultThreshold*3) Pressures[2] = 'N';
 
  
  if (MidBP>MidIdeal+DefaultThreshold*2) Pressures[3] = 'H';
  else if (MidBP<MidIdeal-DefaultThreshold*2) Pressures[3] = 'L';
  else if (MidBP<MidIdeal+DefaultThreshold*2&&MidBP>MidIdeal-DefaultThreshold*2) Pressures[3] = 'N';
  
  if (UpBP>UpIdeal+DefaultThreshold/2) Pressures[4] = 'H';
  else if (UpBP<UpIdeal-DefaultThreshold/2) Pressures[4] = 'L';
  else if (UpBP<UpIdeal+DefaultThreshold/2&&UpBP>UpIdeal-DefaultThreshold/2) Pressures[4] = 'N';
}







