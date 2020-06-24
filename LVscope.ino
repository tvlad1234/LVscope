///Arduino LabVIEW Oscilloscope
///Vlad Tomoiaga 2020
int numSamples=0;
byte sampmem[1030]={0};
bool trig=0;
long t, t0;
byte trigLevel= 193;
int sampFreq=1;

void selectFreq()
{
  ADCSRA=0;
  ADCSRB=0;
  if(sampFreq == 0)
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADPS2);  //128 prescaler for 9.6 KHz
  else if(sampFreq == 1)
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) &~(1 << ADPS0);   //64 prescaler for 19.2 KHz
  else if(sampFreq == 2)
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0) &~(1 << ADPS1);   // 32 prescaler for 38.5 KHz
  else if(sampFreq == 3)
  ADCSRA |= (1 << ADPS2)&~(1 << ADPS1)&~(1 << ADPS0);     // 16 prescaler for 76.9 KHz
  else if(sampFreq == 4)
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0) &~(1 << ADPS2);   // 8 prescaler for 153.8 KHz
  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
  //delay();  
}


void setup()
{
  Serial.begin(250000);

  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register
  ADCSRA |= (1 << ADPS2); ADCSRA |= (1 << ADPS1); 
  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
}

ISR(ADC_vect)
{
  sampmem[numSamples] = ADCH;  // read 8 bit value from ADC
    if(numSamples>0 && (trigLevel<=sampmem[numSamples] && trigLevel>=sampmem[numSamples-1]) && (sampmem[numSamples-1])<=sampmem[numSamples] )
    {
      if(!trig){
        numSamples=0;
        trig=1;
      }
    }
  numSamples++;
}
  
void loop()
{
  if (numSamples>=1023 )
  {
    ADCSRA &= ~(1 << ADIE);
    t = micros()-t0;  // calculate elapsed time
    float sampTime=t/numSamples;
    if(Serial.available()){ 
      byte c=Serial.read();
      if (c==123)
      {
        delay(2);
        trigLevel=Serial.parseInt();
      }
      else if(c == 's')
      {
        sampFreq=Serial.parseInt();
        selectFreq();
      }
 
      else{
  // Serial.print("Sampling frequency: ");
   //Serial.print((float)1000000/t);
   //Serial.println(" KHz");
    for(int i=0;i<1024;i++)
    {
      Serial.println(5*sampmem[i]/255.0);
    }
    // restart
    
    }
    }
    trig=0;
    t0 = micros();
    numSamples=0;
    ADCSRA |= (1 << ADIE);
  }
}
