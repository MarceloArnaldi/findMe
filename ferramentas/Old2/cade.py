import os
import sys
import json
import pandas as pd
sys.path.insert(1, '../')
from comum import p, pj
from comumDB import consulta_instalacao
from comumWIFI import scanner_basic, calcular_distancia, calcular_distancia_diferenca

os.system('cls')

if len(sys.argv) <= 1:  
    instalacao = 1
else:
    instalacao = int(sys.argv[1])
    
show = False
if len(sys.argv) >= 2:
    if sys.argv[2].lower() == 'show': 
        show = True

instalacaoObj = consulta_instalacao(instalacao)

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
            redes[index][area+'_'] = calcular_distancia_diferenca(int(rede_['sinal']), redes[index][area])
            soma[area] += redes[index][area+'_']
            count[area] += 1
        else:
            redes[index][area+'_'] = None    

#print(soma)
#print(count)
s = 0
for soma_ in soma:
    #print(soma[soma_]/count[soma_])
    s += soma[soma_]/count[soma_] * 1000
#print('soma->',s)
for soma_ in soma:    
    probabilidade = (soma[soma_]/count[soma_] * 1000)/ s * 100
    print(soma_,': %.2f' % probabilidade,'%')

if show:
    pd.set_option('display.max_rows', None)
    colunas_ordenadas = ['ssid', 'bssid', 'Quartos', 'Social', 'Area Servico','Atual']
    for area in areas: colunas_ordenadas.append(area+'_')
    df_redes = pd.DataFrame(redes)
    print(df_redes[colunas_ordenadas])    