#include "aquisition.h"


bool EnergyMonitor::calcVI_rms(unsigned int inPinI,unsigned int inPinV, int offsetI, int offsetV, byte numSensors_E)
{
  
   #define NOP __asm__ __volatile__ ("nop\n\t")
   bool FFT_result;
   float filteredI_reel,filteredV_reel; 
   int samples;
   unsigned long start_micro;
   extern float Ical1;
   extern float Vcal;
   int sampleI,sampleV,filteredI,filteredV;
   int i;
   unsigned int crossCount = 0;                             //Used to measure number of times threshold is crossed.
   unsigned int numberOfSamples = 0;                        //This is now incremented
   unsigned int echantillons;
   sumI=0;
  //-------------------------------------------------------------------------------------------------------------------------
  // 1) Waits for the waveform to be close to 'zero' (mid-scale adc) part in sin curve.
  //-------------------------------------------------------------------------------------------------------------------------
  boolean st=false;                                  //an indicator to exit the while loop
  unsigned long start ;    //millis()-start makes sure it doesnt get stuck in the loop if there is an error.
  numberOfSamples=0;
  FFT_result=1;
  if((inPinI==0)|(inPinI==1)|(inPinI==2)){  // calcul energie et autres
  numberOfSamples=0;
  start = millis();
  do
  { 
    numberOfSamples++;
    sampleI = analogRead(inPinI);
    sampleV=analogRead(inPinV);
    filteredI = sampleI - offsetI;
    filteredV= sampleV-offsetV;
    filteredI_reel=filteredI*0.00322265;             //3.3/1024
    filteredV_reel=filteredV*0.00322265; 
    sqI =  filteredI_reel *  filteredI_reel;    //1) square current values
    sqV =  filteredV_reel *  filteredV_reel;    //1) square current values
    sumV += sqV;                                //2) sum
    sumI += sqI;                                //2) sum
    instP = filteredV_reel * filteredI_reel;          //Instantaneous Power
    sumP +=instP;
  
 } while(millis()-start<=80);    // 40 ms deux periodes
  if(numberOfSamples<=0) numberOfSamples=1;
  echantillons=numberOfSamples;
  Irms = sqrt(sumI/echantillons); 
  Vrms=sqrt(sumV/ echantillons);
  realPower=sumP*Vcal*Ical1/echantillons;
  apparentPower = Vrms *Vcal*Irms*Ical1;
  if(apparentPower<=0) {
    apparentPower=1;
    powerFactor=1;
  }
  else  powerFactor=realPower / apparentPower;
  
  }
  //Reset accumulators
  sumV = 0;
  sumI = 0;
  sumP = 0;
  sumI = 0.;

return FFT_result;
}



//...................................................................................................................................



int EnergyMonitor::calcVI_offset(unsigned int inPinI,unsigned int crossings, unsigned int timeout)
{

  unsigned int offset_I;
  extern int samples;
  unsigned int crossCount = 0;                             //Used to measure number of times threshold is crossed.
  unsigned int numberOfSamples = 0;                        //This is now incremented
  int sampleI,sampleI_ancien;
  //-------------------------------------------------------------------------------------------------------------------------
  // 1) Waits for the waveform to be close to 'zero' (mid-scale adc) part in sin curve.
  //-------------------------------------------------------------------------------------------------------------------------
  boolean st=false;                                  //an indicator to exit the while loop
  sumI_Moy=0;
  unsigned long start = millis();    //millis()-start makes sure it doesnt get stuck in the loop if there is an error.
  startV = analogRead(inPinI );  //using the voltage waveform
 
  while(st==false)                                   //the while loop...
  {
    startV_ancien=startV;
    startV = analogRead(inPinI );  //using the voltage waveform
    if ((startV_ancien < 512) && (startV >= 512)) st=true;
    if ((millis()-start)>timeout) st = true;
  }
 //-----------------------------------------------------------------------------------------------------------------------
  start = millis();
  sampleI= analogRead(inPinI ); 
  while ((crossCount < crossings) && ((millis()-start)<timeout))
  {
  
    numberOfSamples++;                       
    sampleI_ancien=sampleI;
    sampleI = analogRead(inPinI );
    sumI_Moy += sampleI;                        //2) sumI_moy: calcul valeur moyenne
    if ((sampleI >=512) && (sampleI_ancien < 512)) crossCount++;   
  }
  if(numberOfSamples<=0) numberOfSamples=1;
    offset_I=sumI_Moy/numberOfSamples;
    return(offset_I);
}
