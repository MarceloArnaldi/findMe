import os
import sys
import time
from comum import pj, encontrar_numero
from comumDB import consulta_instalacao
from comumWIFI import scanner_basic

os.system('cls')

if len(sys.argv) <= 1:  
    instalacao = 1
else:
    instalacao = int(sys.argv[1])

instalacaoObj = consulta_instalacao(instalacao)

while True:
    redes = scanner_basic()
    for rede in redes:
        resultado = encontrar_numero(instalacaoObj, rede['bssid'], rede['sinal'])
        #print(len(resultado))
        for res in resultado:
            #print('res->',res)
            #print(f"Localização : {res['local']} : {res['area']}", flush=True)
            #print(f"scanning ... found! : Localização : {res['local']} : {res['area']}", end='\r',flush=True)
            print(f" Localização : {res['local']} : {res['area']}", flush=True)
        time.sleep(1)
