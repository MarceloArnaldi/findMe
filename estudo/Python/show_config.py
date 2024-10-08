import os
import sys
import json
from comumDB import consulta_instalacao

os.system('cls')

if len(sys.argv) <= 1:  
    instalacao = 1
else:
    instalacao = int(sys.argv[1])

instalacaoObj = consulta_instalacao(instalacao)
print(json.dumps(instalacaoObj,indent=4))

local   = os.environ.get('LOCAL')
area    = os.environ.get('AREA')

print('LOCAL    :',local)
print('AREA     :',area)