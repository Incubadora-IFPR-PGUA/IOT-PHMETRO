<!-- Definir um offset

  No contexto de calibração de sensores, definir um offset significa ajustar a leitura do sensor para compensar qualquer desvio ou erro sistemático presente na medição. Este ajuste garante que as leituras do sensor sejam precisas e reflitam corretamente a condição real que está sendo medida.

  Para definir um offset você precisará de um fio para curto-circuitar a parte externa e o centro do conector da sonda. Isso causa uma tensão de 2,5 volts no pino de saída analógica Po. Primeiro conecte a parte externa do conector BNC ao centro do conector da sonda BNC. Após isso, conecte o pino V+ ao VCC 5V do ESP32 e conecte o pino GND ao pino GND do ESP32, por fim, conecte o pino de saída analógica Po ao pino 35 do ESP32.

  LINK -> https://blog.eletrogate.com/monitorando-o-ph-de-liquidos-com-esp32-e-sensor-ph-4502c/
-->

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);
  Wire.setClock(100000);

  if (!ads.begin(0x48)) {
    Serial.println("ADS não encontrado");
    while (1);
  }

  ads.setGain(GAIN_TWOTHIRDS); // ±6.144V
}

void loop() {

  int16_t raw = ads.readADC_SingleEnded(0);
  float voltage = ads.computeVolts(raw);

  Serial.print("RAW: ");
  Serial.print(raw);
  Serial.print(" | Tensão: ");
  Serial.println(voltage, 4);

  delay(1000);
}