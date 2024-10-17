import os
import sys
sys.path.insert(1, '../../')
from comum      import p, pj, pt
from comumDB    import consulta_instalacao, grava_instalacao, existe_area, add_area, existe_local

os.system('cls')

instalacao_id = 1
instalacao = consulta_instalacao(instalacao_id)
    
local   = os.environ.get('LOCAL')
area    = os.environ.get('AREA')
pt('LOCAL',local)
pt('AREA',area)

area = input(f'Entre com a AREA para ser CRIADA ({area}): ').strip() or area
resposta = input(f'Confirma CRIAR a "{area}" na zona {local} ? Digite "{area}" para CRIAR : ')
if resposta == area:
    existe_, _ = existe_local(instalacao, local)
    if existe_:
        existe, _, _ = existe_area(instalacao, local, area)
        if not existe:
            add_area(instalacao, local, area)
            grava_instalacao(instalacao_id, instalacao)
            pj(instalacao)
        else:
            p('area nao existe')
    else:
        p('local nao existe')
else:
    p('area nao criada')