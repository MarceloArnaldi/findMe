import os
import sys
from pymongo import MongoClient

# - rotinas mongo ------------------------------------------------------------------------------------------------
client = MongoClient("mongodb+srv://marceloarnaldi:BITh5VIzm3vY3Eoc@clusterdev.syoui.mongodb.net/?retryWrites=true&w=majority&appName=ClusterDev")
db = client['ClusterDev']
collection = db['instalacoes']
# - Recupera Instalacao----------------------------------------------------------------------------
def consulta_instalacao(instalacao_id):
    documento = collection.find_one({"instalacao": instalacao_id})
    del documento['_id']
    return documento
# - area exist ? ---------------------------------------------------------------------------------
def area_existe(data, local_, area_):
    for local in data.get("locais", []):
        if local['nome'] == local_:
            for x, area in enumerate(local.get("areas",[])):
                if area.get("nome") == area_:
                    return True, x
    return False, -1
# - Incluir ou altera uma area ---------------------------------------------------------------------
def atualiza_area(data, local_, area_, novo_nome_area = ''):
    instalacao_id = data['instalacao']
    novo = (novo_nome_area == '')
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local), -1)
    if local_index > -1:
        area_existe_, area_index = area_existe(data, local_, area_)
        if novo and not area_existe_:    
            data['locais'][local_index]['areas'].append({
                    'nome': area_,
                    "espectros": []
                })
            grava_instalacao(instalacao_id, data)
            return True
        if not novo and area_existe_:
            data['locais'][local_index]['areas'][area_index]['nome'] = novo_nome_area
            grava_instalacao(instalacao_id, data)
# ------------------------------------------------------------------------------------------------      
def grava_instalacao(instalacao_id, obj):
    locais = obj['locais']
    collection.update_one(
        {"instalacao": instalacao_id},
        {"$set": {"locais": locais}}
    )

os.system('cls')

if len(sys.argv) <= 1:
    print('informe nome da area')
    exit()
if os.environ.get('LOCAL') == None:
    print('informe o local. SET LOCAL=andar_1')
    exit()
if os.environ.get('AREA') == None:
    print('informe o area. SET AREA=Sala de Estar')
    exit()

local   = os.environ.get('LOCAL')
area    = os.environ.get('AREA')

print('LOCAL    :',local)
print('AREA     :',area)

instalacao = 1
instalacaoObj = consulta_instalacao(instalacao)

atualiza_area(instalacaoObj, local, area)
