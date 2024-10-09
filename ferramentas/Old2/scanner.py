import os
import sys
sys.path.insert(1, '../')
from comum      import p, pj, pausa
from comumDB    import consulta_instalacao, grava_instalacao, exclui_todos_espectros, registra_espectros, existe_area, add_area
from comumWIFI  import get_index_bssid, scanner, scanner_comparando, normalizando_redes, monta_espectros

os.system('cls')

instalacao_id = 1
instalacao = consulta_instalacao(instalacao_id)

if os.environ.get('LOCAL') == None:
    local = input('informe o local : ')
    os.system('SET LOCAL='+local)
    exit()
if os.environ.get('AREA') == None:
    area = input('informe o area : ')
    os.system('SET AREA='+area)
    exit()

local   = os.environ.get('LOCAL')
area    = os.environ.get('AREA')

area = input(f'Entre com a AREA ({area}): ').strip() or area
print('LOCAL    :',local)
print('AREA     :',area)

# - verifica se area existe ---------------------------------------------
print('')
existe, _, _ = existe_area(instalacao, local, area)
if (existe):
    print('area existe!')
    pausa()
else:
    print('area nao existe!')
    resposta = input(f'Confirma a criação da area "{area}" (s/n) ? ')
    if resposta.lower() == 's':
        p(f'criando area ... "{area}" em "{local}"')
        add_area(instalacao, local, area)
        grava_instalacao(instalacao_id, instalacao)
        instalacao = consulta_instalacao(instalacao_id)
        pj(instalacao)
        pausa('',5)
    else:
        p('area nao criada, scanner cancelado!')
    exit()
os.system('cls')
# - primeira scanner --- usado para identiifica a direcao do roteador ---
redesNorte = scanner('norte')
redesSul   = scanner('sul')
redesLeste = scanner('leste')
redesOeste = scanner('oeste')
direcaoNS  = {}
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
# - scaneando os pontos -------------------------------------------------
redesNorte = scanner_comparando('n', 'norte', direcaoNS)
redesSul   = scanner_comparando('s', 'sul', direcaoNS)
redesLeste = scanner_comparando('l', 'leste', direcaoLO)
redesOeste = scanner_comparando('o', 'oeste', direcaoLO)
redesNorteNew, redesSulNew, redesLesteNew, redesOesteNew = normalizando_redes(redesNorte, redesSul, redesLeste, redesOeste)
espectros = []
espectros = monta_espectros(espectros, redesNorteNew, 'n')
espectros = monta_espectros(espectros, redesSulNew, 's')
espectros = monta_espectros(espectros, redesLesteNew, 'l')
espectros = monta_espectros(espectros, redesOesteNew, 'o')
# - montar objeto para gravar--------------------------------------------
exclui_todos_espectros(instalacao, local, area)
registra_espectros(instalacao, local, area, espectros)
grava_instalacao(instalacao_id, instalacao)
pj(instalacao)
