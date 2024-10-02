#include <ESP8266WiFi.h>

// Função para escanear redes Wi-Fi
void listarRedesWiFi() {
  Serial.println("Iniciando o escaneamento das redes Wi-Fi...");
  
  // Inicia o escaneamento
  int numeroRedes = WiFi.scanNetworks();
  
  // Exibe o número de redes encontradas
  Serial.println("Escaneamento finalizado.");
  Serial.print("Número de redes encontradas: ");
  Serial.println(numeroRedes);

  // Lista todas as redes disponíveis
  for (int i = 0; i < numeroRedes; i++) {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i)); // Nome da rede (SSID)
    Serial.print(" (Sinal: ");
    Serial.print(WiFi.RSSI(i)); // Força do sinal em dBm
    Serial.print(" dBm) ");
    Serial.print((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "Sem senha" : "Protegida"); // Tipo de criptografia
    Serial.println();
  }
}

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);
  delay(10);
  
  // Configura o ESP8266 no modo de estação (para escanear redes)
  WiFi.mode(WIFI_STA);
  
  // Chama a função para listar redes
  listarRedesWiFi();
}

void loop() {
  // Nada no loop neste exemplo
}
