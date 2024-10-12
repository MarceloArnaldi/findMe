import os
import time
from comum import p
from comumWIFI import scanner_basic, listar_redes_wifi

os.system('cls')

wifi_dados = listar_redes_wifi()
print(wifi_dados)

while True:
    redes = scanner_basic()
    for rede in redes:
        #if rede['bssid'] in ['7a:8c:b5:71:ad:b3','7a:8c:b5:21:ad:b2','7a:8c:b5:21:99:92','7a:8c:b5:21:99:93']:
        if rede['bssid'] in ['7a:8c:b5:71:ad:b3']:
            print(rede['bssid'],':',rede['sinal'])
    p('-')
    time.sleep(2)