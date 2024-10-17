import os
from comum   import p, pj
from pymongo import MongoClient

# - rotinas mongo ---------------------------------------------------------------------------------
client = MongoClient("mongodb+srv://marceloarnaldi:BITh5VIzm3vY3Eoc@clusterdev.syoui.mongodb.net/?retryWrites=true&w=majority&appName=ClusterDev")
db = client['ClusterDev']
collection = db['instalacoes']
# # - INSTALACAO ------------------------------------------------------------------------------------
# def consulta_instalacao(instalacao_id):
#     documento = collection.find_one({"instalacao": instalacao_id})
#     return documento

# # def grava_instalacao(instalacao_id, locais):
# #     collection.update_one(
# #         {"instalacao": instalacao_id},
# #         {"$set": {"locais": locais}}
# #     )

# # os.system('cls')
# # instalacao    = consulta_instalacao(1)
# # instalacao_id = instalacao['_id']
# # print(instalacao_id)
# # print(instalacao)

# # locais = {
# #     "locais"     : [
# #         {
# #             "nome":"local_1",
# #             "areas": []
# #         },
# #         {
# #             "nome":"local_2",
# #             "areas": []
# #         }
# #     ]    
# # }
# # grava_instalacao(1, locais)
# # exit()

# # # collection.insert_one(
# # #         {"instalacao": 1},
# # #         {"$set": {"locais": locais}}
# # #     )
def consulta_instalacao(instalacao_id):
    documento = collection.find_one({"instalacao": instalacao_id})
    del documento['_id']
    return documento

def existe_local(data, local_):
    for x, local in enumerate(data.get("locais", [])):
        if local['nome'] == local_:
            return True, x
    return False, -1

def add_local(data, local_):
    data['locais'].append({
        'nome': local_,
        'areas': []
    })

def grava_instalacao(instalacao_id, obj):
    print('gravando ... ', instalacao_id)
    locais = obj['locais']
    print(locais)
    collection.update_one(
        {"instalacao": instalacao_id},
        {"$set": {"locais": locais}}
    )
        
local = input('Entre com a LOCAL para ser CRIADO : ').strip()

resposta = input(f'Confirma CRIAR o LOCAL : "{local}" (s/n)? ')
if resposta.lower() == 's':
    instalacao_id = 1
    instalacao = consulta_instalacao(instalacao_id)
    print(instalacao)
    existe, _ = existe_local(instalacao, local)
    if not existe:
        add_local(instalacao, local)
        grava_instalacao(instalacao_id, instalacao)
        pj(instalacao)
    else:
        p('local existe')
else:
    p('local nao apagada')