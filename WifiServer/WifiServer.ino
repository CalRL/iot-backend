#include <WiFiS3.h>

// WiFi credentials
const char* ssid = "staline";
const char* password = "mdptemporaire1";

// Flask server details
const char* serverIP = "172.20.10.7";  // Replace with your Flask server's IP
const int serverPort = 5000;           // Replace with your Flask server's port

const int pin = 7;
int state = 0;

WiFiClient client;
WiFiServer server(5001);
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(pin, OUTPUT);
  Serial.println("Attempting to connect to WPA SSID: ");
  while (status != WL_CONNECTED) {
    Serial.println(ssid);
    status = WiFi.begin(ssid, password);

    delay(500);
    Serial.print(".");
    Serial.println(WiFi.RSSI());
  }

  Serial.println("\nConnected with Static IP!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Server started on port 5001...");
}

void loop() {
  // Check if a new client is connected
  WiFiClient incomingClient = server.available();
  if (incomingClient) {
    Serial.println("New client connected.");

    // Keep the connection open as long as the client is connected
    while (incomingClient.connected()) {
      // Check if data is available from the client
      if (incomingClient.available()) {
        String receivedData = "";

        // Read all available data
        while (incomingClient.available()) {
          char c = incomingClient.read();
          receivedData += c;
        }

        // Output the received data to the Serial Monitor
        if (receivedData.length() > 0) {
          Serial.println("Received data: " + receivedData);

          // Process the received data
          processData(incomingClient, receivedData);
        }
      }

      // Small delay to prevent busy-waiting
      delay(10);
    }

    Serial.println("Client disconnected.");
  }
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

  } else {
    Serial.println("Unknown command received: " + receivedData);
  }
  sendHttpPost(toSend);
  Serial.println("Response sent to client: " + toSend);
}

// Function to send a properly formatted HTTP POST request
void sendHttpPost(String message) {
    // Attempt to connect to the server if not already connected
    if (!client.connected()) {
        Serial.println("Attempting to connect to the server...");
        if (!client.connect(serverIP, serverPort)) {
            Serial.println("Connection to server failed.");
            return; // Exit if connection fails
        }
        Serial.println("Connected to server.");
    }

    // Build the HTTP POST request
    String httpRequest = "POST / HTTP/1.1\r\n";
    httpRequest += "Host: " + String(serverIP) + "\r\n";
    httpRequest += "Content-Type: text/plain\r\n";
    httpRequest += "Content-Length: " + String(message.length()) + "\r\n";
    httpRequest += "Connection: close\r\n\r\n"; // Close connection after the request
    httpRequest += message;

    // Send the request
    client.print(httpRequest);
    Serial.println("HTTP POST sent:");
    Serial.println(httpRequest);

    // Wait for and print the server response
    Serial.println("Awaiting server response...");
    while (client.connected() || client.available()) {
        if (client.available()) {
            String response = client.readStringUntil('\n');
            Serial.println("Server response: " + response);
        }
    }

    // Close the connection
    client.stop();
    Serial.println("Connection closed.");
}
