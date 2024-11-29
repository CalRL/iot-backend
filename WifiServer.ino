#include <WiFiS3.h>

// Access Point credentials
const char* ssid = "ArduinoR4-AP";
const char* password = "password123";

int pin = 7;

// Create a server on port 80
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
        String toSend = "";
        Serial.print("Received: ");
        Serial.println(receivedData);

        receivedData.trim();

       if(receivedData == "SET HIGH") {
        digitalWrite(pin, HIGH);
        Serial.println("SET HIGH");
       } else if (receivedData == "SET HIGH") {
        digitalWrite(pin, LOW);
        Serial.println("SET LOW");
       }
       toSend = receivedData;
        // Echo back to the client
        client.println("Echo: " + toSend);

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
