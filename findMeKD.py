import os
import sys
import time
import pandas as pd
import paho.mqtt.client as mqtt
sys.path.insert(1, '../')
from comum import p, pt, pj
from comumDB import consulta_instalacao
from comumWIFI import calcular_distancia_diferenca

os.system('cls')

if len(sys.argv) <= 1:  
    instalacao = 1
else:
    instalacao = int(sys.argv[1])
    
show = False
if len(sys.argv) >= 2:
    if sys.argv[2].lower() == 'show': 
        show = True

global_redes = []
# armazena o estado do KD - off = desligado - on = ligado - col = coletando
gKD = {}

# MQTT
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
    if "SCANSSID" in topico:
        set_status(topico,"col")
        redes = add_bssid(topico, msg)        
        if len(redes) > 0 : analisa(redes)

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

def set_bssid(topico, msg):
    global gKD
    if len(topico) > 8:
        kd = topico[:9]
        if kd in gKD:
            gKD[kd]['bssids'].append(msg)

def add_bssid(topico, msg):
    if 'FIM' in msg:
        set_status(topico,"off")
        global_redes_ = monta_frame(topico)
        #pj(global_redes_)        
        return global_redes_
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
            sinal_  = (int(dBm) + 100) * 2  # (−54 dBm + 100) * 2 de dBm para %
            it = {
                "ssid"  : "ssid",
                "bssid" : bssid_.lower(),
                "sinal" : sinal_,
                "dBm"   : dBm,
                "sinais": []
            }
            redes_.append(it)
    gKD[kd]['redes'] = redes_
    return redes_

def connect_mqtt():
    client = mqtt.Client()
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_BROKER)    
    client.loop_forever()
    return True

def scanner_mqtt():
    redes = []
    return redes

def scanner_mqtt_mokado():
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

def analisa(redes_):
    if len(redes_) > 0:
        # redes = recupera todas as redes bssid e coloca o sinal de cada area
        # TO DO isso pode ser armazenado em ves de ficar montando toda vez
        for local in instalacaoObj['locais']:
            redes = []
            areas = []
            for area in local['areas']:
                if area['nome'] not in areas: areas.append(area['nome'])
                for spectro in area['espectros']:
                    index = next((item for item, d in enumerate(redes) if d['bssid'] == spectro['bssid']),-1)
                    if index == -1:
                        it = {
                            'ssid'  : spectro['ssid'],
                            'bssid' : spectro['bssid'],
                            area['nome'] : spectro['c']
                        }
                        redes.append(it)
                    else:
                        redes[index][area['nome']] = spectro['c']
            for rede in redes:
                for area in areas:
                    if area not in rede:
                        rede[area] = 0
            #pj(redes)                
            # - scanner -----------------------------------------------------------------------------------------------
            soma  = {}
            count = {}
            for area in areas: 
                soma[area] = 0
                count[area] = 0
            # localiza no array "redes" o bssid que o modulo informou, inclui a coluna "ATUAL" em redes e calcula a distancia
            # redes_ -> from KD
            # redes  -> sinal de cada bssid em cada area
            # calcula a "distancia" em dBm
            for rede_ in redes_:
                print('>',rede_['bssid'])
                index = next((item for item, d in enumerate(redes) if d['bssid'] == rede_['bssid']),-1)
                if index > -1:
                    #print(rede_['bssid'],':',rede_['sinal'],'->',redes[index])
                    #redes[index]['Atual'] = rede_['sinal']
                    redes[index]['Atual'] = rede_['sinal'] if rede_['sinal'] < 100 else 100 
                    for area in areas:
                        if area in redes[index]:
                            #redes[index][area+'_'] = calcular_distancia_diferenca(int(rede_['sinal']), redes[index][area])
                            redes[index][area+'_'] = calcular_distancia_diferenca(rede_['sinal'], redes[index][area])
                            soma[area] += redes[index][area+'_']
                            count[area] += 1
                            #print(area,'->',rede_['sinal'],redes[index][area])
                            #print(redes[index])
                            #print(calcular_distancia_diferenca(int(rede_['sinal']), redes[index][area]))
                            #print(redes[index])
                            #print(redes[index]['Atual'],rede_['sinal'])
                            #exit()
                            #break
                        else:
                            redes[index][area+'_'] = None
                    #pj(redes[index])
                    #print(redes[index]['Atual'],rede_['sinal'])
                    #exit()
                    #break
            #pj(redes)                        
            #exit()
            # - probabilidade-------------------------------------------------------------------------------------------
            pt('Local',local['nome'])
            print(count)
            print(soma)
            # inversos = {k: 1 / v for k, v in soma.items()}
            # print(inversos)
            # soma_inversos = sum(inversos.values())
            # print(soma_inversos)
            # probabilidades = {k: v/soma_inversos for k, v in inversos.items()}
            # print(probabilidades)
            # print('------------------')_
            inversos = {k: 1 / v if v != 0 else 0 for k, v in soma.items()}
            soma_inversos = sum(inversos.values())
            print('soma_inversos',soma_inversos)
            if soma_inversos == 0:
                probabilidades = {k: 0 for k in inversos.keys()}
            else:
                probabilidades = {k: v / soma_inversos for k, v in inversos.items()}
            print('probabilidades',probabilidades)                
            area_provavel = -1
            area_provavel_percentual = 0
            for i, pr in enumerate(probabilidades):
                pr_ = probabilidades[pr] * 100 
                if area_provavel_percentual < pr_: 
                    area_provavel_percentual = pr_
                    area_provavel = i
            for i, pr in enumerate(probabilidades):
                pr_ = probabilidades[pr] * 100 
                if i == area_provavel:
                    pt(pr,'%.2f' % pr_ +' % <----')
                else:
                    pt(pr,'%.2f' % pr_ +' %')
            p('-')
            # - display tabulado - debug ------------------------------------------------------------------------------
            if show:
                pd.set_option('display.max_rows', None)
                colunas_ordenadas = ['ssid', 'bssid', 'Atual']
                for area in areas: colunas_ordenadas.append(area+'_')
                df_redes = pd.DataFrame(redes)
                print(df_redes[colunas_ordenadas])
                # f = open("modelo.txt", "w")
                # for col in colunas_ordenadas: f.write(col+';')
                # f.write('\n')
                # for rede in redes:
                #     print(rede)
                #     for col in colunas_ordenadas:
                #         if col in rede:
                #             if str(rede[col]).find('0.') > -1:
                #                 aux = str(rede[col])[:8].replace('.',',')
                #             else:
                #                 aux = str(rede[col])
                #             f.write(aux+';')
                #         else:
                #             f.write('0'+';')
                #     f.write('\n')
                # f.close()

        #time.sleep(10)
        #os.system('cls')

instalacaoObj = consulta_instalacao(instalacao)
connect_mqtt()