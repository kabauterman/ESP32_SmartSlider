#include <WiFiClient.h>
#include <ESP32WebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>

//pin config

const int en = 27;
const int cfg1 = 26;
const int stp = 25;
const int dir = 33;

const int chdir0 = 22;
const int chdir1 = 23;

const int shutter = 32;

//declase variables

int ms;
String auto_turn;
String start;
int cc;
int i;
int rt;
int rt_s;

/* change your ssid and password here */
const char* ssid = "WLAN-Slider";
const char* password = "12345678";

ESP32WebServer server(80);

//html code for setup page

const char* setuphtml = "<html><head><title>WLAN SLIDER</title><style>div.content {border: #B6B6B4 solid 1px;}"
                        "ol#toc {height: 8em;margin: 0;padding: 0;}ol#toc a {font-weight:bold;font-size:30px;text-decoration:none;"
                        "color: #000;float: left;line-height: 400%;width: 8em;text-align:center;padding-right: 10px;}"
                        "ol#toc li.current {background-color: #B6B6B4;}ol#toc li.current a {color: #fff;}ol#toc li {"
                        "background: #E5E4E2;float: left;padding-left: 10px;}body{background:#FFFFFF;}"
                        ".bt{width:90%;padding:20%;text-align:center;color:BLACK;border-radius:50%;font-weight:bold;font-size:35px;}"
                        ".r{background:#E5E4E2;} .g{background:#B6B6B4;} .b{background:#E5E4E2;}"
                        "</style></head><body>"
                        "<ol id='toc'><li><a href='init'>INITIAL</a></li><li class='current'><a href='setup'>SETUP</a></li>"
                        "<li><a href='presets'>PRESETS</a></li></ol><div class='content'><table width='100%'><colgroup width='50%' span='2'</colgroup>"
                        "<tr><td><form action='dir' method='get'><input class='bt b' type='submit' name='direct' value='FORWARD'></td>"
                        "<td><input class='bt b' type='submit' name='direct' value='BACKWARD'></form></td></tr>"
                        "<tr><td><form action='turn' method='get'><input class='bt b' type='submit' name='turn' value='AUTOTURN'></td>"
                        "<td><input class='bt b' type='submit' name='turn' value='AUTOSTOP'></form></td></tr>"
                        "<tr><td><form action='rt' method='get'><input class='bt b' type='number' name='rt' value='1' min='1'></td>"
                        "<td><input class='bt b' type='submit' value='RUNTIME'></form></td></tr>"
                        "<tr><td><form action='start' method='get'><input class='bt g' type='submit' name='start' value='START'></td>"
                        "<td><input class='bt r' type='submit' name='start' value='STOP'></form></td></tr>"
                        "</table></div></body></html>";

//html code for init page

const char* inithtml =  "<html><head><title>WLAN SLIDER</title><style>div.content {border: #B6B6B4 solid 1px;}"
                        "ol#toc {height: 8em;margin: 0;padding: 0;}ol#toc a {font-weight:bold;font-size:30px;text-decoration:none;"
                        "color: #000;float: left;line-height: 400%;width: 8em;text-align:center;padding-right: 10px;}"
                        "ol#toc li.current {background-color: #B6B6B4;}ol#toc li.current a {color: #fff;}ol#toc li {"
                        "background: #E5E4E2;float: left;padding-left: 10px;}body{background:#FFFFFF;}"
                        ".bt{width:90%;padding:20%;text-align:center;color:BLACK;border-radius:50%;font-weight:bold;font-size:35px;}"
                        ".r{background:#E5E4E2;} .g{background:#B6B6B4;} .b{background:#E5E4E2;}"
                        "</style></head><body>"
                        "<ol id='toc'><li class='current'><a href='init'>INITIAL</a></li><li><a href='setup'>SETUP</a></li>"
                        "<li><a href='presets'>PRESETS</a></li></ol><div class='content'>"
                        "<table width='100%'><colgroup width='50%' span='2'</colgroup>"
                        "<tr><td><form action='en' method='get'><input class='bt g' type='submit' name='enable' value='ENABLE'></td>"
                        "<td><input class='bt r' type='submit' name='enable' value='DISABLE'></form></td></tr>"
                        "<tr><td><form action='goto' method='get'><input class='bt b' type='submit' name='goto' value='CHANGE SIDE'></form></td>"
                        "<td><form action='calib' method='get'><input class='bt b' type='submit' name='calib' value='CALIBRATE'></form></td></tr>"
                        "</table></div></body></html>";

//html code for preset page

const char* presethtml =  "<html><head><title>WLAN SLIDER</title><style>div.content {border: #B6B6B4 solid 1px;}"
                          "ol#toc {height: 8em;margin: 0;padding: 0;}ol#toc a {font-weight:bold;font-size:30px;text-decoration:none;"
                          "color: #000;float: left;line-height: 400%;width: 8em;text-align:center;padding-right: 10px;}"
                          "ol#toc li.current {background-color: #B6B6B4;}ol#toc li.current a {color: #fff;}ol#toc li {"
                          "background: #E5E4E2;float: left;padding-left: 10px;}body{background:#FFFFFF;}"
                          ".bt{width:90%;padding:20%;text-align:center;color:BLACK;border-radius:50%;font-weight:bold;font-size:35px;}"
                          ".r{background:#E5E4E2;} .g{background:#B6B6B4;} .b{background:#E5E4E2;}"
                          "</style></head><body>"
                          "<ol id='toc'><li><a href='init'>INITIAL</a></li><li><a href='setup'>SETUP</a></li>"
                          "<li class='current'><a href='presets'>PRESETS</a></li></ol><div class='content'><table width='100%'><colgroup width='50%' span='2'</colgroup>"
                          "<tr><td><form action='pre' method='get'><input class='bt b' type='submit' name='preset' value='100%'></td><td><input class='bt b' type='submit' name='preset' value='50%'></td></tr>"
                          "<tr><td><input class='bt b' type='submit' name='preset' value='33%'></td><td><input class='bt b' type='submit' name='preset' value='25%'></td></tr>"
                          "<tr><td><input class='bt b' type='submit' name='preset' value='10%'></td><td><input class='bt b' type='submit' name='preset' value='5%'></form></td></tr>"
                          "</table></div></body></html>";  

void handleRoot() {
  server.send(200, "text/html", inithtml);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  server.send(404, "text/plain", message);
}

void setup(void){
  
  Serial.begin(115200);
  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

//define pins
  pinMode(dir, OUTPUT);
  pinMode(stp, OUTPUT);
  pinMode(en, OUTPUT);
  pinMode(cfg1, OUTPUT);
  pinMode(chdir0, INPUT_PULLUP);
  pinMode(chdir1, INPUT_PULLUP);
  pinMode(shutter, OUTPUT);
//Set CFG1 to HIGH for SILENT Mode 1/4 Step
  digitalWrite(cfg1, HIGH);
//Set EN to HIGH to Disable the Slider at startup
  digitalWrite(en, HIGH);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  server.on("/", handle_init);
  server.on("/init",handle_init);
  server.on("/setup", handle_setup);
  server.on("/presets", handle_preset);
  server.on("/en", do_en);
  server.on("/dir", do_dir);
  server.on("/turn", do_turn);
  server.on("/rt", do_rt);
  server.on("/start", do_start);
  server.on("/calib",do_calib);
  server.on("/goto",do_goto);
  server.on("/pre",do_pre);
  server.begin();
  
  Serial.println("Succesfully stared");
  Serial.print("DIR is: ");
  Serial.println(dir);
  Serial.print("STP is: ");
  Serial.println(stp);
  Serial.print("EN is: ");
  Serial.println(en);
  Serial.print("CFG1 is: ");
  Serial.println(cfg1);
  Serial.print("chdir0 is: ");
  Serial.println(chdir0);
  Serial.print("chdir1 is: ");
  Serial.println(chdir1);


}

void handle_setup(){

  server.send(200, "text/html", setuphtml);
  delay(100);
}

void handle_init(){

  server.send(200, "text/html", inithtml);
  delay(100);
} 

void handle_preset(){

  server.send(200, "text/html", presethtml);
  delay(100);
}

void do_pre(){

String preset=server.arg("preset");

  if (preset == "100%25") do_preset(1);

  if (preset == "50%25")  do_preset(2);

  if (preset == "33%25")  do_preset(3);

  if (preset == "25%25")  do_preset(4);

  if (preset == "10%25")  do_preset(10);

  if (preset == "5%25")   do_preset(20);

  handle_preset();
}

void do_en(){

//get enable from server and set enable pin to LOW (nessecary step to start any other thing)

  String enable=server.arg("enable");

  if (enable == "ENABLE"){
    Serial.println("Enabled");
    digitalWrite(en, LOW);
  }
  
  else if (enable == "DISABLE"){
    Serial.println("Disabled");
    digitalWrite(en, HIGH);
  }
  handle_init();
}

void do_calib(){

//start Calibration run (2 ways to be sure it started at the beginning)
  
  while (i < 2){
    do_calibrate();
    i++;
    Serial.print("Calibration done Part");
    Serial.println(i);
    Serial.print("Calibration Counter: ");
    Serial.println(cc);
  }
  i=0;
    
}

void do_dir(){

//get direction from server, change dir pin

    String direct=server.arg("direct");

    if (direct == "FORWARD"){
    Serial.println("Set direction to Forward");
    digitalWrite(dir, HIGH);
  }  

    if (direct == "BACKWARD"){
    Serial.println("Set direction to backward");
    digitalWrite(dir, LOW);
  }
  handle_setup();  
}

String do_turn(){

//get turn from server return it to loop

    server.send(200, "text/html", setuphtml);
    auto_turn=server.arg("turn");
    Serial.print("AutoTurn has been set to: ");
    Serial.println(auto_turn);
    return auto_turn;
}

String do_start(){

//get start from server return it to loop

    server.send(200, "text/html", setuphtml);
    start=server.arg("start");
    Serial.print("Start has been set to: ");
    Serial.println(start);
    return start;
}

int do_rt(){

//get runtime from server, return it to loop

    server.send(200, "text/html", setuphtml);
    rt=server.arg("rt").toInt();
    Serial.print("Runtime has been set to: ");
    Serial.println(rt);
    return rt;
}

void do_step(int ms){

//do one Step

    digitalWrite(stp, HIGH);
    delay(ms);
    digitalWrite(stp, LOW);
    delay(1); //additional delay for big slider  
}

void do_autoturn(){

//check buttons for AutoTurn

    if (digitalRead(chdir0) == 0) digitalWrite(dir, HIGH);
    if (digitalRead(chdir1) == 0) digitalWrite(dir, LOW); 
  
}

void do_autostop(){
  
//check buttons for AutoStop

    if (digitalRead(chdir0) == 0){
      digitalWrite(dir, HIGH);
      start = "Stop";
    }
    if (digitalRead(chdir1) == 0){
      digitalWrite(dir, LOW);
      start = "Stop";      
    }
  
}

int do_calibrate(){
  cc=0;

  server.send(200, "text/html", inithtml);


//Increase Calibration Counter until DIR Changes (button pushed)
    while (digitalRead(dir) == 0){
          if (digitalRead(chdir0) == 0){
          digitalWrite(dir, HIGH);
          return cc;
          }   
      do_step(ms=1);
      cc++;      
          
    }
  cc=0;

    while (digitalRead(dir) == 1){
          if (digitalRead(chdir1) == 0){
          digitalWrite(dir, LOW); 
          return cc;
          }
      do_step(ms=1);
      cc++;      
          
    }
 
  cc=0;
}

void do_goto() {

// change side in full speed

  server.send(200, "text/html", inithtml);

  if (digitalRead(dir) == 0) do_goto1(3);

  else if (digitalRead(dir) == 1) do_goto2(3);
}

void do_goto1(int ms) {

//if direction is backward then go backward until direction is forward
  
      while (digitalRead(dir) == 0){
        do_step(ms);
        if (digitalRead(chdir0) == 0){
          digitalWrite(dir, HIGH);
          }
      }
}

void do_goto2(int ms) {

//if direction is forward then go forward until direction is backward
  
      while (digitalRead(dir) == 1){
        do_step(ms);    
        if (digitalRead(chdir1) == 0){
          digitalWrite(dir, LOW);
          } 
      }
}

void do_preset(int ms){

//same as do_goto but with ms given by do_pre

  if (digitalRead(dir) == 0) do_goto1(ms);

  else if (digitalRead(dir) == 1) do_goto2(ms);
  
}

void loop() {
  server.handleClient();

//Failover if no turn option was choosen

  if (auto_turn == "") do_autoturn();

  if (auto_turn == "AUTOTURN") do_autoturn();

  if (auto_turn == "AUTOSTOP") do_autostop();

//calculate delay(ms) from runtime and calibration couter

  if (rt != 0){
      rt_s = rt * 60 * 1000;
      ms = rt_s / cc;
  }
  
  if (start == "START") do_step(ms);

}
