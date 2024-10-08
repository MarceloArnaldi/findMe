import os
import sys
import json
import pandas as pd
sys.path.insert(1, '../')
from comum import p, pj
from comumDB import consulta_instalacao
from comumWIFI import scanner_basic, calcular_distancia

os.system('cls')

def calc(s1, s2):
    d1 = calcular_distancia(s1,-50,4)
    d2 = calcular_distancia(s2,-50,4)
    d = abs((d1 - d2) * 10)
    return d

if len(sys.argv) <= 1:  
    instalacao = 1
else:
    instalacao = int(sys.argv[1])

instalacaoObj = consulta_instalacao(instalacao)

local = os.environ.get('LOCAL')
print('LOCAL    :',local)

pd.set_option('display.max_rows', None)
redes = []
areas = []
for local in instalacaoObj['locais']:
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
p('-')
soma  = {}
count = {}
for area in areas: 
    soma[area] = 0
    count[area] = 0
redes_ = scanner_basic()
for rede_ in redes_:
    index = next((item for item, d in enumerate(redes) if d['bssid'] == rede_['bssid']),-1)
    redes[index]['Atual'] = rede_['sinal']
    for area in areas:
        if area in redes[index]:
            redes[index][area+'_'] = calc(int(rede_['sinal']), redes[index][area])
            soma[area] += redes[index][area+'_']
            #print(redes[index][area+'_'])
            count[area] += 1
        else:
            redes[index][area+'_'] = None    
print(soma)
print(count)
for soma_ in soma:
    print(soma[soma_],count[soma_],soma[soma_]/count[soma_])

colunas_ordenadas = ['ssid', 'bssid', 'Quartos', 'Social', 'Area Servico','Atual']
for area in areas: colunas_ordenadas.append(area+'_')
df_redes = pd.DataFrame(redes)
#print(df_redes[colunas_ordenadas])    