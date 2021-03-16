  #include "Wire.h"
#include <MPU6050_light.h>
#include <Servo.h>

#define DEBUG true //display ESP8266 messages on Serial Monitor
String wifiMsg; //Entire Wifi Message
String wifiCommand; //Wifi Command Type
String wifiDataStr; //Wifi Data(String)
int userHeight = 0;

MPU6050 mpu(Wire);

//ESC Variables
int pos = 0; //Sets position variable
Servo ESC;
Servo ESC2;

//Servo Variables
Servo myservo;
Servo myservo2;


//Sensor Variables
const int trigPin = 33;
const int echoPin = 32;
float ultrasonicDuration, ultrasonicDistance;
int sensorCheckFlag = 0;
int actualHeight = 0;
float heightOffset = 9;

// PID variables

//Yaw Gyro Values
long long Desired_Yaw = 0;
long long Actual_Yaw = 0;
long long Error_Yaw = 0;

long long Derivative_Yaw = 0;
long long Integral_Yaw = 0;
long long prev_Yaw = 0;
long long Actuator_Yaw = 0;

int redAdjust = 0;
int blackAdjust = 0;

float sensor_Yaw;
float newSensor_Yaw;
//---------------------------------------------------------------------

//Red Motor Values
long long Desired_Red = 0;
long long Actual_Red = 0;
long long Error_Red = 0;

long long Derivative_Red = 0;
long long Integral_Red = 0;
long long prev_Height_Red = 0;
long long Actuator_Red = 0;

float sensor_Red;
float newSensor_Red;

//---------------------------------------------------------------------

//Black Motor Values
long long Desired_Black = 0;
long long Actual_Black = 0;
long long Error_Black = 0;

long long Derivative_Black = 0;
long long Integral_Black = 0;
long long prev_Height_Black = 0;
long long Actuator_Black = 0;

float sensor_Black;
float newSensor_Black;

//---------------------------------------------------------------------

long long duration, distance;

float Goal_Yaw = 0;
float Goal_Height = 0; 
float max_Height = 50;
float min_Height = 0;
int descentCount = 0;

long long Goal_Angle = 0;
float max_Power = 15;
float min_Power = 10;

float p = 3.44; // 0 - 160
float i = 0.048;
float d = 1.92;

float p_yaw = 0.06;
float i_yaw = 0.002;
float d_yaw = 0.02;


void armRed() 
{
  setSpeedBlack(0);
}
void armBlack() 
{
  setSpeedRed(0);
}

void setSpeedBlack(int speed) 
{
  int angle = map(speed, 0, 150, 0, 180); //Sets servo positions to different speeds 
  ESC.write(angle);
}

void setSpeedRed(int speed) 
{
  int angle = map(speed, 0, 150, 0, 180); //Sets servo positions to different speeds 
  ESC2.write(angle);
}

void setup() 
{
  //Serial.begin(115200);
  Serial1.begin(115200); //WIFI Module Serial 
  Serial.begin(115200);  //MEGA Serial
  sendData("AT+RST\r\n", 2000, DEBUG); //Reset module
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG); //Set station mode
  sendData("AT+CWJAP=\"Virus\",\"naughtyvirus911\"\r\n", 2000, DEBUG);   //Set WIFI/Password in user's 2.4GHz Network
  while(!Serial1.find("OK")) { //Wait for connection
  } 
  sendData("AT+CIFSR\r\n", 1000, DEBUG); //Show IP address
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); //Allow multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // Start web server on port 80
  
  ESC.attach(3, 8000, 16000); //Adds ESC to certain pin.
  armRed();
  ESC2.attach(2, 8000, 16000);
  armBlack(); 
  delay(1000);
    
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Wire.begin();
  myservo.attach(5);
  myservo2.attach(4);
  myservo.write(124);
  myservo2.write(167);
  
  byte status = mpu.begin();
  //Serial.print(F("MPU6050 status: "));
  //Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050

  //Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  //Serial.println("Gyroscope set up\n");
  actualHeight = round(checkUltrasonicSensor(trigPin, echoPin) - heightOffset);

}

float checkUltrasonicSensor(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  ultrasonicDuration = pulseIn(echoPin, HIGH);
  ultrasonicDistance = (ultrasonicDuration*.0343)/2;
  return ultrasonicDistance;
}

float PIDControl_yawStability(long long gyroRead)
{
    //("Absolute max: "+(p*Desired+i*max_Height)
    Error_Yaw = (Goal_Yaw) - (gyroRead);
    Derivative_Yaw = (gyroRead) - (prev_Yaw);
    Integral_Yaw += Error_Yaw;
    //Serial.print("Error_Yaw: ");
    //Serial.println(round(Error_Yaw)); //prints error
  
    //((p*Error)+" "+(i*Integral)+" "+(d*Derivative)
  
    Actuator_Yaw = p_yaw*Error_Yaw + i_yaw*Integral_Yaw - d_yaw*Derivative_Yaw;
    //Serial.print("Actuator: ");
    //Serial.println(round(Actuator_Yaw));
    return Actuator_Yaw;
}

float PIDControl_xStability_Red(long long currentSensor)
{
    //("Absolute max: "+(p*Desired+i*max_Height)
    Error_Red = (Goal_Angle) - (currentSensor);
    Derivative_Red = (currentSensor) - (prev_Height_Red); 
    Integral_Red += Error_Red;
    ////Serial.print("Error: ");
    ////Serial.println(Error); //prints error
    if(Integral_Red > max_Height)
    {
        Integral_Red = max_Height;
    }
    if(Integral_Red < min_Height)
    {
        Integral_Red = min_Height;
    }
  
    //((p*Error)+" "+(i*Integral)+" "+(d*Derivative)
  
    Actuator_Red = p*Error_Red + i*Integral_Red - d*Derivative_Red;

    if(Actuator_Red < min_Power)
    {
        Actuator_Red = min_Power;
    }
    if(Actuator_Red > max_Power)
    {
        Actuator_Red = max_Power;
    }

    return Actuator_Red;
}

float PIDControl_xStability_Black(long long currentSensor)
{
    //("Absolute max: "+(p*Desired+i*max_Height)
    Error_Black = (Goal_Angle) - (currentSensor);
    Derivative_Black = (currentSensor) - (prev_Height_Black); 
    Integral_Black += Error_Black;
    ////Serial.print("Error: ");
    ////Serial.println(Error); //prints error
    if(Integral_Black > max_Height)
    {
        Integral_Black = max_Height;
    }
    if(Integral_Black < min_Height)
    {
        Integral_Black = min_Height;
    }
  
    //((p*Error)+" "+(i*Integral)+" "+(d*Derivative)
  
    Actuator_Black = p*Error_Black + i*Integral_Black - d*Derivative_Black;

    if(Actuator_Black < min_Power)
    {
        Actuator_Black = min_Power;
    }
    if(Actuator_Black > max_Power)
    {
        Actuator_Black = max_Power;
    }

    return Actuator_Black;
}

void loop() 
{
  //Serial.println("Begin Test");
  int speed_Red; //Implements speed variable
  int speed_Black;
  //testing variables
  int time;
  int maxTime = 1000;
  int increment = 5;
  
  for (time = 0; time <= 10000; time += increment)
  {
    mpu.update(); 
  }
  //Serial.println("Calibration Complete");
  delay(1000);
  
  for (time = 0; time <= 100000; time += increment) 
  {
    //WIFI Check
    checkForWifiData();

    //Power Command
    if(wifiMsg=="pow=OFF"){
      //Power-OFF slow decent
      while(min_Power!=10){
        if(descentCount == 100){
          mpu.update();
          min_Power--;
          max_Power--;
          long long readAngle_X = mpu.getAngleX();
          //Red Motor Check
          //newSensor_Red = SensorRed;
          newSensor_Red = -readAngle_X;
          prev_Height_Red = newSensor_Red;
          //read new sensor input
        
          speed_Red = PIDControl_xStability(newSensor_Red);
          setSpeedBlack(speed_Red);
        
          //Black Motor Check
          //newSensor_Black = SensorBlack;
          newSensor_Black = readAngle_X;
          prev_Height_Black = newSensor_Black;
          //read new sensor input
        
          speed_Black = PIDControl_xStability(newSensor_Black);
          setSpeedRed(speed_Black);
          descentCount = 0;
        }
        descentCount++;
        mpu.update();
        long long readAngle_X = mpu.getAngleX();
        //Red Motor Check
        //newSensor_Red = SensorRed;
        newSensor_Red = -readAngle_X;
        prev_Height_Red = newSensor_Red;
        //read new sensor input
      
        speed_Red = PIDControl_xStability(newSensor_Red);
        setSpeedBlack(speed_Red);
      
        //Black Motor Check
        //newSensor_Black = SensorBlack;
        newSensor_Black = readAngle_X;
        prev_Height_Black = newSensor_Black;
        //read new sensor input
      
        speed_Black = PIDControl_xStability(newSensor_Black);
        setSpeedRed(speed_Black);
      }
      setSpeedRed(0);
      setSpeedBlack(0);
      for (int time2 = 0; time2 <= 100000; time2 += increment){
        checkForWifiData();
        if(wifiMsg == "pow=ON"){
          min_Power = 10;
          max_Power = 15;
          break;
        }
      }
    }

    //Height Commands
    if(wifiCommand == "hei"){
      Goal_Height = wifiDataStr.toInt();
    }
    
    //WASD Commands
//    if(wifiCommand == "pos"){
//      
//    }
    
    //read height and update both sensor values
    mpu.update(); 
      long long readAngle_Z = mpu.getAngleZ()/2;
      //yaw checks
      //newSensor_Yaw = readAngle_Z;
      //prev_Yaw = newSensor_Yaw;
      
      if (round(readAngle_Z) == 0){
        redAdjust = 0;
        blackAdjust = 0;
      }
      else if (readAngle_Z > 0){
        redAdjust = -1;
        blackAdjust = 1;
      }
      else if (readAngle_Z < 0){
        redAdjust = 1;
        blackAdjust = -1;
      }
          
    long long readAngle_Y = mpu.getAngleY();
    //Serial.println(round(readAngle_Y));

    //Servo-Motor Correction depending on Height
    if (actualHeight >= 0) 
      {
      if (actualHeight < 7) 
      {
        if(abs(readAngle_Y) > 10)
        {
          myservo2.write(round(readAngle_Y * 1.2 + 167));
          myservo.write(round(124 - readAngle_Y * 1.2));
        }
      }
      else if (actualHeight >= 7)
      {
        myservo2.write(round(readAngle_Y * 1 + 167));
        myservo.write(round(124 - readAngle_Y * 1));
      }
    }
    
    //Global PID height increment/decrement
    if (sensorCheckFlag == 20) {
      actualHeight = round(checkUltrasonicSensor(trigPin, echoPin) - heightOffset);
      if ((actualHeight < Goal_Height) && !((actualHeight <0 ) || (actualHeight >= 100))){
        if(min_Power<80){
          min_Power++;
          max_Power++;
        }
      }
      else if ((actualHeight > Goal_Height) && !((actualHeight <0 ) || (actualHeight >= 100))){
        if(min_Power>10){
          min_Power--;
          max_Power--;
        }
      }
      

      sensorCheckFlag = 0;
    }

    
    //*/
    if(Goal_Height == 0){
      setSpeedRed(0);
      setSpeedBlack(0);
    }
    else if(Goal_Height > 0 && wifiCommand == "hei"){
      

      long long readAngle_X = mpu.getAngleX();
      //Red Motor Check
      //newSensor_Red = SensorRed;
      newSensor_Red = -readAngle_X;
      prev_Height_Red = newSensor_Red;
      //read new sensor input
    
      speed_Red = PIDControl_xStability_Red(newSensor_Red);// + redAdjust;
      setSpeedRed(speed_Black);
    
      //Black Motor Check
      //newSensor_Black = SensorBlack;
      newSensor_Black = readAngle_X;
      prev_Height_Black = newSensor_Black;
      //read new sensor input
    
      speed_Black = PIDControl_xStability_Black(newSensor_Black);// + blackAdjust;
      setSpeedBlack(speed_Red);

      /*
      //Serialprints for Debugging
      Serial.print("\t---------------------------------------------\n");
      
      Serial.print("\t                                            X : ");
      Serial.print(mpu.getAngleX());
      Serial.print("\n");
      Serial.print("\t                                       Y : ");
      Serial.print(mpu.getAngleY());
      Serial.print("\n");
      Serial.print("\t                                  Z : ");
      Serial.print(mpu.getAngleZ());
      Serial.print("\n");
          
      Serial.print("                     Sensor Red: ");
      Serial.print(newSensor_Red);
      Serial.print("\n");
      Serial.print("                      Speed Red: ");
      Serial.print(speed_Red);
      Serial.print("\n");
      Serial.print("                     Adjust Red: ");
      Serial.print(redAdjust);
      Serial.print("\n");
    
      Serial.print("Sensor Black: ");
      Serial.print(newSensor_Black);
      Serial.print("\n");
      Serial.print(" Speed Black: ");
      Serial.print(speed_Black);
      Serial.print("\n");
      Serial.print("Adjust Black: ");
      Serial.print(blackAdjust);
      Serial.print("\n");
      */
      sensorCheckFlag++;
    }
    
  }

  
  setSpeedBlack(0);
  setSpeedRed(0);
  //Serial.println("End Test");
  delay(120000); //end test here

}

//Function used to listen for WIFI data
void checkForWifiData(){
    if (Serial1.available())  //Check if there is data available on ESP8266
  {
    if (Serial1.find("+IPD,")) //If there is Data
    {
      Serial1.find("?"); //Run cursor until command is found
      wifiMsg = Serial1.readStringUntil(' '); //Read the message
      wifiCommand = wifiMsg.substring(0, 3); //First three letters indicate commandd type: "hei/pos/pow" = height/position/power
      wifiDataStr = wifiMsg.substring(4);   //Next characters are Data(String)
//      if (DEBUG) {
//        Serial.println(wifiMsg);
//      }
    }
  }
}

//Function used to send data to the WIFI module (initial setup)
String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  Serial1.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial1.available())
    {
      char c = Serial1.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
