import os
import re
import time
import pyautogui
import subprocess
from comum import p, pl, pausa, atualiza_redes_pc

# - existe a rede ------------------- ---------------------------------------------------------------
def get_bssid(redes, bssid_):
    objeto = next((item for item in redes if item['bssid'] == bssid_),None)
    return objeto
def get_index_bssid(redes, bssid_):
    index = next((item for item, d in enumerate(redes) if d['bssid'] == bssid_),-1)
    return index
# - recupera as redes com maior sinal ---------------------------------------------------------------
def get_ssid_top(top):
    wifi_dados = listar_redes_wifi()
    ssids = extrair_redes_wifi(wifi_dados)
    print('espectros_sinal_ordenado')
    print(ssids)
    espectros_sinal_ordenado = sorted(ssids, key=lambda x: x['sinal'], reverse=True)    
    return espectros_sinal_ordenado[:top]
# - Get SSID simples --- ----------------------------------------------------------------------------
def scanner_basic():
    wifi_dados = listar_redes_wifi()
    redes = extrair_redes_wifi(wifi_dados)
    wifi_dados = listar_redes_wifi()
    redes_ = extrair_redes_wifi(wifi_dados)        
    for rede in redes_:
        index = get_index_bssid(redes, rede['bssid'])
        if index == -1:
            redes.append(rede)
        else:
            redes[index] = rede
    return redes
# - Get SSID ONE for cardial ------------------------------------------------------------------------
def scanner_filter(local, bssids_):
    # pausa(f'Va para o {local}!',10)
    # atualiza_redes_pc()
    # pausa(f'Va para o {local}!',5)
    p(f'Scaneando redes {local} ...')
    for _ in range(3):
        os.system('cls')
        wifi_dados = listar_redes_wifi()
        redes = extrair_redes_wifi(wifi_dados)
        redes = [item for item in redes if item['bssid'] in bssids_]
        for rede in redes: print(rede)
        pyautogui.click(1700,1060)
        pausa()
    os.system('cls')
    return redes
# - Get SSID ONE comparando com a direcao que aumenta ou diminui o sinal ----------------------------
def scanner_comparando_filter(ponto, local, direcao, bssids_):
    pausa(f'Va para o {local}!',10)
    atualiza_redes_pc()
    pausa(f'Va para o {local}!',5)
    p(f'Scaneando redes {local} ...')
    wifi_dados = listar_redes_wifi()
    redes = extrair_redes_wifi(wifi_dados)
    redes = [item for item in redes if item['bssid'] in bssids_]
    for x in range(5):        
        print(f'scanning {x} ...', end='\r')
        wifi_dados = listar_redes_wifi()
        redes_ = extrair_redes_wifi(wifi_dados)
        redes_ = [item for item in redes_ if item['bssid'] in bssids_]
        print(f'scanning {x} ... found {len(redes_)}', end='\r')
        for rede in redes_:            
            index = get_index_bssid(redes, rede['bssid'])
            if index == -1:
                redes.append(rede)
            else:
                if rede['sinal'] > redes[index]['sinal']:
                    if ponto == direcao.get(rede['bssid'], None):
                        redes[index]['sinal'] = rede['sinal']
                else:
                    if rede['sinal'] < redes[index]['sinal']:
                        if ponto != direcao.get(rede['bssid'], None) and direcao.get(rede['bssid'], None) != None:
                            redes[index]['sinal'] = rede['sinal']
        pyautogui.click(1700,1060)
        pausa()
    print('')
    for rede in redes: print(rede)
    pausa()
    os.system('cls')
    return redes
# - Get SSID media ----------------------------------------------------------------------------------
def scanner_media():
    area = os.environ.get('AREA')
    pausa(f'Va para centro da area : {area}',10)
    atualiza_redes_pc()
    pausa(f'Va para centro do area : {area}',5)
    p(f'Scaneando redes ...')
    sinais = []
    wifi_dados = listar_redes_wifi()
    redes = extrair_redes_wifi(wifi_dados)
    for x in range(10):        
        #print(f'scanning {x} ...', end='\r')
        pl(f'scanning {x} ...')
        wifi_dados = listar_redes_wifi()
        redes_ = extrair_redes_wifi(wifi_dados)
        #print(f'scanning {x} ... found {len(redes_)}', end='\r')
        pl(f'scanning {x} ... found {len(redes_)}')
        for rede in redes_:
            index_ = get_index_bssid(redes, rede['bssid'])
            #index = get_index_bssid(sinais, rede['bssid'])
            #if rede['bssid'] == '7a:8c:b5:21:99:92':
            #    print(redes[index_]['bssid'],':',rede['bssid'],': sinal antigo ',redes[index_]['sinal'],':', rede['sinal'])
            if index_ == -1:
                rede['sinais'] = [int(rede['sinal'])]
                redes.append(rede)
                #it = {
                #    'bssid'  : rede['bssid'],
                #    'sinais' : [int(rede['sinal'])]
                #}
                #sinais.append(it)
            else:
                #sinais[index]['sinais'].append(int(rede['sinal']))
                redes[index_]['sinais'].append(int(rede['sinal']))
                #redes[index] = rede
        pyautogui.click(1700,1060)
        pausa()
    print('')
    #for sinal in sinais:
    #    sinal['media'] = sum(sinal['sinais']) / len(sinal['sinais'])
    for rede in redes:
        if len(rede['sinais']) > 0:
            rede['sinal'] = sum(rede['sinais']) / len(rede['sinais'])
            del rede['sinais']
        else:
            del rede
    pausa()
    os.system('cls')
    return redes
    #return sinais
# - Get SSID for cardial ----------------------------------------------------------------------------
def scanner(local):
    pausa(f'Va para o {local}!',10)
    atualiza_redes_pc()
    pausa(f'Va para o {local}!',5)
    p(f'Scaneando redes {local} ...')
    wifi_dados = listar_redes_wifi()
    redes = extrair_redes_wifi(wifi_dados)
    for x in range(3):        
        print(f'scanning {x} ...', end='\r')
        wifi_dados = listar_redes_wifi()
        redes_ = extrair_redes_wifi(wifi_dados)
        print(f'scanning {x} ... found {len(redes_)}', end='\r')
        for rede in redes_:
            index = get_index_bssid(redes, rede['bssid'])
            if index == -1:
                redes.append(rede)
            else:
                redes[index] = rede
        pyautogui.click(1700,1060)
        pausa()
    print('')
    for rede in redes: print(rede)
    pausa()
    os.system('cls')
    return redes
# - Get SSID comparando com a direcao que aumenta ou diminui o sinal --------------------------------
def scanner_comparando(ponto, local, direcao):
    pausa(f'Va para o {local}!',10)
    atualiza_redes_pc()
    pausa(f'Va para o {local}!',5)
    p(f'Scaneando redes {local} ...')
    wifi_dados = listar_redes_wifi()
    redes = extrair_redes_wifi(wifi_dados)
    for x in range(5):        
        print(f'scanning {x} ...', end='\r')
        wifi_dados = listar_redes_wifi()
        redes_ = extrair_redes_wifi(wifi_dados)
        print(f'scanning {x} ... found {len(redes_)}', end='\r')
        for rede in redes_:            
            index = get_index_bssid(redes, rede['bssid'])
            if index == -1:
                redes.append(rede)
            else:
                # se ja existe pega o sinal mais extremo do espectro 
                # se o sinal novo for maior que o antigo
                if rede['sinal'] > redes[index]['sinal']:
                    if ponto == direcao.get(rede['bssid'], None):
                        #print('aumenta -->',redes[index],' - ',rede,' - ',ponto,direcao.get(rede['bssid'], None))
                        redes[index]['sinal'] = rede['sinal']
                else:
                    if rede['sinal'] < redes[index]['sinal']:
                        if ponto != direcao.get(rede['bssid'], None) and direcao.get(rede['bssid'], None) != None:
                            #print('diminui -->',redes[index],' - ',rede,' - ',ponto,direcao.get(rede['bssid'], None))
                            redes[index]['sinal'] = rede['sinal']
        pyautogui.click(1700,1060)
        pausa()
    #print('',end="\n",flush=True)
    print('')
    for rede in redes: print(rede)
    pausa()
    os.system('cls')
    return redes
# - Filtrando existente nos quatro pontos cardeais --------------------------------------------------
def normalizando_redes(redes_norte, redes_sul, redes_leste, redes_oeste):    
    bssids_norte    = {rede['bssid'] for rede in redes_norte}
    bssids_sul      = {rede['bssid'] for rede in redes_sul}
    bssids_leste    = {rede['bssid'] for rede in redes_leste}
    bssids_oeste    = {rede['bssid'] for rede in redes_oeste}
    bssids_comuns   = bssids_norte & bssids_sul & bssids_leste & bssids_oeste
    redes_norte_new = [rede for rede in redes_norte if rede['bssid'] in bssids_comuns]
    redes_sul_new   = [rede for rede in redes_sul   if rede['bssid'] in bssids_comuns]
    redes_leste_new = [rede for rede in redes_leste if rede['bssid'] in bssids_comuns]
    redes_oeste_new = [rede for rede in redes_oeste if rede['bssid'] in bssids_comuns]
    return redes_norte_new, redes_sul_new, redes_leste_new, redes_oeste_new
# - monta espectros ---------------------------------------------------------------------------------
def monta_espectros(espectros, redes, cardeal):
    for rede in redes:
        bssid = next((item for item, d in enumerate(espectros) if d['bssid'] == rede['bssid']),-1)
        if bssid == -1:
            espectro = {
                    'ssid'  : rede['ssid'],
                    'bssid' : rede['bssid'],
                    cardeal : rede['sinal']
                }
            espectros.append(espectro)
        else:
            espectros[bssid][cardeal] = rede['sinal']
    return espectros
# - Get SSID da coordenada usando como referencia o Top 6 -------------------------------------------
def get_ssid(ssid_top6=None):
    wifi_dados = listar_redes_wifi()
    ssids = extrair_redes_wifi(wifi_dados)
    if ssid_top6:
        ssid_intersecao = [
            item for item in ssids if any(
                item['bssid'] == top['bssid'] for top in ssid_top6
            )
        ]
        return ssid_intersecao
    else:
        return ssids
# - rotinas genericas wifi --------------------------------------------------------------------------
def calcular_distancia(rssi, p0, n):
    return 10 ** ((p0 - rssi) / (10 * n))

def calcular_distancia_diferenca(s1, s2):
    d1 = calcular_distancia(s1,-50,4)
    d2 = calcular_distancia(s2,-50,4)
    d = abs((d1 - d2) * 10)
    return d

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
                "sinal" : sinal_atual,
                "sinais" : []
            }
            redes.append(it)
    return redes