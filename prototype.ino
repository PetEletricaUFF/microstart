#include <Arduino.h>

#define IR_PIN   2   // Pino do receptor IR (PB2 no ATtiny85)
#define LED_PIN  1   // Pino do LED (PB1 no ATtiny85)

// Variáveis globais
volatile uint32_t lastChangeMicros = 0;
volatile uint16_t rawBuffer[128];   // Armazena até 128 pulsos
volatile uint8_t rawCount = 0;
volatile bool frameReady = false;

// Protótipo
void decodeSony();

// Interrupção de mudança no pino IR
ISR(PCINT0_vect) {
  uint32_t now = micros();
  uint32_t dur = now - lastChangeMicros;
  lastChangeMicros = now;

  if (rawCount < 32) {
    rawBuffer[rawCount++] = dur;
  }

  // Se ficou tempo demais sem pulso, fecha o frame
  if (dur > 5000 && rawCount > 0) {
    frameReady = true;
  }
}

void setup() {
  pinMode(IR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Habilita interrupção de pin change no PB2 (pino 2 Arduino)
  GIMSK |= (1 << PCIE);     // habilita interrupção de mudança
  PCMSK |= (1 << PCINT2);   // habilita no PB2
  sei();                    // global interrupt enable
}

void loop() {
  if (frameReady) {
    noInterrupts();
    uint8_t count = rawCount;
    uint16_t buffer[32];
    for (uint8_t i = 0; i < count; i++) buffer[i] = rawBuffer[i];
    rawCount = 0;
    frameReady = false;
    interrupts();

    // Decodifica
    decodeSony(buffer, count);
  }
}

// Função para decodificar Sony SIRC
void decodeSony(uint16_t *buf, uint8_t count) {
  uint16_t bits = 0;
  uint8_t nbits = 0;

  for (uint8_t i = 0; i < count; i++) {
    uint16_t dur = buf[i];

    // Pulso de start ~2400us
    if (i == 0 && (dur < 2000 || dur > 2800)) {
      return; // não é Sony
    }

    // Depois do start, cada "1" ~1200us, cada "0" ~600us
    if (i > 0) {
      if (dur > 900 && dur < 1500) {
        bits |= (1 << nbits); // bit 1
        nbits++;
      } else if (dur > 400 && dur < 800) {
        // bit 0
        nbits++;
      }
    }
  }

  if (nbits >= 12) { // Sony tem 12, 15 ou 20 bits
    uint8_t command = bits & 0x7F; // 7 bits menos significativos
    uint8_t address = (bits >> 7) & 0x1F;

    // Aqui você trata os botões
    if (command == 0x15) { // Exemplo: botão com código 0x15
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }
}
