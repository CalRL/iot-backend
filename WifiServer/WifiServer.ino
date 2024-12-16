#include <WiFiS3.h>

// WiFi credentials
const char* ssid = "staline";
const char* password = "mdptemporaire1";

// Flask server details
const char* serverIP = "172.20.10.3";  // Replace with your Flask server's IP
const int serverPort = 5000;           // Replace with your Flask server's port

int pin = 7;
int state = 0;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(pin, OUTPUT);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      Serial.println(WiFi.RSSI());
  }

  Serial.println("\nConnected with Static IP!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Signal Strength (RSSI): ");
  Serial.println(WiFi.RSSI());
}

void loop() {
  if (!client.connected()) {
    Serial.println("Connecting to Flask server...");
    if (client.connect(serverIP, serverPort)) {
      Serial.println("Connected to Flask server");
    } else {
      Serial.println("Failed to connect to server");
      delay(1000);
      return;
    }
  }

  // Send state information
  String message;
  if (state == 0) {
    digitalWrite(pin, HIGH);
    message = "Pin " + String(pin) + " set to HIGH";
    state = 1;
  } else {
    digitalWrite(pin, LOW);
    message = "Pin " + String(pin) + " set to LOW";
    state = 0;
  }

  client.println(message);  // Send message to Flask server
  Serial.println("Sent to server: " + message);
}


void processData(WiFiClient& client, String receivedData) {
  Serial.println("Received data: " + receivedData);
  receivedData.trim();  // Remove any extra whitespace or newline characters
  String toSend;

  if (receivedData == "STATE1") {
    // Toggle pin state and prepare the response message
    if (state == 0) {
      digitalWrite(pin, HIGH);
      toSend = "Pin " + String(pin) + " set to HIGH";
      Serial.println(toSend);
      state = 1;
    } else if (state == 1) {
      digitalWrite(pin, LOW);
      toSend = "Pin " + String(pin) + " set to LOW";
      Serial.println(toSend);
      state = 0;
    }

    // Send the response to the Flask server using a properly formatted HTTP POST request
    sendHttpPost(toSend);
  } else {
    Serial.println("Unknown command received: " + receivedData);
  }
}

// Function to send a properly formatted HTTP POST request
void sendHttpPost(String message) {
  if (client.connected()) {
    // Build the HTTP POST request
    String httpRequest = "POST / HTTP/1.1\r\n";
    httpRequest += "Host: 192.168.137.1\r\n";  // Replace with Flask server's IP
    httpRequest += "Content-Type: text/plain\r\n";
    httpRequest += "Content-Length: " + String(message.length()) + "\r\n";
    httpRequest += "\r\n";
    httpRequest += message;

    // Send the request
    client.print(httpRequest);
    Serial.println("HTTP POST sent:");
    Serial.println(httpRequest);
  } else {
    Serial.println("Failed to send POST. Client not connected.");
  }
}
