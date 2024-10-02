/*
 * -4:MQTT_CONNECTION_TIMEOUT - o servidor não respondeu dentro do tempo de manutenção de atividade
 * -3:MQTT_CONNECTION_LOST - a conexão de rede foi interrompida
 * -2:MQTT_CONNECT_FAILED - a conexão de rede falhou
 * -1:MQTT_DISCONNECTED - o cliente é desconectado de forma limpa
 * 0:MQTT_CONNECTED - o cliente está conectado
 * 1:MQTT_CONNECT_BAD_PROTOCOL - o servidor não suporta a versão solicitada de MQTT
 * 2:MQTT_CONNECT_BAD_CLIENT_ID - o servidor rejeitou o identificador do cliente
 * 3:MQTT_CONNECT_UNAVAILABLE - o servidor não conseguiu aceitar a conexão
 * 4:MQTT_CONNECT_BAD_CREDENTIALS - o nome de usuário / senha foram rejeitados
 * 5:MQTT_CONNECT_UNAUTHORIZED - o cliente não foi autorizado a conectar
 * 
*/

#include <ESP8266WiFi.h>
#include <Ethernet.h>
#include <PubSubClient.h>

const char* BROKER_MQTT        = "mqtt.legoolus.com.br"; 
const int   BROKER_PORT        = 1883;
const char* BROKER_USER        = "obtnegwv"; 
const char* BROKER_PASSWORD    = "7_JhuAE6nm02"; 
char*       CLIENT_MQTT        = "MyClienteTeste";

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

boolean vStatus  = true;
boolean vConnectToMQTT = false;

void setup()
{
  Serial.begin(74880);
  Serial.println("MQTT Teste");
  WiFi.begin ( "ArnaldiNET", "arianeeduarda11" );
  WiFi.waitForConnectResult();
}

void loop()
{
  if (vStatus) {
    StatusSSID();
  }
  if (vConnectToMQTT) {
    vConnectToMQTT = false;
    Serial.println ( " " );
    Serial.print ( "BROKER_MQTT  .... : " );
    Serial.println ( BROKER_MQTT );    
    Serial.print ( "PORT ............ : " );
    Serial.println ( BROKER_PORT );
    Serial.print ( "CLIENT_MQTT  .... : " );
    Serial.println ( CLIENT_MQTT );
    Serial.print ( "USER_MQTT  ...... : " );
    Serial.println ( BROKER_USER );
    Serial.print ( "PWD_MQTT  ....... : " );
    Serial.println ( BROKER_PASSWORD );
    Serial.println ( " " );
    mqttClient.setServer(BROKER_MQTT, BROKER_PORT);
    if (mqttClient.connect(CLIENT_MQTT, BROKER_USER, BROKER_PASSWORD)) {
      Serial.println("MQTT OK");
      Serial.println("MQTT State : " + String(mqttClient.state()));
    } else {
      Serial.println("MQTT_CONNECT_FAILED");
      Serial.println("MQTT State : " + String(mqttClient.state()));
    }
  }
}

void StatusSSID() {
  vStatus = false;  
  Serial.println("Status SSID ");  int s = WiFi.status();
  switch (s) {
    case WL_CONNECT_FAILED:      
      Serial.println("WL_CONNECT_FAILED");      
      break;
    case WL_CONNECTED:
      Serial.println( "Connected" ); 
      vConnectToMQTT = true;   
      delay(1500);
      break;
    case WL_NO_SSID_AVAIL:
      Serial.println("SSID Invalida.");
      WiFi.disconnect();      
      break;
    default:
      Serial.println("WIFI Connect - retorno nao classificado");            
  }
}
