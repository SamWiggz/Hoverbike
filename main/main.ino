//WIFI Module Variables
#define DEBUG true //display ESP8266 messages on Serial Monitor
String wifiMsg; //Entire Wifi Message
String wifiCommand; //Wifi Command Type
String wifiDataStr; //Wifi Data(String)

//Ultrasonic Sensor Variables
const int trigPin[6] = {22, 24, 26, 28, 30, 32};  
const int echoPin[6] = {23, 25, 27, 29, 31, 33};
float sensorOutput[6]; //bottom1, bottom2, left, right, front, rear
float ultrasonicDuration, ultrasonicDistance;
enum States{Init, PerformHeightAdjustment, WaitForUserInput, PerformDirectionAdjustment, PowerOff}state;

void setup() {
  
  Serial1.begin(115200); //WIFI Module Serial 
  Serial.begin(115200);  //MEGA Serial
  sendData("AT+RST\r\n", 2000, DEBUG); //Reset module
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG); //Set station mode
  sendData("AT+CWJAP=\"Betty from bio (2G)\",\"grumpytruck994\"\r\n", 2000, DEBUG);   //Set WIFI/Password in user's 2.4GHz Network
  while(!Serial1.find("OK")) { //Wait for connection
  } 
  sendData("AT+CIFSR\r\n", 1000, DEBUG); //Show IP address
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); //Allow multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // Start web server on port 80

  pinMode(trigPin[0], OUTPUT);
  pinMode(echoPin[0], INPUT);
  pinMode(trigPin[1], OUTPUT);
  pinMode(echoPin[1], INPUT);
  pinMode(trigPin[2], OUTPUT);
  pinMode(echoPin[2], INPUT);
  pinMode(trigPin[3], OUTPUT);
  pinMode(echoPin[3], INPUT);
  pinMode(trigPin[4], OUTPUT);
  pinMode(echoPin[4], INPUT);
  pinMode(trigPin[5], OUTPUT);
  pinMode(echoPin[5], INPUT);
}

void loop()
{
    //Check Wifi Data
//  checkForWifiData();
//
  //Check all 6 Ultrasonic Sensors
//  for(int i = 0; i < 6; i++){
//    ultrasonicSensor(trigPin[i], echoPin[i], i);
//    sensorOutput[i] = distance; 
//  }
//  for(int i = 0; i < 6; i++){
//      Serial.print("Sensor ");
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.println(sensorOutput[i]);
//  }
  switch(state) {
     case Init  :
        checkForWifiData();
        if(wifiCommand == "hei"){
          state = PerformHeightAdjustment;
        }
        state = Init;
        break;
     case PerformHeightAdjustment  :
        if(wifiCommand == "hei"){
          state = WaitForUserInput;
        }
        else if(wifiCommand == "pow"){
          state = PowerOff;
        }             
        break;
     case WaitForUserInput :
        checkForWifiData();
        if(wifiCommand == "hei"){
          state = PerformHeightAdjustment;
        }
        else if(wifiCommand == "pos"){
          state = PerformDirectionAdjustment;          
        } 
        else if(wifiCommand == "pow"){
          state = PerformHeightAdjustment;             
        }
        break;
     case PerformDirectionAdjustment  :
        state = WaitForUserInput;
        break;
     case PowerOff :
        checkForWifiData();
        if(wifiCommand == "pow" && wifiDataStr == "ON"){
          state = Init;
        }
        break;
     default :
        state = Init;
        break;
  }
}

//Function used to check Ultrasonic Sensor height
void checkUltrasonicSensor(int trigPin, int echoPin, int sensorNumber){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  ultrasonicDuration = pulseIn(echoPin, HIGH);
  ultrasonicDistance = (ultrasonicDuration*.0343)/2;
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
      if (DEBUG) {
        Serial.println(wifiMsg);
      }
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
