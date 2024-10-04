import os
import re
import json
import time
import subprocess
from pymongo import MongoClient

mongo_config = os.environ.get('MONGO_CONFIG')
#client = MongoClient("mongodb+srv://marceloarnaldi:BITh5VIzm3vY3Eoc@clusterdev.syoui.mongodb.net/?retryWrites=true&w=majority&appName=ClusterDev")
client = MongoClient("mongodb+srv://"+mongo_config+"/?retryWrites=true&w=majority&appName=ClusterDev")
db = client['ClusterDev']
collection = db['instalacoes']

def consulta_instalacao(instalacao_id):
    documento = collection.find_one({"instalacao": instalacao_id})
    del documento['_id']
    return documento    
# - wifi -----------------------------------------------------------------------------------------------
def listar_redes_wifi():
    resultado = subprocess.run(["netsh", "wlan", "show", "networks", "mode=bssid"], capture_output=True, text=True)
    return resultado.stdout

def extrair_redes_wifi(texto):
    padrao_ssid = re.compile(r'^SSID\s+\d+\s+:\s*(.*)', re.IGNORECASE)
    padrao_bssid = re.compile(r'BSSID\s+\d+\s+:\s([0-9a-f:]+)')
    padrao_sinal = re.compile(r'Sinal\s+:\s(\d+)%')

    redes = []
    ssid_atual = None

    for linha in texto.splitlines():
        match_ssid = padrao_ssid.search(linha)
        if match_ssid:
            ssid_atual = match_ssid.group(1) if match_ssid.group(1).strip() else "SSID Desconhecido"

        match_bssid = padrao_bssid.search(linha)
        if match_bssid:
            bssid_atual = match_bssid.group(1)

        match_sinal = padrao_sinal.search(linha)
        if match_sinal:
            sinal_atual = match_sinal.group(1)
            it = {
                "ssid"  : ssid_atual,
                "bssid" : bssid_atual,
                "sinal" : sinal_atual
            }
            redes.append(it)
    return redes

def get_ssid():
    wifi_dados = listar_redes_wifi()
    ssids = extrair_redes_wifi(wifi_dados)
    return ssids
# ------------------------------------------------------------------------------------------------------
def numero_entre_intervalos(num, espectro):
    return min(int(espectro['n']), int(espectro['s'])) <= num <= max(int(espectro['n']), int(espectro['s'])) and \
           min(int(espectro['l']), int(espectro['o'])) <= num <= max(int(espectro['l']), int(espectro['o']))

def onde_estou(data):
    areas = []
    redes = get_ssid()
    for rede in redes:
        #print('rede --->',rede)
        ssid  = rede['ssid']
        bssid = rede['bssid']
        sinal = int(rede['sinal'])
        for local in data['locais']:    
            for area in local['areas']:
                for especto in area['espectros']:
                    if especto['bssid'] == bssid:
                        #print('achou -->', area['nome'], bssid, ': espectro :',especto['n'], especto['s'], especto['l'], especto['o'])
                        if (numero_entre_intervalos(sinal, especto)):
                            #print('onde estou -->', local['nome'],':',area['nome'],':',ssid,':',sinal)
                            areas.append({
                                    "local": local['nome'],
                                    "area" : area['nome'],
                                    "peso" : 1
                                })
    return areas
# ------------------------------------------------------------------------------------------------------
os.system('cls')
instalacaoObj = consulta_instalacao(1) 
#print(json.dumps(instalacaoObj,indent=4))
while (True):
    areas = onde_estou(instalacaoObj)
    if len(areas) > 0:
        for area in areas:
            print(area['area'])
    time.sleep(3)