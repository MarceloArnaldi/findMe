import os
import re
import sys
import json
import subprocess
from pymongo import MongoClient

# - rotinas mongo ------------------------------------------------------------------------------------------------
client = MongoClient("mongodb+srv://marceloarnaldi:BITh5VIzm3vY3Eoc@clusterdev.syoui.mongodb.net/?retryWrites=true&w=majority&appName=ClusterDev")
db = client['ClusterDev']
collection = db['instalacoes']
# - Recupera Instalacao----------------------------------------------------------------------------
def consulta_instalacao(instalacao_id):
    documento = collection.find_one({"instalacao": instalacao_id})
    del documento['_id']
    return documento
# - area exist ? ---------------------------------------------------------------------------------
def area_existe(data, local_, area_):
    for local in data.get("locais", []):
        if local['nome'] == local_:
            for x, area in enumerate(local.get("areas",[])):
                if area.get("nome") == area_:
                    return True, x
    return False, -1
# - area espectro ? ---------------------------------------------------------------------------------
def espectro_existe(data, local_, area_, bssid_):
    for local in data.get("locais", []):
        if local['nome'] == local_:
            for x, area in enumerate(local.get("areas",[])):
                if area.get("nome") == area_:                    
                    for y, espectro in enumerate(area.get("espectros",[])):
                        if espectro['bssid'] == bssid_:
                            return True, y
    return False, -1
# ------------------------------------------------------------------------------------------------      
def grava_instalacao(instalacao_id, obj):
    locais = obj['locais']
    collection.update_one(
        {"instalacao": instalacao_id},
        {"$set": {"locais": locais}}
    )
# - Exclui TODOS espectro ---------------------------------------------------------------------------
def exclui_todos_espectros(data, local_, area_):
    instalacao_id = data['instalacao']
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    if local_index > -1:        
        area_existe_, area_index = area_existe(data, local_, area_)
        if area_existe_:
            data['locais'][local_index]['areas'][area_index]['espectros'] = []
            grava_instalacao(instalacao_id, data)    
# - recupera TODOS espectros-------------------------------------------------------------------------
def recupera_todos_espectros(data, local_, area_):
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    if local_index > -1:        
        area_existe_, area_index = area_existe(data, local_, area_)
        if area_existe_:
            return data['locais'][local_index]['areas'][area_index]['espectros']
# - Registra o sinal dos SSID de cada ponto ---------------------------------------------------------
def registra_espectro(data, local_, area_, espectros_sinal_, posicao_):
    instalacao_id = data['instalacao']
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    if local_index > -1:        
        area_existe_, area_index = area_existe(data, local_, area_)
        if area_existe_:
            for espectro in espectros_sinal_:
                espectro_existe_, espectro_index = espectro_existe(data, local_, area_, espectro['bssid'])
                if not espectro_existe_ and posicao_ == 'c':
                    espectro_ = {
                        "ssid"   : espectro['ssid'],
                        "bssid"  : espectro['bssid'],
                        posicao_ : espectro['sinal']
                    }
                    data['locais'][local_index]['areas'][area_index]['espectros'].append(espectro_)
                else:
                    data['locais'][local_index]['areas'][area_index]['espectros'][espectro_index][posicao_] = espectro['sinal']
                grava_instalacao(instalacao_id, data)   
# - rotinas wifi -------------------------------------------------------------------------------------------------
def get_ssid_top(top):
    wifi_dados = listar_redes_wifi()
    ssids = extrair_redes_wifi(wifi_dados)
    print('espectros_sinal_ordenado')
    print(ssids)
    espectros_sinal_ordenado = sorted(ssids, key=lambda x: x['sinal'], reverse=True)    
    return espectros_sinal_ordenado[:top]
# - Get SSID da coordenada usando como referencia o Top 6 -------------------------------------------
def get_ssid(ssid_top6):
    wifi_dados = listar_redes_wifi()
    ssids = extrair_redes_wifi(wifi_dados)
    ssid_intersecao = [
        item for item in ssids if any(
            item['bssid'] == top['bssid'] for top in ssid_top6
        )
    ]
    return ssid_intersecao
# - rotinas genericas wifi ---------------------------------------------------------------------------------------
def calcular_distancia(rssi, p0, n):
    return 10 ** ((p0 - rssi) / (10 * n))

def obter_nome_adaptador_wifi():
    try:
        resultado = subprocess.check_output(['netsh', 'interface', 'show', 'interface'], encoding='utf-8')
        padrao = re.compile(r'Habilitado\s+Conectado\s+Dedicado\s+(Wi-Fi\s*\d*)')
        adaptadores = padrao.search(resultado)
        if adaptadores:
            return adaptadores.group(1)
        else:
            print("Nenhum adaptador Wi-Fi encontrado.")
            return None
    except subprocess.CalledProcessError as e:
        print("Erro ao obter o nome do adaptador Wi-Fi:", e)

def listar_redes_wifi():
    resultado = subprocess.run(["netsh", "wlan", "show", "networks", "mode=bssid"], capture_output=True, text=True)
    return resultado.stdout

def extrair_redes_wifi(texto):
    # Expressão regular para capturar SSID, BSSID e Sinal
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
            #redes.append((ssid_atual, bssid_atual, sinal_atual))
            it = {
                "ssid"  : ssid_atual,
                "bssid" : bssid_atual,
                "sinal" : sinal_atual
            }
            redes.append(it)
    return redes
# - main --------------------------------------------------------------------------------------------------
os.system('cls')

if len(sys.argv) <= 1:
    print('informe a posicao no local. c = centro, n = norte, s = sul, etc')
    exit()
if os.environ.get('LOCAL') == None:
    print('informe o local. SET LOCAL=andar_1')
    exit()
if os.environ.get('AREA') == None:
    print('informe o area. SET AREA=Sala de Estar')
    exit()

local   = os.environ.get('LOCAL')
area    = os.environ.get('AREA')
posicao = sys.argv[1].lower()

print('LOCAL    :',local)
print('AREA     :',area)
print('POSICAO  :',posicao)

instalacao = 1
instalacaoObj = consulta_instalacao(instalacao)
if posicao == 'c':
    exclui_todos_espectros(instalacaoObj, local, area )
    espectros_sinal_top = get_ssid_top(6)
    #print(json.dumps(espectros_sinal_top,indent=4))
    registra_espectro(instalacaoObj, local, area, espectros_sinal_top, posicao)
else:
    espectros_sinal_top = recupera_todos_espectros(instalacaoObj,local,area)
    espectros_sinal = get_ssid(espectros_sinal_top)
    registra_espectro(instalacaoObj, local, area, espectros_sinal, posicao)