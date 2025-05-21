/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-multiple-button
 */

#include <ezButton.h>
#include <WiFi.h>
#include "time.h"
#include <PubSubClient.h>

const char* ssid = "IoT_H3/4";
const char* password = "98806829";

const char* ntpServer = "0.dk.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;


const char* mqtt_server = "192.168.0.204";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

WiFiServer server(80);
String header;
String output26State = "off";  // variable to store the LED status
String output27State = "off";  // variable to store the LED status
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

unsigned long lastMsg = 0;
bool isLedOn = false;

// Current time
unsigned long led_currentTime = millis();
// Previous time
unsigned long led_previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long led_timeoutTime = 1000;

#define BUTTON_PIN_1 25  // The ESP32 pin GPIO25 connected to the button 1
#define BUTTON_PIN_2 26  // The ESP32 pin GPIO26 connected to the button 2
#define BUTTON_PIN_3 27  // The ESP32 pin GPIO27 connected to the button 3
#define BUTTON_PIN_4 14  // The ESP32 pin GPIO14 connected to the button 4
#define LED_OUT1 23  // The ESP32 pin GPIO12 connected to the button 5
#define LED_OUT2 22  // The ESP32 pin GPIO12 connected to the button 5
#define LED_OUT3 21  // The ESP32 pin GPIO12 connected to the button 5
#define LED_OUT4 19  // The ESP32 pin GPIO12 connected to the button 5
#define ledPin 2      // The ESP32 pin GPIO2 connected to the LED
#define ON_BOARD_LED 2 


ezButton button1(BUTTON_PIN_1);  // create ezButton object for button 1
ezButton button2(BUTTON_PIN_2);  // create ezButton object for button 2
ezButton button3(BUTTON_PIN_3);  // create ezButton object for button 3
ezButton button4(BUTTON_PIN_4);  // create ezButton object for button 4

void ledOn(int pressed) {
  if (isLedOn)
  {
    return;
  }
  switch (pressed) {
    case 1:
      digitalWrite(LED_OUT1, HIGH);
      break;
    case 2:
      digitalWrite(LED_OUT2, HIGH);
      break;
    case 3:
      digitalWrite(LED_OUT3, HIGH);
      break;
    case 4:
      digitalWrite(LED_OUT4, HIGH); 
      break;
    default:
      break;
  }
  led_previousTime = millis();
  isLedOn = true;
}

void buttonPress(int button) {
  switch (button) {
    case 1:
      Serial.println("Button 1 pressed");
      client.publish("esp32/feedback", "Happy");
      break;
    case 2:
      Serial.println("Button 2 pressed");
      client.publish("esp32/feedback", "Less Happy");
      break;
    case 3:
      Serial.println("Button 3 pressed");
      client.publish("esp32/feedback", "Unhappy");
      break;
    case 4:
      Serial.println("Button 4 pressed");
      client.publish("esp32/feedback", "Malding!");
      break;
    default:
      break;
  }
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_OUT1,OUTPUT);
  pinMode(LED_OUT2,OUTPUT);
  pinMode(LED_OUT3,OUTPUT);
  pinMode(LED_OUT4,OUTPUT);
  pinMode(ledPin, OUTPUT);  // set GPIO2 as an output
  button1.setDebounceTime(100);  // set debounce time to 100 milliseconds
  button2.setDebounceTime(100);  // set debounce time to 100 milliseconds
  button3.setDebounceTime(100);  // set debounce time to 100 milliseconds
  button4.setDebounceTime(100);  // set debounce time to 100 milliseconds
  pinMode(ON_BOARD_LED, OUTPUT);  

  client.setServer(mqtt_server, 1883);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //server.begin();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  printLocalTime();
}

void loop() {
  button1.loop();  // MUST call the loop() function first
  button2.loop();  // MUST call the loop() function first
  button3.loop();  // MUST call the loop() function first
  button4.loop();  // MUST call the loop() function first


  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    Serial.println(timeStr);
    client.publish("esp32/time", timeStr);
  }

  // int button1_state = button1.getState();  // the state after debounce
  // int button2_state = button2.getState();  // the state after debounce
  // int button3_state = button3.getState();  // the state after debounce
  // int button4_state = button4.getState();  // the state after debounce

  //WiFiClient client = server.available();   // Listen for incoming clients

  // if (client) {                             // If a new client connects,
  //   currentTime = millis();
  //   previousTime = currentTime;
  //   Serial.println("New Client.");          // print a message out in the serial port
  //   String currentLine = "";                // make a String to hold incoming data from the client
  //   while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
  //     currentTime = millis();
  //     if (client.available()) {             // if there's bytes to read from the client,
  //       char c = client.read();             // read a byte, then
  //       Serial.write(c);                    // print it out the serial monitor
  //       header += c;
  //       if (c == '\n') {                    // if the byte is a newline character
  //         // if the current line is blank, you got two newline characters in a row.
  //         // that's the end of the client HTTP request, so send a response:
  //         if (currentLine.length() == 0) {
  //           // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  //           // and a content-type so the client knows what's coming, then a blank line:
  //           client.println("HTTP/1.1 200 OK");
  //           client.println("Content-type:text/html");
  //           client.println("Connection: close");
  //           client.println();
            
  //           // turns the GPIOs on and off
  //           if (header.indexOf("GET /26/on") >= 0) {
  //             Serial.println("GPIO 26 on");
  //             output26State = "on";
  //             digitalWrite(LED_OUT1, HIGH);
  //           } else if (header.indexOf("GET /26/off") >= 0) {
  //             Serial.println("GPIO 26 off");
  //             output26State = "off";
  //             digitalWrite(LED_OUT1, LOW);
  //           } else if (header.indexOf("GET /27/on") >= 0) {
  //             Serial.println("GPIO 27 on");
  //             output27State = "on";
  //             digitalWrite(LED_OUT2, HIGH);
  //           } else if (header.indexOf("GET /27/off") >= 0) {
  //             Serial.println("GPIO 27 off");
  //             output27State = "off";
  //             digitalWrite(LED_OUT2, LOW);
  //           }
            
  //           // Display the HTML web page
  //           client.println(F("<!DOCTYPE html><html>"));
  //           client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  //           client.println("<link rel=\"icon\" href=\"data:,\">");
  //           // CSS to style the on/off buttons 
  //           // Feel free to change the background-color and font-size attributes to fit your preferences
  //           client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  //           client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
  //           client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  //           client.println(".button2 {background-color: #555555;}</style></head>");
            
  //           // Web Page Heading
  //           client.println("<body><h1>ESP32 Web Server</h1>");
            
  //           // Display current state, and ON/OFF buttons for GPIO 26  
  //           client.println("<p>GPIO 26 Green LED - State " + output26State + "</p>");
  //           // If the output26State is off, it displays the ON button       
  //           if (output26State=="off") {
  //             client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
  //           } else {
  //             client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
  //           } 
               
  //           // Display current state, and ON/OFF buttons for GPIO 27  
  //           client.println("<p>GPIO 27 Yellow LED - State " + output27State + "</p>");
  //           // If the output27State is off, it displays the ON button       
  //           if (output27State=="off") {
  //             client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
  //           } else {
  //             client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
  //           }
  //           client.println("</body></html>");
            
  //           // The HTTP response ends with another blank line
  //           client.println();
  //           // Break out of the while loop
  //           break;
  //         } else { // if you got a newline, then clear currentLine
  //           currentLine = "";
  //         }
  //       } else if (c != '\r') {  // if you got anything else but a carriage return character,
  //         currentLine += c;      // add it to the end of the currentLine
  //       }
  //     }
  if (button1.isPressed()){
    buttonPress(1);
    ledOn(1);
  }
  if (button2.isPressed()){
    buttonPress(2);
    ledOn(2);
  }
  if (button3.isPressed()){
    buttonPress(3);
    ledOn(3);
  }
  if (button4.isPressed()){
    buttonPress(4);
    ledOn(4);
  }
  
  led_currentTime = millis();
  if (led_currentTime - led_previousTime >= led_timeoutTime && isLedOn) {
    digitalWrite(LED_OUT1, LOW);
    digitalWrite(LED_OUT2, LOW);
    digitalWrite(LED_OUT3, LOW);
    digitalWrite(LED_OUT4, LOW);
    isLedOn = false;
  }
}