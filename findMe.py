import os
import sys
import time
import pandas as pd
sys.path.insert(1, '../')
from comum import pt
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

while True:
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
        # - scanner -----------------------------------------------------------------------------------------------
        soma  = {}
        count = {}
        for area in areas: 
            soma[area] = 0
            count[area] = 0
        redes_ = scanner_basic()
        pt('Redes',len(redes_))
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
        # - probabilidade-------------------------------------------------------------------------------------------
        pt('Local',local['nome'])
        inversos = {k: 1/v for k, v in soma.items()}
        soma_inversos = sum(inversos.values())
        probabilidades = {k: v/soma_inversos for k, v in inversos.items()}
        for p in probabilidades:
            p_ = probabilidades[p] * 100 
            pt(p,'%.2f' % p_ +' %')
        # - display tabulado - debug ------------------------------------------------------------------------------
        if show:
            pd.set_option('display.max_rows', None)
            colunas_ordenadas = ['ssid', 'bssid', 'Quartos', 'Social', 'Area Servico','Atual']
            for area in areas: colunas_ordenadas.append(area+'_')
            df_redes = pd.DataFrame(redes)
            print(df_redes[colunas_ordenadas])    

    time.sleep(3)
    os.system('cls')