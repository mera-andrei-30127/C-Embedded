unsigned long pulseStartTime, echoStartTime, durationOfEcho, timeOfLastInterrupt;
int distance,distance1;
boolean previousInterruptWasHigh = false;
boolean waitingForEcho = false;
boolean sendingPulse = false;

int digit=0,i=0;
 

void display(char p, char c)
{
    PORTB &= 0b11000010;
    PORTD &= 0b00000000;
    
    switch(c){
       case 0:
           PORTD |= 0b00111111; break;
       case 1:
           PORTD |= 0b00000110; break;
       case 2:
           PORTD |= 0b01011011; break;
       case 3:
           PORTD |= 0b01001111; break;
       case 4:
           PORTD |= 0b01100110; break;
       case 5:
           PORTD |= 0b01101101; break;
       case 6:
           PORTD |= 0b01111101; break;
       case 7:
           PORTD |= 0b00000111; break;
       case 8:
           PORTD |= 0b01111111; break;
       case 9:
           PORTD |= 0b01101111; break;
              }
             
     switch(p)
 {
    case 1: PORTB = 0b00001111;
            break;
    case 2: PORTB = 0b00010111;
            break;
    case 3: PORTB = 0b00011011;
            break;
    case 4: PORTB = 0b00011110;
            break;
   }
}
 ISR(PCINT1_vect){  
  timeOfLastInterrupt = millis();
  if(PINC & B00001000 ){                                     
    if(!previousInterruptWasHigh){                        
      echoStartTime = micros();
      previousInterruptWasHigh = true;                  
    }
  }else{
    durationOfEcho = micros() - echoStartTime; 
    waitingForEcho = false; 
    previousInterruptWasHigh = false;
  }
}

ISR(TIMER2_COMPA_vect)
{

  digit++;
  switch(digit){
  case 1: display(1,distance%10); break;
  case 2: display(2,(distance/10)%10); break;
  case 3: display(3,(distance/100)%10); break;
  case 4: display(4,(distance/1000)%10); digit=0; break;}

}

void loop(){
  if(!waitingForEcho){   
   if(!sendingPulse){
        pulseStartTime = micros(); 
        PORTC |= B00000100;
        sendingPulse = true; 
      }else if(pulseStartTime + 12 < micros()){ 
        PORTC &= B00000000;
        sendingPulse = false;
        waitingForEcho = true; 
      }
  }

  distance = ((int)durationOfEcho/2)/29.1;
  
  init_bec();
  init_buzzer();
  
  if(millis()-timeOfLastInterrupt > 20){
    distance = -1;
  }
}
void init_bec(){
DDRB |= 0b00000010;
TCCR1A = 0b10000011;
TCCR1B = 0b00001101;
TCNT1=0;
OCR1A = distance;
}

void init_timer1()
{
  TCCR2A = 0b00000010;
  TCCR2B = 0b00000011;
  TCNT2 = 0;
  OCR2A = 249;
  TIMSK2 |= 0b00000010;
 }
void init_buzzer(){
  DDRB |= 0b00100000;
   int i;
  for(i=0;i<=1;i++)
      {
     PORTB|=0b00111101;
    delay(100);//wait for 1ms
    PORTB&=0b00011101;
    delay(distance);//wait for 1ms
      }
  
}
void setup(){
 /// Serial.begin(9600);
  DDRC |= B00000100;
  DDRB |= 0b00011101;
  DDRD |= 0b11111111; 
  init_timer1();
  SREG |= (1<<7);
  PCICR |= (1 << PCIE1);
  PCMSK1 |= (1 << PCINT11);
}
