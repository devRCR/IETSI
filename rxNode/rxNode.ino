/***************************************************************
// Estación Base para sistema de llamado de pacientes
// by Renzo Chan
// ver. 0.2
****************************************************************/
#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

/************ Radio Setup ***************/
#define RF69_FREQ 915.0       // Frecuencia de 915 MHz
#define SERVER_ADDRESS   1        // ID de la estación base (recepciona las solicitudes de los mandos) 

// Configuración de pines del módulo RFM69
#define RFM69_CS    8
#define RFM69_INT   3
#define RFM69_RST   4
#define LED        13

// Instancia del objeto de controlador de radio
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// Clase para gestionar la entrega y recepción de mensajes, utilizando el controlador declarado anteriormente.
RHReliableDatagram rf69_manager(rf69, SERVER_ADDRESS);

String result;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  digitalWrite(LED, LOW);

  Serial.println("Feather Addressed RFM69 RX Test!");
  Serial.println();

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
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                  };
  rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

// No colocar esto en el void loop, ni void setup
uint8_t data[] = "dato recibido...";
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];

void loop() {
  if (rf69_manager.available()) {
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (rf69_manager.recvfromAck(buf, &len, &from)) {
      buf[len] = 0;
      for (size_t i = 0; i < sizeof(buf); i++) {
        result += (char)buf[i];
      }
      if (result == "*21A") {
        Serial.println("*21A");
      }
      else if (result == "*31A")  {
        Serial.println("*31A");
      }
      else {}
      Blink(LED, 40, 3);
      result = "";

      // Enviar una respuesta al mando remoto
      if (!rf69_manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed");
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
