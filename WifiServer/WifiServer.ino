#include <WiFiS3.h>

const char* ssid = "ArduinoR4-AP";
const char* password = "password123";

int pin = 7;
int state = 0;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(pin, OUTPUT);

  // Configure the board as an Access Point
  if (WiFi.beginAP(ssid, password)) {
    Serial.println("Access Point started");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to start Access Point");
    while (true); // Halt on failure
  }

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  // Check for incoming client connections
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");

    while (client.connected()) {
      if (client.available()) {
        String receivedData = client.readStringUntil('\n');
        processData(receivedData);

        while(client.available()) {
          client.read();
        }
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}

void processData(String receivedData) {
  Serial.println(receivedData);
  receivedData.trim();
  if(receivedData == "STATE1") {
    if(state == 0) {
      digitalWrite(pin, HIGH);
      Serial.println(pin + " set to HIGH: " + receivedData);
      state = 1;
    } else if (state == 1) {
      digitalWrite(pin, LOW);
      Serial.println(pin + " set to LOW: " + receivedData);
      state = 0;
    }
    return;
  }
}
