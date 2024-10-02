import os
import subprocess
import platform
import time
import math
import re
from tabulate import tabulate

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

def atualizar_adaptador_wifi(adaptador):
    # Requer ser administrador
    try:
        subprocess.run(['netsh', 'interface', 'set', 'interface', adaptador, 'admin=disable'], check=True)
        time.sleep(2) 
        subprocess.run(['netsh', 'interface', 'set', 'interface', adaptador, 'admin=enable'], check=True)
        time.sleep(2)  
    except subprocess.CalledProcessError as e:
        print("Erro ao atualizar o adaptador Wi-Fi:", e)

def identificar_sistema_operacional():
    sistema = platform.system()
    return sistema

# Windows - so funciona depois que clica no icone do WIFI no Windows
def listar_redes_wifi_Windows():
    # Executa o comando 'netsh wlan show networks mode=bssid' para listar redes
    resultado = subprocess.run(["netsh", "wlan", "show", "networks", "mode=bssid"], capture_output=True, text=True)
    return resultado.stdout

def listar_redes_wifi_Windows_2():
    try:
        # Executa o comando netsh para listar as redes Wi-Fi
        resultado = subprocess.check_output(['netsh', 'wlan', 'show', 'network', 'mode=bssid'], encoding='utf-8')

        # Exibe o resultado
        print(resultado)
    except subprocess.CalledProcessError as e:
        print("Erro ao listar redes Wi-Fi:", e)


def listar_redes_wifi_Windows_Tab():
    try:
        # Executa o comando netsh para listar as redes Wi-Fi
        resultado = subprocess.check_output(['netsh', 'wlan', 'show', 'network', 'mode=bssid'], encoding='utf-8')        
        print(resultado)


        
        # Dividimos a saída em redes separadas
        redes = resultado.split('SSID ')[1:]
        print(len(redes))
        print(redes[0])
        linhas = redes[0].splitlines()
        print(linhas)
        for linha in linhas:
            bssid = re.search(r'BSSID\s+\d+\s+:\s(.+)', linha)
            sinal = re.search(r'Sinal\s+:\s(\d+)%', linha)
            print(bssid)
            print(sinal)
        return
        print(re.search(r'SSID\s+\d+\s+:\s(.+)', redes[0]))
        redes_listadas = []
    
        for rede in redes:
            linhas = rede.splitlines()

            # Pega o nome da rede (SSID)
            ssid = re.search(r'SSID\s+\d+\s+:\s(.+)', linhas[0])
            if ssid:
                ssid = ssid.group(1)
            
            # Encontra BSSIDs e sinais da rede
            bssids = []
            for linha in linhas:
                bssid = re.search(r'BSSID\s+\d+\s+:\s(.+)', linha)
                sinal = re.search(r'Sinal\s+:\s(\d+)%', linha)

                if bssid and sinal:
                    bssids.append((ssid, bssid.group(1), sinal.group(1)))

            # Adiciona os BSSIDs e sinais correspondentes à lista
            redes_listadas.extend(bssids)

        # Exibe os resultados de forma tabulada
        headers = ["SSID", "BSSID", "Sinal (%)"]
        print(tabulate(redes_listadas, headers=headers, tablefmt="grid"))

    except subprocess.CalledProcessError as e:
        print("Erro ao listar redes Wi-Fi:", e)

# Linux
def listar_redes_wifi_Linux():
    # Executa o comando 'nmcli dev wifi' para listar as redes
    resultado = subprocess.run(["nmcli", "dev", "wifi"], capture_output=True, text=True)
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
            redes.append((ssid_atual, bssid_atual, sinal_atual))
    
    return redes

def calcular_distancia(RSSI, P0, n):
    return 10 ** ((P0 - RSSI) / (10 * n))

# --------------------------------------------------------------------------------------

sistema_operacional = identificar_sistema_operacional()
print(f"Sistema operacional: {sistema_operacional}")

sistema = identificar_sistema_operacional()

if sistema == "Windows":
    #os.system('cls')
    adaptador = obter_nome_adaptador_wifi()
    print('adaptador    :',adaptador)
    wifi_dados = listar_redes_wifi_Windows()
    wifi_info = extrair_redes_wifi(wifi_dados)
    for rede in wifi_info:
        print('SSID         :',rede[0],':',rede[1],':',rede[2])      
        print(calcular_distancia,int(rede[2]),-50,2)
elif sistema == "Linux":
    os.system('clear')
    wifi_info = listar_redes_wifi_Linux()
    print(wifi_info)
elif sistema == "Darwin":
    print("Você está usando macOS.")
else:
    print("Sistema operacional desconhecido.")