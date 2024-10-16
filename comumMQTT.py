import paho.mqtt.client as mqtt

MQTT_BROKER = "mqtt.legoolus.com.br"
MQTT_PORT = 1883 
MQTT_USERNAME = "obtnegwv"
MQTT_PASSWORD = "7_JhuAE6nm02"
MQTT_TOPIC = "KD0100001"
MQTT_TOPIC2 = "KD0100001@"

def on_connect(client, userdata, flags, rc):
    if rc==0:
        print('conectado ao MQTT, assinando')
        client.subscribe(MQTT_TOPIC + "/#")
        client.subscribe(MQTT_TOPIC2 + "/#")
    else:
        print(f"Falha na conexão. Código de retorno: {rc}")

def on_message(client, userdata, msg):
    try:
        print(f"Recebido: Tópico: {msg.topic} | Mensagem: {msg.payload.decode()}")
    except:    
        print(f"Recebido: Tópico: {msg.topic} | Mensagem: {msg.payload}")

def connect_mqtt():
    client = mqtt.Client()
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_BROKER)
    print('Ready!')
    #client.loop_forever()
    return True

def scanner_mqtt():
    redes = []
    return redes

def scanner_mqtt_mokado():
    print('scanner_mqtt_mokado')
    it = [
                {
                    "ssid": "Sala de TV.o,",
                    "bssid": "fa:8f:ca:6e:63:9e",
                    "sinal": "91",
                    "sinais": []
                },
                {
                    "ssid": "SSID Desconhecido",
                    "bssid": "78:8c:b5:51:99:93",
                    "sinal": "86",
                    "sinais": []
                },
                {
                    "ssid": "SSID Desconhecido",
                    "bssid": "78:8c:b5:51:ad:b3",
                    "sinal": "35",
                    "sinais": []
                },
                {
                    "ssid": "CUNHA",
                    "bssid": "48:2c:d0:8e:3a:e8",
                    "sinal": "81",
                    "sinais": []
                },
                {
                    "ssid": "CUNHA",
                    "bssid": "48:2c:d0:8e:3a:ec",
                    "sinal": "46",
                    "sinais": []
                },
                {
                    "ssid": "[LG_CeilingCassette A/C]bff6",
                    "bssid": "a6:36:c7:ff:bf:f6",
                    "sinal": "50",
                    "sinais": []
                },
                {
                    "ssid": "[LG_CeilingCassette A/C]2a10",
                    "bssid": "a6:36:c7:9d:2a:10",
                    "sinal": "81",
                    "sinais": []
                },
                {
                    "ssid": "Pat&Victor Twibi",
                    "bssid": "24:fd:0d:e3:dd:ee",
                    "sinal": "80",
                    "sinais": []
                },
                {
                    "ssid": "ArnaldiNET",
                    "bssid": "a6:33:d7:01:65:cf",
                    "sinal": "86",
                    "sinais": []
                },
                {
                    "ssid": "ArnaldiNET",
                    "bssid": "a4:33:d7:01:65:c8",
                    "sinal": "85",
                    "sinais": []
                },
                {
                    "ssid": "PAT&VICTOR_2G",
                    "bssid": "68:02:b8:f9:bf:42",
                    "sinal": "86",
                    "sinais": []
                },
                {
                    "ssid": "PAT&VICTOR_5G",
                    "bssid": "68:02:b8:f9:bf:43",
                    "sinal": "62",
                    "sinais": []
                },
                {
                    "ssid": "Arnaldi",
                    "bssid": "7a:8c:b5:21:99:92",
                    "sinal": "89",
                    "sinais": []
                },
                {
                    "ssid": "Arnaldi",
                    "bssid": "7a:8c:b5:71:ad:b3",
                    "sinal": "35",
                    "sinais": []
                },
                {
                    "ssid": "Arnaldi",
                    "bssid": "7a:8c:b5:71:99:93",
                    "sinal": "87",
                    "sinais": []
                }
            ]
    return it