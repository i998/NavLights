/* ---------------------------------------------------------------------------
** Navigation Lights with Battery Voltage Alarm (optional - TODO)  
** V0.8  - test version based on project:protothread Library
** Websites: http://harteware.blogspot.com.au/2010/11/protothread-powerfull-library.html
**           http://harteware.blogspot.com.au/2010/11/protothread-and-arduino-first-easy.html
**           http://dunkels.com/adam/pt/examples.html               
** Download: https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/arduinode/pt.zip
**
** ----------------------------------------------------------------------
**-----------Design Notes-------------------------------------------------------
** 1. The protothread Library allows to  run several threads in  parallel which updates global variables to 
**    exchange information betweeh the therads. Local variables are not safe (protothreads do not save the 
**    stack context across a blocking call, local variables are not preserved when the protothread blocks - 
**    please see the protothread library documentation for details) so each thread uses its own set of local
**    variables as a precaution to ensure other similar threads do not affect them.  
** 2. There is a Control Thread (pt0) that can switch on and  off each other thread. Control Thread  can detect 
**    some input changes such as battery voltage, switch off a normal blinking thread and switch on an "emergency"
**    blinking thread and a sound tone thread. That is how different audio and  visual effects can be made.
** 3. In  this  version voltage measurement for each LiPO cell has not been implemented. Instead the Control Thread 
**    can read voltage (0-5v only!) from one of the Arduino input pins and  switch the threads just to  prove functionality. 
** 4. DEBUG mode could potentially affect the Brightness Boost Thread  as debug info output via serial port  takes too 
**    much of CPU time.  
** 5. It has a separate thread to fade-in and fade out a LED using Arduino PWM function. 
**-----------End of Design Notes------------------------------------------------
--*/


//INITIAL SETUP 
#include <pt.h>  // include protothread library

//DEBUG settings
#define DEBUG 0 // 0 - no debug 

//set up output pins
#define LIGHT1_PIN 13 //Single Blink
#define LIGHT2_PIN 3  //Strobe
#define LIGHT3_PIN 4  //Double Strobe
#define LIGHT4_PIN 5  //Double Strobe
#define LIGHT5_PIN 6 
#define LIGHT6_PIN 7
#define SOUND1_PIN 8 // sound output. It  uses tone() function - it will interfere with PWM output on pins 3 and 11 
#define PWM_PIN 9 // PWM output pin: 3, 5, 6, 9, 10, and 11.  
                      

//set up voltage input pins
#define VOLTAGE_IN_1_PIN A1
#define VOLTAGE_IN_2_PIN A2
#define VOLTAGE_IN_3_PIN A3
#define VOLTAGE_IN_4_PIN A4
#define VOLTAGE_IN_5_PIN A5
#define VOLTAGE_IN_6_PIN A6


//set Light on and off settings
#define LIGHT_ON HIGH
#define LIGHT_OFF LOW

//set Fade-In and Fade-Out settings 
#define PWM_FADE_AMOUNT 10 // how many points to use for each step of diming the LED by (0..255)
#define PWM_FADE_PAUSE_AMOUNT 0 //how many points to use for brightness at pause (0..255)  
#define PWM_FADE_MIN_AMOUNT 0 //how many points to use for minimal brightnes  (0..255)
#define PWM_FADE_MAX_AMOUNT 255 //how many points to use for maximum brightnes (0..255)
#define PWM_FADE_TIME_SEC 0.7 //how long the light will fade-in or fade-out in seconds 
#define PWM_FADE_PAUSE_SEC 1.5 //how long the light will stay off after fade-out in seconds   



//set up voltage calculations corrections 
//TODO - there shall be some voltage calculations constants 
//for voltage input pins based on the hardware voltage divider used.


// set thread switches to 1 so all the threads are running 
// threads  execute when switches are 1  
bool Thread1_SWITCH = 1;  //Single Blink
bool Thread2_SWITCH = 1;  //Double Strobe
bool Thread3_SWITCH = 1;  //Strobe
bool Thread4_SWITCH = 1;  //Sound Tone
bool Thread5_SWITCH = 1;  //Fade-In and Fade Out

//END OF INITIAL SETUP

//OTHER GLOBAL VARIABLES 
//set variable to represent ststus of a pulse driver (not used curently - for future use if needed)
bool PulseDriver=1;

//set up Threads 
static struct pt pt0, pt1, pt2, pt3, pt4, pt5; // each protothread needs one of these

//calculate time in milliseconds of a single step of a fade-in or fade-out 
//based on the total fade time and fade step - total time in miliseconds devided by 
//number of dimming steps. 
int DIM_STEP_INTERVAL=(int)((PWM_FADE_TIME_SEC * 1000)/(255/PWM_FADE_AMOUNT)); 
int FADE_PAUSE_INTERVAL =(int) (PWM_FADE_PAUSE_SEC*1000);   
//END OF OTHER GLOBAL VARIABLES 



void setup() {
//Initialise thread variables 
  PT_INIT(&pt0);  
  PT_INIT(&pt1);  
  PT_INIT(&pt2);  
  PT_INIT(&pt3);  
  PT_INIT(&pt4);
  PT_INIT(&pt5);

//Set output pins  
pinMode(LIGHT1_PIN, OUTPUT);      // sets the digital pin as output
pinMode(LIGHT2_PIN, OUTPUT);      // sets the digital pin as output
pinMode(LIGHT3_PIN, OUTPUT);      // sets the digital pin as output
pinMode(LIGHT4_PIN, OUTPUT);      // sets the digital pin as output
pinMode(LIGHT5_PIN, OUTPUT);      // sets the digital pin as output
pinMode(LIGHT6_PIN, OUTPUT);      // sets the digital pin as output
pinMode(SOUND1_PIN, OUTPUT);      // sets the digital pin as output
pinMode(PWM_PIN, OUTPUT);         // sets the digital pin as output

//set output pins to to off 
digitalWrite(LIGHT1_PIN, LIGHT_OFF);
digitalWrite(LIGHT2_PIN, LIGHT_OFF);
digitalWrite(LIGHT3_PIN, LIGHT_OFF);
digitalWrite(LIGHT4_PIN, LIGHT_OFF);
digitalWrite(LIGHT5_PIN, LIGHT_OFF);
digitalWrite(LIGHT6_PIN, LIGHT_OFF);
digitalWrite(SOUND1_PIN,LOW);
digitalWrite(PWM_PIN,LOW);  

//check DEBUG mode and initialize the serial connection if  needed:
  if (DEBUG !=0) {
  Serial.begin(9600);
  Serial.println("NavLights Started ");
  }

}//end of setup()


//DEFINE THREADS
// Control Thread:
static int protothread0(struct pt *pt) {
  static unsigned long timestamp0 = 0;
  PT_BEGIN(pt);

    if (DEBUG !=0) {
    // Print the status message:
    Serial.println("Control Thread called. ");
    }

  while(1) { // never stop 

//  This is an example on how to switch Tread1 on and off every 10 sec
//    //Do action 
//    Thread1_SWITCH=0; //off first thread
//    //Wait  
//    PT_WAIT_UNTIL(pt, millis() - timestamp0 > 10000 );
//    timestamp0 = millis(); // take a new timestamp
//    //Do action
//    Thread1_SWITCH=1; //on first thread
//    //Wait  
//  PT_WAIT_UNTIL(pt, millis() - timestamp0 > 10000 );
//  timestamp0 = millis(); // take a new timestamp



//  This is an example on how to to switch Tread1 and Thread4 on and off based on voltage on A0
//    //Do action 
//    if  (analogRead(A0)<500) //analogRead maps input voltages between 0 and 5 volts into integer values between 0 and 1023.
//    {
//      Thread1_SWITCH=0; //switch off first thread
//      Thread4_SWITCH=0; //switch off fourth thread
//    }
//    else 
//    {
//      Thread1_SWITCH=1; //switch on first thread
//      Thread4_SWITCH=1; //switch on fourth thread
//    }


//Now just switch Thread4 to off 
      Thread4_SWITCH=0; //switch off fourth thread

// This is a loopback - wait for 500 ms 
    //Wait  
    PT_WAIT_UNTIL(pt, millis() - timestamp0 > 500 );
    timestamp0 = millis(); // take a new timestamp

  }
  PT_END(pt);
}


// First custom thread - Single Blink
static int protothread1(struct pt *pt) {
  static unsigned long timestamp1 = 0;
  PT_BEGIN(pt);

    if (DEBUG !=0) {
    // Print the status message:
    Serial.println("Thread 1 called.");
    }

  while(Thread1_SWITCH) { 
    //Do action 
    digitalWrite(LIGHT1_PIN, LIGHT_ON);
    //Wait  
    PT_WAIT_UNTIL(pt, millis() - timestamp1 > 200 );
    timestamp1 = millis(); // take a new timestamp
    //Do action
    digitalWrite(LIGHT1_PIN, LIGHT_OFF);
    //Wait  
    PT_WAIT_UNTIL(pt, millis() - timestamp1 > 200 );
    timestamp1 = millis(); // take a new timestamp

  }
  PT_END(pt);
}


// Second custom Thread - Double Strobe
static int protothread2(struct pt *pt) {
  static unsigned long timestamp2 = 0;
  PT_BEGIN(pt);

    if (DEBUG !=0) {
    // Print the status message:
    Serial.println("Thread 2 called.");
    }

  while(Thread2_SWITCH) { 
    //Do action 
    digitalWrite(LIGHT3_PIN, LIGHT_ON);
    digitalWrite(LIGHT4_PIN, LIGHT_ON);
    //Wait
    PT_WAIT_UNTIL(pt, millis() - timestamp2 > 50 );
    timestamp2 = millis();
    //Do action 
    digitalWrite(LIGHT3_PIN, LIGHT_OFF);
    digitalWrite(LIGHT4_PIN, LIGHT_OFF);
    //Wait
    PT_WAIT_UNTIL(pt, millis() - timestamp2 > 50 );
    timestamp2 = millis();
    //Do action 
    digitalWrite(LIGHT3_PIN, LIGHT_ON);
    digitalWrite(LIGHT4_PIN, LIGHT_ON);
    //Wait
    PT_WAIT_UNTIL(pt, millis() - timestamp2 > 50 );
    timestamp2 = millis();
    //Do action 
    digitalWrite(LIGHT3_PIN, LIGHT_OFF);
    digitalWrite(LIGHT4_PIN, LIGHT_OFF);
    //Wait
    PT_WAIT_UNTIL(pt, millis() - timestamp2 > 600 );
    timestamp2 = millis();
  
  }
  PT_END(pt);
}



// Third custom Thread -  Strobe
static int protothread3(struct pt *pt) {
  static unsigned long timestamp3 = 0;
  PT_BEGIN(pt);

    if (DEBUG !=0) {
    // Print the status message:
    Serial.println("Thread 3 called.");
    }

  while(Thread3_SWITCH) { 
    //Do action 
    digitalWrite(LIGHT2_PIN, LIGHT_ON);
    //Wait
    PT_WAIT_UNTIL(pt, millis() - timestamp3 > 50 );
    timestamp3 = millis();
    //Do action 
    digitalWrite(LIGHT2_PIN, LIGHT_OFF);
    //Wait
    PT_WAIT_UNTIL(pt, millis() - timestamp3 > 800 );
    timestamp3 = millis();
  
  }
  PT_END(pt);
}


// Fourth custom Thread - Sound Tone
static int protothread4(struct pt *pt) {
  static unsigned long timestamp4 = 0;
  PT_BEGIN(pt);

    if (DEBUG !=0) {
    // Print the status message:
    Serial.println("Thread 4 called.");
    }

  while(Thread4_SWITCH) { 
    //Do action 
    tone(SOUND1_PIN, 2500);
    //Wait  
    PT_WAIT_UNTIL(pt, millis() - timestamp4 > 500 );
    timestamp4 = millis(); // take a new timestamp
    //Do action
    noTone(SOUND1_PIN);
    //Wait  
    PT_WAIT_UNTIL(pt, millis() - timestamp4 > 500 );
    timestamp4 = millis(); // take a new timestamp
  
  }
  PT_END(pt);
}



// Fifth custom Thread -  Fade-In and Fade Out
static int protothread5(struct pt *pt) {
  static int brightness = PWM_FADE_MIN_AMOUNT;    // how bright the LED is
  static int fadeAmount = PWM_FADE_AMOUNT;    // how many points to fade the LED by
  static unsigned long timestamp5 = 0;
  PT_BEGIN(pt);

    if (DEBUG !=0) {
    // Print the status message:
    Serial.println("Thread 6 called.");
    }

  while(Thread5_SWITCH) { 
    //Do action 
    analogWrite(PWM_PIN,constrain(brightness, 0, 255)); // set the brightness of the PWM pin   
	//Wait
    PT_WAIT_UNTIL(pt, millis() - timestamp5 > DIM_STEP_INTERVAL );  // wait to see the dimming effect
    timestamp5 = millis();
	
	//Prepare for the next loop
	// change the brightness for next time through the loop:
	brightness = brightness + fadeAmount;
    // reverse the direction of the fading at the ends of the fade:
    if (brightness <= PWM_FADE_MIN_AMOUNT || brightness >= PWM_FADE_MAX_AMOUNT) {
     fadeAmount = -fadeAmount;
	}

    //Wait extra time before fade-in again   
    if (brightness <= PWM_FADE_MIN_AMOUNT) {
    analogWrite(PWM_PIN,constrain(PWM_FADE_PAUSE_AMOUNT, 0, 255)); // set the brightness of the PWM pin    
	  //Wait  
    PT_WAIT_UNTIL(pt, millis() - timestamp5 > FADE_PAUSE_INTERVAL );
    timestamp5 = millis();
    }
 
  }
  PT_END(pt);
}
//END OF DEFINE THREADS

void loop() {
  // schedule  protothreads by calling them infinitely
  protothread0(&pt0); 
  protothread1(&pt1); 
  protothread2(&pt2);
  protothread3(&pt3);
  protothread4(&pt4);
  protothread5(&pt5);
}//end of loop()









