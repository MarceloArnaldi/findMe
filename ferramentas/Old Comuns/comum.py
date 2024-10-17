import os
import time
import json
import pyautogui
from win32api import GetSystemMetrics

def get_resolution():
    return GetSystemMetrics(0), GetSystemMetrics(1)

def pj(msg):
    if msg == '-':
        print('-' * 60)
    else:
        if msg == None:
            print(msg)
        else:
            try:
                print(json.dumps(msg,indent=4))
            except:
                print(msg)
                raise

def pl(msg):
    if msg != None:
        print(' ' * 100, end="\r",flush=True)
        print(msg, end="\r")

def p(msg):
    if msg == '-':
        print('-' * 60)
    else:
        if msg != None:
            print(msg)

def pt(msg, msg2, tabulacao=15):
    if msg != None:        
        if tabulacao >= len(msg):
            print(msg,' ' * (tabulacao - len(msg)),':',msg2)
        else:
            print(msg,':',msg2)

def it(msg, tabulacao=21):
    if msg != None:        
        if tabulacao >= len(msg):
            return input(msg + ' ' * (tabulacao - len(msg)) + ': ')
        else:
            return input(msg + ' : ')

def pausa(msg='', pausa=2):
    for x in range(pausa - 1):
        if msg != '':
            pl(msg + ' ... ' + str(pausa - x - 2))
        time.sleep(1)

def atualiza_redes_pc():
    pyautogui.moveTo(1700,1080)
    pausa()
    pyautogui.click(1700,1060)
    pausa()
    pyautogui.click(1700,1060)
    pausa()
    pyautogui.click(1700,1060)    
    
def numero_entre_intervalos(num, espectro):
    return min(int(espectro['n']), int(espectro['s'])) <= int(num) <= max(int(espectro['n']), int(espectro['s'])) and \
           min(int(espectro['l']), int(espectro['o'])) <= int(num) <= max(int(espectro['l']), int(espectro['o']))

def encontrar_numero(instalacao, bssid_, sinal):
    resultado = []
    for local in instalacao['locais']:    
        for area in local['areas']:                    
            for especto in area['espectros']:
                #if especto['ssid'] == 'Arnaldi' and especto['bssid'] == bssid_:
                #if especto['bssid'] == '7a:8c:b5:71:ad:b3' and especto['bssid'] == bssid_:
                    #print(area['nome'],':',especto['ssid'], sinal, especto, flush=True)
                if especto['bssid'] == bssid_ and especto['ssid'] == 'Arnaldi':
                    print(area['nome'],':',especto['ssid'], sinal, especto, flush=True)
                    valores = {chave: especto[chave] for chave in ['n', 's', 'l', 'o']}
                    if numero_entre_intervalos(sinal, valores):
                        #print('found! ',local['nome'],':',area['nome'],':',especto['ssid'],':',sinal,':',valores, end='\r',flush=True)
                        resultado.append({
                            "local": local['nome'],
                            "area": area['nome'],
                            "espectro_id": especto['ssid']
                        })
    return resultado

def set_env_bat():
    f = open('setEnv.bat', "w")
    instalacao_id = os.environ.get('INSTALACAO')
    local         = os.environ.get('LOCAL')
    area          = os.environ.get('AREA')
    f.write('@echo off\n')
    if instalacao_id != None: f.write(f'SET INSTALACAO={instalacao_id}\n')
    if local         != None: f.write(f'SET LOCAL={local}\n')
    if area          != None: f.write(f'SET AREA={area}\n')
    f.close()