import os
import sys
sys.path.insert(1, '../../')
from comum   import p, pt
from comumDB import consulta_instalacao, grava_instalacao, apaga_local, existe_local

local   = os.environ.get('LOCAL')
os.system('cls')
pt('LOCAL',local)

local = input(f'Entre com a LOCAL para ser APAGADA ({local}): ').strip() or local

resposta = input(f'Confirma APAGAR a Zona "{local}"  ? Digite "{local}" para apaga : ')
if resposta == local:
    instalacao_id = 1
    instalacao = consulta_instalacao(instalacao_id)
    existe_, index = existe_local(instalacao, local)    
    if existe_:
        apaga_local(instalacao, local)
        grava_instalacao(instalacao_id, instalacao)
    else:
        p('local nao existe')
else:
    p('local nao apagada')