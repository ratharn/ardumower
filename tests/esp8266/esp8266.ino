/*
ESP8266 ESP-01 Wifi module - test (Arduino Mega)
http://playground.boxtec.ch/doku.php/wireless/esp8266

wiring (IC side):
                                ESP8266-01
Arduino Mega TX1 ------------RX---1 2---VCC (3.3V)
                          GPIO0---3 4---nRST
               |Antenna|  GPIO2---5 6---nCHPD ---------- connect to VCC (3.3V)
                            GND---7 8---TX-------------- RX  Arduino Mega RX1
*/

// encryption types
enum {
  ENCRYPT_NO, ENCRYPT_WEP, ENCRYPT_WPA_PSK, ENCRYPT_WPA2_PSK, ENCRYPT_WPA_WPA2_PSK, ENCRYPT_AUTO,
};


// Wifi network settings (please adjust!)
String wifiSSID = "GRAUNET";
String wifiPass = "71979";
int wifiEncrypt = ENCRYPT_AUTO;  // do not change
int wifiChannel = 5;  // do not change (only used if used non-auto encryption)


void writeWifi(String s){
  Serial.println("SEND: "+s);
  Serial1.print(s);  
}


String readWifi(){
  Serial.print("RECV: ");
  String s;
  char data;
  while (Serial1.available()){
    data=Serial1.read();
    s += char(data);
    Serial.print(data);
  }
  Serial.println();
  return s;
}

String writeReadWifi(String s, int waitMillis = 1000){
  writeWifi(s);
  delay(waitMillis);
  String res = readWifi();        
  return res;
}

// try out baudrate and connect
boolean connectWifi(){
  Serial.println("--------connectWifi--------");
  String s;
  Serial1.setTimeout(5000);
  while (true){    
    Serial.println("trying 115200...");  
    Serial1.begin(115200);   
    s = writeReadWifi("AT\r");
    if (s.indexOf("OK") != -1) break;
    Serial.println("trying 57600...");  
    Serial1.begin(57600);     
    s = writeReadWifi("AT\r");
    if (s.indexOf("OK") != -1) break;
    Serial.println("trying 9600...");  
    Serial1.begin(9600);         
    s = writeReadWifi("AT\r");
    if (s.indexOf("OK") != -1) break;
    Serial.println("ERROR: cannot connect");    
    return false;
  }    
  Serial.println("success");      
  return true;  
}


// join Wifi network
boolean joinWifi(){
  Serial.println("--------joinWifi--------");
  writeReadWifi("AT+RST\r");  // reset module
  writeReadWifi("AT+CWMODE=3\r");  // station mode
  writeReadWifi("AT+CIPMUX=1\r");  // multiple connection mode
  boolean res = false;
  // joining network
  for (int retry = 0; retry < 4; retry++){
    String conn = "AT+CWJAP=\"" + wifiSSID + "\",\"" + wifiPass + "\"";
    if (wifiEncrypt != ENCRYPT_AUTO){
      conn += ",";
      conn += String(wifiChannel);
      conn += ",";      
      conn += String(wifiEncrypt);
    }
    String s = writeReadWifi(conn + "\r", 3500);
    res = (s.indexOf("OK") != -1);
    if (res) break;
  }
  if (res) {
    Serial.println("success");                      
  } else Serial.println("ERROR joining");      
  return res;
}


void startServer(){
  Serial.println("--------startServer--------");
  writeReadWifi("AT+CIPSERVER=1,80\r");   // start server
  writeReadWifi("AT+CIPSTO=120\r");  
  for (int retry = 0; retry < 5; retry++){
      Serial.println("waiting for getting IP (DHCP)...");
      delay(8000);
      String s = writeReadWifi("AT+CIFSR\r");  // get IP address      
      if (s.indexOf("OK") != -1) break;
  }  
}

void runServer(){
  String s;
  char data;
  while (Serial1.available()){
    data=Serial1.read();
    s += char(data);
    Serial.print(data);
  }
}


void setup(){
  Serial.begin(19200);
  Serial.println("START");
  if (connectWifi()){
    if (joinWifi()){
      startServer();
    }
  }
}


void loop(){
  runServer();  
}



