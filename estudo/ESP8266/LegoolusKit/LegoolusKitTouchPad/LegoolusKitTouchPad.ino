/*
 *   LKTOUCHPAD
 *   Baseado no LegoolusKitSmart
 *   
 *   Reset de Fabrica : Pressionar o tecla 3 por 40 segundos
 *   
 *   Monitor Serial : 74880 bps
 *   Monitor Serial : 57600 bps for D2E3 verison
 *   
 *   Bibliotecas:
 *   PubSubClient - biblioteca MQTT - versao 2.6.0
 *   WiFiManager - biblioteca ESP8266 - versao 0.12.0 - 0.15.0
 *   WebSocket by MarkusSattler - 2.0.6 - 2.1.4
 *   
 *   Versao 0.11 - Beta Test Touch Pad
 *   
*/

#include <dummy.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <PubSubClient.h>

/*
20 = LegoolusKit R4
21 = LegoolusKit ER6
22 = LegoolusKit R4E6
23 = LegoolusKit R8E9
24 = LegoolusKit D4R4E6
25 = LegoolusKit R3E3
26 = LegoolusKit D2E3
27 = LegoolusKit Bridge - Usada para ligar uma rede 485 no sistema sem precisar de cabo - apenas uma iBB sozinha com um ESP8266 ESP12
28 = LegoolusKit BridgeMC - Os modulos configurados como 5 a MC envia os comandos para a UART4 e a ESP8266 envia via socket para  LegoolusKit Bridge - Essa versao eh usada apenas no LegoolusStarter com LKER6 combinados, assim os LKE6 comunicao com a MC sem precisar de configuracao
29 = LegoolusKit LED Strip
30 = LegoolusKit LED RGB
31 = LegoolusKit LED Digital
32 = LegoolusKit SmartPlug
33 = LegoolusKit SmartSwitch
34 = LegoolusKit Shutter - Modulo Cortina
35 = LegoolusKit TouchPad
 */

const unsigned char LKR3E3        = 25; 
const unsigned char LKD2E3        = 26; 
const unsigned char LKSHUTTER     = 34;
const unsigned char LKRGB         = 30;
const unsigned char LKLED         = 29;
const unsigned char LKTOUCHPAD    = 35;

//  AJUSTE PARA COMPILAR UM
//
//String vNS = "00001"; // D2E3
//String vNS = "00011"; // RGB
//String vNS = "00002"; // R3E3
//String vNS = "00009"; // Led Strip
//String vNS = "00001"; // Shutter
String vNS = "00001"; // TouchPad

//
// TECLADOS
//
// Todos os modulos tem 3 teclas - LKER6 tem 6 teclas
// R3E3 - 0 2 4 --> Existe o conector verde para teclado nas portas 5 12 14 - como o D2 nao tem conector verde, para mim essa placa nao tem conector verde - afinal Para que padronizar ?? 
// D2E3 - 0 2 5 --> Para que padronizar ?? a placa sera retrabalhada para usar a porta 4
// ER6  - 0 2 4 5 12 14
//
int gpio0_pin = 0; // O pulsador 1 esta na pora ZERO a mesma que informa ao ESP8266 para entrar em modo de gravação - em outras palavras se esta com JUMPER de gravacao a tecla 1 nao funciona! PQP! Se a tecla estiver pressionada e o modulo eh ligado ele tambem entra em modo de gravacao PQP!!!
int gpio2_pin = 2; // O pulsador 2
int gpio4_pin = 4; // O pulsador 3
//int gpio4_pin = 5; // No D2E3 - pulsador 3 - uso temporario ate ter a placa do D2E3 com a costura - porque qdo projetou o D2E3 nao foi mantido a padronizacao PQP! //  AJUSTE PARA COMPILAR TRES


// ER6
// Existe um programa exclusivo para o ER6 derivado desse

// TouchPad
#define             cTipo         35
const char          *cTipoModulo  = "TouchPad"; 
const String        cTpNS         = "05"; 

// Shutter
/*
#define             cTipo         34 
const char          *cTipoModulo  = "Shutter"; 
const String        cTpNS         = "034"; 
const boolean       c4Entradas    = false; 
*/
// R3E3
/*
#define             cTipo         25 
const char          *cTipoModulo  = "R3E3"; 
const String        cTpNS         = "025"; 
const boolean       c4Entradas    = true; // 0.07 - Viabiliza o uso de 4 entradas - feito para o cliente Roger
*/
// RGB
/*
#define             cTipo         30
const char          *cTipoModulo  = "RGB"; 
const String        cTpNS         = "030"; 
const boolean       c4Entradas    = false; 
*/
// D2E3
/*
#define             cTipo         26
const char          *cTipoModulo  = "D2E3"; 
const String        cTpNS         = "026"; 
const boolean       c4Entradas    = false; 
*/
// LED Strip
/*
#define             cTipo         29
const char          *cTipoModulo  = "LEDStrip"; 
const String        cTpNS         = "029"; 
const boolean       c4Entradas    = false; 
*/

const unsigned char cModulo       = 1;
const unsigned char cHeader       = 0xFE;
const byte          LIGA          = HIGH; 
const byte          DESLIGA       = LOW;
const byte          FECHA         = 1;
const byte          ABRE          = 2;
const int           SocketPort    = 22022;
const int           aAP_IP[4]     = {192, 168, 47, 1};
const int           vMCPort       = 11011;
// Valores para dimerizar fita de LED Branco e RGB
const int           DIM0          = 0;
const int           DIM5          = 51;
const int           DIM10         = 102;
const int           DIM20         = 204;
const int           DIM30         = 307;
const int           DIM40         = 409;
const int           DIM50         = 512;
const int           DIM60         = 614;
const int           DIM70         = 716;
const int           DIM80         = 819;
const int           DIM90         = 912;
const int           DIM100        = 1024;
const int           cTempoShutter = 180; // 1.10

// Maquina de Estado
const int cInicio           = 0; // Mensagem inicial
const int cAPConfig         = 1; // Configura AP
const int cAPConnect        = 2; // Aguarda a conexÃ£o de alguem no AP
const int cWebServer        = 3; // Socket - WebServer que o App se conecta para configurar o SSID, Senha, IP da MC e ID do mÃ³dulo
const int cSaveCredentials  = 4; // Grava o SSID e senha informados
const int cConnectToSSID    = 5; // Tenta a conexao na rede WIFI do cliente
const int cStatusSSID       = 6; // Verifica a conexao na rede WIFI do cliente
const int cConnectSSID_Ok   = 7; // Informa via LED que a conexÃ£o na rede SSID informada foi com sucesso
const int cTeclado          = 8; // Modo Teclado - os pulsadores funcionam
const int cSocket           = 9; // Habilita o Socket para comunicacao com a MC
const int cDebounce         = 10; // Evita disparo indesejado
const int cWebSocketMenu    = 11; // Aguarda comando para enviar os dados de configuracao para o App ou o comando para conectar a rede SSID do cliente e pular para o teclado
//const int cLEDTest          = 12;
const int cEnviaPara485     = 13; // Rotina que envia para a 485 
const int cMQTT             = 14; // Configura o MQTT
const int cNothing          = 98;
const int cErro             = 99;
//
// Variaveis
//
int           vEstado                   = 0;
boolean       vConnected                = false;
int           vStatus                   = WL_IDLE_STATUS;
boolean       vErro                     = false;
//int           vRecebendo                = 0;
boolean       vClientSSID_OK            = false;  // TRUE se existe nome da SSID armazenado
String        vConnectSSID_Ok           = "NT"; // Versao 0.08 - 2019 09 19 - Trocado 0 e 1 por OK e NT
boolean       vPulaFixaIP               = true;  // Versao 0.13 - 2019 04 01 - O Fixo do LK nao eh mais fixo
int           vContPrg                  = 0; // usado para temporizar para entrae no estado Reset de Fabrica
int           vContTeclaP0              = 0;
int           vContTeclaP2              = 0;
int           vContTeclaP4              = 0;
int           vContReConnect            = 0; // 2018 04 02 - Temporiza para reconexÃ£o quando ocorre um erro (cErro)
char*         host                      = "192.168.0.144";
String        vMsgInicial;
unsigned char frame[5];                 // array do Frame que eh enviado para MC
char          vString[16]               = {0};
int           vCanal;
int           vModulo;
String        aux;
int           dimAtual1                 = 0;
int           dimAtual2                 = 0;
int           dimAtual3                 = 0;
int           canalAtual1               = 0;
int           canalAtual2               = 0;
int           canalAtual3               = 0;
// versao 0.15
int           vMaxValueChannel_1        = 0; // memoria do ultimo valor de dimerizacao do canal 1
int           vMaxValueChannel_2        = 0;
String        vValorAux                 = "000000000"; // Antigo vTempoShutter - tempo de abertura e fechamanto em milisegundos
// versao 0.18
int           vTempoShutter             = 30; // tempo em segundos que o canal fica acionado para abrir ou fechar a cortina
int           vStatusShutter            = 0;
//
boolean       vMostraMsgTeclado         = false; // Flag para mostrar a mensagem "Teclado" somente uma vez
boolean       vSSIDInvalida             = false;
String        line2;
boolean       vMCID_OK                  = false;
boolean       vBlink_OK                 = false;
boolean       vBlink_NotOK              = false;
boolean       vClientMQTT_OK            = false; // informa que houve conexao com MQTT com sucesso
String        vLastStatus               = " ";   // Armazena o ultimo status enviado pelo STATUS
String        appToken                  = "0000000000"; // Armazena o Token do Aplicativo
// versao 0.19
boolean       vDebug                    = true; // false = inibe mensagens de debug
boolean       vMsg                      = true; // false = inibe mensagens gerais
// versao 1.00 - Envia STATUS somente uma vez quando os tres canais eh assionaco
int           vContEnviaStatus          = 0;
boolean       vEnviaStatus              = false;
// versao 1.02 - Verifica se esta conectado no WIFI e na Internet antes de conectar no MQTT
int           WIFI_Inter_Erro_Count     = 0;
boolean       vNaoSalvaCredentials      = false; // 1.05 - Quando desconecta o MQTT a forca nao grava as credenciais
// versao 1.06 - no D2E3 quando recebe um INVERTE, bloqueia o recebimento de outro comando INVERTE no mesmo canal por 2s  

// Cenarios
//
typedef struct 
 {
     int tipo;
     int id;
     int valor1;
     int valor2;
     int valor3;
     int chk;
 }  scene_type;
scene_type scenes[30]; // temos reservado 1024 bytes ou 42 acoes para um cenario
boolean vIsScene  = false;
boolean vAllON    = false; // dispara cenario Broadcast Liga Todas
boolean vAllOFF   = false; // dispara cenario Broadcast Desiga Todas
//
// LEDs 
//
#define LED_1 10
#define LED_2 13

int resetFabrica = gpio4_pin; // Reset de Fabrica

//
// Saidas - Rele R3E3 - Led - Shutter
//
// Por definicao os dois canais do extremo serao 1 e 2, o do meio sera 3 - Afinal para que logica?
//
#define SAIDA_1 05
#define SAIDA_2 14 
#define SAIDA_3 12 
//

volatile byte estado_p0 = LOW;
volatile byte estado_p2 = LOW;
volatile byte estado_p4 = LOW;
volatile byte ep0 = LOW;
volatile byte ep2 = LOW;
volatile byte ep4 = LOW;
volatile byte haschanged = 0;

//
// Blink
//
unsigned long previousMillis            = 0; 
long          interval                  = 3000;
unsigned long currentMillis             = 0;
int           vContBlink                = 0;

//
// SSID AP
//
char          aSSID[19];
const char    *softAP_password          = "legooluskit";
IPAddress     apIP(aAP_IP[0], aAP_IP[1], aAP_IP[2], aAP_IP[3]);
IPAddress     netMsk(255, 255, 255, 0);

//
// Socket - WebServer
//
WiFiServer    vServer(SocketPort); // Porta
WiFiClient    vClient;

//
// SSID Client
//
String        vClientSSID;
String        vClientPWD;
char          ssid[32] = "";
char          password[32] = "";
// SSID Client Confuguracao
IPAddress     wifiMK = IPAddress(255, 255, 255, 0);
IPAddress     wifiGW = IPAddress(0, 0, 0, 0);
IPAddress     wifiIP = IPAddress(0, 0, 0, 0);

//
// MC Socket
//
WiFiClient vMCCliente;  
unsigned int vSocketCntDown = 3;
unsigned long timeout;
unsigned long debounce;
//char* host = "192.168.0.144";     

//
// MQTT
//
//#define MQTT_SOCKET_TIMEOUT 60
//#define MQTT_KEEPALIVE 60
WiFiClient MQTTClient;
WiFiClient MQTTClientLocal;
PubSubClient MQTT(MQTTClient);
PubSubClient MQTTLocal(MQTTClientLocal);

boolean     MQTTLocalOk        = false; // informa se existe IP da MC e o Broker MQTT Local foi configurtado
boolean     MQTTLocalConnected = false; // informa se o Broker MQTT Local esta conectado
boolean     MQTTConnected      = false; // informa se o Broker MQTT Remoto esta conectado
int         MQTTLocalCount     = 3100000; // Temporiza as tentativas de conexao ao Broker MQTT Local - ele comeca com valor para tentar a conexao na primeira vez que passa pela verificacao - 3100000 = 1 mim
int         MQTTLocalCountStep = 155000; // se desconectou o LK tenta conectar um vez em 5 segundos e depois a cada 1 mim - 155.000 = 5 seg
int         MQTTCount          = 3100000; // Temporiza as tentativas de conexao ao Broker MQTT Remoto - ele comeca com valor para tentar a conexao na primeira vez que passa pela verificacao
int         MQTTCountReset     = 0;       // Versao 1.04 - Temporiza para forcar o DISCONNECT do MQTT - porque sozinho ele nao identifica que houve perda da Internet
int         MQTTState          = 99;      // Armazena o status da conexao MQTT - pode ser usado para reconectar ao servidor e/ou descobrir porque o LK foi desconectado do servidor

char*       BROKER_MQTT_LOCAL  = "255.255.255.255"; 

const char* BROKER_MQTT        = "mqtt.legoolus.com.br"; 
const int   BROKER_PORT        = 1883;
const char* BROKER_USER        = "obtnegwv"; 
const char* BROKER_PASSWORD    = "7_JhuAE6nm02"; 

/*
const char* BROKER_MQTT        = "m15.cloudmqtt.com"; 
const int   BROKER_PORT        = 14751;
const char* BROKER_USER        = "qxvmlenq"; 
const char* BROKER_PASSWORD    = "Ynh7yA_Sch7h"; 
*/
char*       SUBTOPIC_FROM_MC   = "MC"; // Sub topico quando a MC envia para o LK
char*       SUBTOPIC_TO_MC     = "LK"; // Sub topico quando o LK envia para MC
char*       SUBTOPIC_STATUS    = "STATUS"; // Status das saidas do LK
char*       SUBTOPIC_SETIP     = "SETIP";  // TOPIC_SETIP - Sub topico de Publicacao com o valor do IP do LK  
char*       MASTER_TOPIC       = "LK00000000/#"; // Status das saidas do LK
char*       MASTER_TOPIC_MC    = "MC00000/#"; // Sub topico de assinatura de todas as mensagens da MC qunado houver MC - comando tipo liga e desliga todas
char*       TOPIC_TO_MC        = "LK00000000/LK"; // Sub topico de publicacao - quando o LK envia para MC - LK02500004/LK/FE030601F8
char*       TOPIC_TO_MC_ALTERNATIVO = "MC00000LK/LK/00000000/LK/"; // Sub topico de publicacao - quando o LK envia para MC - MC00001LK/LK02500004/LK/FE030601F
char*       TOPIC_APPTOKEN     = "APP000000000000/#"; // Sub topico de assinatura de mensagens tipo broadcast do APP para os LKs - comando tipo liga e desliga todas
char*       TOPIC_TO_OTHER_LK  = "LK00000000/MC"; // Sub topico de publicacao - quando o LK envia para o outro LK

// o arroba eh para diferenciar do SETIDMC quando procuro a string no Topico - se nao quando procuro GETIP encontro GETIP e GETLK
String      TOPIC_BROADCAST = "/BROADCAST@";          // Sub topico de assinatura - comando tipo liga e desliga todas
String      TOPIC_FROM_MC   = "/MC@";                 // Sub topico de assinatura - quando a MC envia para LK - frame padrao rede cabeada
String      TOPIC_SETIDMC   = "/SETIDMC@";            // Sub topico de assinatura - quando a MC envia para LK o ID da MC - LK02500004/SETIDMC/00001
String      TOPIC_SETIDLK   = "/SETID@";              // Sub topico de assinatura - quando a MC envia para LK o ID do LK na rede cabeada - LK02500004/SETID/00003 
String      TOPIC_SETIPMC   = "/SETIPMC@";            // Sub topico de assinatura - quando a MC envia para LK o IP da MC - MC00001/SETIPMC/192.168.15.14
String      TOPIC_GETSTATUS = "/GETSTATUS@";          // Sub topico de assinatura - quando a MC ou APP solicita o STATUS do LK - LK02500004/GETSTATUS ou tipo broacast - MC00001/GETSTATUS
char*       TOPIC_SETIP_ALTERNATIVO = "MC00000LK/LK/00000000/SETIP/"; // Sub topico de publicacao - quando o LK envia para MC - MC00001LK/LK02500004/LK/FE030601F
String      TOPIC_GETIP = "/GETIP@";                  // Sub topico de assinatura - quando a MC pede para o LK enviar o IP do LK - LK02500004/GETIP
String      TOPIC_GETIPLK = "/GETIPLK@";              // Sub topico de assinatura - quando a MC pede para o LK enviar o IP do LK - tipo Broadcast - MC00001/GETIPLK
String      TOPIC_SETAPPTOKEN = "/SETAPPTOKEN@";      // Sub topico de assinatura - quando o APP envia para LK o ID do APP - LK02500004/SETAPPTOKEN/190427094099 - serve para receber comandos broadcast do APP
String      TOPIC_SETSHUTTER = "/SETSHUTTER@";        // Sub topico de assinatura - quando o APP envia para LKSHUTTER o TEMPO de abertura e fechamento da cortian - LK02500004/SETSHUTER/90 - valor em segundos, limitado em 5 mim = 300
String      TOPIC_SETSCENE = "/SETSCENE@";            // Sub topico de assinatura - quando o APP envia para LK um cenario - LK02500004/SETSCENE/010102600004000000000 - onde: 01 = numero do cenario - 01 = sequencia - 02600004 = id do lk - 000 = valor do canal 1 - 000 = valor do canal 2 - 000 = valor do canal 3
String      TOPIC_ZERA_MC_INFO = "/ZERAMCINFO@";      // Versao 1.05 -Sub topico de assinatura - informa para o LK zerar as informacoes da MC
String      TOPIC_ZERA_ALL_INFO = "/ZERAALLINFO@";    // Versao 1.05 - Sub topico de assinatura - informa para o LK zerar as TODAS informacoes 
// nome do cliente no servidor MQTT
char*       CLIENT_MQTT        = "LK00000000";

//
// ID e IP do Modulo LegoolusKit - IP MC
//
String        vKitID;
String        vKitIP;
String        vMCIP;
String        vMCID;
char          kitid[32] = "";
char          kitip[32] = "";
char          mcip[32] = "";
char          connectSSID_Ok[32] = ""; // 2018 04 02
char          mcid[32] = "";
char          apptoken[32] = ""; // versao 0.15 - Token do App do celular
char          valoraux[32] = ""; // versao 1.02 - Armazena alem o tempo de abertura e fechamento da cortina, armazena os valores dimerizados dos canais - versao 1.04 - antigo vTempoShutter

// MyTimer
int           vTimer        = -1;
int           vSegundo      = 0;
int           vTimerCanal1  = 0;
int           vTimerCanal2  = 0;
int           vTimerCanal3  = 0;

void setup()
{
  // se for Dimmer, a velocidade tem que ser igual ao ATMEL 57600
  if (cTipo == LKD2E3) { 
    Serial.begin(57600);
    Serial.println(" ");
    Serial.println(" ");
    Serial.println("Boudrate set : 57600");
    Serial.println(" ");
  } else { Serial.begin(74880); }

  //pinMode(_485, OUTPUT);
  pinMode(LED_1, OUTPUT);
  digitalWrite(LED_1, DESLIGA); 
  pinMode(LED_2, OUTPUT);
  digitalWrite(LED_2, DESLIGA);

  pinMode(SAIDA_1, OUTPUT);
  analogWrite(SAIDA_1, DESLIGA); 
  pinMode(SAIDA_2, OUTPUT);
  analogWrite(SAIDA_2, DESLIGA);
  pinMode(SAIDA_3, OUTPUT);
  analogWrite(SAIDA_3, DESLIGA);
  
  // Entradas - Todos tem 3 entradas
  pinMode(gpio0_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(gpio0_pin), nivel, CHANGE);
  pinMode(gpio2_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(gpio2_pin), nivel, CHANGE);
  pinMode(gpio4_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(gpio4_pin), nivel, CHANGE);
  // versao 1.04 - Exclusao suporte LK ER6
  // ER6
  //pinMode(gpio5_pin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(gpio5_pin), nivel, CHANGE);
  //pinMode(gpio12_pin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(gpio12_pin), nivel, CHANGE);
  //pinMode(gpio14_pin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(gpio14_pin), nivel, CHANGE);  

  // versao 0.18 - Le os dados armezanados para saber se existe o IP da MC
  //saveCredentials();
  loadCredentials();
  showConfig();
  
  // versao 0.11 - MQTT
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   
  MQTT.setCallback(callbackMQTT);    
  vMsgInicial = "LK";
  vMsgInicial.concat(cTpNS);
  vMsgInicial.concat(vNS);
  vMsgInicial.toCharArray(CLIENT_MQTT, 11);   
  // versao 0.18 - MQTT Local na LegoolusMC
  if (vMCIP.length() > 6) {
    MQTTLocalOk = true;
    vMsgInicial = vMCIP;
    vMsgInicial.toCharArray(BROKER_MQTT_LOCAL, vMCIP.length() + 1); 
    MQTTLocal.setServer(BROKER_MQTT_LOCAL, BROKER_PORT);   
    MQTTLocal.setCallback(callbackMQTTLocal);      
  }

  if (vMsg) {
    Serial.println ( " " );
    Serial.print ( "BROKER_MQTT  .... : " );
    Serial.println ( BROKER_MQTT );
    Serial.print ( "BROKER_MQTT_LOCAL : " );
    Serial.println ( BROKER_MQTT_LOCAL );
    Serial.print ( "PORT ............ : " );
    Serial.println ( BROKER_PORT );
    Serial.print ( "CLIENT_MQTT  .... : " );
    Serial.println ( CLIENT_MQTT );
    Serial.println ( " " );
  }
  
  // Nome da rede WIFI ou SSID
  vMsgInicial = "LegoolusKit";
  vMsgInicial.concat(cTpNS);
  vMsgInicial.concat(vNS);
  vMsgInicial.toCharArray(aSSID, 20);   

  // Mensagem inicial - essa mensagem eh mostrada no cInicio
  vMsgInicial = "LegoolusKit";
  vMsgInicial.concat(cTipoModulo);
  vMsgInicial.concat(" - Versao 1.09 - Data : 04/05/2020 - ns : ");
  vMsgInicial.concat(cTpNS);
  vMsgInicial.concat(vNS);

  // Le o cenario armazenado  
  loadScenes();
  
  digitalWrite(LED_1, LIGA); 
  digitalWrite(LED_2, LIGA);
  // Se o modulo for diferente de D2E3 (dimmer) aciona as saidas
  if (cTipo != LKD2E3) {
    // Se o modulo for diferente de SHUTTER (cortina) aciona as saidas
    if (cTipo != LKSHUTTER) {
      // 1.04 - Se for RGB e tem valor armazenado no vValorAux nao faz LIGA TODAS porque ele volta para a ultima cor
      if ((cTipo != LKRGB) || ((cTipo == LKRGB) && (vValorAux == "000000000"))) {
        acionaSaida(1,LIGA);
        delay(1000);
        acionaSaida(2,LIGA);
        delay(1000);
        acionaSaida(3,LIGA);
      }
    } else {
      // Se o modulo for SHUTTER (cortina) inverte as saidas - porque nao pode acionar as duas saidas ao mesmo tempo 
      // mesmo não existindo o rele do canal 3 abro o canal 
      acionaSaida(2,DESLIGA);
      acionaSaida(3,DESLIGA);
      delay(1000);
      acionaSaida(1,LIGA); 
      delay(1000);
    }
    delay(1000);           
    if (cTipo != LKSHUTTER) { 
      // 1.04 - Se for RGB e tem valor armazenado no vValorAux nao faz DESLIGA TODAS porque ele volta para a ultima cor
      if ((cTipo != LKRGB) || ((cTipo == LKRGB) && (vValorAux == "000000000"))) {
        acionaSaida(1,DESLIGA);
        delay(1000);
        acionaSaida(2,DESLIGA);
        delay(1000);
        acionaSaida(3,DESLIGA);
      }
    } else {
      acionaSaida(1,DESLIGA);
      acionaSaida(3,DESLIGA);
      delay(1000);
      acionaSaida(2,LIGA);
      delay(1000);
      acionaSaida(2,DESLIGA);
    }
  } else {
    testeD2E3();
  }
  digitalWrite(LED_1, DESLIGA); 
  digitalWrite(LED_2, DESLIGA);  
  
  vEstado = cInicio;
  
  // Limpa informacoes e cenas - usado para zerar um ESP já usado    
  //eraseCredentials(); // Forca a limpeza da area de armazenamento
  
  /*
  //vClientSSID = String(ssid);
  //vClientPWD  = String(password);  
  Serial.println ( " TENTANDO LIMPAR A MEMORIA " );
  delay(3000);
  vClientSSID = "ArnaldiNET";
  vClientPWD  = "arianeeduarda11";
  vConnectSSID_Ok = "OK";
  showConfig;
  Serial.println ( " TENTANDO LIMPAR A MEMORIA - saveCredentials" );
  delay(3000);
  saveCredentials(); // Forca a limpeza da area de armazenamento
  Serial.println ( " TENTANDO LIMPAR A MEMORIA - loadCredentials" );
  delay(3000);
  loadCredentials();
  delay(3000);
  showConfig();
  delay(5000);  
  vClientSSID = "ArnaldiNET";
  vClientPWD  = "arianeeduarda11";
  vConnectSSID_Ok = "OK";
  */
  
  /*
  //eraseCredentialsParcial(); // Forca a limpeza da area de armazenamento - MENOS SSID e PWD
  loadCredentials();
  showConfig();
  //
  int num = int(sizeof(scenes) / 6 / 4); // cada int ocupa 4 bytes e cada evento do cenaio ocupa 6 bytes
  for ( int i=0 ; i < num ; i++ ) {
    scenes[i].tipo = 0;
    scenes[i].id = 0;
    scenes[i].valor1 = 0;
    scenes[i].valor2 = 0;
    scenes[i].valor3 = 0;
    scenes[i].chk = 0;
  }
  saveScenes();
  */
}

void loop() {

  // MyTimer
  // Se vTimerCanalX = -1 o timer do canal esta parado
  if (vTimer >= 0) {
    vTimer += 1;
    // 1.08 
    //if ((vTimer % 95000) == 0) {
    if ((vTimer % 44000) == 0) {
      vSegundo += 1;
      //debug("Segundo :"); 
      //debug(String(vSegundo));
      //debug(" - vTimerCanal1 :"); 
      //debugln(String(vTimerCanal1));
      if (vTimerCanal1 == vSegundo) {
        acionaSaida(1,DESLIGA);
        debug("Para vTimer1 :"); 
        debugln(String(vTimerCanal1));
        vTimerCanal1 = -1;
        if (cTipo == LKSHUTTER) {
          // se for cortina, usa o terceiro STATUS para informa se a cortina esta aberto (000) ou fechado (100).
          // o canal 1 ssempre eh fechado
          vStatusShutter = 100;
          publicaTopicStatusMQTT();
        }
      }
      if (vTimerCanal2 == vSegundo) {
        acionaSaida(2,DESLIGA); 
        debug("Para vTimer2 :");
        debugln(String(vTimerCanal2));
        vTimerCanal2 = -1;
        if (cTipo == LKSHUTTER) {
          // se for cortina, usa o terceiro STATUS para informa se a cortina esta aberto (000) ou fechado (100).
          // o canal 2 ssempre eh aberto
          vStatusShutter = 000;
          publicaTopicStatusMQTT();
        }
      }
      if (vTimerCanal3 == vSegundo) {
        acionaSaida(3,DESLIGA); 
        debug("Para vTimer3 :");
        debugln(String(vTimerCanal3));
        vTimerCanal3 = -1;
      }
      // Se nao ha mais nenhum timer rodando, para o timer
      if ((vTimerCanal1 == -1) and (vTimerCanal2 == -1) and (vTimerCanal3 == -1)) { 
        vTimer = -1; 
        vSegundo = 0;
      }
      if (vSegundo == 100) { vSegundo = 0; }
    }
  }


  switch (vEstado) {
    case cInicio:
      msgln(" ");
      msgln(vMsgInicial);      
      //Serial.print("Define : ");
      //Serial.println(MQTT.showDefineTimeOut());
      //Serial.println(MQTT.showDefineKeepAlive());
      digitalWrite(LED_1, DESLIGA); 
      digitalWrite(LED_2, DESLIGA);            
      vConnected = false;
      vErro      = false;
      if (vClientSSID_OK) {
        // Se o nome da rede for encontrado desliga o AP, se houver erro na configuracao, usar o reset de fabrica
        WiFi.softAPdisconnect(true);
        vEstado = cMQTT; // Versao 1.11 - Antes de preparar o WebServer, configura e ativa o MQTT
      } else {vEstado = cAPConfig;}             
      break;
    case cAPConfig:      
      //if (WiFi.softAPdisconnect(false)) { msgln("cAPConfig - softAPdisconnect OK"); } else { msgln("cAPConfig - softAPdisconnect Not OK"); }
      WiFi.softAPdisconnect(false);
      delay(500);
      msg("Ligando o AP ... ");
      if (WiFi.softAPConfig(apIP, apIP, netMsk)) {
          if(WiFi.softAP(aSSID, softAP_password)) {      
            // 1.00 - Reducao do tempo de subida
            delay(150); // Without delay I've seen the IP address blank
            if (vMsg) {              
              Serial.println("\n");
              Serial.print("AP SSID ... : ");
              Serial.println(aSSID);         
              Serial.print("AP Password : ");
              Serial.println(softAP_password);            
              Serial.print("AP IP ..... : ");
              Serial.println(WiFi.softAPIP());
              Serial.println(" ");
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
      blinkLED(6,2);
      if (WiFi.softAPgetStationNum() > 0) {
        msgln ( "Client Connect" );                
        vEstado = cWebServer;                
        vServer.begin();
      }
      digitalWrite(LED_1, DESLIGA);
      digitalWrite(LED_2, DESLIGA);
      break;
    case cWebServer:
      //vClient = vServer.available();  
      if (!vClient) { vClient = vServer.available(); } // 3.39 - 2019-06-02
      // 0.07 - else desse if - quando o vClienteSSID_OK eh Ok vai direto para o cConnectToSSID
      if (vClientSSID_OK) {vEstado = cConnectToSSID;}  
      else {
        // Fica preparado para receber as informacoes via APP
        blinkLED(2,2);
        if ((vClient) and (vEstado != cWebSocketMenu)) {
          msgln("Client Socket Connected."); 
          msgln(" ");
          msgln("Receiving information from App : cWebServer ...");
          digitalWrite(LED_1,DESLIGA);     
          digitalWrite(LED_2,DESLIGA);  
          while ((vClient.connected()) and (vEstado != cWebSocketMenu)) {
            if (vClient.available()) {
              String line = vClient.readString();
              //
              debugln("cWebServer-------------------------- 2 ");
              debugln(line);
              debugln("cWebServer-------------------------- 3 ");
              //                         
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
              // Versao 0.14 - 2019 03 28 - Somente a rede e senha sao enviados ao LK               
              if (line.indexOf("[END]") > -1) {
                delay(500);                
                vClient.println("[OK]\n");
                msgln(" ");
                // Versao 0.13 - 2019 04 01 - Limpando as variaveis que nao sao mais usadas
                vKitID = " ";
                vKitIP = " ";
                vMCIP  = " ";
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
      if (!vClient) { vClient = vServer.available(); } // 3.39 - 2019-06-02
      blinkLED(2,2);
      if (vClient) {
        msgln(" ");
        msgln("WebServerMenu - Client Socket Connected."); 
        msgln("Receiving information from App : cWebSocketMenu ...");
        digitalWrite(LED_1,DESLIGA);
        digitalWrite(LED_2,DESLIGA);        
        while ((vClient.connected()) and (vEstado == cWebSocketMenu)) {
          if (vClient.available()) {
            String line = vClient.readString();
            //
            debugln("cWebServer-------------------------- 2 ");
            debugln(line);
            debugln("cWebServer-------------------------- 3 ");
            //
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
              // 1.02
              vClient.stop();
              vServer.stop();
              vEstado = cConnectToSSID;                            
            }
          }
        }
      }
      break;
    //
    case cConnectToSSID:
      ConnectToSSID();
      vEstado = cStatusSSID;
      break; 
    case cStatusSSID:
      if (!StatusSSID()) { vEstado = cErro; }
      break;
    case cConnectSSID_Ok:
      if (blinkLED(1,2)) { vContBlink += 1; }
      digitalWrite(LED_1,digitalRead(LED_2));
      // versao 1.00 - reduzir o tempo de link de 6 para 2
      if (vContBlink > 2) { 
        digitalWrite(LED_1,DESLIGA);
        digitalWrite(LED_2,DESLIGA);
        vContBlink = 0;
        if ((strlen(kitip) > 0) and (!vPulaFixaIP)) { 
          // Fixa IP - Versao 0.15 com MQTT o IP ficou como DHCP - o vPulaFixaIP fica TRUE antes de chegar 
          msgln(" ");
          msgln( "Fixando IP do LegoolusKit ...");
          msg( "IP Fixo ... : " );              
          String strIP = kitip;
          int Parts[4] = {0,0,0,0};
          int Part = 0;
          for ( int i=0; i<strIP.length(); i++ )
          {
            char c = strIP[i];
            if ( c == '.' )
            {
              Part++;
              continue;
            }
            Parts[Part] *= 10;
            Parts[Part] += c - '0';
          }
          wifiIP = IPAddress ( Parts[0], Parts[1], Parts[2], Parts[3] );
          wifiGW = IPAddress ( Parts[0], Parts[1], Parts[2], 1 );
          msgln ( String(wifiIP) );    
          msg   ( "Mask ...... : " );
          msgln ( String(wifiMK) );
          msg   ( "Gateway ... : " );
          msgln ( String(wifiGW) );                
          WiFi.config(wifiIP, wifiGW, wifiMK);
          vPulaFixaIP = true;
          vEstado = cStatusSSID;
        } else { 
          // 2018 04 02 - Informa que houve uma conexao com sucesso na rede WIFI, quando erro tenta conexao a cada 5 segundos
          // 2019 09 19 - Versao 0.08 - Trocando 0 e 1 por OK e NT
          vConnectSSID_Ok = "OK";
          saveCredentials();          
          vServer.begin();
          vMostraMsgTeclado = false;
          vEstado = cTeclado;
          // 1.02 - Se for RGB volta a ultima cor setada
          if (cTipo == LKRGB) {
            voltaUltimaCor();
          }
        }        
      }      
      break;
    case cSocket:
      msgln("Socket");      
      // versao 0.14 - 2019 03 28 - Nao conecta na MC se nao houver informacao do IP 
      if (vMCIP != " ") {
        if (!vMCCliente.connect(string2char(vMCIP), vMCPort)) {
          vSocketCntDown -= 1;
          if (!vSocketCntDown) {
            msgln("Socket connection failed .. is dead");                
            vSocketCntDown = 3;  
            // Nao vai para inicio para possibilitar o reset pela tecla 6
            vEstado = cErro;                      
          } else {
            msgln("Socket connection failed .. try again");
            delay(1000);
          }
        } else {
          msgln("Socket connection OK");
          vEstado = cNothing;      
          vSocketCntDown = 3;
          
          vModulo = vKitID.toInt();
          frame[0] = cHeader;
          frame[1] = vModulo;//cModulo;//0x03;
          frame[2] = vCanal;//0x06;
          frame[3] = 0x01; // Sempre eh 1 quando pulsador
          int f = 0;
          f = 0x100 - (frame[0] + frame[1] + frame[2] + frame[3]);
          while (f < 0) {
            f = 0x100 + f;
          } 
          frame[4] = f;
          aux = "Frame     : ";
          msg(aux);
          aux = "";
          aux.concat(String(frame[0], HEX));
          aux.concat(" ");
          aux.concat(String(frame[1], HEX));
          aux.concat(" ");
          aux.concat(String(frame[2], HEX));
          aux.concat(" ");
          aux.concat(String(frame[3], HEX));
          aux.concat(" ");
          aux.concat(String(frame[4], HEX));
          aux.toUpperCase();
          msgln(aux);
          aux = "";
          for(int k = 0; k < 5; k++){
            aux += char(frame[k]);
          }          
          
          vMCCliente.print(aux); // envia o frame para a MC        
          timeout = millis();
          while (vMCCliente.available() == 0) {
            if (millis() - timeout > 5000) {
              msgln("Client Timeout !");
              vMCCliente.stop();            
              vEstado = cTeclado;      
              return;
            }
          }
          vMCCliente.stop();
          vEstado = cDebounce;            
        }
        digitalWrite(LED_1, DESLIGA);
        digitalWrite(LED_2, DESLIGA);
      } else { vEstado = cTeclado; }
      break;      
    case cDebounce:
      debounce = millis();
      while (millis() - debounce < 500) {  
          // do nothing           
      }
      vEstado = cTeclado;      
      vMostraMsgTeclado = false;      
      break;
    case cTeclado:     
      //
      // Feedback atraves dos LEDs - durante o blink o processamento continua
      //
      if (vBlink_OK) {  
        if (vContBlink > 6) { 
          vContBlink = 0;
          vBlink_OK = false; 
          digitalWrite(LED_1,DESLIGA);
          digitalWrite(LED_2,DESLIGA);
        }
        if (blinkLED(1,2)) { vContBlink += 1; }
        digitalWrite(LED_1,digitalRead(LED_2));
      }
      if (vBlink_NotOK) {  
        if (vContBlink > 6) { 
          vContBlink = 0;
          vBlink_NotOK = false; 
          digitalWrite(LED_1,DESLIGA);
          digitalWrite(LED_2,DESLIGA);
        }
        if (blinkLED(1,2)) { vContBlink += 1; }
        digitalWrite(LED_1,!digitalRead(LED_2));
      }
      //
      // Socket
      //
      //vClient = vServer.available();
      if (!vClient) { vClient = vServer.available(); } // 3.39 - 2019-06-02
      // Se houver alguem conectado no Socket
      if (vClient) { 
        msgln ("Client Conectado"); 
        String line = vClient.readString();
            line.trim();
            if (line.indexOf("[ENVIASTATUS]") > -1) {              
              vClient.print("[SSID]");
              vClient.println(vClientSSID);
              vClient.print("[PWD]");              
              vClient.println(vClientPWD);                            
              vClient.println("[OK]\n");
            } else {           
              msg("RECEBEU via Socket : ");
              //
              aux = "";
              aux.concat(String(line[0], HEX));
              aux.concat(" ");
              aux.concat(String(line[1], HEX));
              aux.concat(" ");
              aux.concat(String(line[2], HEX));
              aux.concat(" ");
              aux.concat(String(line[3], HEX));
              aux.concat(" ");
              aux.concat(String(line[4], HEX));
              aux.toUpperCase();
              msgln(aux);
              //
              // 1.07 - Exclusao do tratamento de comandos via socket
              //
              /*
              // Versao 0.11 - Inclusao do LKD2E3
              if (cTipo != LKD2E3) {
                if (line[0] == 0xF5) {
                  switch (line[3]) {
                    case 0xF1:
                      // 1.07 - CORRECAO ERRO - BROACAST - comando broadcast quando dimeriza um modulo ou dimeriza todos os modulos o byte de dado eh F0 e o LK entedia que era comnado Desliga Todas e nao dimriza        
                      if ((payload[1] == 0xFA) && (payload[2] == 0x00)) { 
                        if (cTipo != LKSHUTTER) {
                          acionaSaida(1,LIGA);
                          acionaSaida(2,LIGA);
                          acionaSaida(3,LIGA);
                        }
                        msgln("Liga Todas");
                      }
                    break;
                    case 0xF0:
                      if ((payload[1] == 0xFA) && (payload[2] == 0x00)) {
                        if (cTipo != LKSHUTTER) {
                          acionaSaida(1,DESLIGA);
                          acionaSaida(2,DESLIGA);
                          acionaSaida(3,DESLIGA);
                        }
                        msgln("Desliga Todas");
                      }
                    break;
                    default:
                      analisaFrame(line[0],line[1],line[2],line[3],line[4]);   
                  }
                }
              } else {
                mandaFrameTX(line[0],line[2],line[3]);
              }
              // Versao 0.15 - Atualiza o Topico STATUS no MQTT
              //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
              // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
              if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
              */
              //
              //
              //
            }
      }
      //
      // Modo Reset de Fabrica
      //
      if (digitalRead(resetFabrica) == 0) {  
        // versao 0.18 - se nao consegue se conectar ao servidor MQTT - o reset de fabrica demora menos tempo para acontecer
        // versao 1.19 - DESABILITANDO - o vContPrg era igual 5 e provocava reset involuntario
        /*
        if ((!MQTT.connected()) && (!MQTTLocal.connected()) && (vContPrg > 5)) {
          msgln("Reset de Fabrica by MQTT not Connected");
          vContPrg = 2000000;
        }
        */
        //
        vContPrg += 1;           
        if (((vContPrg % 30000) == 0) && (vContPrg > 500000)) { // um tempinho antes de iniciar a piscar o LED a cada 30.000 ciclos
          //debugln(String(vContPrg));
          if (digitalRead(LED_2) == DESLIGA) { digitalWrite(LED_2, LIGA); } else { digitalWrite(LED_2, DESLIGA); }          
        }
        if (vContPrg > 2000000) { // Quando chega em 20s, sai do Modo Teclado e Habilita o AP
          msgln("Reset de Fabrica");
          digitalWrite(LED_2, DESLIGA);
          vEstado = cInicio;
          vPulaFixaIP = true;  // Versao 0.13 - 2019 04 01 - Trocando FALSE por TRUE - O Fixo do LK nao eh mais fixo
          vClientSSID_OK = false;
          eraseCredentials();
          // 0.07
          delay(500);
          loadCredentials();
          delay(500);
          showConfig();            
          delay(1000);
          // Versao 0.15 - Reseta o ESP fisicamente apos o reset de fabrica
          //ESP.restart(); // nao funciona
          ESP.reset();
        }
      } else {
        vContPrg = 0;   
        // 0.06 - 2008 08 23 - Quando a SSID nao for valida (por erro de digitacao ou troca de rede) e nao esta tentando fazer o reset, tenta conectar novamente 
        if (vSSIDInvalida) {
          vSSIDInvalida = false;
          vErro = true;
          vEstado = cInicio;
        }     
      }
      // Flag para mostrar a mensagem "Teclado" somente uma vez
      if (vMostraMsgTeclado == false) {
        vMostraMsgTeclado = true;
        msgln("Teclado ...");                
      }
      //
      // Controle de quanto tempo a tecla esta pressionada - usado para dimerizar o D2E3 - FALTA IMPLEMENTAR NO LedStrip e LedRGB
      //
      if (cTipo == LKD2E3) {
        if (vContTeclaP0 > 0) { 
          vContTeclaP0++; 
          if (vContTeclaP0 % 90000 == 0)  {
            debug("vContTeclaP0 : ");
            debugln(String(vContTeclaP0));            
            // quando chega nos 100% limita o contador para nao estourar
            if (vContTeclaP0 > 1000000) { 
              vContTeclaP0 == 1000000; 
            } else {
              incrementaCanalATMEL(1);
              vMaxValueChannel_1 = dimAtual1;  
              saveCredentials();
            }
          }
        }
        if (vContTeclaP2 > 0) { 
          vContTeclaP2++; 
          if (vContTeclaP2 % 90000 == 0)  {
            debug("vContTeclaP2 : ");
            debugln(String(vContTeclaP2));            
            // quando chega nos 100% limita o contador para nao estourar
            if (vContTeclaP2 > 1000000) { 
              vContTeclaP2 == 1000000; 
            } else {
              incrementaCanalATMEL(2);
              vMaxValueChannel_2 = dimAtual2;  
              saveCredentials();
            }
          }
        }
      }
      //
      //
      // Analisando se uma tecla foi pressionada
      //
      testaTecla();
      // 1.02 - So conecta no MQTT quando o WIFI esta conectado e houver Internet
      if (WiFi.status() == WL_CONNECTED) {
        // 1.03
        testaMQTT();        // Verifica se o MQTTLocal se esta conectado (se nao conecta) e executa o pull das assinatutas
        testaMQTTLocal();   // Verifica se o MQTTLocal existe, se esta conectado (se nao conecta) e executa o pull das assinatutas
        testaEnviaStatus(); // 1.00 - em vez de publicar o STATUS direto -  aguardo um tempo ate receber o ultima solicitacao (interna ou externa) de envio de STATUS
        //if (cTipo == LKD2E3) { testaBloqueiaInverteD2(); }// 1.06 - analisaFrameATMEL - quando recebe um comando de INVERSAO, quando o ATMEL nao terminar o fadein ou fadeout nao pode enviar outro comando INVERTE no mesmo canal porque o ATMEL nao aceita e o STATUS fica errado
        //        
      } else {        
        // 1.02 - So conecta no MQTT quando o WIFI esta conectado e houver Internet
        // Abaixo apenas Mensagem DEBUG 
        if (WiFi.status() != WL_CONNECTED) {
          if ((WIFI_Inter_Erro_Count % 100000) == 0) {
            //debug("WIFI_Inter_Erro_Count ...");
            debug(" w ");
          }
        }
        WIFI_Inter_Erro_Count += 1;
        if (WIFI_Inter_Erro_Count > 900000) { 
          WIFI_Inter_Erro_Count = 0; 
          debugln(" ");
        }
      }
      break;
    case cMQTT:   
      // a consiguracao do MQTT ficou para depois de conectar   
      vEstado = cWebServer;
      break;
    case cNothing:                      
      break;
    case cErro :
      vContReConnect += 1;      
      // Versao 0.08 - 2019 09 19 - Trocado o valor vConnectSSID_Ok de 0 e 1 por OK e NT
      if (vContReConnect > 500000) ( vContReConnect = 0 );
      // Versao 0.08 - 2019 09 19 - Trocado o valor vConnectSSID_Ok de 0 e 1 por OK e NT
      if ((vContReConnect == 500000) && (vConnectSSID_Ok == "OK")) {          
        vContReConnect = 0;
        debugln("Tenta conectar novamente ... ");
        vEstado = cInicio;
      }
      blinkLED(1,2);
      digitalWrite(LED_1,!digitalRead(LED_2));    
      break;    
  }
  yield(); // Passa o controle para outros processos
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
      // Versao 0.07 - Quando WL_CONNECT_FAILED - Se NUNCA onectou na rede WIFI apaga as credenciais ELSE vai para o teclado possibilitando o RESET de fabribra
      //if (vConnectSSID_Ok == 0) {
      // Versao 0.08 - 2019 09 19 - Trocado o valor vConnectSSID_Ok de 0 e 1 por OK e NT
      if (vConnectSSID_Ok == "NT") {
        debugln("se nunca se conectou nessa rede - apaga as credenciais .");
        // se nunca se conectou nessa rede - apaga as credenciais
        eraseCredentials();
      } else {
        // se conectou uma vez com sucesso - pode ser que o roteador esteja desligado ou mudou de rede 
        // 2018 08 23 - 0.06 - Vai para o teclado para possibilitar o reset de frabrica
        vSSIDInvalida = true; // vai para teclado, ve se esta em reset de fabrica, se nao tenta novamente a conexao
        vMostraMsgTeclado = false; // para aparecer a palavra TECLADO
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
        Serial.println ( vKitID );
        Serial.print ( "Kit IP Fixo .... : " );
        Serial.println ( vKitIP );
        Serial.print ( "MC IP .......... : " );
        Serial.println ( vMCIP );
      }
      //
      wifiIP = WiFi.localIP();
      wifiMK = WiFi.subnetMask();
      wifiGW = WiFi.gatewayIP(); 
      //
      digitalWrite(LED_1, DESLIGA);
      digitalWrite(LED_2, DESLIGA);  
      vEstado = cConnectSSID_Ok;
      break;
    case WL_NO_SSID_AVAIL:
      msgln ("SSID Invalida.");
      WiFi.disconnect();
      // Versao 0.07 - Quando SSID Invalida - Se NUNCA conectou na rede WIFI apaga as credenciais ELSE vai para o teclado possibilitando o RESET de fabribra
      //if (vConnectSSID_Ok == 0) {
      // Versao 0.08 - 2019 09 19 - Trocado o valor vConnectSSID_Ok de 0 e 1 por OK e NT
      if (vConnectSSID_Ok == "NT") {
        debugln("se nunca se conectou nessa rede - apaga as credenciais .");
        // se nunca se conectou nessa rede - apaga as credenciais
        eraseCredentials();
      } else {
        // se conectou uma vez com sucesso - pode ser que o roteador esteja desligado ou mudou de rede 
        vSSIDInvalida = true; // vai para teclado, ve se esta em reset de fabrica, se nao tenta novamente a conexao
        vMostraMsgTeclado = false; // para aparecer a palavra TECLADO
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

void loadCredentials() {
  debugln("LoadCredencial   :");
  EEPROM.begin(2048);
  EEPROM.get(0, ssid);  
  EEPROM.get(0+sizeof(ssid), password);
  EEPROM.get(0+sizeof(ssid)+sizeof(password), kitid);
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kitid), kitip);
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip), mcip);
  // 2018 04 02
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip), connectSSID_Ok);
  // versao 0.11
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok), mcid);
  // versao 0.15
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid), vMaxValueChannel_1);
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid)+sizeof(vMaxValueChannel_1), vMaxValueChannel_2);
  // versao 0.15 - Token do App do celular usado para comandos broadcast via pulsador sem MC
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid)+sizeof(vMaxValueChannel_1)+sizeof(vMaxValueChannel_2), apptoken);
  // versao 0.18 - Tempo de abertura e fechamento da cortina - somente no LKShutter
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid)+sizeof(vMaxValueChannel_1)+sizeof(vMaxValueChannel_2)+sizeof(apptoken), valoraux);
  EEPROM.end();
  
  vMCIP       = String(mcip);
  vKitIP      = String(kitip);  
  vKitID      = String(kitid);
  vClientSSID = String(ssid);
  vClientPWD  = String(password);  

  // 2018 04 02
  vConnectSSID_Ok = String(connectSSID_Ok);  
  // versao 0.11 - ler e trabalhar com o ID da MC
  vMCID       = String(mcid); 
  // versao 0.15
  appToken = String(apptoken);
  // versao 1.02 - armazenamento alem do tempo de abertura e fechamento da cortina - agora armazena os valores dos canais 
  vValorAux = String(valoraux); // 1.04 - Antigo vTempoShutter
  /*
  debug("loadCredentials : temposhutter : ");
  debug(temposhutter);
  debug(" : vValorAux : ");
  debug(vTempoShutter);
  */
  // Informa se existe ID da MC e consequentemente se existe MC
  if ((vMCID.toInt() > 0) and (vMCID.toInt() < 99999)) { vMCID_OK = true; } else { vMCID_OK = false; }
  
  // Versao 0.10 - qdo o campo vConnectSSID_Ok = NT entao vClientSSID_OK = false estava ao contrario, sua ANTA
  if (((vClientSSID[0] == 0xFF) and (vClientSSID[1] == 0xFF) and (vClientSSID[2] == 0xFF) and (vClientSSID[3] == 0xFF)) or (vConnectSSID_Ok != "OK")) { 
    debugln("Sugeira na memoria ou SSID_NOT OK");
    eraseCredentials();
    vClientSSID_OK = false; 
    ssid[0] = 0;
    vClientSSID = "";
  } else { 
    vClientSSID_OK = true;
  }


  //vClientPWD = "arianeeduarda11";
  //saveCredentials();

}

void saveCredentials() {
  vClientSSID.toCharArray(ssid, sizeof(ssid) - 1);
  vClientPWD.toCharArray(password, sizeof(password) - 1);
  vKitID.toCharArray(kitid, sizeof(kitid) - 1);
  vKitIP.toCharArray(kitip, sizeof(kitip) - 1);
  vMCIP.toCharArray(mcip, sizeof(mcip) - 1);
  vMCID.toCharArray(mcid, sizeof(mcid) - 1); // versao 0.11
  // 2018 04 02
  vConnectSSID_Ok.toCharArray(connectSSID_Ok, sizeof(connectSSID_Ok) - 1);
  // versao 0.15 - armazenando o ultimo valor dimerizado atraves do pulsador - esse valor eh usado no ON como limite
  // vMaxValueChannel_1 e vMaxValueChannel_2
  appToken.toCharArray(apptoken, sizeof(apptoken) - 1); // versao 0.15
  // versao 0.18 - vTempoShutter - tempo de abertura e fechamento da cortina  
  vValorAux.toCharArray(valoraux, sizeof(valoraux) - 1); // versao 1.02 - versao 1.04 - Antigo vTempoShutter
  /*
  debug("saveCredentials : temposhutter : ");
  debug(temposhutter);
  debug(" : vTempoShutter : ");
  debug(vTempoShutter);
  */
  //EEPROM.begin(512);  
  EEPROM.begin(2048);  
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  EEPROM.put(0+sizeof(ssid)+sizeof(password), kitid);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid), kitip);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip), mcip);
  // 2018 04 02
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip), connectSSID_Ok);
  // versao 0.11 - ID da MC
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok), mcid); // versao 0.11
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid), vMaxValueChannel_1); // versao 0.15
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid)+sizeof(vMaxValueChannel_1), vMaxValueChannel_2); // versao 0.15
  //
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid)+sizeof(vMaxValueChannel_1)+sizeof(vMaxValueChannel_2), apptoken); // versao 0.15
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid)+sizeof(vMaxValueChannel_1)+sizeof(vMaxValueChannel_2)+sizeof(apptoken), valoraux); // versao 0.18
  //
  EEPROM.commit();
  EEPROM.end();  
}

void loadScenes() {
  int addr = 1024;     // endereço inicial
  int size_mem = 2048; // tamanho da memoria comecando no endereco ZERO  
  debugln("loadScenes");   
  int num = int(sizeof(scenes) / 6 / 4); // cada int ocupa 4 bytes e cada evento do cenaio ocupa 6 bytes
  for ( int i=0 ; i < num ; i++ ) {
    scenes[i].tipo = 0;
    scenes[i].id = 0;
    scenes[i].valor1 = 0;
    scenes[i].valor2 = 0;
    scenes[i].valor3 = 0;
    scenes[i].chk = 0;
  }
  int chk;
  EEPROM.begin(size_mem);  // tamanho da memoria
  EEPROM.get(addr, scenes);  
  vIsScene = false;
  vAllON   = false;
  vAllOFF  = false; 
  for ( int i=0 ; i < num ; i++ ) {
    chk = calcChkSumScene(i);
    if ((chk == scenes[i].chk) && (chk != 0)) { 
      vIsScene = true; 
      debug("Sequencia ");
      debug(String(i));
      debug(" : ");
      debug(" ");
      debug(String(scenes[i].tipo));
      debug(" ");
      debug(String(scenes[i].id));
      debug(" ");
      debug(String(scenes[i].valor1));
      debug(" ");
      debug(String(scenes[i].valor2));
      debug(" ");
      debug(String(scenes[i].valor3));
      debug(" ");
      debug(String(scenes[i].chk));      
      debugln(" ");
    }
  }
  if (vIsScene) { 
    msgln("Ha Cenario"); 
  } else {         
    if (scenes[0].tipo == 998) {
      msgln("Desliga Todas");
      vAllOFF = true;
    } else {
      if (scenes[0].tipo == 997) {
        msgln("Liga Todas"); 
        vAllON =  true;
      } else {
        msgln("Nao Ha Cenarios"); 
      }
    }
    if ((vAllOFF) || (vAllON)) {
      int i = 0;
      debug("Sequencia ");
      debug(String(i));
      debug(" : ");
      debug(" ");
      debug(String(scenes[i].tipo));
      debug(" ");
      debug(String(scenes[i].id));
      debug(" ");
      debug(String(scenes[i].valor1));
      debug(" ");
      debug(String(scenes[i].valor2));
      debug(" ");
      debug(String(scenes[i].valor3));
      debug(" ");
      debug(String(scenes[i].chk));      
      debugln(" ");
    }
  }
  
}

void saveScenes() {
  int addr = 1024;     // endereço inicial
  int size_mem = 2048; // tamanho da memoria comecando no endereco ZERO
  msgln(" ");
  Serial.println("saveScenes");
  //int  num = 2123456789; // valor maximo de um INT no ESP8266 = 2.147.483.647

  if ((scenes[0].tipo == 998) || (scenes[0].tipo == 997)) {    
    debugln("LIGA OU DESLIGA TODAS");
  }
  
  int num = int(sizeof(scenes) / 6 / 4); // cada int ocupa 4 bytes e cada evento do cenaio ocupa 6 bytes
  for ( int i=0 ; i < num ; i++ ) {
    if ((scenes[i].chk != calcChkSumScene(i)) && (scenes[i].tipo != 998) && (scenes[i].tipo != 997)) {    
      // se o checksum nao bate e se nao for Liga Todas e Desliga Todas preenche com zeros      
      scenes[i].tipo = 0;
      scenes[i].id = 0;
      scenes[i].valor1 = 0;
      scenes[i].valor2 = 0;
      scenes[i].valor3 = 0;
      scenes[i].chk = 0;
    }
  }
  
  EEPROM.begin(size_mem); 
  EEPROM.put(addr, scenes);    
  EEPROM.commit();
  EEPROM.end();
}

void zeroAndSaveScenes() {
  int num = int(sizeof(scenes) / 6 / 4); // cada int ocupa 4 bytes e cada evento do cenaio ocupa 6 bytes
  for ( int i=0 ; i < num ; i++ ) {
    scenes[i].tipo = 0;
    scenes[i].id = 0;
    scenes[i].valor1 = 0;
    scenes[i].valor2 = 0;
    scenes[i].valor3 = 0;
    scenes[i].chk = 0;
  }
  saveScenes();
  loadScenes();
}

int calcChkSumScene(int pIndex) {   
  scene_type scene_aux;
  scene_aux = scenes[pIndex];
  return scene_aux.tipo + scene_aux.id + scene_aux.valor1 + scene_aux.valor2 + scene_aux.valor3;
}

void eraseCredentials() {
  msgln("eraseCredentials");
  ssid[0] = 0;
  password[0] = 0;  
  kitid[0] = 0;
  kitip[0] = 0;
  mcip[0] = 0;
  mcid[0] = 0; // versao 0.11
  vMaxValueChannel_1 = 0; // versao 0.15
  vMaxValueChannel_2 = 0; // versao 0.15  
  // 2018 11 08 
  vConnectSSID_Ok = "NT";
  vConnectSSID_Ok.toCharArray(connectSSID_Ok, sizeof(connectSSID_Ok) - 1);  
  // 1.08
  apptoken[0] = 0;
  valoraux[0] = 0;
  
  EEPROM.begin(2048);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  EEPROM.put(0+sizeof(ssid)+sizeof(password), kitid);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid), kitip);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip), mcip);
  // 2018 04 02
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip), connectSSID_Ok);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok), mcid); // versao 0.11
  // versao 0.15
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid)+sizeof(vMaxValueChannel_1), vMaxValueChannel_2); 
  // 1.08
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid)+sizeof(vMaxValueChannel_1)+sizeof(vMaxValueChannel_2), apptoken); // versao 0.15
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok)+sizeof(mcid)+sizeof(vMaxValueChannel_1)+sizeof(vMaxValueChannel_2)+sizeof(apptoken), valoraux); // versao 0.18
  
  EEPROM.commit();
  EEPROM.end();
}

void eraseCredentialsParcial() {
  vClientSSID.toCharArray(ssid, sizeof(ssid) - 1);
  vClientPWD.toCharArray(password, sizeof(password) - 1);  
  vConnectSSID_Ok.toCharArray(connectSSID_Ok, sizeof(connectSSID_Ok) - 1);  
  //
  kitid[0] = 0;
  kitip[0] = 0;
  mcip[0] = 0;
  mcid[0] = 0; // versao 0.11
  //
  //EEPROM.begin(512);
  EEPROM.begin(2048);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  EEPROM.put(0+sizeof(ssid)+sizeof(password), kitid);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid), kitip);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip), mcip);
  // 2018 04 02
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip), connectSSID_Ok);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(kitid)+sizeof(kitip)+sizeof(mcip)+sizeof(connectSSID_Ok), mcid); // versao 0.11
  EEPROM.commit();
  EEPROM.end();
}


boolean blinkLED(int pInterval, int pLED) {
  interval = pInterval * 500;
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (pLED == 2) {
      if (digitalRead(LED_2) == DESLIGA) { digitalWrite(LED_2, LIGA); } else { digitalWrite(LED_2, DESLIGA); }
      return true;
    } else {
      if (digitalRead(LED_1) == DESLIGA) { digitalWrite(LED_1, LIGA); } else { digitalWrite(LED_1, DESLIGA); }
      return true;
    }
  } else { return false; }
}

void nivel () {
  if (digitalRead(gpio0_pin)  == 1) { estado_p0 = HIGH; }  else { estado_p0 = LOW;}
  if (digitalRead(gpio2_pin)  == 1) { estado_p2 = HIGH; }  else { estado_p2 = LOW;}
  if (digitalRead(gpio4_pin)  == 1) { estado_p4 = HIGH; }  else { estado_p4 = LOW;} 
  // versao 1.04 - Exclusao suporte LK ER6
  /*
  if (cTipo == LKER6) {
    if (digitalRead(gpio5_pin)   == 1) { estado_p5  = HIGH; }  else { estado_p5 = LOW;} 
    if (digitalRead(gpio12_pin)  == 1) { estado_p12 = HIGH; }  else { estado_p12 = LOW;} 
    if (digitalRead(gpio14_pin)  == 1) { estado_p14 = HIGH; }  else { estado_p14 = LOW;} 
  }
  */
  // 0.07 - Possibilita o uso de 4 entradas - Feito para o cliente Roger
  
  haschanged = 0;
  // versao 1.04 - Exclusao suporte LK ER6
  //if (ep0 != estado_p0 || ep2 != estado_p2 || ep4 != estado_p4 || ep5 != estado_p5 || ep12 != estado_p12 || ep14 != estado_p14) { haschanged = 1; }
  if (ep0 != estado_p0 || ep2 != estado_p2 || ep4 != estado_p4) { haschanged = 1; }
}

char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        //Serial.println(p);
        return p;
    } 
}

void showConfig() {  
  // 1.08
  vTempoShutter = vValorAux.toInt();
  if (vMsg) {    
    //Serial.println ( " " );
    Serial.print ( "vClient SSID ... : " );
    Serial.println ( vClientSSID );
    Serial.print ( "vClient PWD..... : " );
    Serial.println ( vClientPWD );  
    Serial.print ( "Kit ID ......... : " );
    Serial.println ( vKitID );
    Serial.print ( "Kit IP Fixo .... : " );
    Serial.println ( vKitIP );
    Serial.print ( "MC IP .......... : " );
    Serial.println ( vMCIP );
    // versao 0.11
    Serial.print ( "MC ID .......... : " );
    Serial.println ( vMCID );
    // versao 0.15
    Serial.print ( "Valor Maximo 1.. : " );
    Serial.println ( vMaxValueChannel_1 );
    Serial.print ( "Valor Maximo 2.. : " );
    Serial.println ( vMaxValueChannel_2 );
    //
    Serial.print ( "Connec. SSID OK  : " );
    Serial.println ( vConnectSSID_Ok );
    Serial.print ( "AppToken         : ");
    Serial.println( appToken );
    Serial.print ( "vValorAux        : ");
    Serial.println( vValorAux );
    Serial.print ( "TempoShutter     : ");
    Serial.println( vTempoShutter );
  }
}

void analisaFrame(char byte1, char byte2, char byte3, char byte4, char byte5) {
  // Verificando o canal  
  /*
  Serial.println("--- Analisa Frame ---");
  Serial.println(String(byte1, HEX));
  Serial.println(String(byte2, HEX));
  Serial.println(String(byte3, HEX));
  Serial.println(String(byte4, HEX));
  Serial.println("---");
  */
  switch (byte3) {
    // Liga e Desliga
    case 0x00:
      if (byte4 == 0x00) { 
        desligaCanal(1); 
      } else { 
        if (cTipo == LKSHUTTER) {
          // no Shutter sempre antes de acionar um relé ou outros são abertos
          desligaCanal(2); 
          desligaCanal(3); 
          delay(300);                
          // 1.08 - no lugar de acionar a cortina tem que temporizar 
          if (vTempoShutter == 0 || vTempoShutter > 300) { 
            vTempoShutter = cTempoShutter; // 1.10
          }
          temporizaCanal(1,vTempoShutter * 9); // canal um SEMPRE eh ABRIR
        } else {
          ligaCanal(1); 
        }
      }
    break;
    case 0x01:
      if (byte4 == 0x00) { 
        desligaCanal(2); 
      } else { 
        if (cTipo == LKSHUTTER) {
          desligaCanal(1); 
          desligaCanal(3); 
          delay(300);        
          // 1.08 - no lugar de acionar a cortina tem que temporizar
          if (vTempoShutter == 0 || vTempoShutter > 300) { 
            vTempoShutter = cTempoShutter; // 1.10
          }
          temporizaCanal(2,vTempoShutter * 9); // canal dois SEMPRE eh FECHAR          
        } else {
          ligaCanal(2); 
        }
      }
    break;
    case 0x02:
      if (byte4 == 0x00) { 
        desligaCanal(3); 
      } else { 
        if (cTipo == LKSHUTTER) {
          desligaCanal(1); 
          desligaCanal(2); 
          delay(300);                
        } else {
          ligaCanal(3); 
        }
      }
      //Serial.println ("ON/OFF - canal nao existe");
    break;
    case 0x04:
      Serial.println ("ON/OFF - canal nao existe");
    break;
    case 0x08:
      if (cTipo == LKSHUTTER) {
          desligaCanal(2); 
          desligaCanal(3); 
          delay(300);                
      }
      inverteCanal(1);
    break;
    case 0x09:
      if (cTipo == LKSHUTTER) {
          desligaCanal(1); 
          desligaCanal(3); 
          delay(300);                
      }
      inverteCanal(2); 
    break;
    case 0x0A:
      if (cTipo == LKSHUTTER) {
          desligaCanal(1); 
          desligaCanal(2); 
          delay(300);                
      }
      inverteCanal(3); 
      //Serial.println ("inverte - canal nao existe");
    break;
    case 0x0B:
      Serial.println ("inverte - canal nao existe");
    break;
    case 0x58: 
      if (byte4 == 0x00) {
        if ((cTipo != LKR3E3) and (cTipo != LKSHUTTER)) decrementaCanal(1);
      } else {
        if ((cTipo != LKR3E3) and (cTipo != LKSHUTTER)) dimerizaCanal(1,byte4);
      }
    break;
    case 0x59: // Canal 2
      if (byte4 == 0x00) {
        if ((cTipo != LKR3E3) and (cTipo != LKSHUTTER)) decrementaCanal(2);
      } else {
        if ((cTipo != LKR3E3) and (cTipo != LKSHUTTER)) dimerizaCanal(2,byte4);
      }
    break;
    case 0x5A:      
      if (byte4 == 0x00) {
        if ((cTipo != LKR3E3) and (cTipo != LKSHUTTER)) decrementaCanal(3);
      } else {
        if ((cTipo != LKR3E3) and (cTipo != LKSHUTTER)) dimerizaCanal(3,byte4);
      }
      //Serial.println ("decrementa ou dimeriza - canal nao existe");
    break;
    case 0x5B:      
      Serial.println ("decrementa ou dimeriza - canal nao existe");
    break;
    case 0x5C: 
      if ((cTipo != LKR3E3) and (cTipo != LKSHUTTER)) incrementaCanal(1);
    break;
    case 0x5D:
      if ((cTipo != LKR3E3) and (cTipo != LKSHUTTER)) incrementaCanal(2);
    break;
    case 0x5E:
      if ((cTipo != LKR3E3) and (cTipo != LKSHUTTER)) incrementaCanal(3);
      //Serial.println ("incrementa - canal nao existe");
    break;
    case 0x5F:
      Serial.println ("incrementa - canal nao existe");
    break;
    case 0x40:
      //Serial.println ("Temporiza Canal 1");
      temporizaCanal(1,byte4);
    break;
    case 0x41:
      //Serial.println ("Temporiza Canal 2");
      temporizaCanal(2,byte4);
    break;
    case 0x42:
      //Serial.println ("Temporiza Canal 3");
      temporizaCanal(3,byte4);
    break;
    default:
      Serial.println ("erro no canal");      
  }
} 

void decrementaCanal(int pCanal) {
  Serial.print("decrementaCanal ");
  Serial.print(pCanal);
  Serial.print(" - ");
  int dimAtual = 0;
  int canalAtual = 1;
  if (pCanal == 1) { 
    dimAtual = dimAtual1; 
  } else {
    if (pCanal == 2) { 
      dimAtual = dimAtual2; 
    } else {
      dimAtual = dimAtual3; 
    }
  }
  switch (dimAtual) {
    case DIM0:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM0); } else { analogWrite(SAIDA_2,DIM0); }
      dimerizandoCanal(pCanal, DIM0);
      dimAtual = DIM0;
      canalAtual = 0;
    break;
    case DIM5:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM0); } else { analogWrite(SAIDA_2,DIM0); }
      dimerizandoCanal(pCanal, DIM0);
      dimAtual = DIM0;
      canalAtual = 0;
    break;
    case DIM10:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM5); } else { analogWrite(SAIDA_2,DIM5); }
      dimerizandoCanal(pCanal, DIM5);
      dimAtual = DIM5;
    break;
    case DIM20:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM10); } else { analogWrite(SAIDA_2,DIM10); }
      dimerizandoCanal(pCanal, DIM10);
      dimAtual = DIM10;
    break;
    case DIM30:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM20); } else { analogWrite(SAIDA_2,DIM20); }
      dimerizandoCanal(pCanal, DIM20);
      dimAtual = DIM20;
    break;
    case DIM40:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM30); } else { analogWrite(SAIDA_2,DIM30); }
      dimerizandoCanal(pCanal, DIM30);
      dimAtual = DIM30;
    break;
    case DIM50:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM40); } else { analogWrite(SAIDA_2,DIM40); }
      dimerizandoCanal(pCanal, DIM40);
      dimAtual = DIM40;
    break;
    case DIM60:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM50); } else { analogWrite(SAIDA_2,DIM50); }
      dimerizandoCanal(pCanal, DIM50);
      dimAtual = DIM50;
    break;
    case DIM70:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM60); } else { analogWrite(SAIDA_2,DIM60); }
      dimerizandoCanal(pCanal, DIM60);
      dimAtual = DIM60;
    break;
    case DIM80:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM70); } else { analogWrite(SAIDA_2,DIM70); }
      dimerizandoCanal(pCanal, DIM70);
      dimAtual = DIM70;
    break;
    case DIM90:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM80); } else { analogWrite(SAIDA_2,DIM80); }
      dimerizandoCanal(pCanal, DIM80);
      dimAtual = DIM80;
    break;
    case DIM100:
      //if (pCanal == 1) { analogWrite(SAIDA_1,DIM90); } else { analogWrite(SAIDA_2,DIM90); }
      dimerizandoCanal(pCanal, DIM90);
      dimAtual = DIM90;
    break;
    default:
      Serial.println ("erro no incrementaCanal");   
  }
  if (pCanal == 1) { 
    dimAtual1 = dimAtual; 
    canalAtual1 = canalAtual;
  } else { 
    if (pCanal == 2) {
      dimAtual2 = dimAtual; 
      canalAtual2 = canalAtual;
    } else {
      dimAtual3 = dimAtual;
      canalAtual3 = canalAtual;
    }
  }
  //Serial.print(" - ");   
  //Serial.println(dimAtual);  
  //if (MQTT.connected()) { publicaTopicStatusMQTT(); } 
  // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
  if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
}

void dimerizandoCanal(int pCanal, int pFator) {  
  // dimeriza usando fator - tipo DIM10, DIM20 ...
  if (pCanal == 1) { 
    analogWrite(SAIDA_1,pFator); 
    Serial.print("dimerizandoCanal 1 : ");
    Serial.println(pFator);    
  } else { 
    if (pCanal == 2) { 
      analogWrite(SAIDA_2,pFator); 
      Serial.print("dimerizandoCanal 2 : ");
      Serial.println(pFator);
    } else {
      analogWrite(SAIDA_3,pFator); 
      Serial.print("dimerizandoCanal 3 : ");
      Serial.println(pFator);
    }
  }
}

void incrementaCanal(int pCanal) {
  Serial.print("incrementaCanal ");
  Serial.print(pCanal);
  Serial.print(" - ");
  int dimAtual = 0;
  int canalAtual = 1;
  if (pCanal == 1) { 
    dimAtual = dimAtual1; 
  } else { 
    if (pCanal == 2) {
      dimAtual = dimAtual2; 
    } else {
      dimAtual = dimAtual3;
    }
  }    
  switch (dimAtual) {
    case DIM0:
      //if (pCanal == 1) { analogWrite(LED_1,DIM5); } else { analogWrite(LED_2,DIM5); }
      dimerizandoCanal(pCanal, DIM5);
      dimAtual = DIM5;
    break;
    case DIM5:
      //if (pCanal == 1) { analogWrite(LED_1,DIM10); } else { analogWrite(LED_2,DIM10); }
      dimerizandoCanal(pCanal, DIM10);
      dimAtual = DIM10;
    break;
    case DIM10:
      //if (pCanal == 1) { analogWrite(LED_1,DIM20); } else { analogWrite(LED_2,DIM20); }
      dimerizandoCanal(pCanal, DIM20);
      dimAtual = DIM20;
    break;
    case DIM20:
      //if (pCanal == 1) { analogWrite(LED_1,DIM30); } else { analogWrite(LED_2,DIM30); }
      dimerizandoCanal(pCanal, DIM30);
      dimAtual = DIM30;
    break;
    case DIM30:
      //if (pCanal == 1) { analogWrite(LED_1,DIM40); } else { analogWrite(LED_2,DIM40); }
      dimerizandoCanal(pCanal, DIM40);
      dimAtual = DIM40;
    break;
    case DIM40:
      //if (pCanal == 1) { analogWrite(LED_1,DIM50); } else { analogWrite(LED_2,DIM50); }
      dimerizandoCanal(pCanal, DIM50);
      dimAtual = DIM50;
    break;
    case DIM50:
      //if (pCanal == 1) { analogWrite(LED_1,DIM60); } else { analogWrite(LED_2,DIM60); }
      dimerizandoCanal(pCanal, DIM60);
      dimAtual = DIM60;
    break;
    case DIM60:
      //if (pCanal == 1) { analogWrite(LED_1,DIM70); } else { analogWrite(LED_2,DIM70); }
      dimerizandoCanal(pCanal, DIM70);
      dimAtual = DIM70;
    break;
    case DIM70:
      //if (pCanal == 1) { analogWrite(LED_1,DIM80); } else { analogWrite(LED_2,DIM80); }
      dimerizandoCanal(pCanal, DIM80);
      dimAtual = DIM80;
    break;
    case DIM80:
      //if (pCanal == 1) { analogWrite(LED_1,DIM90); } else { analogWrite(LED_2,DIM90); }
      dimerizandoCanal(pCanal, DIM90);
      dimAtual = DIM90;
    break;
    case DIM90:
      //if (pCanal == 1) { analogWrite(LED_1,DIM100); } else { analogWrite(LED_2,DIM100); }
      dimerizandoCanal(pCanal, DIM100);
      dimAtual = DIM100;
    break;
    case DIM100:
      //if (pCanal == 1) { analogWrite(LED_1,DIM100); } else { analogWrite(LED_2,DIM100); }
      dimerizandoCanal(pCanal, DIM100);
      dimAtual = DIM100;
    break;
    default:
      Serial.println("erro no incrementaCanal");   
  }
  if (pCanal == 1) { 
    dimAtual1 = dimAtual; 
    canalAtual1 = canalAtual;
  } else { 
    if (pCanal == 2) {
      dimAtual2 = dimAtual; 
      canalAtual2 = canalAtual;
    } else {
      dimAtual3 = dimAtual;
      canalAtual3 = canalAtual;
    }
  }
  Serial.print(" - ");   
  Serial.println(dimAtual);
  //if (MQTT.connected()) { publicaTopicStatusMQTT(); }   
  // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
  if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
}

void dimerizaCanal(int pCanal, int pByte3) {
  // Dimeriza o canal usando o Byte recebido
  Serial.print("dimerizaCanal ");
  Serial.print(pCanal);
  Serial.print(" : ");
  Serial.println(pByte3);
  int dimAtual = 0;
  int canalAtual = 1;
  switch (pByte3) {
    case 0x01: // 100%
      //if (pCanal == 1) { analogWrite(LED_1,DIM100); } else { analogWrite(LED_2,DIM100); }
      dimerizandoCanal(pCanal, DIM100);
      dimAtual = DIM100;
      Serial.println(" 100%");
    break;
    case 0x02: // 90%
      //if (pCanal == 1) { analogWrite(LED_1,DIM90); } else { analogWrite(LED_2,DIM90); }
      dimerizandoCanal(pCanal, DIM90);
      dimAtual = DIM90;
      Serial.println(" 90%");
    break;
    case 0x03: // 80%
      //if (pCanal == 1) { analogWrite(LED_1,DIM80); } else { analogWrite(LED_2,DIM80); }
      dimerizandoCanal(pCanal, DIM80);
      dimAtual = DIM80;
      Serial.println(" 80%");
    break;
    case 0x04: // 70%
      //if (pCanal == 1) { analogWrite(LED_1,DIM70); } else { analogWrite(LED_2,DIM70); }
      dimerizandoCanal(pCanal, DIM70);
      dimAtual = DIM70;
      Serial.println(" 70%");
    break;
    case 0x05: // 60%
      //if (pCanal == 1) { analogWrite(LED_1,DIM60); } else { analogWrite(LED_2,DIM60); }
      dimerizandoCanal(pCanal, DIM60);
      dimAtual = DIM60;
      Serial.println(" 60%");
    break;
    case 0x06: // 50%
      //if (pCanal == 1) { analogWrite(LED_1,DIM50); } else { analogWrite(LED_2,DIM50); }
      dimerizandoCanal(pCanal, DIM50);
      dimAtual = DIM50;
      Serial.println(" 50%");
    break;
    case 0x07: // 40%
      //if (pCanal == 1) { analogWrite(LED_1,DIM40); } else { analogWrite(LED_2,DIM40); }
      dimerizandoCanal(pCanal, DIM40);
      dimAtual = DIM40;
      Serial.println(" 40%");
    break;
    case 0x08: // 30%
      //if (pCanal == 1) { analogWrite(LED_1,DIM30); } else { analogWrite(LED_2,DIM30); }
      dimerizandoCanal(pCanal, DIM30);
      dimAtual = DIM30;
      Serial.println(" 30%");
    break;
    case 0x09: // 20%
      //if (pCanal == 1) { analogWrite(LED_1,DIM20); } else { analogWrite(LED_2,DIM20); }
      dimerizandoCanal(pCanal, DIM20);
      dimAtual = DIM20;
      Serial.println(" 20%");
    break;
    case 0x0A: // 10%
      //if (pCanal == 1) { analogWrite(LED_1,DIM10); } else { analogWrite(LED_2,DIM10); }
      dimerizandoCanal(pCanal, DIM10);
      dimAtual = DIM10;
      Serial.println(" 10%");
    break;
    case 0x0B: // 5%
      //if (pCanal == 1) { analogWrite(LED_1,DIM5); } else { analogWrite(LED_2,DIM5); }
      dimerizandoCanal(pCanal, DIM5);
      dimAtual = DIM5;
      Serial.println(" 5%");
    break;
    case 0x0C: // 0%
      //if (pCanal == 1) { analogWrite(LED_1,DIM0); } else { analogWrite(LED_2,DIM0); }
      dimerizandoCanal(pCanal, DIM0);
      dimAtual = DIM0;
      canalAtual = 0;
      Serial.println(" 0%");
    break;
    default:
      Serial.println ("erro no dimerizaCanal");   
  }
  if (pCanal == 1) { 
    dimAtual1 = dimAtual; 
    canalAtual1 = canalAtual;
  } else { 
    if (pCanal == 2) {
      dimAtual2 = dimAtual; 
      canalAtual2 = canalAtual;
    } else {
      dimAtual3 = dimAtual;
      canalAtual3 = canalAtual;
    }
  }
  //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
  // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
  if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
}

void desligaCanal(int pCanal) {
  Serial.print("desligaCanal ");
  Serial.println(pCanal);
  if (pCanal == 1) {
    acionaSaida(1,DESLIGA);     
    dimAtual1 = DIM0;
    canalAtual1 = 0;
  } else {
    if (pCanal == 2) {
      acionaSaida(2,DESLIGA);
      dimAtual2 = DIM0;
      canalAtual2 = 0;
    } else {
      acionaSaida(3,DESLIGA);
      dimAtual3 = DIM0;
      canalAtual3 = 0;
    }
  }  
}

void ligaCanal(int pCanal) {
  Serial.print("ligaCanal ");
  Serial.println(pCanal);
  if (pCanal == 1) { 
    acionaSaida(1,LIGA);    
    dimAtual1 = DIM100;
    canalAtual1 = 1;
  } else {
    if (pCanal == 2) {
      acionaSaida(2,LIGA);    
      dimAtual2 = DIM100;
      canalAtual2 = 1;
    } else {
      acionaSaida(3,LIGA);    
      dimAtual3 = DIM100;
      canalAtual3 = 1;
    }
  }
}

void inverteCanal(int pCanal) {
  Serial.print("inverteCanal ");
  Serial.println(pCanal);
  switch (pCanal) {
    case 1:
      if (dimAtual1 > 0) {
        acionaSaida(1,DESLIGA);         
      } else {
        acionaSaida(1,LIGA);         
      }      
    break;
    case 2:
      //
      if (dimAtual2 > 0) {
        acionaSaida(2,DESLIGA);         
      } else { 
        acionaSaida(2,LIGA);         
      }      
    break;
    case 3:
      //
      if (dimAtual3 > 0) {
        acionaSaida(3,DESLIGA);         
      } else {
        acionaSaida(3,LIGA);        
      }      
    break;
  }
}

void intercalaCanal() {
  Serial.print("intercalaCanal : ");
  //Serial.print(String(dimAtual2));
  //Serial.print(" ");
  //Serial.print(String(dimAtual3));
  
  // alterna entre o canal 2 e 3
  if ((dimAtual2 == 0) && (dimAtual3 == 0)) {
    acionaSaida(2,LIGA);
    dimAtual2 = DIM100;
  } else {
    if ((dimAtual2 == DIM100) && (dimAtual3 == 0)) {
      acionaSaida(3,LIGA);
      dimAtual3 = DIM100;
    } else {
      if ((dimAtual2 == DIM100) && (dimAtual3 == DIM100)) {
        acionaSaida(2,DESLIGA);
        dimAtual2 = DIM0;
      } else {
        acionaSaida(3,DESLIGA);
        dimAtual3 = DIM0;
      }
    }
  }
  //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
}


void acionaSaida(int pSaida, byte pEstado) {
  // 0.07 - Troca de lugar de atualizacao do dimAtual 1/2/3 do inverteCanal() para acionaCanal()
  int dimAux = 0;
  int canalAtual = 0;
  Serial.print("Aciona Saida ... ");
  Serial.print(pSaida);
  Serial.print(" - ");
  if (pEstado == LIGA) { 
    Serial.println("LIGA"); 
    dimAux = DIM100;
    canalAtual = 0;
  } else { 
    Serial.println("DESLIGA"); 
    dimAux = DIM0;
    canalAtual = 1;
  }

  // 0.09 - Para fita de LED RGB tem que ser "analogWrite" 
  if (cTipo != LKRGB || cTipo != LKLED) {
    if (pSaida == 1) {
      analogWrite(SAIDA_1, dimAux);
      dimAtual1 = dimAux;
      canalAtual1 = canalAtual;
    } else {
       if (pSaida == 2) {       
          analogWrite(SAIDA_2, dimAux); 
          dimAtual2 = dimAux;
          canalAtual2 = canalAtual;
       } else {
          analogWrite(SAIDA_3, dimAux); 
          dimAtual3 = dimAux;
          canalAtual3 = canalAtual;
       }
    }
  }
  else
  {
    if (pSaida == 1) {
      digitalWrite(SAIDA_1, pEstado); 
      dimAtual1 = dimAux;
      canalAtual1 = canalAtual;
    } else {
       if (pSaida == 2) {
          digitalWrite(SAIDA_2, pEstado); 
          dimAtual2 = dimAux;
          canalAtual2 = canalAtual;
       } else {
          digitalWrite(SAIDA_3, pEstado); 
          dimAtual3 = dimAux;
          canalAtual3 = canalAtual;
       }
    }
  }
  //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
  // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
  if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
}

void temporizaCanal(int pCanal, int pByte3) {
  Serial.print("temporizaCanal : ");
  Serial.print(pCanal);
  Serial.print(" - Tempo : ");
  Serial.print(String(pByte3,HEX));
  int tempo = pByte3 * 110; // 110 milesegundos
  Serial.print(" Hex - ");
  Serial.print(tempo);
  Serial.println(" milesegundos ");
  if (tempo < 1000) { tempo = 1000; } // estou determinando que o tempo minimo sera de 1 segundo
  tempo = tempo / 1000;
  Serial.print(tempo);
  Serial.println(" segundos ");
  if (cTipo == LKSHUTTER) {
    // se for shutter, desliga todos os canais e temporizacoes antes de comecar a temporizar    
    // 1.08
    //acionaSaida(1,DESLIGA); 
    //acionaSaida(2,DESLIGA); 
    //acionaSaida(3,DESLIGA);     
    vTimerCanal1 = -1;
    vTimerCanal2 = -1;
    vTimerCanal3 = -1;    
  }
  if (pCanal == 1) {
    acionaSaida(1,LIGA); 
    vTimerCanal1 = vSegundo + tempo;
    if (vTimerCanal1 > 100) { vTimerCanal1 = vTimerCanal1 - 100; }
  } else {
     if (pCanal == 2) {
        vTimerCanal2 = vSegundo + tempo;
        if (vTimerCanal2 > 100) { vTimerCanal2 = vTimerCanal2 - 100; }
        acionaSaida(2,LIGA);        
     } else {
        vTimerCanal3 = vSegundo + tempo;
        if (vTimerCanal3 > 100) { vTimerCanal3 = vTimerCanal3 - 100; }
        acionaSaida(3,LIGA); 
     }
  }
  // Dispara o timer se ele estiver parado
  if (vTimer == -1) { 
    vTimer = 0;
    vSegundo = 0;
  }
}

void callbackMQTTLocal(char* topic, byte* payload, unsigned int length) {
  debugln(">> callback local");
  //callback(topic, payload, length); 
  callbackMQTT(topic, payload, length); 
}
/*
 * 1.07 - Unificacao do callcak MQTT Local e Remoto
// versao 0.18
void callback(char* topic, byte* payload, unsigned int length) {
  // versao 0.11 - MQTT - essa rotina eh ativada quando recebe alguma coisa dos itens assinados 
  // armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);
  strMSG.trim();
  String strTopic = String((char*)topic);
  strTopic.trim();
  //Serial.println("callback");  
  debug("Callback Local Topico MQTT  : ");
  debug(topic);
  debug(" - Mensagem : ");
  String aux  = String(topic);
  aux.trim();  
  // Coloco um terminador no final da frase recebida para nao confundir sub-tipicos com inicio igual como: TOPIC_SETIDLK e 
  aux = aux + "@";
  if ((aux.indexOf(TOPIC_FROM_MC) == -1) && (aux.indexOf(TOPIC_BROADCAST) == -1)) {    
    debugln(strMSG); 
    if (aux.indexOf(TOPIC_GETSTATUS) > -1) { 
      vLastStatus = " ";      
      // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
      debugln("publicaTopicStatusMQTT");
      if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
    }
    if ((aux.indexOf(TOPIC_GETIP) > -1) || (aux.indexOf(TOPIC_GETIPLK) > -1)) {
      debugln("publicaTopicSetIPMQTT");
      publicaTopicSetIPMQTT();
    }     
    if (aux.indexOf(TOPIC_SETIDMC) > -1) {
      setIDMC(strMSG);
    }
    if (aux.indexOf(TOPIC_SETAPPTOKEN) > -1) {     
      appToken = strMSG;
      //debug("appToken : ");
      //debugln(appToken);
      saveCredentials();
      assinaAppToken();      
    }
    if (aux.indexOf(TOPIC_SETSCENE) > -1) {
      // Sub topico de assinatura - quando o APP envia para LK um cenario - LK02500004/SETSCENE/010102600004000000000 - onde: 01 = numero do cenario - 01 = sequencia - 02600004 = id do lk - 000 = valor do canal 1 - 000 = valor do canal 2 - 000 = valor do canal 3
      trataScene(strMSG);
    }
    if (aux.indexOf(TOPIC_SETIPMC) > -1) {
      setIPMC(strMSG);
    }    
    if (aux.indexOf(TOPIC_SETIDLK) > -1) {
      setIDLK(strMSG);
    }
  } else {            
    //
    // versao 0.15 - quando o byte do canal (terceiro Byte) for FF mudar para ZERO - porque o ZERO eh usado como fim de mensagem e por algum motivo quando esse byte eh zero o frame eh corrompido - zero em outros lugarem nao aconrece isso - Bug da biblioteca ?? 
    if (payload[2] == 0xFF) { payload[2] = 0x00; }
    //
    aux = String(payload[0], HEX);
    aux.concat(" ");
    aux.concat(String(payload[1], HEX));
    aux.concat(" ");
    aux.concat(String(payload[2], HEX));
    aux.concat(" ");
    aux.concat(String(payload[3], HEX));
    aux.concat(" ");
    aux.concat(String(payload[4], HEX));
    aux.toUpperCase();
    debugln(aux);
    // LK menos LK D2
    if (cTipo != LKD2E3) {
      Serial.println("nao eh D2");
      if (payload[0] == 0xF5) {
        switch (payload[3]) {
          case 0xF1:
            // 1.07 - CORRECAO ERRO - BROACAST - comando broadcast quando dimeriza um modulo ou dimeriza todos os modulos o byte de dado eh F0 e o LK entedia que era comnado Desliga Todas e nao dimriza
            // F5 FA 00 F1
            if ((payload[1] == 0xFA) && (payload[2] == 0x00)) { 
              if (cTipo != LKSHUTTER) {
                acionaSaida(1,LIGA);
                acionaSaida(2,LIGA);
                acionaSaida(3,LIGA);
              }
              Serial.println("Liga Todas");  
            }
          break;
          case 0xF0:
            Serial.println("eh F0");
            // 1.07 - CORRECAO ERRO - BROACAST - comando broadcast quando dimeriza um modulo ou dimeriza todos os modulos o byte de dado eh F0 e o LK entedia que era comnado Desliga Todas e nao dimriza
            // F5 FA 00 F0
            if ((payload[1] == 0xFA) && (payload[2] == 0x00)) {
              if (cTipo != LKSHUTTER) {
                acionaSaida(1,DESLIGA);
                acionaSaida(2,DESLIGA);
                acionaSaida(3,DESLIGA);
              }
              Serial.println("Desliga Todas");
            } else {
              // 1.07 - Incrementa TODOS os Dimmers
              // F5 FA 70 F0 B1 ou F5 FA MM F0 CKS
              if ((cTipo == LKRGB) || (cTipo == LKLED)) {
                Serial.println("eh RGB ou LED");
                if (payload[1] == 0xFA) {       
                  Serial.println("eh FA");       
                  if (payload[2] == 0x70) {
                    // INCREMENTA TODOS CANAIS de TODOS modulos
                    Serial.println("Incrementa Todos Dimmers");
                  } else {
                    // INCREMENTA TODOS CANAIS de UM modulo
                    Serial.println("Incrementa Todos Dimmers de UM modulo");
                  }
                }              
              }
            }
          break;
          case 0x0F:
            Serial.println("eh 0F");
            // 1.07 - Decrementa Todos Canais Dimmer
            // F5 FA 70 0F 92 ou F5 FA MM 0F CKS
            if ((cTipo == LKRGB) || (cTipo == LKLED)) {
              Serial.println("eh RGB ou LED");
              if (payload[2] == 0x70) {           
                Serial.println("eh 70");    
                if (payload[1] == 0xFA) { 
                  // DECREMENTA TODOS CANAIS de TODOS MODULOS;
                  Serial.println("Decrementa Todos Dimmers");
                } else {
                  // DECREMENTA TODOS CANAIS de UM MODULOS;
                  Serial.println("Decrementa Todos Dimmersde Um modulo");
                }
              }              
            }
          break;
          default:
            analisaFrame(payload[0],payload[1],payload[2],payload[3],payload[4]);
        }                       
        //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
        // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
        if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
      } // payload[0] == 0xF5      
    } else {     
      //Serial.println(" ((( callbackMQTT D2E3 ! ))) ");
      // Rotina para D2E3 que tem um ATMEL
      if (payload[0] == 0xF5) {
        // so manda o frame para o ATMEL se ele for MC -> LK                 
        mandaFrameTX(payload[0],payload[2],payload[3]);
        //Versao 0.15 - 2019 04 21 - MQTT - Analisa o frame que foi enviado para o ATMEL atualalizando as variaveis dimAtual<numero do canal>      
        analisaFrameATMEL(payload[2],payload[3]);        
        //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
        // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
        if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
      }
    }    
  }  
}
*/
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  // versao 0.11 - MQTT - essa rotina eh ativada quando recebe alguma coisa dos itens assinados 
  debugln(">> callbackMQTT");
  
  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);
  strMSG.trim();
  String strTopic = String((char*)topic);
  strTopic.trim();
  
  // Versao 1.04 - Temporiza para forcar o DISCONNECT do MQTT 
  MQTTCountReset = 0;
  
  debug("Callback Remoto Topico MQTT : ");
  debug(topic);
  debug(" - Mensagem :");
  String aux  = String(topic);
  aux.trim();
  
  // Coloco um terminador no final da frase recebida para nao confundir sub-tipicos com inicio igual como: TOPIC_SETIDLK e 
  aux = aux + "@";
  if ((aux.indexOf(TOPIC_FROM_MC) == -1) && (aux.indexOf(TOPIC_BROADCAST) == -1)) {    
    debugln(strMSG); 
    // 1.05 - Zera Info
    if (aux.indexOf(TOPIC_ZERA_MC_INFO) > -1) {
      debugln("ANTES TOPIC_ZERA_MC_INFO ALFA"); 
      if (strMSG == "LEGOOLUS_ALFA") zeraMCInfo();
    }
    if (aux.indexOf(TOPIC_ZERA_ALL_INFO) > -1) {      
      debugln("ANTES TOPIC_ZERA_MC_INFO SIGMA"); 
      if (strMSG == "LEGOOLUS_SIGMA") zeraALLInfo();
    }
    if (aux.indexOf(TOPIC_GETSTATUS) > -1) { 
      vLastStatus = " ";
      //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
      // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
      if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); } 
    }
    if ((aux.indexOf(TOPIC_GETIP) > -1) || (aux.indexOf(TOPIC_GETIPLK) > -1)) {
      publicaTopicSetIPMQTT();
    }     
    if (aux.indexOf(TOPIC_SETIDMC) > -1) {
      setIDMC(strMSG);
    }
    if (aux.indexOf(TOPIC_SETAPPTOKEN) > -1) {     
      appToken = strMSG;
      debug("appToken : ");
      debugln(appToken);
      saveCredentials();
      assinaAppToken();
    }
    // 1.08 - Topico para definir o tempo de abertura e fechamento da cortina - se nao houver o padrao eh 30 segundos
    // So verifica o topico se for SHUTTER  
    if (cTipo == LKSHUTTER) {
      if (aux.indexOf(TOPIC_SETSHUTTER) > -1) {     
        vValorAux = strMSG;
        debug("Tempo Shutter : ");
        debugln(vValorAux);
        saveCredentials();
        assinaAppToken();
      }
    }
    if (aux.indexOf(TOPIC_SETSCENE) > -1) {
      // Sub topico de assinatura - quando o APP envia para LK um cenario - LK02500004/SETSCENE/010102600004000000000 - onde: 01 = numero do cenario - 01 = sequencia - 02600004 = id do lk - 000 = valor do canal 1 - 000 = valor do canal 2 - 000 = valor do canal 3
      trataScene(strMSG);
    }
    if (aux.indexOf(TOPIC_SETIPMC) > -1) {
      setIPMC(strMSG);
    }    
    if (aux.indexOf(TOPIC_SETIDLK) > -1) {
      setIDLK(strMSG);
    }
  } else {            
    //
    // versao 0.15 - quando o byte do canal (terceiro Byte) for FF mudar para ZERO - porque o ZERO eh usado como fim de mensagem e por algum motivo quando esse byte eh zero o frame eh corrompido - zero em outros lugarem nao aconrece isso - Bug da biblioteca ?? 
    if (payload[2] == 0xFF) { payload[2] = 0x00; }
    //
    aux = String(payload[0], HEX);
    aux.concat(" ");
    aux.concat(String(payload[1], HEX));
    aux.concat(" ");
    aux.concat(String(payload[2], HEX));
    aux.concat(" ");
    aux.concat(String(payload[3], HEX));
    aux.concat(" ");
    aux.concat(String(payload[4], HEX));
    aux.toUpperCase();
    debugln(aux);
    
    if (cTipo != LKD2E3) {
      //Serial.print(" ( callbackMQTT NOT ! D2E3 ) ");
      if (payload[0] == 0xF5) {
        switch (payload[3]) {
          // 1.07 - CORRECAO ERRO - BROACAST - comando broadcast quando dimeriza um modulo ou dimeriza todos os modulos o byte de dado eh F0 e o LK entedia que era comnado Desliga Todas e nao dimriza
          case 0xF1: 
            // F5 FA 00 F1 20 - Liga Todas
            if ((payload[1] == 0xFA) && (payload[2] == 0x00)) {
              if (cTipo != LKSHUTTER) {
                acionaSaida(1,LIGA);
                acionaSaida(2,LIGA);
                acionaSaida(3,LIGA);
              }
              Serial.println("Liga Todas");
            }
          break;
          case 0xF0:    
            /*
            if ((payload[1] == 0xFA) && (payload[2] == 0x00)) {   
              if (cTipo != LKSHUTTER) {
                acionaSaida(1,DESLIGA);
                acionaSaida(2,DESLIGA);
                acionaSaida(3,DESLIGA);
              }
              Serial.println("Desliga Todas");
            }
            */
            Serial.println("eh F0");
            // 1.07 - CORRECAO ERRO - BROACAST - comando broadcast quando dimeriza um modulo ou dimeriza todos os modulos o byte de dado eh F0 e o LK entedia que era comnado Desliga Todas e nao dimriza
            // F5 FA 00 F0 21 - Desliga Todas
            if ((payload[1] == 0xFA) && (payload[2] == 0x00)) {
              if (cTipo != LKSHUTTER) {
                acionaSaida(1,DESLIGA);
                acionaSaida(2,DESLIGA);
                acionaSaida(3,DESLIGA);
              }
              Serial.println("Desliga Todas");
            } else {
              // 1.07 - Incrementa TODOS os Dimmers ou Incrementa TODOS os CANAIS de UM Dimmer - No D2E3 quem faz o trabalho sujo eh o ATMEL
              // F5 FA 70 F0 B1 ou F5 FA MM F0 CKS
              if ((cTipo == LKRGB) || (cTipo == LKLED)) {
                Serial.println("eh RGB ou LED");
                if (payload[2] == 0x70) {       
                  Serial.println("eh 70");       
                  if (payload[1] == 0xFA) {
                    // INCREMENTA TODOS CANAIS de TODOS modulos
                    incrementaCanal(1);
                    incrementaCanal(2);
                    incrementaCanal(3);
                    Serial.println("Incrementa Todos Dimmers");
                  } else {
                    // INCREMENTA TODOS CANAIS de UM modulo
                    Serial.println(vKitID);
                    if (payload[1] == vKitID.toInt()) {
                      incrementaCanal(1);
                      incrementaCanal(2);
                      incrementaCanal(3);                    
                      Serial.println("Incrementa Todos camais DESSE modulo");
                    }                    
                  }
                }              
              }
            }
          break;
          case 0x0F:
            Serial.println("eh 0F");
            // 1.07 - Decrementa TODOS os Dimmers ou Decrementa TODOS os CANAIS de UM Dimmer - No D2E3 quem faz o trabalho sujo eh o ATMEL
            // F5 FA 70 0F 92 ou F5 FA MM 0F CKS
            if ((cTipo == LKRGB) || (cTipo == LKLED)) {
              Serial.println("eh RGB ou LED");
              if (payload[2] == 0x70) {           
                Serial.println("eh 70");    
                if (payload[1] == 0xFA) { 
                  // DECREMENTA TODOS CANAIS de TODOS MODULOS;
                  decrementaCanal(1);
                  decrementaCanal(2);
                  decrementaCanal(3);
                  Serial.println("Decrementa Todos Dimmers");
                } else {                  
                  // DECREMENTA TODOS CANAIS de UM MODULOS;
                  Serial.print("Decrementa Todos Dimmers de Um modulo - vKitID : ");
                  Serial.println(vKitID);
                  if (payload[1] == vKitID.toInt()) {
                    decrementaCanal(1);
                    decrementaCanal(2);
                    decrementaCanal(3);                  
                    Serial.println("Decrementa Todos Dimmers DESSE modulo");
                  }
                }
              }              
            }
          break;
          default:
            analisaFrame(payload[0],payload[1],payload[2],payload[3],payload[4]);
        }                       
        //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
        // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
        if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
      } // payload[0] == 0xF5      
    } else {     
      //Serial.print(" ( callbackMQTT D2E3 ! ) ");
      // Rotina para D2E3 que tem um ATMEL
      if (payload[0] == 0xF5) {
        // so manda o frame para o ATMEL se ele for MC -> LK 
        mandaFrameTX(payload[0],payload[2],payload[3]);
        //Versao 0.15 - 2019 04 21 - MQTT - Analisa o frame que foi enviado para o ATMEL atualalizando as variaveis dimAtual<numero do canal>      
        analisaFrameATMEL(payload[2],payload[3]);        
        //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
        // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
        if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
      }
    }
    //publicaTopicStatusMQTT();
  }
}

void montaTopicosMQTT() {
  // Versao 0.12 - Montando os topicos para serem assinados
  // Versao 0.15 - Master_Topic, GET_STATUS, TOPIC_SETIP, etc
  // MASTER_TOPIC
  vMsgInicial = "LK";
  vMsgInicial.concat(cTpNS);
  vMsgInicial.concat(vNS);
  vMsgInicial.concat("/#");
  vMsgInicial.toCharArray(MASTER_TOPIC, 13); 
  // TOPIC_GETSTATUS
  /*
  vMsgInicial = "LK";
  vMsgInicial.concat(cTpNS);
  vMsgInicial.concat(vNS);
  vMsgInicial.concat("/GETSTATUS");
  vMsgInicial.toCharArray(TOPIC_GETSTATUS, 21);  
  */
  // TOPIC_FROM_MC
  /*
  vMsgInicial = "LK";
  vMsgInicial.concat(cTpNS);
  vMsgInicial.concat(vNS);
  vMsgInicial.concat("/");
  vMsgInicial.concat(SUBTOPIC_FROM_MC); // MC
  vMsgInicial.toCharArray(TOPIC_FROM_MC, 14); 
  */ 
  // TOPIC_TO_MC
  vMsgInicial = "LK";
  vMsgInicial.concat(cTpNS);
  vMsgInicial.concat(vNS);
  vMsgInicial.concat("/");
  vMsgInicial.concat(SUBTOPIC_TO_MC); // LK
  vMsgInicial.toCharArray(TOPIC_TO_MC, 14); 
  // BROADCAST FROM MC
  if (vMCID_OK) {
    // TOPIC_BROADCAST
    /*
    vMsgInicial = "MC";
    vMsgInicial.concat(vMCID);
    vMsgInicial.concat("/BROADCAST");
    vMsgInicial.toCharArray(TOPIC_BROADCAST, 18); 
    */    
    // MASTER_TOPIC_MC
    vMsgInicial = "MC";
    vMsgInicial.concat(vMCID);
    vMsgInicial.concat("/#");
    vMsgInicial.toCharArray(MASTER_TOPIC_MC, 10);     
    // TOPIC_TO_MC_ALTERNATIVO - MC00001LK/LK02500004/LK/FE030601F
    vMsgInicial = "MC";
    vMsgInicial.concat(vMCID);
    vMsgInicial.concat("LK/LK");    
    vMsgInicial.concat(cTpNS);
    vMsgInicial.concat(vNS);
    vMsgInicial.concat("/LK");
    //vMsgInicial.concat(SUBTOPIC_TO_MC); // LK
    vMsgInicial.toCharArray(TOPIC_TO_MC_ALTERNATIVO, 24);
    
    //
    // TOPIC_SETIP_ALTERNATIVO - MC00001LK/LK02500004/SETIP/192.168.15.18
    vMsgInicial = "MC";
    vMsgInicial.concat(vMCID);
    vMsgInicial.concat("LK/LK");    
    vMsgInicial.concat(cTpNS);
    vMsgInicial.concat(vNS);
    vMsgInicial.concat("/SETIP");
    //vMsgInicial.concat(SUBTOPIC_SETIP); // SETIP
    vMsgInicial.toCharArray(TOPIC_SETIP_ALTERNATIVO, 28); 
    
    debug("MontaTopicoMQTT : TOPIC_SETIP_ALTERNATIVO : ");
    debugln(TOPIC_SETIP_ALTERNATIVO);
    debug("MontaTopicoMQTT : TOPIC_TO_MC_ALTERNATIVO : ");
    debugln(TOPIC_TO_MC_ALTERNATIVO);
  }  
}

void assinaturasMQTT() {
  if (MQTT.connected()) {
    msgln ( " " );
    msgln ( "Broker MQTT Remoto - Topicos de Assinatura: " );
    msgln ( MASTER_TOPIC );
    if (vMCID_OK) {
      msgln ( MASTER_TOPIC_MC );      
    }    
    //msgln ( " Antes  Assinando MASTER_TOPIC " );
    MQTT.subscribe(MASTER_TOPIC);
    //msgln ( " Depois  Assinando MASTER_TOPIC " );
    // so assina broadcast quando o ID da MC existir
    if (vMCID_OK) { 
      //msgln ( " Antes  Assinando MASTER_TOPIC_MC " );
      MQTT.subscribe(MASTER_TOPIC_MC); 
      //msgln ( " Depois  Assinando MASTER_TOPIC_MC " );
    } 
    if ((appToken != "") && (appToken != "9999999999") && (isNumber(appToken))) {
      //msgln ( " ANTES DO assinaAppToken" );
      assinaAppToken();
      //msgln ( " DEPOIS DO assinaAppToken" );
    } 
  } else {
    msgln ( " " );
    msgln ( "Broker MQTT Remoto NENHUM Topico foi Assinado." );
  }  
  msgln ( " " );   
}

void assinaturasMQTTLocal() {
  if (MQTTLocal.connected()) {
    Serial.println ( " " );
    Serial.println ( "Broker MQTT Local - Topicos de Assinatura: " );
    Serial.println ( MASTER_TOPIC );
    if (vMCID_OK) {
      Serial.println ( MASTER_TOPIC_MC );      
    }
    MQTTLocal.subscribe(MASTER_TOPIC);
    // so assina broadcast quando o ID da MC existir
    if (vMCID_OK) { 
      MQTTLocal.subscribe(MASTER_TOPIC_MC); 
    } 
    /*
    // so assina AppToken se existir AppToken
    if ((appToken != "") && (appToken != "9999999999") && (isNumber(appToken))) {
      assinaAppToken();
    }    
    */
  } else {
    Serial.println ( " " );
    Serial.println ( "Broker MQTT Local NENHUM Topico foi Assinado." );
  }
  Serial.println ( " " );
}


bool isNumber (String str) { 
  bool retorno = true;
  for (byte i = 0; i <str.length () ; i ++) {
    if (!isDigit(str.charAt(i))) { retorno = false; }
  } 
  return retorno; 
}  
/*
bool isNumber(String pNumber) {
  bool retorno = false;
  int  t = pNumber.length();
  String p1;
  String p2;
  if (t > 0) {
    if (t > 9) {
      p1 = pNumber.substring(0,9);
      p2 = pNumber.substring(9);    
    } else {
      p1 = pNumber;
      p2 = "";
    }
    int x1 = p1.toInt();
    int x2 = p2.toInt(); 
    String a1 = String(x1);    
    String a2 = String(x2); 
    if ((a1.length() + a2.length()) == t) { 
      retorno = true;     
    }
  }
  return retorno;
}
*/
void assinaAppToken() {
  //debugln("assinaAppToken");
  vMsgInicial = "APP";
  vMsgInicial.concat(appToken);
  vMsgInicial.concat("/#");
  vMsgInicial.toCharArray(TOPIC_APPTOKEN, vMsgInicial.length() + 1);
  MQTT.subscribe(TOPIC_APPTOKEN);
  //Serial.print ( "Topico Assinado: " );
  msgln ( TOPIC_APPTOKEN );
}

void testeD2E3() {
  //
  // Manda um liga todas para o ATMEL      
  //
  //delay(5000); 
  Serial.print ("testeD2E3 : ");
  
  String recebeu;
  int    contRetornoArmel = 0;
  /*
   * Rotina para aguardar o ID do ATMEL quando este acorda
  contRetornoArmel = 0;
  while ((Serial.available() == 0) && (contRetornoArmel < 50)) { 
    contRetornoArmel += 1;
    Serial.print(".");
    delay(100);
  };  
  Serial.println(" ");
  Serial.print("Recebeu : Bytes : ");
  Serial.print(Serial.available());
  Serial.print(" : ");
  recebeu = Serial.readString();
  aux = String(recebeu[0], HEX);  
  aux.concat(" ");
  aux.concat(String(recebeu[1], HEX));
  aux.concat(" ");
  aux.concat(String(recebeu[2], HEX));
  aux.concat(" ");
  aux.concat(String(recebeu[3], HEX));
  aux.toUpperCase();
  Serial.println(aux);
  Serial.println(" ");
  */
  line2 = "     ";
  line2[0] = 0xF5;
  line2[1] = 0x01;
  line2[2] = 0x00;
  line2[3] = 0xF1;
  line2[4] = 0x19;
  
  aux = "";
  aux.concat(String(line2[0], HEX));
  aux.concat(" ");
  aux.concat(String(line2[1], HEX));
  aux.concat(" ");
  aux.concat(String(line2[2], HEX));
  aux.concat(" ");
  aux.concat(String(line2[3], HEX));
  aux.concat(" ");
  aux.concat(String(line2[4], HEX));
  aux.toUpperCase();
  Serial.print("Enviando : ");
  Serial.println(aux);
  
  frame[0] = line2[0];
  frame[1] = line2[1];
  frame[2] = line2[2];
  frame[3] = line2[3];
  frame[4] = line2[4];
  
  Serial.write(frame,5);  
  Serial.println(" ");
  
  delay(2000);
  
  line2[0] = 0xF5;
  line2[1] = 0x01;
  line2[2] = 0x00;
  line2[3] = 0xF0;
  line2[4] = 0x1A;
  
  aux = "";
  aux.concat(String(line2[0], HEX));
  aux.concat(" ");
  aux.concat(String(line2[1], HEX));
  aux.concat(" ");
  aux.concat(String(line2[2], HEX));
  aux.concat(" ");
  aux.concat(String(line2[3], HEX));
  aux.concat(" ");
  aux.concat(String(line2[4], HEX));
  aux.toUpperCase();
  Serial.print("Enviando : ");
  Serial.println(aux);
  
  frame[0] = line2[0];
  frame[1] = line2[1];
  frame[2] = line2[2];
  frame[3] = line2[3];
  frame[4] = line2[4];
  
  Serial.write(frame,5);
  
  // Get ID do ATMEL
  /*
  delay(2000);
  line2[0] = 0xF5;
  line2[1] = 0xFA;
  line2[2] = 0x80;
  line2[3] = 0x00;
  line2[4] = 0x91;
  
  aux = "";
  aux.concat(String(line2[0], HEX));
  aux.concat(" ");
  aux.concat(String(line2[1], HEX));
  aux.concat(" ");
  aux.concat(String(line2[2], HEX));
  aux.concat(" ");
  aux.concat(String(line2[3], HEX));
  aux.concat(" ");
  aux.concat(String(line2[4], HEX));
  aux.toUpperCase();
  Serial.println(" ");
  Serial.print("Enviando : ");
  Serial.println(aux);
  
  frame[0] = line2[0];
  frame[1] = line2[1];
  frame[2] = line2[2];
  frame[3] = line2[3];
  frame[4] = line2[4];
  
  Serial.write(frame,5);

  contRetornoArmel = 0;
  while ((Serial.available() == 0) && (contRetornoArmel < 50)) { 
    contRetornoArmel += 1;
    Serial.print(".");
    delay(100);
  };  
  Serial.println(" ");
  Serial.print("Recebeu : Bytes : ");
  Serial.print(Serial.available());
  Serial.print(" : ");
  recebeu = Serial.readString();
  aux = String(recebeu[0], HEX);  
  aux.concat(" ");
  aux.concat(String(recebeu[1], HEX));
  aux.concat(" ");
  aux.concat(String(recebeu[2], HEX));
  aux.concat(" ");
  aux.concat(String(recebeu[3], HEX));
  aux.toUpperCase();
  Serial.println(aux);
  Serial.println(" ");
  */
  
}

void mandaFrameTX(char byte1, char byte3, char byte4) {
  // Versao 0.12 - Quando LKD2E3 - Funcao para enviar o frame recebido para o ATMEL
  frame[0] = byte1;  
  frame[1] = 0x01; // o ID é 1 porque o ATMEL esta esperando o ID 1
  frame[2] = byte3;
  frame[3] = byte4;
  // Calcula novo Checksum
  int f = 0;
  f = 0x100 - (frame[0] + frame[1] + frame[2] + frame[3]);
  while (f < 0) { f = 0x100 + f; } 
  frame[4] = f;
  
  //aux = "";
  //aux.concat(String(frame[0], HEX));
  aux = String(frame[0], HEX);
  aux.concat(" ");
  aux.concat(String(frame[1], HEX));
  aux.concat(" ");
  aux.concat(String(frame[2], HEX));
  aux.concat(" ");
  aux.concat(String(frame[3], HEX));
  aux.concat(" ");
  aux.concat(String(frame[4], HEX));
  aux.toUpperCase();
  Serial.print("mandaFrameTX : ");
  Serial.println(aux);
  Serial.write(frame,5);
  Serial.println(" ");
  //
  // Se for um pedido de status padrao EEPROM 0x88 - Sem uso - foi usado para DEBUG
  if (frame[2] >= 0x80) { 
    delay(500);
    Serial.print("mandaFrameTX : Recebeu :");
    String recebeu = Serial.readString();
    //Serial.println(recebeu);  
    aux = String(recebeu[0], HEX);  
    //aux = "";
    //aux.concat(String(recebeu[0], HEX));
    aux.concat(" ");
    aux.concat(String(recebeu[1], HEX));
    aux.concat(" ");
    aux.concat(String(recebeu[2], HEX));
    aux.concat(" ");
    aux.concat(String(recebeu[3], HEX));
    aux.toUpperCase();
    Serial.println(aux);
  }
  //
  // Versao 0.15 - Enquanto o ATMEL esta fazendo fade-in e fade-out nao aceita comnados isso o controle do valor do dimAtual<canal> fica errado
  // Resolvi não usar o comnado INVERTE do APP porque eh muito lento, com ON e OFF o ATMEL responde imediatamente, assim o STATUS não fica errado
  /*
  if ((byte3 == 0x08) || (byte3 == 0x09) || (byte3 == 0x0A)) {
    Serial.print("Dando uma pausa de 3s");
    delay(3000);
  }
  */
}

void analisaFrameATMEL(char byte3, char byte4) {
  // Rotina que analisa do comando recebido para ser enviado para o ATMEL para atualizar o estado dos canais
  /*
  String aux = String(byte3, HEX);
  aux.concat(" ");
  aux.concat(String(byte4, HEX));
  aux.concat(" ");
  aux.toUpperCase();
  Serial.print("analisaFrameATMEL : Byte 3 e 4 = ");
  Serial.println(aux);
  */
  switch (byte3) {
    // Liga e Desliga
    case 0x00:
      if (byte4 == 0x00) { 
        //desligaCanal(1); 
        dimAtual1 = 0;        
      } else {
        // 1.07 - CORRECAO ERRO - BROACAST - comando broadcast quando dimeriza um modulo ou dimeriza todos os modulos o byte de dado eh F0 e o LK entedia que era comnado Desliga Todas e nao dimriza
        // Essa rotina nao sofreu alteracao - parece que funcinado -  quando voltar no D2 - verifico melhor
        // F5 FA 00 F1/F0 20/21
        if (byte4 == 0xF1) {
          // Liga Todas
          dimAtual1 = 100;
          dimAtual2 = 100;
          dimAtual3 = 100; 
        } else {
          if (byte4 == 0xF0) {
            // Desliiga Todas
            dimAtual1 = 0;
            dimAtual2 = 0;
            dimAtual3 = 0; 
          } else {
            //ligaCanal(1); 
            dimAtual1 = 100;
          }
        }
      }
    break;
    case 0x01:
      if (byte4 == 0x00) { 
        //desligaCanal(2); 
        dimAtual2 = 0;
      } else { 
        //ligaCanal(2);
        dimAtual2 = 100; 
      }
    break;
    case 0x02:
      if (byte4 == 0x00) { 
        //desligaCanal(3); 
        dimAtual3 = 0;
      } else { 
        //ligaCanal(3); 
        dimAtual3 = 100;
      }      
    break;
    case 0x04:
      Serial.println ("ON/OFF - canal nao existe");
    break;
    case 0x08:      
      //inverteCanal(1);      
      if (dimAtual1 > 0) { dimAtual1 = 0; } else { dimAtual1 = 100; }
    break;
    case 0x09:      
      //inverteCanal(2); 
      if (dimAtual2 > 0) { dimAtual2 = 0; } else { dimAtual2 = 100; }
    break;
    case 0x0A:      
      //inverteCanal(3); 
      if (dimAtual3 > 0) { dimAtual3 = 0; } else { dimAtual3 = 100; }      
    break;
    case 0x0B:
      Serial.println ("inverte - canal nao existe");
    break;
    case 0x58: 
      if (byte4 == 0x00) {
        // decrementaCanal(1);        
        dimAtual1 = dimAtual1 - 10;
        if (dimAtual1 < 0) { dimAtual1 = 0; }
      } else {
        // dimerizaCanal(1,byte4);
        dimAtual1 = traduzDimmerValue(byte4);
      }
    break;
    case 0x59: // Canal 2
      if (byte4 == 0x00) {
        // decrementaCanal(2);
        dimAtual2 = dimAtual2 - 10;
        if (dimAtual2 < 0) { dimAtual2 = 0; }
      } else {
        // dimerizaCanal(2,byte4);
        dimAtual2 = traduzDimmerValue(byte4);
      }
    break;
    case 0x5A:      
      if (byte4 == 0x00) {
        // decrementaCanal(3);
        dimAtual3 = dimAtual3 - 10;
        if (dimAtual3 < 0) { dimAtual3 = 0; }
      } else {
        // dimerizaCanal(3,byte4);
        dimAtual3 = traduzDimmerValue(byte4);
      }      
    break;
    case 0x5B:      
      Serial.println ("decrementa ou dimeriza - canal nao existe");
    break;
    case 0x5C: 
      // incrementaCanal(1);
      dimAtual1 = dimAtual1 + 10;
      if (dimAtual1 > 100) { dimAtual1 = 100; }
    break;
    case 0x5D:
      // incrementaCanal(2);
      dimAtual2 = dimAtual2 + 10;
      if (dimAtual2 > 100) { dimAtual2 = 100; }
    break;
    case 0x5E:
      // incrementaCanal(3);
      dimAtual3 = dimAtual3 + 10;
      if (dimAtual3 > 100) { dimAtual3 = 100; }
    break;
    case 0x5F:
      Serial.println ("incrementa - canal nao existe");
    break;
    case 0x40:
      Serial.println ("Temporiza Canal 1");
      //temporizaCanal(1,byte4);
    break;
    case 0x41:
      Serial.println ("Temporiza Canal 2");
      //temporizaCanal(2,byte4);
    break;
    case 0x42:
      Serial.println ("Temporiza Canal 3");
      //temporizaCanal(3,byte4);
    break;
    default:
      Serial.println ("erro no canal");      
  }
}

int traduzDimmerValue(int pByte3) {
  // traduz o BYTE para percentual 
  int retorno = 0;
  Serial.print("traduzDimmerValue ");  
  switch (pByte3) {
    case 0x01: // 100%
      retorno = 100;
      Serial.println(" 100%");
    break;
    case 0x02: // 90%
      retorno = 90;
      Serial.println(" 90%");
    break;
    case 0x03: // 80%
      retorno = 80;
      Serial.println(" 80%");
    break;
    case 0x04: // 70%
      retorno = 70;
      Serial.println(" 70%");
    break;
    case 0x05: // 60%
      retorno = 60;
      Serial.println(" 60%");
    break;
    case 0x06: // 50%
      retorno = 50;
      Serial.println(" 50%");
    break;
    case 0x07: // 40%
      retorno = 40;
      Serial.println(" 40%");
    break;
    case 0x08: // 30%
      retorno = 30;
      Serial.println(" 30%");
    break;
    case 0x09: // 20%
      retorno = 20;
      Serial.println(" 20%");
    break;
    case 0x0A: // 10%
      retorno = 10;
      Serial.println(" 10%");
    break;
    case 0x0B: // 5%
      retorno = 10;
      Serial.println(" 5%");
    break;
    case 0x0C: // 0%
      retorno = 0;
      Serial.println(" 0%");
    break;
    default:
      Serial.println ("erro no traduzDimmerValue");   
      retorno = 0;
  }
  return retorno;
}

//
int traduzDimmerValuePercentageToByte(int pValor) {
  // traduz o percentual para BYTE
  int retorno = 0;
  Serial.println("traduzDimmerValuePercentageToByte");  
  switch (pValor) {
    case 100: // 100%
      retorno = 0x01;
      //Serial.println(" 100%");
    break;
    case 90: // 90%
      retorno = 0x02;
      //Serial.println(" 90%");
    break;
    case 80: // 80%
      retorno = 0x03;
      //Serial.println(" 80%");
    break;
    case 70: // 70%
      retorno = 0x04;
      //Serial.println(" 70%");
    break;
    case 60: // 60%
      retorno = 0x05;
      //Serial.println(" 60%");
    break;
    case 50: // 50%
      retorno = 0x06;
      //Serial.println(" 50%");
    break;
    case 40: // 40%
      retorno = 0x07;
      //Serial.println(" 40%");
    break;
    case 30: // 30%
      retorno = 0x08;
      //Serial.println(" 30%");
    break;
    case 20: // 20%
      retorno = 0x09;
      //Serial.println(" 20%");
    break;
    case 10: // 10%
      retorno = 0x0A;
      //Serial.println(" 10%");
    break;
    case 5: // 5%
      retorno = 0x0B;
      //Serial.println(" 5%");
    break;
    case 0: // 0%
      retorno = 0x0C;
      //Serial.println(" 0%");
    break;
    default:
      Serial.println ("erro no traduzDimmerValuePercentageToByte");   
      retorno = 0;
  }
  return retorno;
}

// Versao 0.15
int traduzFatorDimmer(int pFator) {
  // Usado para montar o topico STATUS no MQTT para LED e LED RGB
  int retorno = 0;
  //Serial.println("traduzFatorDimmer ");  
  switch (pFator) {
    case 1024: // 100%
      retorno = 100;      
    break;
    case 912: // 90%
      retorno = 90;      
    break;
    case 819: // 80%
      retorno = 80;      
    break;
    case 716: // 70%
      retorno = 70;
    break;
    case 614: // 60%
      retorno = 60;      
    break;
    case 512: // 50%
      retorno = 50;      
    break;
    case 409: // 40%
      retorno = 40;      
    break;
    case 307: // 30%
      retorno = 30;
    break;
    case 204: // 20%
      retorno = 20;
    break;
    case 102: // 10%
      retorno = 10;
    break;
    case 51: // 5%
      retorno = 10;      
    break;
    case 0: // 0%
      retorno = 0;      
    break;
    default:
      Serial.println ("erro no traduzFatorDimmer");   
      retorno = 0;
  }
  return retorno;
}

void publicaTopicStatusMQTT() {
  // 1.00 - em vez de publicar o STATUS direto -  aguardo um tempo ate receber o ultima solicitacao (interna ou externa) de envio de STATUS
  vEnviaStatus = true;
  vContEnviaStatus = 0;
}

void publicaTopicStatusMQTT_2() {
  // Versao 0.15 
  // Montando frame de STATUS do MQTT - Formato XXXYYY - onde XXX eh valor do dimmer 1 de 000 até 100 podemdo 01 = 100% e YYY eh o valor do dimmer 2          
  // Serial.println("publicaTopicStatusMQTT");
  if (cTipo != LKRGB && cTipo != LKLED && cTipo != LKD2E3) {
      // Se NAO for Dimmer, LED ou RGB envia 000 e 100    
      if (dimAtual1 > 0) { aux = "100"; } else { aux = "000"; }
      if (dimAtual2 > 0) { aux = aux + "100"; } else { aux = aux + "000"; }
      if (cTipo == LKSHUTTER) {
        dimAtual3 = vStatusShutter; 
      }
      if (dimAtual3 > 0) { aux = aux + "100"; } else { aux = aux + "000"; }
    } else {
      if (cTipo == LKD2E3) {
        String a = String(dimAtual1);
        while (a.length() < 3) { a = "0" + a; }
        aux = a;
        a = String(dimAtual2);
        while (a.length() < 3) { a = "0" + a; }
        aux = aux + a;
        a = String(dimAtual3);
        while (a.length() < 3) { a = "0" + a; }
        aux = aux + a;        
      } else {  
        /*      
        Serial.println("eh LED ou RGB");
        Serial.print("dimAtual1 : ");
        Serial.println(dimAtual1);
        Serial.print("dimAtual2 : ");
        Serial.println(dimAtual2);
        Serial.print("dimAtual3 : ");
        Serial.println(dimAtual3);
        */
        // Se for LED ou RGB envia o percentual de dimeizacao 
        String a = String(traduzFatorDimmer(dimAtual1));
        while (a.length() < 3) { a = "0" + a; }
        aux = a;
        a = String(traduzFatorDimmer(dimAtual2));
        while (a.length() < 3) { a = "0" + a; }
        aux = aux + a;
        a = String(traduzFatorDimmer(dimAtual3));
        while (a.length() < 3) { a = "0" + a; }
        aux = aux + a;                
        //Serial.println(".");
      }
    }
    // sempre que muda o valor de algum canal o LK envia o STATUS
    if (aux != vLastStatus) {
      vLastStatus = aux;
      enviaTopicoLKGenericoMQTT(SUBTOPIC_STATUS,aux);
    }
    //debug("publicaTopicStatusMQTT_2 : vTempoShutter :");   
    // 1.04 - Antigo vTempoShutter 
    // se for Fita de LEd RGB armazena o ultimo valor de dimerizacao
    if (cTipo == LKRGB) {
      vValorAux = aux;
      debugln(vValorAux);
      if (!vNaoSalvaCredentials) { 
        vNaoSalvaCredentials = false;
        saveCredentials(); 
      }
    }
}

void publicaTopicSetIPMQTT() {
  // Versao 0.15 
  // Montando frame de SETIP com o IP do LK  
  // Serial.println("publicaTopicSetIPMQTT");
  char frameIP[15];  
  vMsgInicial = "";  
  vMsgInicial = WiFi.localIP().toString();
  vMsgInicial.toCharArray(frameIP, vMsgInicial.length() + 1);
  if (vMsgInicial.length() <= 14) {
    frameIP[vMsgInicial.length() + 1] = '\0';
  }
  // somente envia de houver MC e se o ID da MC foi informado - no formarto alternativo para MC
  if (vMCID_OK) {
    if (MQTT.connected()) { MQTT.publish(TOPIC_SETIP_ALTERNATIVO, frameIP); }
    if (MQTTLocal.connected()) { MQTTLocal.publish(TOPIC_SETIP_ALTERNATIVO, frameIP); }
  }
  // TOPIC_SETIP - Sub topico de Publicacao com o valor do IP do LK  
  enviaTopicoLKGenericoMQTT(SUBTOPIC_SETIP,vMsgInicial);
}

void enviaTopicoLKGenericoMQTT(String pSubTopico, String pFrame) {  
  // Publica topicos do LK para o mundo
  // 0.19 - inclusao do @ no topico para diferenciar os topicos de publicacao dos topicos de assinatura quando o mesmo eh o ID do LK
  // Subtopicos: STATUS, SETIP e EVENT
  
  char*  TOPIC = ""; 
  char   frameLocal[30];
  String vGeneric = "LK";
  boolean vEnviou = false;
  String vDestino = " - ";
  vGeneric.concat(cTpNS);
  vGeneric.concat(vNS);
  vGeneric.concat("@/"); // 0.19 - inclusao do @ no topico para diferenciar os topicos de publicacao dos topicos de assinatura quando o mesmo eh o ID do LK
  vGeneric.concat(pSubTopico); // SETIP
  vGeneric.toCharArray(TOPIC, vGeneric.length() + 1); 
  pFrame.toCharArray(frameLocal, pFrame.length() + 1);
  if (pFrame.length() <= 30) {
    frameLocal[pFrame.length() + 1] = '\0';
  }
  if (MQTT.connected()) { 
    vEnviou = true;
    MQTT.publish(TOPIC, frameLocal); 
    vDestino = vDestino + "Remoto";
  } 
  if (MQTTLocal.connected()) { 
    vEnviou = true;
    MQTTLocal.publish(TOPIC, frameLocal); 
    vDestino = vDestino + " Local";
  }  
  if (vEnviou) {
    debug("enviaTopicoLKGenericoMQTT . : ");    
    debug(pSubTopico);  
    debug(" - ");
    debug(pFrame);  
    debugln(vDestino);    
    //if (!MQTT.connected()) { Serial.println("MQTT.connected is FALSE"); } else { Serial.println("MQTT.connected is TRUE"); }
    //if (!MQTTLocal.connected()) { Serial.println("MQTTlocal.connected is FALSE"); } else { Serial.println("MQTTLocal.connected is TRUE"); }
    //Serial.println("enviaTopicoLKGenericoMQTT ... : FIM");
  }
}

void trataScene(String pFrame) {
  // TOPIC_SETSCENE = "/SETSCENE@";  // Sub topico de assinatura - quando o APP envia para LK um cenario - LK02500004/SETSCENE/010102600004000000000 - onde: 01 = numero do cenario - 01 = sequencia - 02600004 = id do lk - 000 = valor do canal 1 - 000 = valor do canal 2 - 000 = valor do canal 3
  Serial.println("trataScene");
  // 01 01 026 00004 000 000 000 chk
  // Serial.println(pFrame.length());
  if (pFrame.length() > 21) {
    String cena = pFrame.substring(0,2);
    String seq  = pFrame.substring(2,4);
    String tipo = pFrame.substring(4,7);
    String id   = pFrame.substring(7,12);
    String vlr1 = pFrame.substring(12,15);
    String vlr2 = pFrame.substring(15,18);
    String vlr3 = pFrame.substring(18,21);
    String chk  = pFrame.substring(21);

    /*
    Serial.print("Cena :");
    Serial.print(cena);
    Serial.print(" ");
    Serial.print(seq);
    Serial.print(" ");
    Serial.print(tipo);
    Serial.print(" ");
    Serial.print(id);
    Serial.print(" ");
    Serial.print(vlr1);
    Serial.print(" ");
    Serial.print(vlr2);
    Serial.print(" ");
    Serial.print(vlr3);
    Serial.print(" ");
    Serial.print(chk);
    Serial.println(" ");    
    */
    
    int c = cena.toInt();
    int s = seq.toInt();
    int t = tipo.toInt();
    int i = id.toInt();
    int v1 = vlr1.toInt();
    int v2 = vlr2.toInt();
    int v3 = vlr3.toInt();
    int k = chk.toInt();
    int x = c + s + t + i + v1 + v2 + v3;
    if (k == x) {
      // Serial.print(" CheckSum OK");
      // se a cena for a 1
      if (c == 1) {
        // se a sequencia for entre 1 e 30
        if ((s > 0) && (s < 31)) {          
          scenes[s - 1].tipo = t;
          scenes[s - 1].id = i;
          scenes[s - 1].valor1 = v1;
          scenes[s - 1].valor2 = v2;
          scenes[s - 1].valor3 = v3;
          scenes[s - 1].chk = calcChkSumScene(s - 1);
          //  
          debugln(" ");
          debug("Sequencia ");
          debug(String(s - 1));
          debug(" : ");
          debug(String(scenes[s - 1].tipo));
          debug(" ");
          debug(String(scenes[s - 1].id));
          debug(" ");
          debug(String(scenes[s - 1].valor1));
          debug(" ");
          debug(String(scenes[s - 1].valor2));
          debug(" ");
          debug(String(scenes[s - 1].valor3));
          debug(" ");
          debug(String(scenes[s - 1].chk));        
          //
          saveScenes();
          loadScenes();
        }
      }
      
    } else { 
      Serial.println(" CheckSum NOT OK");    
    } 
  } else {
    if (pFrame.length() == 7) {      
      String cena = pFrame.substring(0,2);
      String seq  = pFrame.substring(2,4);
      String tipo = pFrame.substring(4,7);
      int c = cena.toInt();
      int s = seq.toInt();
      int t = tipo.toInt();
      // comando recebido para zeras o cenario armazenado
      if ((c == 1) && (s == 0) && (t == 999)) {
        Serial.println("zeroAndSaveScenes");
        zeroAndSaveScenes();        
      } else { 
        if ((c == 1) && (s == 0) && (t == 998)) {
          Serial.println("DESLIGA TODAS");
          zeroAndSaveScenes();
          scenes[0].tipo = 998;
          saveScenes();
          loadScenes();
        } else {
          if ((c == 1) && (s == 0) && (t == 997)) {
            Serial.println("LIGA TODAS");
            zeroAndSaveScenes();
            scenes[0].tipo = 997;
            saveScenes();
            loadScenes();
          }
        }
      }
    }
  }
}

void setIDMC(String pIDMC) {
  // Versao 0.15 - Trata quando recebe o ID da MC via MQTT
  debug("setIDMC - ");    
  debug(" vMCID : ");
  debug(vMCID);
  debug(" - pIDMC : ");
  debugln(pIDMC);
  
  // 1.06 - Quando recebe o ID da MC - so Grava as credenciais se o ID for diferente do armazenado
  if ((vMCID != pIDMC) || (vMCID_OK == false)) {
    vMCID_OK = true;
    vMCID    = pIDMC;
    saveCredentials();
    loadCredentials();  
    showConfig();  
    // Com a informacao do ID da MC posso assinar os Topicos de Broadcast
    montaTopicosMQTT();
    assinaturasMQTT();
    assinaturasMQTTLocal();
    //publicaTopicSetIPMQTT();
    //publicaTopicStatusMQTT();  
    //if (MQTT.connected()) { publicaTopicSetIPMQTT(); }
    //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
    // versao 0.18 - a verificacao se esta conectado esta na rotina generica de envio
    publicaTopicSetIPMQTT(); 
    publicaTopicStatusMQTT(); 
    //if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicSetIPMQTT(); };
    //if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
  }
}

void setIDLK(String pIDLK) {
  // Versao 0.15 - Trata quando recebe o ID do LK via MQTT
  Serial.println("setIDLK");
  vKitID  = pIDLK;
  vModulo = vKitID.toInt();
  saveCredentials();
  loadCredentials();
  showConfig();
}

void setIPMC(String pIPMC) {
  // Versao 0.15 - Trata quando recebe o IP da MC via MQTT
  debug("setIPMC - ");
  debug(" vMCIP : ");
  debug(vMCIP);
  debug(" - pIPMC : ");
  debugln(pIPMC);  
  /*
  1.06 - Quando recebe o IP da MC - so Grava as credenciais se o IP for valido e se for diferente do armazenado
  vMCIP = pIPMC;
  saveCredentials();
  loadCredentials();
  showConfig();
  */
  // versao 0.18 - MQTT Local na LegoolusMC - isso faz que na rotina cTeclado ele tenta conexao com a MC
  if ((pIPMC.length() > 6) && (vMCIP != pIPMC)) {
    // 1.06 - Quando recebe o IP da MC - so Grava as credenciais se o IP for valido e se for diferente do armazenado
    vMCIP = pIPMC;
    saveCredentials();
    loadCredentials();
    showConfig();
    //
    MQTTLocalOk = true;
    vMsgInicial = vMCIP;
    vMsgInicial.toCharArray(BROKER_MQTT_LOCAL, vMCIP.length() + 1); 
    MQTTLocal.setServer(BROKER_MQTT_LOCAL, BROKER_PORT);   
    MQTTLocal.setCallback(callbackMQTTLocal);      
  }
}

void enviaFrameMQTT() {
  // so envia o frame que a tecla foi pressionada para a MC se o LK possiu ID e IP da MC 
  // versao 0.18 - a variavel vModulo - nao era atualizada com o ID do LK
  vModulo = vKitID.toInt();
  //Serial.print("enviaFrameMQTT : vModulo .. : ");  
  //Serial.print(vModulo);
  //Serial.print(" : ");
  debug("enviaFrameMQTT : vModulo .. : ");  
  debug(String(vModulo));
  debug(" : ");
  
  if (vModulo != 0) {    
    unsigned char frameAux[5]; 
   
    vModulo = vKitID.toInt();
    frame[0] = cHeader;
    frame[1] = vModulo;//cModulo;//0x03;  
    frame[2] = vCanal;//0x06;
    frame[3] = 0x01; // Sempre eh 1 quando pulsador
    int f = 0;
    f = 0x100 - (frame[0] + frame[1] + frame[2] + frame[3]);
    while (f < 0) {
      f = 0x100 + f;
    } 
    frame[4] = f;
    
    //aux = "enviaFrameMQTT : ";
    //Serial.print(aux);
    aux = String(frame[0], HEX);
    aux.concat(" ");
    aux.concat(String(frame[1], HEX));
    aux.concat(" ");
    aux.concat(String(frame[2], HEX));
    aux.concat(" ");
    aux.concat(String(frame[3], HEX));
    aux.concat(" ");
    aux.concat(String(frame[4], HEX));
    aux.toUpperCase();
    //Serial.println(aux);
    debugln(aux);
    aux = "";
    for(int k = 0; k < 5; k++){
      aux += char(frame[k]);
    }          
    
    char frameToMC[06];
      
    frameToMC[0] = frame[0];
    frameToMC[1] = frame[1];
    frameToMC[2] = frame[2];
    frameToMC[3] = frame[3];
    frameToMC[4] = frame[4];
    
    frameToMC[5] = '\0';
  
    char topicToMC[14];
    vMsgInicial = "LK";
    vMsgInicial.concat(cTpNS);
    vMsgInicial.concat(vNS);
    vMsgInicial.concat("@/MC");  // 0.19 - uso do @ para publicacoes que usam o ID do LK
    //vMsgInicial.toCharArray(topicToMC, 14);  
    // 0.19 - Troca do tamanho fixo pelo comando length + 1 
    vMsgInicial.toCharArray(topicToMC, vMsgInicial.length() + 1); 
  
  
    if (MQTT.connected()) { MQTT.publish(topicToMC, frameToMC); }
    // porque mandar para o Broker Local mesmo se o ID da MC nao existir? Posso ter uma MC somente trabalhando como Broker e nao como central de automacao
    // 0.19 - uso do @ para publicacoes que usam o ID do LK
    // A MC nao usa esse topico, ela usa o TOPIC_TO_MC_ALTERNATIVO
    if (MQTTLocal.connected()) { MQTTLocal.publish(topicToMC, frameToMC); }
    if (vMCID_OK) {
      if (MQTT.connected()) { MQTT.publish(TOPIC_TO_MC_ALTERNATIVO, frameToMC); }
      if (MQTTLocal.connected()) { 
        debug("TOPIC_TO_MC_ALTERNATIVO ... : ");        
        debugln(TOPIC_TO_MC_ALTERNATIVO);
        // toda comunicacao entre o LK e a MC eh local
        MQTTLocal.publish(TOPIC_TO_MC_ALTERNATIVO, frameToMC); 
      }      
    } 
  }
  // envia o evento de tecla pressionada para o MQTT
  switch (vCanal) {
  case 4: // a tecla 1 foi pressionada
    aux = "100000";
  break;
  case 5:
    aux = "010000";
  break;
  case 6:
    aux = "001000";
  break;
  case 7:
    aux = "000100";
  break;
  case 8:
    aux = "000010";
  break;
  case 9:
    aux = "000001";
  break;
  default:
    aux = "000000";
  }  
  char topicEvent[14];
  // informa ao mundo qual e o estado de suas "saidas" - tipo sniffer para identificar qual tecla foi pressionada - falta desenvolver um SW para ler essa informacao
  if (MQTT.connected()) { enviaTopicoLKGenericoMQTT("EVENT", aux); }  
  //if (MQTTLocal.connected()) { enviaTopicoLKGenericoMQTT("EVENT", aux); }    
  vEstado = cDebounce;            
}

void inverteCanalATMEL(int pCanal) {
  // mandaFrameTX(0xF5,0x08,0x00);
  // o inverte eh LENTO - use o aciona ON / PFF
  char frm = 0x08;
  if (pCanal == 2) {
    frm = 0x09;
  }
  mandaFrameTX(0xF5,frm,0x00);
  analisaFrameATMEL(frm,0x00);  
  //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
  // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
  if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
}

void acionaCanalATMEL(int pCanal) {
  // mandaFrameTX(0xF5,0x00,0x01);    
  char frm3;
  char frm4;   
  if (pCanal == 1) {    
    frm3 = 0x00; 
    if (dimAtual1 > 0) {
      frm4 = 0x00; 
    } else {
      if (vMaxValueChannel_1 > 0) {
        // existe um valor armazenado que limita o valor maximo do canal
        frm3 = 0x58;
        frm4 = traduzDimmerValuePercentageToByte(vMaxValueChannel_1);
      } else {   
        frm4 = 0x01;
      }
    }  
  } else {
    frm3 = 0x01;
    if (dimAtual2 > 0) {
      frm4 = 0x00;
    } else {
      if (vMaxValueChannel_2 > 0) {
        // existe um valor armazenado que limita o valor maximo do canal
        frm3 = 0x59;
        frm4 = traduzDimmerValuePercentageToByte(vMaxValueChannel_2);
      } else {   
        frm4 = 0x01;
      }
    }
  }
  mandaFrameTX(0xF5,frm3,frm4);
  analisaFrameATMEL(frm3,frm4);
  //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
  // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
  if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
}

void incrementaCanalATMEL(int pCanal) {
  // mandaFrameTX(0xF5,0x5C,0x00);
  char frm = 0x5C;
  if (pCanal == 2) {
    frm = 0x5D;
  }
  mandaFrameTX(0xF5,frm,0x00);
  analisaFrameATMEL(frm,0x00);
  // if (MQTT.connected()) { publicaTopicStatusMQTT(); }
  // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
  if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
}

void desligaTodas() {
  Serial.println("desligaTodas");
  if (cTipo == LKD2E3) { 
    mandaFrameTX(0xF5,0x00,0xF0);
    analisaFrameATMEL(0x00,0xF0);
  } else {
    if ((cTipo == LKR3E3) || (cTipo == LKRGB) || (cTipo == LKLED)) { 
      acionaSaida(1,DESLIGA);
      acionaSaida(2,DESLIGA);
      acionaSaida(3,DESLIGA);
      dimAtual1 = 0;
      dimAtual2 = 0;
      dimAtual3 = 0;
    }
  }
  // if (MQTT.connected()) { publicaTopicStatusMQTT(); }
  // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
  if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
  digitalWrite(LED_2, DESLIGA);
}

void montaEnviaFrameMQTT(int tipo, int id, int vlr1, int vlr2, int vlr3) {
  debugln("montaEnviaFrameMQTT");
  int frm3;
  int frm4;
  int perc;
  String a;      
  char frameToLK[06];
  // Verifica se o comando eh local  
  if ((tipo == cTpNS.toInt()) && (id == vNS.toInt())) {
    debugln("Comando Local");
    if (tipo == LKR3E3) { 
      if (vlr1 > 0) { ligaCanal(1); } else { desligaCanal(1); }
      if (vlr2 > 0) { ligaCanal(2); } else { desligaCanal(2); }
      if (vlr3 > 0) { ligaCanal(3); } else { desligaCanal(3); }
      //if (MQTT.connected()) { publicaTopicStatusMQTT(); }      
      // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
      if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
    } else { 
      if (tipo == LKD2E3) { 
        // canal 1
        if (vlr1 == 0) { 
          // desliga
          frm3 = 0x00;
          frm4 = 0x00;          
        } else {
          if (vlr1 == 100) {
            // liga
            frm3 = 0x00;
            frm4 = 0x01;
          } else {
            // dimeriza
            frm3 = 0x58;
            frm4 = traduzDimmerValuePercentageToByte(vlr1);
          }
        }
        mandaFrameTX(0xF5,frm3,frm4); 
        analisaFrameATMEL(frm3,frm4);
        // canal 2
        if (vlr2 == 0) { 
          // desliga
          frm3 = 0x01;
          frm4 = 0x00;          
        } else {
          if (vlr2 == 100) {
            // liga
            frm3 = 0x01;
            frm4 = 0x01;
          } else {
            // dimeriza
            frm3 = 0x59;
            frm4 = traduzDimmerValuePercentageToByte(vlr2);
          }
        }
        mandaFrameTX(0xF5,frm3,frm4); 
        analisaFrameATMEL(frm3,frm4);
        // if (MQTT.connected()) { publicaTopicStatusMQTT(); }
        // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
        if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
      } else {
        if ((tipo == LKRGB) || (tipo == LKLED)) { 
          if (vlr1 > 0) {             
            perc = int(vlr1);
            dimerizaCanalPerc(1, perc);
          } else { desligaCanal(1); }
          if (vlr2 > 0) { 
            perc = int(vlr2);
            dimerizaCanalPerc(2, perc);
          } else { desligaCanal(2); }
          if (vlr3 > 0) { 
            perc = int(vlr3);
            dimerizaCanalPerc(3, perc);
          } else { desligaCanal(3); }
        }
      }
    }
  } else {
    // comando para outro LK - publica no MQTT no Topico TOPIC_TO_OTHER_LK - LK02600004/MC/F5010001CK
    char topicToLK[14];    
    vMsgInicial = "LK";
    a = String(tipo);
    while (a.length() < 3) { a = "0" + a; } 
    vMsgInicial.concat(a);
    a = String(id);
    while (a.length() < 5) { a = "0" + a; }     
    vMsgInicial.concat(a);
    vMsgInicial.concat("/MC");  
    vMsgInicial.toCharArray(topicToLK, 14); 
    debugln("Comando MQTT : ");
    debug(topicToLK);
    debug("/");
    frameToLK[0] = 0xF5;
    frameToLK[1] = 0x01; // o id do modulo pode ser qualquer um porque nao verifico 
    frameToLK[4] = 0x00; // o chk pode ser qualquer um porque nao verifico 
    frameToLK[5] = '\0';
    //
    // canal 1    
    //
    if (vlr1 == 0) {      
      frameToLK[2] = 0xFF; // quando manda ZERO como canal o frame nao chega, porque user ZERO? PQP!!!
      frameToLK[3] = 0x00;
    } else {
      if (vlr1 == 100) {        
        frameToLK[2] = 0x00;
        frameToLK[3] = 0x01;
      } else {        
        frameToLK[2] = 0x58;
        frameToLK[3] = traduzDimmerValuePercentageToByte(vlr1);        
      }
    }    
    a = String(frameToLK[0], HEX);
    a.concat(" ");
    a.concat(String(frameToLK[1], HEX));
    a.concat(" ");
    a.concat(String(frameToLK[2], HEX));
    a.concat(" ");
    a.concat(String(frameToLK[3], HEX));
    a.concat(" ");
    a.concat(String(frameToLK[4], HEX));
    a.toUpperCase();    
    debug(a);
    debugln(" ");
    if (MQTT.connected()) { MQTT.publish(topicToLK, frameToLK); }
    //
    // canal 2 
    //
    if (vlr2 == 0) {
      frameToLK[2] = 0x01;
      frameToLK[3] = 0x00;
    } else {
      if (vlr2 == 100) {        
        frameToLK[2] = 0x01;
        frameToLK[3] = 0x01;
      } else {        
        frameToLK[2] = 0x59;
        frameToLK[3] = traduzDimmerValuePercentageToByte(vlr2);
      }
    }
    a = String(frameToLK[0], HEX);
    a.concat(" ");
    a.concat(String(frameToLK[1], HEX));
    a.concat(" ");
    a.concat(String(frameToLK[2], HEX));
    a.concat(" ");
    a.concat(String(frameToLK[3], HEX));
    a.concat(" ");
    a.concat(String(frameToLK[4], HEX));
    a.toUpperCase();    
    //
    debug(topicToLK);
    debug("/");
    debug(a);
    debugln(" ");
    if (MQTT.connected()) { MQTT.publish(topicToLK, frameToLK); }
    //
    // canal 3 
    //
    if (vlr3 == 0) {
      frameToLK[2] = 0x02;
      frameToLK[3] = 0x00;
    } else {
      if (vlr3 == 100) {        
        frameToLK[2] = 0x02;
        frameToLK[3] = 0x01;
      } else {        
        frameToLK[2] = 0x5A;
        frameToLK[3] = traduzDimmerValuePercentageToByte(vlr3); 
      }
    }
    //
    a = String(frameToLK[0], HEX);
    a.concat(" ");
    a.concat(String(frameToLK[1], HEX));
    a.concat(" ");
    a.concat(String(frameToLK[2], HEX));
    a.concat(" ");
    a.concat(String(frameToLK[3], HEX));
    a.concat(" ");
    a.concat(String(frameToLK[4], HEX));
    a.toUpperCase();    
    //
    debug(topicToLK);
    debug("/");
    debug(a);
    debugln(" ");
    if (MQTT.connected()) { MQTT.publish(topicToLK, frameToLK); }
    //
  }    
}

void dimerizaCanalPerc(int pCanal, int pPerc) {
  // Dimeriza o canal usando o Percentual tipo 10 ... 100
  Serial.print("dimerizaCanalPerc ");
  Serial.print(pCanal);
  Serial.print(" : ");
  Serial.println(pPerc);
  //Serial.print(" : ");
  int dimAtual = 0;
  int canalAtual = 1;
  switch (pPerc) {
    case 95 ... 100:
      dimerizandoCanal(pCanal, DIM100);
      dimAtual = DIM100;
      //Serial.println("95 -  100%");
    break;
    case 85 ... 94: 
      dimerizandoCanal(pCanal, DIM90);
      dimAtual = DIM90;
      //Serial.println("85 - 94%");  
     break;
     case 75 ... 84: 
      dimerizandoCanal(pCanal, DIM80);
      dimAtual = DIM80;
      //Serial.println("75 - 84%");  
     break;
     case 65 ... 74: 
      dimerizandoCanal(pCanal, DIM70);
      dimAtual = DIM70;
      //Serial.println("65 - 74%");  
     break;
     case 55 ... 64: 
      dimerizandoCanal(pCanal, DIM60);
      dimAtual = DIM60;
      //Serial.println("55 - 64%");  
     break;
     case 45 ... 54: 
      dimerizandoCanal(pCanal, DIM50);
      dimAtual = DIM50;
     // Serial.println("45 - 54%");  
     break;
     case 35 ... 44: 
      dimerizandoCanal(pCanal, DIM40);
      dimAtual = DIM40;
      //Serial.println("35 - 44%");  
     break;
     case 25 ... 34: 
      dimerizandoCanal(pCanal, DIM30);
      dimAtual = DIM30;
      //Serial.println("25 - 34%");  
     break;
     case 15 ... 24: 
      dimerizandoCanal(pCanal, DIM20);
      dimAtual = DIM20;
      //Serial.println("15 - 24%");  
     break;
     case 05 ... 14: 
      dimerizandoCanal(pCanal, DIM10);
      dimAtual = DIM10;
      //Serial.println("05 - 14%");  
     break;
     case 00 ... 04: 
      dimerizandoCanal(pCanal, DIM0);
      dimAtual = DIM0;
      //Serial.println("0 - 4%");  
     break;
  default:
    Serial.println ("erro no dimerizaCanalPerc");   
  }
  if (pCanal == 1) { 
    dimAtual1 = dimAtual; 
    canalAtual1 = canalAtual;
  } else { 
    if (pCanal == 2) {
      dimAtual2 = dimAtual; 
      canalAtual2 = canalAtual;
    } else {
      dimAtual3 = dimAtual;
      canalAtual3 = canalAtual;
    }
  }
  //if (MQTT.connected()) { publicaTopicStatusMQTT(); }
  // versao 0.18 - a verificacao se esta conectado ou nao no broker esta na rotima generica de envio de mensagem MQTT
  if ((MQTT.connected()) || (MQTTLocal.connected())){ publicaTopicStatusMQTT(); };
}

void montaEnviaBroadcast(bool pLiga) {    
    // Comando Broadcast de Liga e Desliga Todas - APP<ID do APP>/BROADCAST/F5FA00F120 
    if ((appToken != "9999999999") && (appToken != "")) { 
      String a;      
      char frameToLK[06];   
      // Topico    
      char*  TOPIC = ""; 
      String vGeneric = "APP";
      vGeneric.concat(appToken);
      vGeneric.concat("/");
      vGeneric.concat("BROADCAST");
      vGeneric.toCharArray(TOPIC, vGeneric.length() + 1); 
      //
      debugln("Comando MQTT : ");
      debug(TOPIC);
      debug("/");
      frameToLK[0] = 0xF5;
      frameToLK[1] = 0xFA; // Broadcast
      frameToLK[2] = 0x01; // Sem Uso
      if (pLiga) { 
        frameToLK[3] = 0xF1; // Liga Todas
      } else {
        frameToLK[3] = 0xF0; // Desiga Todas
      }
      frameToLK[4] = 0x00; // o chk pode ser qualquer um porque nao verifico 
      frameToLK[5] = '\0';
      //    
      a = String(frameToLK[0], HEX);
      a.concat(" ");
      a.concat(String(frameToLK[1], HEX));
      a.concat(" ");
      a.concat(String(frameToLK[2], HEX));
      a.concat(" ");
      a.concat(String(frameToLK[3], HEX));
      a.concat(" ");
      a.concat(String(frameToLK[4], HEX));
      a.toUpperCase();    
      debug(a);
      debugln(" ");
      if (MQTT.connected()) { MQTT.publish(TOPIC, frameToLK); }
    } else {
      debugln("montaEnviaBroadcast : Nao mandou Broadcast porque nao tem AppToken");
    }
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

void voltaUltimaCor() {  
  debug("voltaUltimaCor . : ");
  debugln(vValorAux);
  
  int x      = 0;
  int valor  = 0;
  /*
  String aux = vTempoShutter.substring(0, 3);
  int x1 = aux.toInt();
  debug(" :" );
  debug(aux);
  aux = vTempoShutter.substring(3, 6);  
  int x2 = aux.toInt();
  debug(" : " );
  debug(aux);
  aux = vTempoShutter.substring(6, 9);
  int x3 = aux.toInt();
  debug(" : " );
  debugln(aux);
  valor = fatorDimmerToPerc(x1);
  //if (valor > 0) { dimerizaCanal(1, 6); }
  if (valor > 0) { dimerizandoCanal(1, valor); }
  valor = fatorDimmerToPerc(x2);
  if (valor > 0) { dimerizandoCanal(2, valor); }
  valor = fatorDimmerToPerc(x3);
  if (valor > 0) { dimerizandoCanal(3, valor); }
  */
  for ( int i=1 ; i < 4 ; i++ ) {
    aux = vValorAux.substring(x, x+3);
    int y = aux.toInt();
    valor = fatorDimmerToPerc(y);
    if (valor > 0) { dimerizandoCanal(i, valor); }
    if (i == 1) { dimAtual1 = valor; }
    if (i == 2) { dimAtual2 = valor; }
    if (i == 3) { dimAtual3 = valor; }
    x = x + 3;
  }
  publicaTopicStatusMQTT();
}

int fatorDimmerToPerc(int pFator) {
  /*
   * case 0x01: // 100%
    case 0x02: // 90%
    case 0x03: // 80%
    case 0x04: // 70%
    case 0x05: // 60%
    case 0x06: // 50%
    case 0x07: // 40%
    case 0x08: // 30%
    case 0x09: // 20%
    case 0x0A: // 10%
    case 0x0B: // 5%
    case 0x0C: // 0%

   */
  // Versao 1.02
  // Trasforma o STATUS em Fator Dimmer - usado em voltaUltimaCor
  int retorno = 0;
  //Serial.println("fatorDimmerToPerc ");  
  switch (pFator) {
    case 100: // 100%
      retorno = 1024;      
    break;
    case 90: // 90%
      retorno = 912;      
    break;
    case 80: // 80%
      retorno = 819;      
    break;
    case 70: // 70%
      retorno = 716;
    break;
    case 60: // 60%
      retorno = 614;      
    break;
    case 50: // 50%
      retorno = DIM50;      
    break;
    case 40: // 40%
      retorno = 409;      
    break;
    case 30: // 30%
      retorno = 307;
    break;
    case 20: // 20%
      retorno = 204;
    break;
    case 10: // 10%
      retorno = 102;
    break;
    case 5: // 5%
      retorno = 51;      
    break;
    case 0: // 0%
      retorno = 0;      
    break;
    default:
      Serial.println ("erro no fatorDimmerToPerc");   
      retorno = 0;
  }
  return retorno;
}

// 1.03
void testaMQTT() {

  // 1.10
  if (MQTTState != MQTT.state()) {
    MQTTState = MQTT.state();
    Serial.println("MQTT State : " + String(MQTTState));
    if (MQTTState != 0) {      
      //if (MQTT.connect(CLIENT_MQTT, BROKER_USER, BROKER_PASSWORD)) {
      //  Serial.println("re-connected");        
    } 
  }
  
  
  WIFI_Inter_Erro_Count = 0;
  // versao 0.11 - MQTT - Verifica se esta conectado no MQTT - tentar conectat e se há algo no servidor MQTT
  if (!MQTT.connected()) {
    // 0.19 - Temporizar quando nao consegue connectar - para nao ficar tentando a toda hora
    // 3.100.000 = 1 mim   
    // 500.000 + - 10 segundos
    // 1.09 - reducao o tempo entre tentativas de conexao 
    //if (MQTTCount > 1000000) {    
    if (MQTTCount > 50000) {
      MQTTCount = 0;            
      debugln(" ");
      debugln("Tentando Conectar no MQTT remoto : ");
      if (MQTT.connect(CLIENT_MQTT, BROKER_USER, BROKER_PASSWORD)) {                  
          debug("Conectado ao Broker MQTT REMOTO com sucesso : ");              
          debug(" - ");
          debug(BROKER_MQTT);
          debug(" - ");
          debug(BROKER_USER);
          debug(" - ");
          debugln(BROKER_PASSWORD);                
          MQTTConnected  = true;
          vClientSSID_OK = true;
          vBlink_OK      = true;
          vContBlink     = 0;              
          montaTopicosMQTT();
          assinaturasMQTT();
          // Toda vez que a conexao eh estabelecida - o LK publica seu IP e seu STATUS                            
          publicaTopicStatusMQTT();
          publicaTopicSetIPMQTT();              
          MQTT.loop();              
      }
      else {              
          debugln("Não foi possivel se conectar ao broker MQTT na NUVEM via Internet.");                                        
          MQTTConnected = false;
      }
    } else { 
      //if ((MQTTCount % 51000) == 0) {  Serial.print("MQTTCount : " + String(MQTTCount));  }
      if ((MQTTCount % 100000) == 0) {  debug(" m ");  }
      MQTTCount += 1; 
    }    
  } else { 
    // Versao 1.04 - Temporiza para forcar o DISCONNECT do MQTT 
    //if ((MQTTCountReset % 500000) == 0) {  debug(" l ");  }
    MQTTCountReset += 1;
    // 1.08
    if (MQTTCountReset > 3000000) {
      MQTTCountReset = 0;
      MQTTCount      = 500001;
      vNaoSalvaCredentials = true;
      //debug("MQTT Disconnect");
      //MQTT.disconnect();
    }
    MQTT.loop(); 
  }  
}

void testaMQTTLocal() {
  // versao 0.18 - Se houver IP tenta conectar no Broker MQTT Local      
  if (MQTTLocalOk) {
    if (!MQTTLocal.connected()) { 
      // 3.100.000 = 1 mim  
      if (MQTTLocalCount > 3100000) {
        MQTTLocalCount = 0;
        debug("Tentando Conexao Broker MQTT Local ... ");
        debug(" - ");
        debug(CLIENT_MQTT);
        debug(" - ");
        debug(BROKER_MQTT_LOCAL);
        debug(" - ");
        debug(BROKER_USER);
        debug(" - ");
        debugln(BROKER_PASSWORD);
        if (MQTTLocal.connect(CLIENT_MQTT, BROKER_USER, BROKER_PASSWORD)) {
            debug("Conectado ao Broker MQTT Local com sucesso");
            debug(" - ");
            debug(BROKER_MQTT_LOCAL);
            debug(" - ");
            debug(BROKER_USER);
            debug(" - ");
            debugln(BROKER_PASSWORD);
            MQTTLocalConnected = true;
            vClientSSID_OK     = true;
            vBlink_OK          = true;
            vContBlink         = 0;
            MQTTLocalCount     = 3100000; // se conectou uma vez, ao perder a conexao ele tenta novamente imediatamente, se nao conseguir ira tentar apos 1 mim
            montaTopicosMQTT();
            assinaturasMQTTLocal();
            // Toda vez que a conexao eh estabelecida - o LK publica seu IP e seu STATUS                            
            publicaTopicStatusMQTT();
            publicaTopicSetIPMQTT();
            MQTTLocal.loop();
        }
        else {              
            debugln("Não foi possivel se conectar ao broker MQTT Local");   
            MQTTLocalConnected = false;
        }
      } else { 
        if ((MQTTLocalCount % 51000) == 0) {  
          //Serial.print("MQTTlocalCount : " + String(MQTTLocalCount));  
          if ((MQTTCount % 100000) == 0) {  
            debug("ml ");  
          }
        }
        MQTTLocalCount += 1; 
      }
    } else { MQTTLocal.loop(); }
  }
}

void testaEnviaStatus() {
  // 1.00 - em vez de publicar o STATUS direto -  aguardo um tempo ate receber o ultima solicitacao (interna ou externa) de envio de STATUS
  if (vEnviaStatus) {
    //
    /*
    if ((vContEnviaStatus % 5000) == 0) {
      //debug("vContEnviaStatus ... ");
      //debug(String(vContEnviaStatus));
      debug(" s ");
    }
    */
    //
    vContEnviaStatus += 1;
    if (vContEnviaStatus > 20000) {
      vEnviaStatus = false;
      vContEnviaStatus = 0;          
      vLastStatus = " ";
      publicaTopicStatusMQTT_2();
    }
  }
}

void testaTecla() {
  if (haschanged == 1) {
    haschanged = 0;        
    //
    // Tecla 1
    //
    if (ep0 != estado_p0) {
      ep0 = estado_p0;
      if (estado_p0 == LOW) {
        msgln("p0");
        vContTeclaP0 = 1;
        vCanal = 4;
        digitalWrite(LED_2, LIGA);
        // 1.08 - se houver algum timer rodando para o canal 1 sera cancelado
        vTimerCanal1 = -1;        
        // versao 0.15 - configuracao padrao do PULSADOR 1 do R3E3,RGB e LEDSTRIP - inverte canal 1
        // versao 0.18 - MESMO se existir MC executa o padrao LK e envia o frame teclado para MC - se quiser nao executar o padrao colocal um if (!vMCID_OK) {
        if ((cTipo == LKR3E3) || (cTipo == LKRGB) || (cTipo == LKLED)) {
          inverteCanal(1);              
        } else {
          // 1.08
          if (cTipo == LKSHUTTER) {
            desligaCanal(2); 
            desligaCanal(3); 
            delay(300);                            
            if (vTempoShutter == 0 || vTempoShutter > 300) { 
              vTempoShutter = 30;
            }
            // se o canal ja esta temporizaando - cancela o timer e abre os reles
            if (vTimerCanal1 = -1) {              
              temporizaCanal(1,vTempoShutter * 9);
            } else {
              vTimerCanal1 = -1;
              desligaCanal(1);
            }
          }
        }
        // versao 0.15 - se o MQTT estiver conectado envio o comando para o MQTT e nao para o Socket
        // if (!MQTT.connected()) { vEstado = cSocket; } else { enviaFrameMQTT(); }
        // versao 0.18 - nao tem Socklet somente MQTT local e remoto - se houver MQTT local o LK nao envia o comando de teclado para o remoto
        enviaFrameMQTT(); 
      } else {
        // versao 0.15 - configuracao padrao da tecla do D2E3 - inverte canal 1 e dimeriza canal 1
        if (cTipo == LKD2E3) {
          debug("Cont P0 : ");
          debugln(String(vContTeclaP0));
          // se contador for maior que zero e a dimerizacao ainda nao comecou - inverte o canal
          if ((vContTeclaP0 > 0) && (vContTeclaP0 < 90000)) {
            acionaCanalATMEL(1);
          } 
          vContTeclaP0 = 0;              
        }
        digitalWrite(LED_2, DESLIGA);
      }
    }
    //
    // Tecla 2
    //
    if (ep2 != estado_p2) {
      ep2 = estado_p2;          
      if (estado_p2 == LOW) {             
        msgln("p2");
        vContTeclaP2 = 1;
        vCanal = 5;
        digitalWrite(LED_2, LIGA);
        // 1.08 - se houver algum timer rodando para o canal 1 sera cancelado
        vTimerCanal2 = -1;
        // versao 0.15 - configuracao padrao da tecla do R3E3 - intercala ente o canal 2 e 3
        // versao 0.18 - MESMO se existir MC executa o padrao LK e envia o frame teclado para MC - se quiser nao executar o padrao colocal um if (!vMCID_OK) {
        if ((cTipo == LKR3E3) || (cTipo == LKRGB)) {
          intercalaCanal();              
        } else {
          // 0.15 - configuracao padrao para tecla 2 no LEDSTRIP
          if (cTipo == LKLED) {
            inverteCanal(2);
          } else {
            // 1.08
            if (cTipo == LKSHUTTER) {
              desligaCanal(1); 
              desligaCanal(3); 
              delay(300);                            
              if (vTempoShutter == 0 || vTempoShutter > 300) { 
                vTempoShutter = 30;
              }
              // se o canal ja esta temporizaando - cancela o timer e abre os reles
              if (vTimerCanal2 = -1) {              
                temporizaCanal(2,vTempoShutter * 9);        
              } else {
                vTimerCanal2 = -1;
                desligaCanal(2);
              }
            }
          }
        }
        // versao 0.15 - se o MQTT estiver conectado envio o comando para o MQTT e nao para o Socket
        // if (!MQTT.connected()) { vEstado = cSocket; } else { enviaFrameMQTT(); }
        // versao 0.18 - nao tem Socklet somente MQTT local e remoto - se houver MQTT local o LK nao envia o comando de teclado para o remoto
        enviaFrameMQTT(); 
      } else {
        // versao 0.15 - configuracao padrao da tecla do D2E3 - inverte canal 2 e dimeriza canal 2
        if (cTipo == LKD2E3) {
          debug("Cont P2 : ");
          debugln(String(vContTeclaP2));
          // se contador for maior que zero e a dimerizacao ainda nao comecou - inverte o canal
          if ((vContTeclaP2 > 0) && (vContTeclaP2 < 90000)) {
            acionaCanalATMEL(2);
          } 
          vContTeclaP2 = 0;              
        }
        digitalWrite(LED_2, DESLIGA);
      }
    }
    //
    // Tecla 3
    //
    if (ep4 != estado_p4) {
      ep4 = estado_p4;    
      if (estado_p4 == LOW) {
        msgln("p4");
        vContTeclaP4 = 1;
        vCanal = 6;
        digitalWrite(LED_2, LIGA);
        // versao 0.15 - se existe cenario dispara o cenario se nao apaga todos os canais do modulo 
        // versao 0.18 - se existir MC so manda o frame teclado para MC 
        if (!vMCID_OK) {
          if (vIsScene) {
            msgln("Dispara cena");
            int num = int(sizeof(scenes) / 6 / 4);
            int chk;              
            for ( int i=0 ; i < num ; i++ ) {
              chk = calcChkSumScene(i);
              if ((chk == scenes[i].chk) && (chk != 0)) {
                montaEnviaFrameMQTT(scenes[i].tipo,scenes[i].id,scenes[i].valor1,scenes[i].valor2,scenes[i].valor3);                  
              }
            }              
          } else {
            // quando eh programado o comando broadcast tipo MC para ligar todas ou desligar todas no pulsador
            if (vAllON) {
              debugln("ALL ON"); 
              montaEnviaBroadcast(true);  
            } else {
              if (vAllOFF) {
                debugln("ALL OFF");   
                montaEnviaBroadcast(false);
              } else {
                desligaTodas();    
              }
            }
          }
        }
        digitalWrite(LED_2, DESLIGA);
        //
        // versao 0.15 - se o MQTT estiver conectado envio o comando para o MQTT e nao para o Socket
        // if (!MQTT.connected()) { vEstado = cSocket; } else { enviaFrameMQTT(); }
        // versao 0.18 - nao tem Socklet somente MQTT local e remoto - se houver MQTT local o LK nao envia o comando de teclado para o remoto
        enviaFrameMQTT(); 
      } 
    }  
    
    // versao 1.04 - Exclusao suporte LK ER6
    /*
    // 
    // FALTA implementar o LK ER6
    //
    if (ep5 != estado_p5) {
      ep5 = estado_p5;
  
      if (estado_p5 == LOW) {
        msgln("p5");
        vCanal = 7;
        digitalWrite(LED_2, LIGA);
        // versao 0.18
        //vEstado = cSocket; 
      }
    }
  
    if (ep12 != estado_p12) {
      ep12 = estado_p12;
  
      if (estado_p12 == LOW) {
        msgln("p12");
        vCanal = 8;
        digitalWrite(LED_2, LIGA);
        // versao 0.18
        // vEstado = cSocket; 
      }
    }
  
    if (ep14 != estado_p14) {
      ep14 = estado_p14;    
      if (estado_p14 == LOW) {
        msgln("p14");
        vCanal = 9;
        digitalWrite(LED_2, LIGA);
        // versao 0.18
        // vEstado = cSocket;
      } 
    }
    */
  }
}

// Versao 1.05
void zeraMCInfo() {
  //debug("TOPIC_ZERA_MC_INFO : " + strMSG);
  debug("TOPIC_ZERA_MC_INFO");
  // 1.04 - Ve houver MQTT Local, zera ID e IP da MC
  if (MQTTLocalOk) {
    MQTTLocalOk = false;
    vMCIP = " ";
    vMCID = " ";
    saveCredentials();          
    loadCredentials();
    showConfig();
  }
}

void zeraALLInfo() {
  //debug("TOPIC_ZERA_MC_INFO : " + strMSG);
  debug("TOPIC_ZERA_ALL_INFO");
  // 1.04 - zera todas infos
  eraseCredentials();          
  loadCredentials();
  showConfig();
}

// 1.06 analisaFrameATMEL - quando recebe um comando de INVERSAO, quando o ATMEL nao terminar o fadein ou fadeout nao pode enviar outro comando INVERTE no mesmo canal porque o ATMEL nao aceita e o STATUS fica errado
/*
void testaBloqueiaInverteD2() {
  if (vBloqueiaInverteD2_1) {
    if (vContBloqueiaInverteD2_1 > 0) {
      vContBloqueiaInverteD2_1  += 1;
      if (vContBloqueiaInverteD2_1 > 10000) {
        vContBloqueiaInverteD2_1 = 0;
        vBloqueiaInverteD2_1 = false;
      }
    }
  }
  if (vBloqueiaInverteD2_2) {
    if (vContBloqueiaInverteD2_2 > 0) {
      vContBloqueiaInverteD2_2  += 1;
      if (vContBloqueiaInverteD2_2 > 10000) {
        vContBloqueiaInverteD2_2 = 0;
        vBloqueiaInverteD2_2 = false;
      }
    }
  }
}
*/
