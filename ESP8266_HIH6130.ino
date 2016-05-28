#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>

// HIH6130 I2C address is 0x27(39)
#define Addr 0x27

const char* ssid     = "NETGEAR34";
const char* password = "sillyviolet195";

ESP8266WebServer server ( 80 );

////////////////////////////////////////////////////////////////
void handleRoot() {
  char temp[400];
  unsigned int data[4];
   Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x00);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Request 4 bytes of data
  Wire.requestFrom(Addr, 4);

  // Read 4 bytes of data
  // humidity msb, humidity lsb, temp msb, temp lsb
  if (Wire.available() == 4)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
  }

  // Convert the data to 14-bits
  int humidity = ((((data[0] & 0x3F) * 256) + data[1]) * 100.0) / 16383.0;
  int temp1 = ((data[2] * 256) + (data[3] & 0xFC)) / 4;
  int cTemp = (temp1 / 16384.0) * 165.0 - 40.0;
  int fTemp = cTemp * 1.8 + 32;

  // Output data to serial monitor
  Serial.print("Relative Humidity :");
  Serial.print(humidity);
  Serial.println(" %RH");
  Serial.print("Temperature in Celsius :");
  Serial.print(cTemp);
  Serial.println(" C");
  Serial.print("Temperature in Fahrenheit :");
  Serial.print(fTemp);
  Serial.println(" F");
  delay(500);


 snprintf ( temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Weather Monitoring using ESP8266</h1>\
    <h1>HIH6130 I2C sensor mini Module</h1>\
    <p>fTemp_cTemp_Humidity: %02d:%02d:%02d</p>\
    </body>\
</html>",

    fTemp, cTemp, humidity
  );
  
  server.send ( 200, "text/html", temp );
  }
/////////////////////////////////////////////////////////////////
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void setup ( void ) {
  Wire.begin(2,14);
  Serial.begin ( 115200 );
  WiFi.begin ( ssid, password );
  Serial.println ( "" );

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  server.on ( "/", handleRoot );

 
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop ( void ) {
  server.handleClient();
}
