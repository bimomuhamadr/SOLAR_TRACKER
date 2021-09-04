#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
//******************************************************
const char* ssid     = "ASUSX555Q";
const char* password = "welcomestark";
const char* host = "solartracker.promonissanbogor.com";
//const char* host = "192.168.43.201";
//******************************************************
bool Parsing = false;
String dataPHP, data[8], command;
//******************************************************
float shuntvoltage;
float busvoltage;
float current_mA;
float loadvoltage;
float power_mW;
float lux;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("connecting to ");
  Serial.println(host);
  while (!Serial);
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client;
  if (client.connect(host, 80)) {
    while (Serial.available()) {
      char dataParsing = Serial.read();
      if (dataParsing == '\n') {
        parseCommand(command);
        command = "";
      }
      else {
        command += dataParsing;
      }
    }

    Serial.print("lux "); Serial.println(lux);
    Serial.print("v "); Serial.println(busvoltage);
    Serial.print("ma "); Serial.println(current_mA);
    Serial.print("mw "); Serial.println(power_mW);

    String url = "/tambah-proses.php";
    url += "?lux=";
    url += lux;
    url += "&v=";
    url += busvoltage;
    url += "&ma=";
    url += current_mA;
    url += "&mw=";
    url += power_mW;
    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print(String("GET /") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n" + "\r\n");
    while (client.connected()) {
      if (client.available()) {
        dataPHP = client.readStringUntil('\n');
        int q = 0;
        Serial.println(dataPHP);
        data[q] = "";
        for (int i = 0; i < dataPHP.length(); i++) {
          if (dataPHP[i] == '#') {
            q++;
            data[q] = "";
          } else {
            data[q] = data[q] + dataPHP[i];
          }
        }
        Parsing = false;
        dataPHP = "";
      }
    }
    Serial.println("[Disconnected]");
  } else {
    Serial.println("connection failed!]");
  }
  delay(5000);
}

void parseCommand(String com) {
  String part1;
  String part2;
  part1 = com.substring(0, com.indexOf(" "));
  part2 = com.substring(com.indexOf(" ") + 1);
  if (part1.equalsIgnoreCase("lux")) {
    lux = part2.toFloat();
  }
  if (part1.equalsIgnoreCase("v")) {
    busvoltage = part2.toFloat();
  }
  if (part1.equalsIgnoreCase("ma")) {
    current_mA = part2.toFloat();
  }
  if (part1.equalsIgnoreCase("mw")) {
    power_mW = part2.toFloat();
  }
}
