#include "RF24.h"
#include "RF24Mesh.h"
#include "RF24Network.h"
#include <EEPROM.h>
#include <SPI.h>

RF24 radio(9, 10);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

#define nodeID 1
#define INTERVAL_MS 10000

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

uint32_t timer = 0;

void setup() {

  Serial.begin(115200);

  // Set the nodeID manually
  mesh.setNodeID(nodeID);

  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();

  // Initialize seed
  randomSeed(analogRead(0));
}

void loop() {

  mesh.update();

  // Send to the master node every second
  if (millis() - timer >= INTERVAL_MS) {
    timer = millis();

    uint32_t randNumber = random(5) + 80;

    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&randNumber, 'M', sizeof(randNumber))) {

      // If a write fails, check connectivity to the mesh network
      if (!mesh.checkConnection()) {
        // refresh the network address
        Serial.println("Renewing Address");
        mesh.renewAddress();
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      Serial.print("Send OK: ");
    }
  }

  while (network.available()) {
    RF24NetworkHeader header;
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
  }
}
