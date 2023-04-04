#include <ezButton.h>

#include "ArduPID.h"

ArduPID myController;

#include "PID_RT.h"

PID_RT PID;

unsigned long previousMillis = 0;

const long interval = 5000;

// To calculate the commanded power

float error = 0;
float errorPrev = 0;
float derivative = 0;
float integral = 0;
float period = 1;
float uk = 0;
float ukprev = 0;


// Create ezButton objects for different buttons

// For the Train Driver

ezButton setSetpoint(7); // create eZButton object that attaches to pin 7, for setting the setpoint

ezButton buttonDoors(6); // button for opening and closing doors

ezButton buttonLights(5); // button for turning on and off the interior and exterior lights

ezButton incrSetpoint(4); // button for increasing the setpoint speed

ezButton decrSetpoint(3); // button for decreasing the setpoint speed


// For the Engineer

ezButton joystick(2);


  String station[10] = {"Carnegie","Bell","Inglewood","Crafton","Ingram","Sheraden","Allegheny","North Side","Gateway Center","Wood Street"};
  // define variable for current station
  String currStation;
  
  // define variables for state of doors and lights
  bool doors, interiorLights, exteriorLights;

  
  // define variable for setpoint
  float setpoint = 0;

  
  // define variables for authority and suggested speed
  float authority = 300; 
  float suggested_speed = 50;

  
  // define variables for commanded speed and commanded power
  double commanded_speed;
  
  double commanded_power_1, commanded_power_2, commanded_power_3;
  double commanded_speed_metric;

  // define variable for final commanded power, after voting
  float final_commanded_power;

  // define variable for maximum power
  float maximum_power;

  // define variable for actual speed
  double actual_speed;
  double actual_speed_metric;

  // define variable speed limit
  float speed_limit = 40;


  // define variable manual/automatic mode, 0 is manual, 1 is automatic
  bool mode;

  // define Kp and Ki for the engineers to set
  float Kp = 0;
  float Ki = 0;

  // define Kp and Ki that the train currently is using
  float Kptrain, Kitrain;
    
  int x = 0;  

void setup() {
  // put your setup code here, to run once:



  Serial.begin(9600);

  // Setup code for ArduPID

  myController.begin(&actual_speed, &commanded_power_2, &commanded_speed, 0.0, 0.0, 0.0);
  myController.start();

  

  // Setup code for PID_RT


  PID.reset();
  PID.setOutputRange(0,maximum_power);
  PID.start();





  





  // initialize digital pin 11 as an output
  pinMode(11, OUTPUT);

  // initialize digital pin 13 as an output
  pinMode(13, OUTPUT);


  
  buttonDoors.setDebounceTime(100);
  buttonLights.setDebounceTime(100);

  setSetpoint.setDebounceTime(100);
  incrSetpoint.setDebounceTime(100);
  decrSetpoint.setDebounceTime(100);


}

int i = 0;
void loop() {
  // put your main code here, to run repeatedly:

  unsigned long currentMillis = millis();








  currStation = station[i];
  i++;
  if (i == 9) {
    i = 0;  
  }


// For the Train Driver

  // Check if button for switching between manual and automatic mode is pressed

  // Check if button for increasing setpoint is pressed
  incrSetpoint.loop();

  if (incrSetpoint.isPressed()) {
    Serial.println("The increasing setpoint button is pressed.");
    setpoint += 5;
    
    Serial.print("The new setpoint is: ");
    Serial.println(setpoint);
  }

  // Check if button for decreasing setpoint is pressed
  decrSetpoint.loop();
  
  if (decrSetpoint.isPressed())  {
    Serial.println("The decreasing setpoint button is pressed.");
  
    if (setpoint >= 5)
      setpoint -= 5;
    else
      Serial.println("Decreasing the setpoint by 5 mph would result in a negative setpoint, which is not allowed.");

    Serial.print("The new setpoint is: ");
    Serial.println(setpoint);

  }

  // Check if button for setting setpoint is pressed
  setSetpoint.loop();

  if (setSetpoint.isPressed()) {
      Serial.println("The setting setpoint button is pressed.");
    
    
      if (setpoint <= speed_limit) {
          // communicate that the new setpoint is acceptable and under the speed limit
          Serial.print("The new setpoint is an acceptable speed, and the commanded speed will be updated to ");
          Serial.print(setpoint);
          Serial.println(" mph.");
         
          digitalWrite(13, HIGH);
          
          commanded_speed = setpoint; 
      }
      else {
          Serial.println("The new setpoint is not an acceptable speed, it's above the speed limit! The commanded speed will be updated to the speed limit, which is ");
          Serial.print(speed_limit);
          Serial.println(" mph.");
          digitalWrite(11, HIGH);
          commanded_speed = speed_limit;
        
      }
    
  }
    
      
    

  // Check if doors open/close button is pressed

  buttonDoors.loop();

  if (buttonDoors.isPressed()) {
    Serial.println("The doors open/close button is pressed.");
    doors = !doors;
    if (doors == 0)
    // communicate that the doors are closing
      Serial.println("Doors are closing.");  
    if (doors == 1)
      Serial.println("Doors are opening.");

     
  }

  // Check if Lights on/off button is pressed

  buttonLights.loop();

  if(buttonLights.isPressed()) {
    
    interiorLights = !interiorLights;
    exteriorLights = !exteriorLights;  

    Serial.println("The lights button is pressed");

   
    
  }
    



  // For the Engineer

  

  // Check if set Kp is pressed on joystick

  joystick.loop();
  
  if (joystick.isPressed()) {
    Serial.println("The Kp is set to ");
    Serial.println(Kp);
    Kptrain = Kp;

    // ArduPID
    myController.begin(&actual_speed_metric, &commanded_speed_metric, &commanded_power_2, Kp, Ki, 0.0);







    // PID_RT
    PID.setKp(Kp);
  
  }





  // Check if set Ki pressed on joystick
  
  if (joystick.isPressed()) {
    Serial.println("The Ki is set to ");
    Serial.println(Ki);
    
    Kitrain = Ki;

    // ArduPID
    myController.begin(&actual_speed_metric, &commanded_speed_metric, &commanded_power_2, Kp, Ki, 0.0);






    // PID_RT
    PID.setKi(Ki);
  
  }





  

  x++;
  if (x == 1000) {
  x = 0;




  


  

  // if the interval of time has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
          
    // Check if increase Kp is selected on joystick
    int xValue = analogRead(A0);
    int yValue = analogRead(A1);

    if (xValue > 923) {
      Serial.println("Increase Kp");
      Kp += 0.5;
    }


    // Check if decrease Kp is selected on joystick
  
    if (xValue < 100) {
      Serial.println("Decrease Kp");
      Kp -= 0.5;
    }
          
        
  
  
    // Check if increase Ki selected on joystick
  
    if (yValue > 923) {
      Serial.println("Increasing Ki");
      Ki += 0.5;
    }


    // Check if decrease Ki selected on joystick
  
    if (yValue < 100) {
      Serial.println("Decreasing Ki");
      Ki -= 0.5;
    }



      
    // send and print out the states of everything, make sure everything is communicated

    Serial.println();
    Serial.println("---------------------------------------------------------------------------");
    Serial.println();

    Serial.print("Suggested Speed ");
    Serial.print(suggested_speed);
    Serial.println(" mph");

    // print state of doors
    Serial.print("The doors are ");
    if (doors) {
      Serial.println("open");
    }
    else
    {
      Serial.println("closed");  
    }
  
  
    // print state of lights
    Serial.print("The interior lights are ");

    if (interiorLights) {
      Serial.println("on");  
    
    }
    else {
      Serial.println("off");  
    }

    Serial.print("The exterior lights are ");
    if (exteriorLights) {
      Serial.println("on");
      
    }
    else {
      Serial.println("off");  
    }


    // turn lights off

    digitalWrite(13, LOW);
    digitalWrite(11, LOW);
    
    // print suggested speed
    Serial.print("The suggested speed is ");
    Serial.print(suggested_speed);
    Serial.println(" mph");
    
  
  
    // print authority
    Serial.print("The authority is ");
    Serial.print(authority);
    Serial.println(" ft");
    
  
    // print speed limit
    Serial.print("The speed limit is ");
    Serial.print(speed_limit);
    Serial.println(" mph");
  
    // print current setpoint that hasn't been confirmed yet
    Serial.print("The current unconfirmed stepoint is ");
    Serial.print(setpoint);
    Serial.println(" mph");
  
  
    // do commanded speed calculation for automatic mode
    if (mode == 1) {
      commanded_speed = suggested_speed;  
    }
  
    // Calculate the commanded power

    // Redundancy and Variety
    // voting system, commanded power calculated three ways

    // Way 1: Using my own code to implement PI controller
  
    commanded_speed_metric = commanded_speed * 0.44704;
    actual_speed_metric = actual_speed * 0.44704;
  
  
    errorPrev = error;
  
    error = commanded_speed_metric - actual_speed_metric;
  
    
  
    // Derivative
    derivative = error * Kp;
  
    // Integral
    ukprev = uk; 
    
    if (commanded_power_1 < maximum_power)
      uk = (ukprev + (period/2) * (error+errorPrev));
    else
      uk = ukprev;
    
    integral = uk * Ki;
  
    // calculate the updated commanded power
    commanded_power_1 = derivative + integral;


    // Way 2: Using ArduPID



    myController.compute();




    


    // Way 3: PID_RT PID Controller



    PID.setPoint(commanded_speed_metric);

    PID.compute(actual_speed_metric);
    

    commanded_power_3 = PID.getOutput();

    // Voting System

    // if they all disagree, use my code

    if (commanded_power_1 != commanded_power_2 && commanded_power_2 != commanded_power_3 && commanded_power_3 != commanded_power_1)
      final_commanded_power = commanded_power_1;

    // if 1 and 2 agree, but 3 is different, 1 and 2 are the majority, so use 1 and 2's commanded power
    if (commanded_power_1 == commanded_power_2 && commanded_power_1 != commanded_power_3)
      final_commanded_power = commanded_power_1;

    // if 2 and 3 agree, but 1 is different, use 2 and 3
    if (commanded_power_2 == commanded_power_3 && commanded_power_1 != commanded_power_2)
      final_commanded_power = commanded_power_2;

    // if 1 and 3 agree, but 2 is different, use 1 and 3
    if (commanded_power_1 == commanded_power_3 && commanded_power_2 != commanded_power_1)
      final_commanded_power = commanded_power_1;

   // if all three agree, use the agreed commanded power
    if (commanded_power_1 == commanded_power_2 && commanded_power_1 == commanded_power_3)
      final_commanded_power = commanded_power_1;
    
    
    // print commanded power

    Serial.println();
    Serial.println("----------------------------------------------");
    
    Serial.print("The Commanded Power (own code) is ");
    Serial.println(commanded_power_1);

    Serial.print("The Commanded Power (ArduPID) is ");
    Serial.println(commanded_power_2);
    
    Serial.print("The Commanded Power (PID_RT) is ");
    Serial.println(commanded_power_3);

    Serial.print("The Commanded Power (final) is ");
    Serial.print(final_commanded_power);
    Serial.println(" kW.");
  
    // print commanded speed
    Serial.print("The Commanded Speed is ");
    Serial.print(commanded_speed);
    Serial.println(" mph.");
  
  
    // print actual speed
    Serial.print("The Actual Speed is ");
    Serial.print(actual_speed);
    Serial.println(" mph.");
  
    // print Kp
    Serial.print("The Kp is ");
    Serial.print(Kptrain);
    Serial.println(".");
  
    // print Ki
    Serial.print("The Ki is ");
    Serial.print(Kitrain);
    Serial.println(".");
    Serial.println();
  
    // print current station
    Serial.print("The current station is ");
    // announce the current station
    Serial.print(currStation);
    Serial.println(".");

  }




  }


}
