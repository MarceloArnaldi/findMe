import os
from comum      import p, pj
from comumDB    import consulta_instalacao, grava_instalacao, apaga_area, existe_area

os.system('cls')

if os.environ.get('LOCAL') == None:
    print('informe o local. SET LOCAL=andar_1')
    exit()
if os.environ.get('AREA') == None:
    print('informe o area. SET AREA=Sala de Estar')
    exit()

local   = os.environ.get('LOCAL')
area    = os.environ.get('AREA')

area = input(f'Entre com a AREA para ser APAGADA ({area}): ').strip() or area

resposta = input(f'Confirma APAGAR a "{area}" ? Digite "{area}" para apaga : ')
if resposta == area:
    instalacao_id = 1
    instalacao = consulta_instalacao(instalacao_id)
    existe, _, _ = existe_area(instalacao, local, area)
    if (existe):
        apaga_area(instalacao, local, area)
        grava_instalacao(instalacao_id, instalacao)
        pj(instalacao)
    else:
        p('area nao existe')
else:
    p('area nao apagada')