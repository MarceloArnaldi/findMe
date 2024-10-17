import os
import sys
import paho.mqtt.client as mqtt

os.system('cls')
sys.path.insert(1, '../')

from comum      import p, pj, pt, pausa
from comumDB    import consulta_instalacao, grava_instalacao, exclui_todos_espectros, registra_espectros
from comumDB    import existe_area, add_area
from comumDB    import existe_local
from comumWIFI  import scanner_media, monta_espectros, calcular_distancia_diferenca

instalacao_id = 1
local         = os.environ.get('LOCAL')
area          = os.environ.get('AREA')

pt('INSTALACAO',instalacao_id)
pt('LOCAL',local)
pt('AREA',area)

instalacao = consulta_instalacao(instalacao_id)

mqtt_ok = False
global_redes = []
gKD = {}

MQTT_BROKER = "mqtt.legoolus.com.br"
MQTT_PORT = 1883 
MQTT_USERNAME = "obtnegwv"
MQTT_PASSWORD = "7_JhuAE6nm02"
MQTT_TOPIC = "KD0100001"
MQTT_TOPIC2 = "KD0100001@"
MQTT_TOPIC3 = "KD0100002@"

def on_connect(client, userdata, flags, rc):
    if rc==0:
        print('MQTT : Ready!')
        client.subscribe(MQTT_TOPIC + "/#")
        client.subscribe(MQTT_TOPIC2 + "/#")
        client.subscribe(MQTT_TOPIC3 + "/#")
    else:
        print(f"Falha na conexão. Código de retorno: {rc}")

def on_message(client, userdata, msg):
    #try:
    #pt(msg.topic, msg.payload.decode())
    analisa_topic(msg.topic, msg.payload.decode())
    #except:    
    #    print(f"Recebido.: Tópico: {msg.topic} | Mensagem: {msg.payload}")

def analisa_topic(topico, msg):
    global gKD
    if 'STATUS' in topico:
        set_status(topico,"on")
        del_bssid(topico)
    if "SCANSSID" in topico:
        set_status(topico,"col")
        redes = add_bssid(topico, msg)        
        if len(redes) > 0 : 
            grava_espectros(redes)
            pj(gKD)

def set_status(topico,status):
    global gKD
    if len(topico) > 8:
        kd = topico[:9]
        if kd not in gKD: 
            gKD[kd] = {
                "status" : status,
                "bssids" : []
            }
        else:
            gKD[kd]['status'] = status

def del_bssid(topico):
    global gKD
    if len(topico) > 8:
        kd = topico[:9]
        if kd in gKD:
            gKD[kd]['bssids'] = []

def set_bssid(topico, msg):
    global gKD
    if len(topico) > 8:
        kd = topico[:9]
        if kd in gKD:
            gKD[kd]['bssids'].append(msg)

def add_bssid(topico, msg):
    if 'FIM' in msg:
        set_status(topico,"off")
        return monta_frame(topico)        
    else:        
        set_bssid(topico, msg)
        return []

def monta_frame(topico):
    global gKD
    redes_ = []
    if len(topico) > 8:
        kd = topico[:9]
        for bssid in gKD[kd]['bssids']:
            sinal   = bssid.find('-')
            bssid_  = bssid[:sinal-1]
            dBm     = bssid[sinal:]
            sinal_  = (int(dBm) + 100) * 2
            #sinal_  = (int(dBm) + 100) * 1.7
            sinal_   = min(100, max(0, sinal_))
            it = {
                "ssid"  : "ssid",
                "bssid" : bssid_.lower(),
                "sinal" : sinal_,
                "dBm"   : dBm
            }
            redes_.append(it)
    #gKD[kd]['redes'] = redes_
    return redes_

def connect_mqtt():
    client = mqtt.Client()
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_BROKER)    
    client.loop_forever()
    return True

def grava_espectros(redes):
    global mqtt_ok
    global local
    global area
    global instalacao_id
    global instalacao
    print(f'Gravando Espectros na Area {area} da Zona {local}')
    espectros = []
    espectros = monta_espectros(espectros, redes, 'c')
    exclui_todos_espectros(instalacao, local, area)
    registra_espectros(instalacao, local, area, espectros)
    grava_instalacao(instalacao_id, instalacao)
    troca_area()
    #pj(instalacao)

def troca_area():
    global mqtt_ok
    global local
    global area
    global instalacao_id
    global instalacao
    local = input(f'Entre com a ZONA ({local}): ').strip() or local
    existe, _ = existe_local(instalacao, local)
    if existe:
        area = input(f'Entre com a AREA ({area}): ').strip() or area
        existe, _, _ = existe_area(instalacao, local, area)
        if existe:
            print('ok')
        else:
            print('Area inexistente')
            exit()
    else:
        print('Zona inexistente')
        exit()

    if not mqtt_ok: 
        connect_mqtt()
        mqtt_ok = True

troca_area()


# sai = False
# while not sai:
#     area = input(f'Entre com a AREA ({area}): ').strip() or area
#     if area.lower() == 'sai':
#         sai = True
#         break
#     print('LOCAL    :',local)
#     print('AREA     :',area)
#     # - verifica se area existe ---------------------------------------------
#     print('')
#     existe, _, _ = existe_area(instalacao, local, area)
#     if (existe):
#         print('area existe!')
#         pausa()
#     else:
#         print('area nao existe!')
#         resposta = input(f'Confirma a criação da area "{area}" (s/n) ? ')
#         if resposta.lower() == 's':
#             p(f'criando area ... "{area}" em "{local}"')
#             add_area(instalacao, local, area)
#             grava_instalacao(instalacao_id, instalacao)
#             instalacao = consulta_instalacao(instalacao_id)
#             pj(instalacao)
#             pausa('',5)
#         else:
#             p('area nao criada, scanner cancelado!')
#         break
#     os.system('cls')
#     # - scanner -------------------------------------------------------------
#     redes = scanner_media()
#     espectros = []
#     espectros = monta_espectros(espectros, redes, 'c')
#     exclui_todos_espectros(instalacao, local, area)
#     registra_espectros(instalacao, local, area, espectros)
#     grava_instalacao(instalacao_id, instalacao)
#     pj(instalacao)