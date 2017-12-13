
#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

const int trigPin = 3;
const int echoPin = 2;
long dummy=0;
int per=0,val,p,r=0,check=0;
char str[120];
char ssid[] = "wifihotspot";            // your network SSID (name)
char pass[] = "access99";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
int i;
char server[] = "192.168.43.119";

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 1000L; // delay between updates, in milliseconds

// Initialize the Ethernet client object
//void ultrasound_trig();
WiFiEspClient client;
RingBuffer buf(8);

void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  WiFi.init(&Serial1);

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
    delay(4);
  }

  Serial.println("You're connected to the network");
  
  printWifiStatus();
}

void loop()
{
long duration, inches, cm;
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
//  duration = pulseIn(echoPin, HIGH);
cm = microsecondsToCentimeters(duration);
//Serial.print(cm);
//Serial.print("in, ");
//Serial.println();
delay(100);
if (cm!=dummy)
{
  dummy=cm;
   
   if (cm>=14 && cm<=17)
      val=1;
   else if (cm>=10 && cm<=13)
      val=30;
   else if (cm>=5 && cm<=9)
      val=70;
   else if (cm>=3 && cm<=4)
      val=99;
}

// Serial.print(val);
// Serial.print("%");
// Serial.println();

if(val==per)
{
  r++;
  delay(2);
}
else
{
  r=0;
  per=val;
}

  

if(r==20)
{
   if (val!=check)
   {
    
  check=val;
  Serial.print(val);
  Serial.print("%");
  Serial.println();
  for(p=0;p<=2;p++)
  {
 httpRequest();
 delay(50);
  }
  r=0;
   }
}
   



  
  memset(str,'\0',sizeof(str));

// while (client.available()) {
//    char c = client.read();
//    Serial.write(c);
//    buf.push(c);
//    
//  }
//}
}

// this method makes a HTTP connection to the server
void httpRequest()
{
  Serial.println();
    
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 8080)) {
    Serial.println("Connecting...");
    
    // send the HTTP PUT request
    sprintf(str,"GET /garbagecollector/status/current/update?binID=1&currentCapacity=%d HTTP/1.1",val);
    client.println(str);
    client.println(F("Host: 192.168.43.119"));
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
void ultrasound()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}


