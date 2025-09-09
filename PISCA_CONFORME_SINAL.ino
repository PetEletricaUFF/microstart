#include <Arduino.h>
// #include "PinDefinitionsAndMore.h" // Set IR_RECEIVE_PIN for diferentes CPUs
#include "TinyIRReceiver.hpp"      // include the TinyIRReceiver code

#define LED_PIN 1  // LED interno do ATtiny85 (PB1)
#define IR_RECEIVE_PIN 2  // LED interno do ATtiny85 (PB1)

void setup() {
    pinMode(LED_PIN, OUTPUT);           // Configura o LED como saída
    initPCIInterruptForTinyReceiver();  // Inicializa o receptor IR
}

void loop() {
    if (TinyReceiverDecode()) {
        // Recebeu algum sinal IR
        digitalWrite(LED_PIN, HIGH);  // Acende o LED

        // Aqui você pode tratar address/command se quiser
        // uint16_t address = TinyIRReceiverData.Address;
        // uint8_t command = TinyIRReceiverData.Command;
        // uint8_t flags = TinyIRReceiverData.Flags;

    } else {
        // Não recebeu nada
        digitalWrite(LED_PIN, LOW);   // Desliga o LED
    }
}
