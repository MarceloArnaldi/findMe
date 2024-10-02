#include <ESP8266WiFi.h>

// Defina o nome e a senha da rede Wi-Fi
const char* ssid = "NOME_DA_SUA_REDE";
const char* password = "SENHA_DA_SUA_REDE";

void setup() {
  // Inicializa a comunicação serial para monitorar a conexão
  Serial.begin(115200);
  delay(10);
  
  // Conecta-se à rede Wi-Fi
  Serial.println();
  Serial.println("Conectando à rede Wi-Fi...");
  WiFi.begin(ssid, password);

  // Espera até conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Mostra informações de conexão
  Serial.println();
  Serial.println("Conectado à rede Wi-Fi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Nada no loop neste exemplo, mas aqui você pode adicionar o que quiser
}
