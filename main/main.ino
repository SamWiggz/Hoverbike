#define DEBUG true //display ESP8266 messages on Serial Monitor

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

}

void loop()
{
  waitForData();
}

//Function used to listen for WIFI data
void waitForData(){
    if (Serial1.available())  //Check if there is data available on ESP8266
  {
    if (Serial1.find("+IPD,")) //If there is Data
    {
      String msg;
      Serial1.find("?"); //Run cursor until command is found
      msg = Serial1.readStringUntil(' '); //Read the message
      String command = msg.substring(0, 3); //First three letters indicate commandd type: "hei/pos/pow" = height/position/power
      String valueStr = msg.substring(4);   //Next characters are Data
      if (DEBUG) {
        Serial.println(msg);
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
