#include <EEPROM.h>
#include <RF24.h>
#include <RF24Mesh.h>
#include <RF24Network.h>
#include <SPI.h>

// Iniciación de librerías
RF24 radio(9, 10);             // Pines para CE y CSN
RF24Network network(radio);    // network implementa la capa de red
RF24Mesh mesh(radio, network); // mesh implementa una red mesh

// Configuración
const uint8_t node_id = 1;         // ID del nodo, 0 es para el master
const uint32_t interval_ms = 2000; // Intervalo entre envío de mensajes

// Globales
uint32_t timer = 0; // Timer usado para los intervalos

void setup() {
  // Se inicia la comunicación con el puerto serie usado para imprimir
  // información de depuración
  Serial.begin(115200);

  // Se ajusta la ID del nodo
  mesh.setNodeID(node_id);

  // Se conecta a la red
  Serial.println(F("Conectando a la red..."));
  mesh.begin();
}

void loop() {
  // Hay que llamar periódicamente a esta función
  mesh.update();

  // Se comprueba que haya pasado "interval_ms" milisegundos desde que mandamos
  // el mensaje anterior
  if (millis() - timer >= interval_ms) {
    // Ajustamos los parámetros del mensaje
    const int message_len = 16;
    const char message_type = 'R';
    const char message[message_len] = "hola mundo";

    // Guardamos el instante actual de tiempo
    timer = millis();

    // Se envía el mensaje
    if (!mesh.write(message, message_type, message_len)) {

      // Si falla el envío se comprueba si hay conexión a la red para
      // volver a conectarse
      if (!mesh.checkConnection()) {
        Serial.println("Renewing Address");
        mesh.renewAddress();
      }
    }
  }
}
