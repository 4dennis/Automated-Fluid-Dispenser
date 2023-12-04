//Fully functional code for group 54's AFD


//add libraries
#include <AFMotor.h>
#include <Servo.h>

//setup buttons
#define pbStart A1
#define pbStop A2
#define limitSwitch A3

//set button states
bool pbStartState = HIGH;
bool pbStopState = HIGH;
bool limitSwitchState = HIGH;
bool startProgram = false;



//define DCmotor and base speed
AF_DCMotor DCmotor(1);
const int DCSpeed = 110;

//setup servo - we are using a continious servo FS90
#define myServo_PIN_SIG	2
Servo myServo;
int STOP = 93; //stop speed for continious servo

//setup water pump
AF_DCMotor pump(4);


//declare array to hold the amount of time to dispense
const unsigned long ST[5] = {18000, 18000, 36000, 18000, 18000};             //servo time 18000ms/g         mass (g) * rate = ms  //final task 1g, 1g, 2g, 1g, 1g
const unsigned long WT[5] = {21367, 21367, 21367, 21367, 21367};            //water time 0.000936ml/ms     vol (ml) / rate = ms  //20mL in each

int count = 0; //for each test tube count
int lengthDelay = 10; //universal delay length


//---------------
//Functions

//Timing Code
//servo run for this amount of time
void servoTime(long x){
  Serial.print("servoTime\n");

  for(long i=0; i < x; i += lengthDelay){
    pbStopState = digitalRead(pbStop); //get button state each time

    if(pbStopState == LOW){ //emergency switch check
      emergencyStop();
    }
    else{
    myServo.write(110); // run servo
    delay(lengthDelay);
    }
  }
  myServo.write(STOP); //stop when time done
}

//DCMotor run for this amount of time
void dcTime(long x){
  Serial.print("dcTime\n");
  DCmotor.run(FORWARD); //start with motor run


  for(long i=0; i < x; i += lengthDelay){
  pbStopState = digitalRead(pbStop); //check button states
  limitSwitchState = digitalRead(limitSwitch);


    if(pbStopState == LOW){
      emergencyStop();
    }
    else if(limitSwitchState == LOW){ //once testube hit limit switch stop motor, exit loop
      DCmotor.run(RELEASE);
      Serial.print("dcStop\n");

      return;
    }
    else{
    Serial.print("dcRun\n");

    DCmotor.run(FORWARD);
    
    }
  }
  DCmotor.run(RELEASE);
}

//Water Pump run for this amount of time
void pumpTime(long x){
  //run pump for given time unless emergency stopped
  Serial.print("pumpTime\n");

  for(long i=0; i < x; i += lengthDelay){
    pbStopState = digitalRead(pbStop);

    if(pbStopState == LOW){
      emergencyStop();
    }
    else{
    pump.setSpeed(255);
    pump.run(FORWARD);
    delay(lengthDelay);
    }
  }
  pump.run(RELEASE);
}


//Emergency Stop code
void emergencyStop(){
  while(1<2){
    if (pbStopState==LOW){
      DCmotor.run(RELEASE); //stop DC
      myServo.write(STOP); //stop servo
      pump.run(RELEASE);  //stop water pump
      Serial.print("emergency stop hit\n");  
      
    }  
    
    for(;;){ //endless loop to stop
      DCmotor.run(RELEASE); //stop DC
      myServo.write(STOP); //stop servo
      pump.run(RELEASE);  //stop water pump
    } 
    return;
  }
}

// 

void setup() {
  // Start serial monitor
    Serial.begin(9600);

  pinMode(pbStart, INPUT_PULLUP);
  pinMode(pbStop, INPUT_PULLUP);
  pinMode(limitSwitch, INPUT_PULLUP);
  myServo.attach(9); // attach the servo
  myServo.write(STOP); //stop servo
  DCmotor.setSpeed(DCSpeed);
  DCmotor.run(RELEASE); // stop DC motor
  pump.run(RELEASE); //stop water pump

  Serial.print("completed setup\n");


}

void loop() {

  //Serial.print("looping\n");
  //get button states
  pbStartState = digitalRead(pbStart);
  pbStopState = digitalRead(pbStop);
  limitSwitchState = digitalRead(limitSwitch);


 if(pbStopState==LOW){
    emergencyStop();
 }
 else if(pbStartState == LOW){
    startProgram = true; // set flag to be able to start program
    Serial.print("Starting program\n");
    delay(1000); //delay program from start button
 }
 else if (count < 5 && startProgram == true){
    Serial.print(count);
    Serial.print("running\n");
    

    dcTime(15000); //run the motor, it goes off limit switch to stop but otherwise will stop in default 15seconds 
    delay(500); //small delay between actions
    servoTime(ST[count]); //servo code based on array position for test tube position 
    delay(500);
    pumpTime(WT[count]);
    delay(500);

    count++; //increse test tube count
 }
}




