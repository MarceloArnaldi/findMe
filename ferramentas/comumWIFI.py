import re
import time
import subprocess

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
# - Get SSID ----------------------------------------------------------------------------------------
def scanner():
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
        time.sleep(2)
    print('')
    return redes
# - Get SSID ----------------------------------------------------------------------------------------
def scanner_comparando(ponto, direcao):
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
        time.sleep(2)
    print('',end="\n",flush=True)
    return redes
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