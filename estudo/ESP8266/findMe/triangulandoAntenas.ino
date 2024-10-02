/*

Como funciona a triangulação de Wi-Fi?
A triangulação Wi-Fi usa a força do sinal (RSSI) de vários pontos de acesso Wi-Fi conhecidos para estimar a distância até esses pontos. A partir disso, com múltiplas medições, é possível triangular uma posição. O processo envolve basicamente os seguintes passos:

Obtenção dos pontos de acesso Wi-Fi conhecidos: Capturar a intensidade do sinal (RSSI) de vários pontos de acesso Wi-Fi ao seu redor.
Conversão da intensidade do sinal (RSSI) em distância: Usar uma fórmula de atenuação de sinal para estimar a distância entre o dispositivo e cada ponto de acesso com base na intensidade do sinal.
Triangulação: Com as distâncias estimadas de pelo menos três pontos de acesso (para triangulação em 2D), calcular a localização do dispositivo.
Fórmula de conversão de RSSI para distância
Uma fórmula comum para converter o RSSI em distância é a seguinte:

𝑑 = 10 evelado (𝑃0−𝑅𝑆𝑆𝐼) / 10⋅𝑛

Onde:

d é a distância em metros.
𝑃0 é o valor de referência do RSSI a 1 metro do ponto de acesso.
RSSI é a intensidade do sinal medida.
n é o fator de atenuação, que depende do ambiente (por exemplo, em áreas abertas, o valor de 
n pode ser 2; em ambientes com muitos obstáculos, pode ser maior, como 3 ou 4).

Limitações
Precisão do RSSI: O valor de RSSI pode ser muito impreciso devido a interferências de obstáculos (paredes, móveis, etc.) e outras fontes de sinal, o que faz com que a estimativa de distância seja bastante imprecisa.

Posições conhecidas dos pontos de acesso: Para fazer triangulação, você precisa saber as coordenadas exatas dos pontos de acesso Wi-Fi, o que pode não ser viável em muitos casos, especialmente em ambientes desconhecidos.

Múltiplos pontos de acesso: A triangulação requer a leitura de, pelo menos, três pontos de acesso Wi-Fi distintos, cujas posições (coordenadas) você precisa conhecer.

*/

#include <ESP8266WiFi.h>

// Função para converter RSSI em distância
float calcularDistancia(int RSSI, int P0, float n) {
  return pow(10, (P0 - RSSI) / (10 * n));
}

void listarRedesWiFi() {
  Serial.println("Escaneando redes Wi-Fi...");

  int numeroRedes = WiFi.scanNetworks();
  Serial.println("Escaneamento finalizado.");
  Serial.print("Número de redes encontradas: ");
  Serial.println(numeroRedes);

  // Lista as redes disponíveis
  for (int i = 0; i < numeroRedes; i++) {
    String ssid = WiFi.SSID(i);
    int rssi = WiFi.RSSI(i);
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(ssid);
    Serial.print(" (Sinal: ");
    Serial.print(rssi);
    Serial.print(" dBm) ");

    // Suponha que o P0 seja -50 dBm a 1 metro e n seja 2 (ambiente livre)
    // potência de referência P0, e do fator de atenuação n.
    float distancia = calcularDistancia(rssi, -50, 2);
    Serial.print(" Distância estimada: ");
    Serial.print(distancia);
    Serial.println(" metros");
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_STA);

  listarRedesWiFi();
}

void loop() {
  // Não é necessário loop neste exemplo
}
