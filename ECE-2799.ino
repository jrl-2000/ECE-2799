#include "MQ135.h"
#include <avr/interrupt.h>
#define buzzer 11 //change to any output pin (not analog inputs) 
#define PPM_BUFFER_SIZE 5 // size - 1
#define PPM_THRSHD 600

const int ANALOGPIN=0;

MQ135 gasSensor = MQ135(ANALOGPIN);

unsigned int counter = 0;
unsigned int i = 0;
int decre = 0; // 0--increament, 1--decreament

float ppm;
unsigned index = 0;
float sumppm = 0.0f;
float ppm_buff [PPM_BUFFER_SIZE];

void setup(){
  cli();//stop interrupts

  //set timer0 interrupt at 1kHz
    TCCR0A = 0;// set entire TCCR0A register to 0
    TCCR0B = 0;// same for TCCR0B
    TCNT0  = 0;//initialize counter value to 0
    // set compare match register for 1khz increments
    OCR0A = 249;// = (16*10^6) / (1000*64) - 1 (must be <256)
    // turn on CTC mode
    TCCR0A |= (1 << WGM01);
    // Set CS01 and CS00 bits for 64 prescaler
    TCCR0B |= (1 << CS01) | (1 << CS00);   
    // enable timer compare interrupt
    TIMSK0 |= (1 << OCIE0A);

  //set timer1 interrupt at 1Hz
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler
    TCCR1B |= (1 << CS12) | (1 << CS10);  
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);

  // //set timer2 interrupt at 8kHz
  //   TCCR2A = 0;// set entire TCCR2A register to 0
  //   TCCR2B = 0;// same for TCCR2B
  //   TCNT2  = 0;//initialize counter value to 0
  //   // set compare match register for 8khz increments
  //   OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  //   // turn on CTC mode
  //   TCCR2A |= (1 << WGM21);
  //   // Set CS21 bit for 8 prescaler
  //   TCCR2B |= (1 << CS21);   
  //   // enable timer compare interrupt
  //   TIMSK2 |= (1 << OCIE2A);


  sei();//allow interrupts

  Serial.begin(9600);
  pinMode(buzzer, OUTPUT); //tell arduino the buzzer is an output device

  // load ppm_buffer
  for (int i = 0; i < PPM_BUFFER_SIZE; i++) {
    ppm_buff [i] = 0.0f;
  }
}

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz
  char readings [500];
  ppm = gasSensor.getPPM();
  Serial.println(ppm);

  if (index < PPM_BUFFER_SIZE - 1) {
    sumppm += ppm;
    sumppm = sumppm - ppm_buff[index];
    ppm_buff[index] = ppm;
    index ++;
  } else {
    sumppm += ppm;
    sumppm = sumppm - ppm_buff[index];
    ppm_buff[index] = ppm;
    index = 0;
  }

  // Serial.println(sumppm);
}

ISR(TIMER0_COMPA_vect){//timer1 interrupt at 1kHz
  if (counter > 9) {
    counter = 0;
    if (i == 255) {
      decre = 1;
    } else if (i == 0) {
      decre = 0;
    }
    if (decre) { // if decreament
      i --;
    } else {
      i ++;
    }
    if (sumppm > PPM_BUFFER_SIZE * PPM_THRSHD)  
      analogWrite(buzzer, i); //raise the voltage sent out of the pin by 1
    else
      analogWrite(buzzer, 0); //stop the buzzer
  } else {
    counter ++;
  }
}

void loop(){

}


// words in the double slash are not part of the code

// #define buzzer 11 //change to any output pin (not analog inputs) 

// void setup() { //run this code once
//   pinMode(buzzer, OUTPUT); //tell arduino the buzzer is an output device
// }

// void loop(){ //loop forever...
  
//   for(int i=0; i<255; i++) { //do this 255 times
//     analogWrite(buzzer, i); //raise the voltage sent out of the pin by 1
//     delay(10); //wait 10 milliseconds to repeat 
//   }

//   for(int i=255; i>0; i--) { // do this 255 times
//     analogWrite(buzzer, i); //lower the voltage sent out of the pin by 1
//     delay(10); //wait 10 milliseconds to repeat
    
//   }
// }
