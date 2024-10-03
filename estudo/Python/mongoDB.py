import os
import json
from pymongo import MongoClient

os.system('cls')

# Conectar ao MongoDB
client = MongoClient("mongodb+srv://marceloarnaldi:BITh5VIzm3vY3Eoc@clusterdev.syoui.mongodb.net/?retryWrites=true&w=majority&appName=ClusterDev")
db = client['ClusterDev']
collection = db['instalacoes']

obj1 = {
        "instalacao": 1,
        "locais": [{ 
                "nome" :"1_andar",
                "areas": [{
                    "nome": "Home Theaer",
                    "espectros": [
                        {
                            "ssid": "Arnaldi",
                            "bssid": "11:11:11:11",
                            "a": 45,
                            "b": 40,
                            "c": 50,
                            "d": 52
                        },
                        {
                            "ssid": "ArnaldiNet",
                            "bssid": "11:11:11:11",
                            "a": 40,
                            "b": 44,
                            "c": 54,
                            "d": 57
                        }
                    ]
                },
                {
                    "nome": "Sala Estar",
                    "espectros": [{
                            "ssid": "Arnaldi",
                            "bssid": "11:11:11:11",
                            "a": 45,
                            "b": 40,
                            "c": 50,
                            "d": 52
                        },
                        {
                            "ssid": "ArnaldiNet",
                            "bssid": "11:11:11:11",
                            "a": 30,
                            "b": 40,
                            "c": 54,
                            "d": 30
                        }]
                }]
            },
            {
                "nome": "2_andar",
                "areas": []
            }]
        }

def inserir_instalacao(instalacao):
    collection.insert_one(instalacao)

def alterar_instalacao(instalacao_id, nova_area):
    collection.update_one(
        {"instalacao": instalacao_id},
        {"$set": {"locais.0.areas": nova_area}}
    )
# - Consulta -------------------------------------------------------------------------------------
def consulta_instalacao(instalacao_id):
    documento = collection.find_one({"instalacao": instalacao_id})
    del documento['_id']
    return documento
# ------------------------------------------------------------------------------------------------      
def grava_instalacao(instalacao_id, obj):
    locais = obj['locais']
    collection.update_one(
        {"instalacao": instalacao_id},
        {"$set": {"locais": locais}}
    )
# - incluir ou alterar local ----------------------------------------------------------------------
local = '2_andar'
def atualiza_local(data, local_, novo_nome_local = ''):
    novo = (novo_nome_local == '')
    index_local = next((i for i, d in enumerate(data['locais']) if d["nome"] == local), -1)
    if novo and index_local == -1:
        data['locais'].append({
                'nome': local_,
                'areas': []
            })
        grava_instalacao(instalacao, instalacaoObj)
        return True
    if not novo and index_local > -1:
        data['locais'][index_local]['nome'] = novo_nome_local
        grava_instalacao(instalacao, instalacaoObj)    
# ------------------------------------------------------------------------------------------------
# - Consulta -------------------------------------------------------------------------------------
instalacao = 1
instalacaoObj = consulta_instalacao(instalacao)
print(json.dumps(instalacaoObj,indent=4))
#exit()
# - incluir ou alterar local ----------------------------------------------------------------------
local = '2_andar'
def atualiza_local(data, local_, novo_nome_local = ''):
    novo = (novo_nome_local == '')
    index_local = next((i for i, d in enumerate(data['locais']) if d["nome"] == local), -1)
    if novo and index_local == -1:
        data['locais'].append({
                'nome': local_,
                'areas': []
            })
        grava_instalacao(instalacao, instalacaoObj)
        return True
    if not novo and index_local > -1:
        data['locais'][index_local]['nome'] = novo_nome_local
        grava_instalacao(instalacao, instalacaoObj)
#atualiza_local(instalacaoObj, local)               # inclui area se nao existir
#atualiza_local(instalacaoObj, local, '2_andar')     # atualiza o nome da area
#exit()
# - exclui local ----------------------------------------------------------------------------------
local = '2_andar'
def exclui_local(data, local_):
    index_local = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    print(index_local)
    if index_local > -1:    
        print('exclui Local')    
        del data['locais'][index_local]
        grava_instalacao(instalacao, instalacaoObj)
#exclui_local(instalacaoObj,local)        
# - area exist ? ---------------------------------------------------------------------------------
local = '2_andar'
area  = 'area_2'
def area_existe(data, local_, area_):
    for local in data.get("locais", []):
        if local['nome'] == local_:
            for x, area in enumerate(local.get("areas",[])):
                if area.get("nome") == area_:
                    return True, x
    return False, -1
# - exclui area ------------------------------------------------------------------------------------
local = '2_andar'
area  = 'area_um'
def exclui_area(data, local_, area_):
    index_local = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    if index_local > -1:
        area_existe_, area_index = area_existe(data, local_, area_)
        if area_existe_:    
            del data['locais'][index_local]['areas'][area_index]
            grava_instalacao(instalacao, instalacaoObj)
exclui_area(instalacaoObj, local, area)      
# - Incluir ou altera uma area ---------------------------------------------------------------------
local = '2_andar'
area  = 'area_1'
def atualiza_area(data, local_, area_, novo_nome_area = ''):
    novo = (novo_nome_area == '')
    index_local = next((i for i, d in enumerate(data['locais']) if d["nome"] == local), -1)
    if index_local > -1:
        area_existe_, area_index = area_existe(data, local_, area_)
        if novo and not area_existe_:    
            data['locais'][index_local]['areas'].append({
                    'nome': area_,
                    "espectos": []
                })
            grava_instalacao(instalacao, instalacaoObj)
            return True
        if not novo and area_existe_:
            data['locais'][index_local]['areas'][area_index]['nome'] = novo_nome_area
            grava_instalacao(instalacao, instalacaoObj)
atualiza_area(instalacaoObj, local, area)               # inclui area se nao existir
#atualiza_area(instalacaoObj, local, area, 'Home Theater')     # atualiza o nome da area
print(json.dumps(instalacaoObj,indent=4))
exit()
# - Atualizar nome de uma area -------------------------------------------------------------------
local = '1_andar'
area_atual = 'Home Theaer'
area_nova  = 'Home Theater'
instalacaoObj = consulta_instalacao(instalacao)

def substituir_nome_area(data, local_, nome_antigo, nome_novo):
    for local in data.get("locais", []):
        if local['nome'] == local_:
            for area in local.get("areas", []):
                if area.get("nome") == nome_antigo:
                    area["nome"] = nome_novo
substituir_nome_area(instalacaoObj, local, area_atual, area_nova)
#grava_instalacao(instalacao, instalacaoObj)
print(json.dumps(instalacaoObj,indent=4))
exit()

# - Localizando a área do objeto ------------------------------------------------------------------

def numero_entre_intervalos(num, espectro):
    return min(espectro["a"], espectro["b"]) <= num <= max(espectro["a"], espectro["b"]) and \
           min(espectro["c"], espectro["d"]) <= num <= max(espectro["c"], espectro["d"])

def encontrar_numero(instalacao, sinal):
    resultado = []
    for local in instalacao['locais']:    
        for area in local['areas']:            
            for especto in area['espectros']:
                valores = {chave: especto[chave] for chave in ['a', 'b', 'c', 'd']}
                print(local['nome'],area['nome'],especto['ssid'],valores)
                if numero_entre_intervalos(sinal, valores):
                    resultado.append({
                        "local": local['nome'],
                        "area": area['nome'],
                        "espectro_id": especto['ssid']
                    })
    return resultado

sinal = 35
instalacaoObj = consulta_instalacao(instalacao)
print(json.dumps(instalacaoObj,indent=4))
resultado = encontrar_numero(instalacaoObj, sinal)
for res in resultado:
    print(res)
    print(f"Localização : {res['local']} : {res['area']} ! espectro {res['espectro_id']}.")
