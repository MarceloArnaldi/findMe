/*
 * 
 * findMe
 * 
 * Monitor Serial : 115.200
 * 
 * Versao 0.01 - 10/10/2024 - 1st Version
 * 
 * Doko - onde
 * KOCHIRA (nesta direção), SOCHIRA (nessa direção), ACHIRA (naquela direção), e DOCHIRA (em qual direção
 * 
 */

#include <dummy.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <PubSubClient.h>

// Configuracao
const String cTipoModulo  = "KD"; 
const String cTpNS        = "01"; 
const String cNS          = "00001";
const String cVersao      = "0.01";
const String cData        = "11/11/2024";

// Maquina de Estado
const int cInicio           = 0; // Mensagem boas vindas
const int cAPConfig         = 1; // Configura AP
const int cAPConnect        = 2; // Aguarda a conexÃ£o de alguem no AP
const int cWebServer        = 3; // Socket - WebServer que o App se conecta para configurar o SSID, Senha, IP da MC e ID do mÃ³dulo
const int cSaveCredentials  = 4; // Grava o SSID e senha informados
const int cConnectToSSID    = 5; // Tenta a conexao na rede WIFI do cliente
const int cStatusSSID       = 6; // Verifica a conexao na rede WIFI do cliente
const int cConnectSSID_Ok   = 7; // Informa via LED que a conexÃ£o na rede SSID informada foi com sucesso
const int cWebSocketMenu    = 11; // Aguarda comando para enviar os dados de configuracao para o App ou o comando para conectar a rede SSID do cliente 
const int cApagarEEPROM     = 90;
const int cLerEEPROM        = 91;
const int cGravarEEPROM     = 92;
const int cShowEEPROM       = 93;
const int cErro             = 98;
const int cNothing          = 99;

// Constantes
#define       LED_1         D0 // 16
const byte    LIGA          = HIGH;
const byte    DESLIGA       = LOW;
const int     SocketPort    = 22022;
const int     aAP_IP[4]     = {192, 168, 47, 1};
// Variaveis Globais
String        vMsgInicial;
String        vAux;
int           vEstado                   = 0;
boolean       vConnected                = false;
int           vStatus                   = WL_IDLE_STATUS;
boolean       vErro                     = false;
boolean       vClientSSID_OK            = false;
String        vConnectSSID_Ok           = "NT"; // armazena se a conexao com SSID foi feita com sucesso - OK ou NT
boolean       vmqttPort_OK              = false;
boolean       vDebug                    = true;
boolean       vMsg                      = true;
// Variaveis usadas para gravar na EEPROM
char          kdid[32] = "";           // vkdid
char          kdip[32] = "";           // vkdip
char          ssid[32] = "";           // vClientSSID
char          password[32] = "";       // vClientPWD
char          connectSSID_Ok[32] = ""; // vConnectSSID_Ok 
char          apptoken[32] = "";       // appToken
char          mqttURL[32]  = "";       // vmqttURL
char          mqttPort[32] = "";       // vmqttPort
// Modulo
String        vkdid    = "0";          // ID do modulo
String        vkdip    = "0.0.0.0";    // ID do mudulo
String        appToken = "0000000000"; // ID do APP - quando o modulo eh adcionado no APP ele recebe o ID do APP
// Modulo - usado para atualizar remotamewnte - WebSocket
String        vKdID;
String        vKdIP;

// SSID AP
char          aSSID[19];
//const char    *softAP_password = "legooluskit"; 
const char    *softAP_password = "achira1234@";
IPAddress     apIP(aAP_IP[0], aAP_IP[1], aAP_IP[2], aAP_IP[3]);
IPAddress     netMsk(255, 255, 255, 0);
// SSID Client
String        vClientSSID;
String        vClientPWD;
// SSID Client Confuguracao
IPAddress     wifiMK = IPAddress(255, 255, 255, 0);
IPAddress     wifiGW = IPAddress(0, 0, 0, 0);
IPAddress     wifiIP = IPAddress(0, 0, 0, 0);
// Socket - WebServer
WiFiServer    vServer(SocketPort); // Porta
WiFiClient    vClient;
// MQTT
WiFiClient    MQTTClient;
WiFiClient    MQTTClientLocal;
PubSubClient  MQTT(MQTTClient);
PubSubClient  MQTTLocal(MQTTClientLocal);
String        vmqttURL  = "mqtt.legoolus.com.br"; // BROKER_MQTT
String        vmqttPort = "1883"; // BROKER_PORT
char*         BROKER_MQTT_LOCAL  = "255.255.255.255";
const char*   BROKER_MQTT        = "mqtt.legoolus.com.br"; 
const int     BROKER_PORT        = 1883;
const char*   BROKER_USER        = "obtnegwv"; 
const char*   BROKER_PASSWORD    = "7_JhuAE6nm02";

void setup() {
  // Hardware
  pinMode(D0, OUTPUT);
  Serial.begin(115200);
  // Nome da rede WIFI armnazenada em aSSID
  //vAux = "LegoolusKit";
  vAux = "ModuloKade_";// DOCHIRA
  vAux.concat(cTpNS);
  vAux.concat(cNS);
  vAux.toCharArray(aSSID, 20); 
  // Msg Inicial  
  Serial.println("\n");
  vMsgInicial.concat(cTipoModulo);
  vMsgInicial.concat(cTpNS);
  vMsgInicial.concat(" - Versao ");
  vMsgInicial.concat(" - Data : ");
  vMsgInicial.concat(cData);
  vMsgInicial.concat(" - ns : ");
  vMsgInicial.concat(cNS);
  vEstado = cInicio;
}

void loop() {
  switch (vEstado) {
    case cInicio:
      msgln(" ");
      msgln(vMsgInicial);
      msgln(apptoken);
      vEstado = cLerEEPROM;
      break;
    case cApagarEEPROM:
      msgln("cApagarEEPROM");
      eraseCredentials();
      vEstado = cLerEEPROM;
      break;
    case cLerEEPROM:
      msgln("cLerEEPROM");
      loadCredentials();
      vEstado = cShowEEPROM;
      break;
    case cShowEEPROM:
      msgln("cShowEEPROM");
      showConfig();
      vEstado = cAPConfig;
      break;
    case cGravarEEPROM:
      msgln("cGravarEEPROM");
      vClientSSID = "Arnaldi";
      vClientPWD  = "AriEdu11@";
      delay(1000);
      saveCredentials();
      delay(1000);
      loadCredentials();
      delay(1000);
      showConfig();
      vEstado = cAPConfig;
      break;
    case cAPConfig:      
      WiFi.softAPdisconnect(false);
      delay(150); 
      if (vMsg) {              
        Serial.println("\n");
        Serial.print("AP SSID ... : ");
        Serial.println(aSSID);         
        Serial.print("AP Password : ");
        Serial.println(softAP_password);            
        Serial.print("AP IP ..... : ");
        Serial.println(WiFi.softAPIP());
        Serial.println("\n");
      }
      delay(500);      
      msg("Configurando o AP ... ");
      if (WiFi.softAPConfig(apIP, apIP, netMsk)) {
          msg("Ligando o AP ... ");
          if(WiFi.softAP(aSSID, softAP_password)) {      
            delay(150); 
            if (vMsg) {              
              Serial.print("Ready for AP connection.");
              Serial.println("\n");
            }
            vEstado = cAPConnect;
          } else {
            msgln("erro no WiFi.softAP");
            vEstado = cErro;
          }
      } else {
        msgln("erro no WiFi.softAPConfig");
        vEstado = cErro;
      }
      break;
    case cAPConnect:     
      if (WiFi.softAPgetStationNum() > 0) {
        msgln ( "Client Connect" );                
        vEstado = cWebServer;
        vServer.begin();
      }
      digitalWrite(LED_1, DESLIGA);
      break;      
    case cWebServer:
      // recebe SSID e senha do WebSocker (PC ou App)
      if (!vClient) { vClient = vServer.available(); } 
      if (vClientSSID_OK) {vEstado = cConnectToSSID;}  
      else {
        if ((vClient) and (vEstado != cWebSocketMenu)) {
          msgln("Client Socket Connected."); 
          msgln(" ");
          msgln("Receiving information from App : cWebServer ...");
          while ((vClient.connected()) and (vEstado != cWebSocketMenu)) {
            if (vClient.available()) {
              String line = vClient.readString();
              debugln("cWebServer--------------------------");
              debugln(line);
              line.trim();
              if (line.indexOf("[SSID]") > -1) {
                msg("SSID .... : ");
                vClientSSID = line.substring(6);
                msgln(vClientSSID);
                delay(300);
                vClient.println("[OK]");                
              } 
              if (line.indexOf("[PWD]") > -1) {
                msg("Senha ... : ");
                vClientPWD = line.substring(5);
                msgln(vClientPWD);
                delay(300);
                vClient.println("[OK]");
              }
              if (line.indexOf("[END]") > -1) {
                delay(500);                
                vClient.println("[OK]\n");
                msgln(" ");
                vKdID = " ";
                vKdIP = " ";
                saveCredentials();
                vEstado = cWebSocketMenu;
              }
            }
          }
          debugln("SAIU cWebServer - vClient.connected() NOT -------------------------- ");          
        }
      }
    break;
    case cWebSocketMenu:
      // envia SSID e senha para WebSocker (PC ou App)
      if (!vClient) { vClient = vServer.available(); } 
      if (vClient) {
        msgln(" ");
        msgln("WebServerMenu - Client Socket Connected."); 
        msgln("Receiving information from App : cWebSocketMenu ...");
        while ((vClient.connected()) and (vEstado == cWebSocketMenu)) {
          if (vClient.available()) {
            String line = vClient.readString();
            debugln("cWebServer-------------------------- ");
            debugln(line);
            line.trim();
            if (line.indexOf("[ENVIASTATUS]") > -1) {
              msgln("Envia Status.");
              showConfig();
              vClient.print("[SSID]");
              vClient.print(vClientSSID);
              vClient.print("[PWD]");              
              vClient.print(vClientPWD);              
              vClient.println("[OK]\n");              
            } 
            if (line.indexOf("[CONNECTTOSSID]") > -1) {
              msgln("Connect to SSID Client.");
              delay(500);
              vClient.println("[OK]\n");
              delay(500);
              vClient.println("[OK]\n");
              vClient.stop();
              vServer.stop();
              vEstado = cConnectToSSID;                            
            }
          }
        }
      }
      break;    
    case cConnectToSSID:
      ConnectToSSID();
      vEstado = cStatusSSID;
      break; 
    case cStatusSSID:
      if (!StatusSSID()) { vEstado = cErro; }
      break;
    case cConnectSSID_Ok:      
      vConnectSSID_Ok = "OK";
      saveCredentials();          
      vServer.begin();
      vEstado = cNothing;
      break;
    case cNothing:   
      break;
    case cErro :
      break;   
  }
}

void ConnectToSSID() {
  msgln(" ");
  msg("Desligando AP ... :");
  if (WiFi.softAPdisconnect(true)) { msgln("ConnectToSSID OK"); } else { msgln("Erro ao desligar"); }
  msgln(" ");
  msg("Connecting WIFI client ");
  msg( ssid );
  msg(" ... ");
  msg( password );
  msgln(" ... ");
  WiFi.disconnect();
  WiFi.begin ( ssid, password );
  //WiFi.begin ( "Restaurante DonDoh", "mar48621" );
  //WiFi.begin ( "ArnaldiNET", "arianeeduarda11" );
  WiFi.waitForConnectResult();
}

boolean StatusSSID() {
  vErro = false;
  int s = WiFi.status();
  switch (s) {
    case WL_CONNECT_FAILED:      
      msgln ("WL_CONNECT_FAILED");
      WiFi.disconnect();
      if (vConnectSSID_Ok == "NT") {
        debugln("se nunca se conectou nessa rede - apaga as credenciais .");
        eraseCredentials();
      } else {
        vEstado = cInicio;
      }
      break;
    case WL_CONNECTED:
      if (vMsg) {
        Serial.println( " " );
        Serial.print( "Connected to ... : " );
        Serial.println ( vClientSSID );
        Serial.print ( "IP address ..... : " );
        Serial.println ( WiFi.localIP() );  
        Serial.print ( "Kit ID ......... : " );
        Serial.println ( vKdID );
        Serial.print ( "Kit IP Fixo .... : " );
        Serial.println ( vKdIP );
      }
      //
      wifiIP = WiFi.localIP();
      wifiMK = WiFi.subnetMask();
      wifiGW = WiFi.gatewayIP(); 
      //
      vEstado = cConnectSSID_Ok;
      break;
    case WL_NO_SSID_AVAIL:
      msgln ("SSID Invalida.");
      WiFi.disconnect();
      if (vConnectSSID_Ok == "NT") {
        debugln("se nunca se conectou nessa rede - apaga as credenciais .");
        eraseCredentials();
      } else {
        vSSIDInvalida = true; // vai para teclado, ve se esta em reset de fabrica, se nao tenta novamente a conexao
        vEstado = cTeclado;
      }
      break;
    default:
      debugln("WIFI Connect - retorno nao classificado");      
      // 2018 04 02
      vEstado = cInicio;        
  }
  return !vErro;
}

void showConfig() {  
  if (vMsg) {    
    Serial.print ( "vClient SSID ... : " );
    Serial.println ( vClientSSID );
    Serial.print ( "vClient PWD..... : " );
    Serial.println ( vClientPWD );  
    Serial.print ( "KD ID .......... : " );
    Serial.println ( vkdid );
    Serial.print ( "KD IP Fixo ..... : " );
    Serial.println ( vkdip );
    Serial.print ( "MQTT ........... : " );
    Serial.println ( vmqttURL );
    Serial.print ( "MQTT Port....... : " );
    Serial.println ( vmqttPort );    
    Serial.print ( "Connec. SSID OK  : " );
    Serial.println ( vConnectSSID_Ok );
    Serial.print ( "AppToken         : ");
    Serial.println( appToken );    
  }
}

void loadCredentials() {
  debugln("LoadCredencial   :");
  EEPROM.begin(2048);
  EEPROM.get(0, ssid);  
  EEPROM.get(0+sizeof(ssid), password);
  EEPROM.get(0+sizeof(ssid)+sizeof(password), kdid);
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kdid), kdip);
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip), mqttURL);
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip)+sizeof(mqttURL), mqttPort);
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip)+sizeof(mqttURL)+sizeof(mqttPort), connectSSID_Ok);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip)+sizeof(mqttURL)+sizeof(mqttPort)+sizeof(connectSSID_Ok), apptoken);
  EEPROM.end();
  vClientSSID     = String(ssid);
  vClientPWD      = String(password);  
  vkdid           = String(kdid);
  vkdip           = String(kdip);  
  vmqttURL        = String(mqttURL);
  vmqttPort       = String(mqttPort); 
  vConnectSSID_Ok = String(connectSSID_Ok);  
  appToken        = String(apptoken);
  if ((vmqttPort.toInt() > 0) and (vmqttPort.toInt() < 99999)) { vmqttPort_OK = true; } else { vmqttPort_OK = false; }
  /*
  if (((vClientSSID[0] == 0xFF) and (vClientSSID[1] == 0xFF) and (vClientSSID[2] == 0xFF) and (vClientSSID[3] == 0xFF)) or (vConnectSSID_Ok != "OK")) { 
    debugln("Sujeira na memoria ou SSID_NOT OK");
    eraseCredentials();
    vClientSSID_OK = false; 
    ssid[0]        = 0;
    vClientSSID    = "";
  } else { 
    vClientSSID_OK = true;
  }
  */
  vClientSSID_OK = true;
}

void saveCredentials() {
  vClientSSID.toCharArray(ssid, sizeof(ssid) - 1);
  vClientPWD.toCharArray(password, sizeof(password) - 1);
  vkdid.toCharArray(kdid, sizeof(kdid) - 1);
  vkdip.toCharArray(kdip, sizeof(kdip) - 1);
  vmqttURL.toCharArray(mqttURL, sizeof(mqttURL) - 1);
  vmqttPort.toCharArray(mqttPort, sizeof(mqttPort) - 1); 
  vConnectSSID_Ok.toCharArray(connectSSID_Ok, sizeof(connectSSID_Ok) - 1);
  appToken.toCharArray(apptoken, sizeof(apptoken) - 1);
  EEPROM.begin(2048);  
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  EEPROM.put(0+sizeof(ssid)+sizeof(password), kdid);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid), kdip);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip), mqttURL);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip)+sizeof(mqttURL), mqttPort);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip)+sizeof(mqttURL)+sizeof(mqttPort), connectSSID_Ok);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip)+sizeof(mqttURL)+sizeof(mqttPort)+sizeof(connectSSID_Ok), apptoken);
  EEPROM.commit();
  EEPROM.end();  
}

void eraseCredentials() {
  msgln("eraseCredentials");
  ssid[0]     = 0;
  password[0] = 0;  
  kdid[0]     = 0;
  kdip[0]     = 0;
  mqttURL[0]  = 0;
  mqttPort[0] = 0;
  apptoken[0] = 0;
  vConnectSSID_Ok = "NT";
  vConnectSSID_Ok.toCharArray(connectSSID_Ok, sizeof(connectSSID_Ok) - 1);  
  EEPROM.begin(2048);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  EEPROM.put(0+sizeof(ssid)+sizeof(password), kdid);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid), kdip);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip), mqttURL);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip)+sizeof(mqttURL), mqttPort);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip)+sizeof(mqttURL)+sizeof(mqttPort), connectSSID_Ok); 
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kdid)+sizeof(kdip)+sizeof(mqttURL)+sizeof(mqttPort)+sizeof(connectSSID_Ok), apptoken);
  EEPROM.commit();
  EEPROM.end();
}

void debug(String msg) {
  if (vDebug) {
    Serial.print(msg);
  }
}

void debugln(String msg) {
  if (vDebug) {
    Serial.println(msg);
  }
}

void msg(String msg) {
  if (vMsg) {
    Serial.print(msg);
  }
}

void msgln(String msg) {
  if (vMsg) {
    Serial.println(msg);
  }
}

void pausa(int tempo) {
  //digitalWrite(LED_1,!digitalRead(LED_1));
  digitalWrite(LED_1,LIGA);
  delay(500 * tempo);
}
