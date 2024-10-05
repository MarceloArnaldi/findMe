import os
import time
from comum      import p
#from comumDB    import consulta_instalacao
from comumWIFI  import get_ssid, get_bssid, get_index_bssid, scanner, scanner_comparando

os.system('cls')

instalacao_id = 1
#instalacao = consulta_instalacao(instalacao_id)

if os.environ.get('LOCAL') == None:
    print('informe o local. SET LOCAL=andar_1')
    exit()
if os.environ.get('AREA') == None:
    print('informe o area. SET AREA=Sala de Estar')
    exit()

local   = os.environ.get('LOCAL')
area    = os.environ.get('AREA')
posicao = 'c'

print('LOCAL    :',local)
print('AREA     :',area)
print('POSICAO  :',posicao)


# - primeira scanner --- usado para identiifica a direcao do roteador ---
print('- primeira scanner --- usado para identiifica a direcao do roteador ---')
input('sinal NORTE : press ENTER')
redesNorte = scanner()
# for rede in redesNorte: print(rede)
input('sinal SUL : press ENTER')
redesSul = scanner()
# for rede in redesSul: print(rede)
input('sinal LESTE : press ENTER')
redesLeste = scanner()
# for rede in redesLeste: print(rede)
input('sinal OESTE : press ENTER')
redesOeste = scanner()
# for rede in redesOeste: print(rede)
print('- identificando qual direcao estao os roteadores ----------------------')
direcaoNS = {}
for rede in redesNorte:     
    index = get_index_bssid(redesSul, rede['bssid'])
    if index > -1:
        if redesSul[index]['sinal'] > rede['sinal']:
            direcaoNS_ = 's'
        else:
            direcaoNS_ = 'n'
        direcaoNS[rede['bssid']] = direcaoNS_
direcaoLO = {}
for rede in redesLeste:     
    index = get_index_bssid(redesOeste, rede['bssid'])
    if index > -1:
        if redesOeste[index]['sinal'] > rede['sinal']:
            direcaoLO_ = 'o'
        else:
            direcaoLO_ = 'l'
        direcaoLO[rede['bssid']] = direcaoLO_
# p(direcaoNS)
# p(direcaoLO)
# - scaneando os pontos -------------------------------------------------
input('sinal NORTE : press ENTER')
redesNorte = scanner_comparando('n',direcaoNS)
for rede in redesNorte: print(rede)
input('sinal SUL : press ENTER')
redesSul = scanner_comparando('s',direcaoNS)
for rede in redesSul: print(rede)
input('sinal LESTE : press ENTER')
redesLeste = scanner_comparando('l',direcaoLO)
for rede in redesLeste: print(rede)
input('sinal OESTE : press ENTER')
redesOeste = scanner_comparando('o',direcaoLO)
for rede in redesOeste: print(rede)
# - montar objeto para gravar--------------------------------------------
#grava_instalacao(instalacao_id, instalacao)
#p(instalacao)
