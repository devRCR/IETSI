/***************************************************************
// Mando inalámbrico para sistema de llamado de pacientes
// by Renzo Chan
// ver. 0.2
****************************************************************/
#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

const int pushButton = 12;      // Pulsador conectado al pin 12 de la tarjeta

/************ Radio Setup ***************/
#define RF69_FREQ 915.0         // Frecuencia de 915 MHz
#define MY_ADDRESS   3          // ID del mando (acá podría ir el ID del Usuario Ventanillero)
#define DEST_ADDRESS 1          // ID de la estación base (recepciona las solicitudes de los mandos)

// Configuración de pines del módulo RFM69
#define RFM69_CS    8
#define RFM69_INT   3
#define RFM69_RST   4
#define LED        13

// Instancia del objeto de controlador de radio
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// Clase para gestionar la entrega y recepción de mensajes, utilizando el controlador declarado anteriormente.
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

void setup() {
  Serial.begin(115200);
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // reset manual
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69_manager.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  rf69.setTxPower(20, true); 

  // La clave de cifrado debe ser la misma que la del cliente.
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

// No colocar esto en el void loop, ni void setup
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
uint8_t data[] = "  OK";

void loop() {
  bool stateButton = digitalRead(pushButton);

  if (stateButton == LOW) {
    char radiopacket[20] = "*31A";
    Serial.print("Enviando "); Serial.println(radiopacket);

    // Envio del mensaje al destino
    if (rf69_manager.sendtoWait((uint8_t *)radiopacket, strlen(radiopacket), DEST_ADDRESS)) {
      // Esperamos por la respuesta del server
      uint8_t len = sizeof(buf);
      uint8_t from;
      if (rf69_manager.recvfromAckTimeout(buf, &len, 2000, &from)) {
        buf[len] = 0; // poner a 0 el string restante

        Serial.print("Obteniendo respuesta de #"); Serial.print(from);
        Serial.print(" [RSSI :");
        Serial.print(rf69.lastRssi());
        Serial.print("] : ");
        Serial.println((char*)buf);
        Blink(LED, 40, 3); // LED parpadea 3 veces, 40 ms entre parpadeo
      } else {
        Serial.println("No hay respuesta, el servidor está operativo?");
      }
    } else {
      Serial.println("Envio fallido");
    }
  }
}

void Blink(byte pin, byte delay_ms, byte loops) {
  while (loops--) {
    digitalWrite(pin, HIGH);
    delay(delay_ms);
    digitalWrite(pin, LOW);
    delay(delay_ms);
  }
}
