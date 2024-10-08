import os
import sys
os.system('cls')
sys.path.insert(1, '../')

from comum      import p, pj, pausa
from comumDB    import consulta_instalacao, grava_instalacao, existe_area, add_area, exclui_todos_espectros, registra_espectros
from comumWIFI  import scanner_media, monta_espectros

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
sai = False
while not sai:
    area = input(f'Entre com a AREA ({area}): ').strip() or area
    if area.lower() == 'sai':
        sai = True
        break
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
        break
    os.system('cls')
    # - scanner -------------------------------------------------------------
    redes = scanner_media()
    #for rede in redes: print(rede)
    espectros = []
    espectros = monta_espectros(espectros, redes, 'c')
    #pj(espectros)
    exclui_todos_espectros(instalacao, local, area)
    registra_espectros(instalacao, local, area, espectros)
    grava_instalacao(instalacao_id, instalacao)
    pj(instalacao)


