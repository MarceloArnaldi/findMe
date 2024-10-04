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
# - Recupera Instalacao----------------------------------------------------------------------------
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
def atualiza_local(data, local_, novo_nome_local = ''):
    instalacao_id = data['instalacao']
    novo = (novo_nome_local == '')
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local), -1)
    if novo and local_index == -1:
        data['locais'].append({
                'nome': local_,
                'areas': []
            })
        grava_instalacao(instalacao_id, instalacaoObj)
        return True
    if not novo and local_index > -1:
        data['locais'][local_index]['nome'] = novo_nome_local
        grava_instalacao(instalacao_id, instalacaoObj)    
# - exclui local ----------------------------------------------------------------------------------
def exclui_local(data, local_):
    instalacao_id = data['instalacao']
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    print(local_index)
    if local_index > -1:    
        print('exclui Local')    
        del data['locais'][local_index]
        grava_instalacao(instalacao_id, instalacaoObj)
# - area exist ? ---------------------------------------------------------------------------------
def area_existe(data, local_, area_):
    for local in data.get("locais", []):
        if local['nome'] == local_:
            for x, area in enumerate(local.get("areas",[])):
                if area.get("nome") == area_:
                    return True, x
    return False, -1
# - exclui area ------------------------------------------------------------------------------------
def exclui_area(data, local_, area_):
    instalacao_id = data['instalacao']
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    if local_index > -1:
        area_existe_, area_index = area_existe(data, local_, area_)
        if area_existe_:    
            del data['locais'][local_index]['areas'][area_index]
            grava_instalacao(instalacao_id, instalacaoObj)
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
            grava_instalacao(instalacao_id, instalacaoObj)
            return True
        if not novo and area_existe_:
            data['locais'][local_index]['areas'][area_index]['nome'] = novo_nome_area
            grava_instalacao(instalacao_id, instalacaoObj)
# - area espectro ? ---------------------------------------------------------------------------------
def espectro_existe(data, local_, area_, bssid_):
    for local in data.get("locais", []):
        if local['nome'] == local_:
            for x, area in enumerate(local.get("areas",[])):
                if area.get("nome") == area_:
                    for y, espectro in enumerate(area.get("espectros",[])):
                        if espectro['bssid'] == bssid_:
                            return True, y
    return False, -1
# - Incluir ou alterar espectro ---------------------------------------------------------------------                
def atualiza_espectro(data, local_, area_, espectro_):
    instalacao_id = data['instalacao']
    bssid_ = espectro_['bssid']
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    if local_index > -1:        
        area_existe_, area_index = area_existe(data, local_, area_)
        if area_existe_:
            espectro_existe_, espectro_index = espectro_existe(data, local_, area_, bssid_)
            if not espectro_existe_:
                data['locais'][local_index]['areas'][area_index]['espectros'].append(espectro_)
            else:
                data['locais'][local_index]['areas'][area_index]['espectros'][espectro_index] = espectro_
            grava_instalacao(instalacao_id, data)   
# - Exclui espectro ---------------------------------------------------------------------------------
def exclui_espectro(data, local_, area_, bssid_):
    instalacao_id = data['instalacao']
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    if local_index > -1:        
        area_existe_, area_index = area_existe(data, local_, area_)
        if area_existe_:
            espectro_existe_, espectro_index = espectro_existe(data, local_, area_, bssid_)
            if espectro_existe_:
                del data['locais'][local_index]['areas'][area_index]['espectros'][espectro_index]
                grava_instalacao(instalacao_id, data)
# - Exclui TODOS espectro ---------------------------------------------------------------------------
# ao fazer um mapeamento da area, excluir todos os espectros
def exclui_todos_espectros(data, local_, area_):
    instalacao_id = data['instalacao']
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    if local_index > -1:        
        area_existe_, area_index = area_existe(data, local_, area_)
        if area_existe_:
            data['locais'][local_index]['areas'][area_index]['espectros'] = []
            grava_instalacao(instalacao_id, data)
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

# - Consulta --------------------------------------------------------------------------------------
instalacao = 1
#instalacaoObj = consulta_instalacao(instalacao)
#print(json.dumps(instalacaoObj,indent=4))
#exit()
# - incluir ou alterar local ----------------------------------------------------------------------
local = '2_andar'
#atualiza_local(instalacaoObj, local)               # inclui area se nao existir
#atualiza_local(instalacaoObj, local, '2_andar')     # atualiza o nome da area
#exit()
# - exclui local ----------------------------------------------------------------------------------
local = '2_andar'
#exclui_local(instalacaoObj,local)        
# - exclui area -----------------------------------------------------------------------------------
local = '2_andar'
area  = 'area_um'
# exclui_area(instalacaoObj, local, area)      
# - Incluir ou altera uma area --------------------------------------------------------------------
local = '2_andar'
area  = 'Home Theater'
#atualiza_area(instalacaoObj, local, area)               # inclui area se nao existir
#atualiza_area(instalacaoObj, local, area, 'area_2')     # atualiza o nome da area
# - Incluir ou alterar espectro ---------------------------------------------------------------------                
local = '2_andar'
area  = 'area_2'
espectro = {
    "ssid": "NomeDaRede2",
    "bssid": '13:11:11:11',
    "a": 3,
    "b": 2,
    "c": 33,
    "d": 34
}
#atualiza_espectro(instalacaoObj, local, area, espectro)
# - Exclui espectro ---------------------------------------------------------------------------------
local = '1_andar'
area  = 'area_2'
bssid = '13:11:11:11'
#print(json.dumps(instalacaoObj,indent=4))
#exclui_espectro(instalacaoObj, local, area, bssid)
# ---------------------------------------------------------------------------------------------------
# - Marcando um ponto -------------------------------------------------------------------------------
# sinal dos 6 SSIO com sinal mais forte -------------------------------------------------------------
# ou marcar o ponto central e usar o sinal mais forte desse ponto (parece mais inteligente) ---------
instalacao = 1
instalacaoObj = consulta_instalacao(instalacao)
local = 'unico'
area  = 'Home Theater'
espectro = {
    "ssid": "NomeDaRede1",
    "bssid": '15:11:11:11',
    "a": 3,
    "b": 2,
    "c": 33,
    "d": 34
}
# get SSID Top 6 ----------------------------------------------------
def get_ssid_top6():
    ssids = [
        {
            "ssid": "NomeDaRede1",
            "bssid": '15:11:11:11',
            "sinal": 3
        },
        {
            "ssid": "NomeDaRede2",
            "bssid": '16:11:11:11',
            "sinal": 7
        },
        {
            "ssid": "NomeDaRede3",
            "bssid": '17:11:11:11',
            "sinal": 9
        },
        {
            "ssid": "NomeDaRede4",
            "bssid": '18:11:11:11',
            "sinal": 2
        },
        {
            "ssid": "NomeDaRede5",
            "bssid": '55:11:11:11',
            "sinal": 1
        },
        {
            "ssid": "NomeDaRede6",
            "bssid": '12:11:11:11',
            "sinal": 17
        },
        {
            "ssid": "NomeDaRede7",
            "bssid": '77:11:11:11',
            "sinal": 7
        },
        {
            "ssid": "NomeDaRede8",
            "bssid": '12:11:11:11',
            "sinal": 1
        }
    ]
    espectros_sinal_ordenado = sorted(ssids, key=lambda x: x['sinal'], reverse=True)
    return espectros_sinal_ordenado[:6]
# - Get SSID da coordenada usando como referencia o Top 6 -------------------------------------------
def get_ssid(ssid_top6):
    ssids = [        
        {
            "ssid": "NomeDaRede2",
            "bssid": '16:11:11:11',
            "sinal": 3
        },
        {
            "ssid": "NomeDaRede31",
            "bssid": '31:11:11:11',
            "sinal": 32
        },
        {
            "ssid": "NomeDaRede1",
            "bssid": '15:11:11:11',
            "sinal": 2
        },
        {
            "ssid": "NomeDaRede4",
            "bssid": '18:11:11:11',
            "sinal": 6
        },
        {
            "ssid": "NomeDaRede5",
            "bssid": '55:11:11:11',
            "sinal": 7
        },
        {
            "ssid": "NomeDaRede61",
            "bssid": '61:11:11:11',
            "sinal": 62
        },
        {
            "ssid": "NomeDaRede7",
            "bssid": '77:11:11:11',
            "sinal": 8
        },
        {
            "ssid": "NomeDaRede6",
            "bssid": '12:11:11:11',
            "sinal": 9
        }
    ]
    ssid_intersecao = [
        item for item in ssids if any(
            item['bssid'] == top['bssid'] for top in ssid_top6
        )
    ]
    return ssid_intersecao

# - Registra o sinal dos SSID de cada ponto ---------------------------------------------------------
def registra_espectro(data, local_, area_, espectros_sinal_, posicao_):
    instalacao_id = data['instalacao']
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    if local_index > -1:        
        area_existe_, area_index = area_existe(data, local_, area_)
        if area_existe_:
            for espectro in espectros_sinal_:
                espectro_existe_, espectro_index = espectro_existe(data, local_, area_, espectro['bssid'])
                # so cria se for tipo c = central
                if not espectro_existe_ and posicao_ == 'c':
                    espectro_ = {
                        "ssid"   : espectro['ssid'],
                        "bssid"  : espectro['bssid'],
                        posicao_ : espectro['sinal']
                    }
                    data['locais'][local_index]['areas'][area_index]['espectros'].append(espectro_)
                else:
                    data['locais'][local_index]['areas'][area_index]['espectros'][espectro_index][posicao_] = espectro['sinal']
                grava_instalacao(instalacao_id, data)   
            
#print(json.dumps(instalacaoObj,indent=4))
# - Logica de Registro ------------------------------------------------------------------------------
#exclui_todos_espectros(instalacaoObj, local, area )
# - Central (c) -------------------------------------------------------------------------------------
espectros_sinal_top6 = get_ssid_top6()
posicao = 'c' # central - esses SSID serao os usados para localizar o modulo
registra_espectro(instalacaoObj, local, area, espectros_sinal_top6, posicao)
# - Cardeais n, s, l, o ------------------------------------------------------------------------------
posicao = 'n' # sinal lado "norte" da area - registrar somente os SSID existentes do registro tipo "c"
espectros_sinal = get_ssid(espectros_sinal_top6)
#print(json.dumps(espectros_sinal,indent=4))
registra_espectro(instalacaoObj, local, area, espectros_sinal, posicao)
# - Cardeais n, s, l, o -----------------------------------------------------------------------------
posicao = 's' 
espectros_sinal = get_ssid(espectros_sinal_top6)
registra_espectro(instalacaoObj, local, area, espectros_sinal, posicao)
print(json.dumps(instalacaoObj,indent=4))
exit()

# - Localizando a área do objeto --------------------------------------------------------------------

sinal = 35
instalacaoObj = consulta_instalacao(instalacao)
print(json.dumps(instalacaoObj,indent=4))
resultado = encontrar_numero(instalacaoObj, sinal)
for res in resultado:
    print(res)
    print(f"Localização : {res['local']} : {res['area']} ! espectro {res['espectro_id']}.")


# def numero_entre_intervalos(num, espectro):
#     return min(espectro["a"], espectro["b"]) <= num <= max(espectro["a"], espectro["b"]) and \
#            min(espectro["c"], espectro["d"]) <= num <= max(espectro["c"], espectro["d"])

# def encontrar_numero(instalacao, sinal):
#     resultado = []
#     for local in instalacao['locais']:    
#         for area in local['areas']:            
#             for especto in area['espectros']:
#                 valores = {chave: especto[chave] for chave in ['a', 'b', 'c', 'd']}
#                 print(local['nome'],area['nome'],especto['ssid'],valores)
#                 if numero_entre_intervalos(sinal, valores):
#                     resultado.append({
#                         "local": local['nome'],
#                         "area": area['nome'],
#                         "espectro_id": especto['ssid']
#                     })
#     return resultado



# # - Atualizar nome de uma area -------------------------------------------------------------------
# local = '2_andar'
# area_atual = 'area_1'
# area_nova  = 'Home Theater'
# instalacaoObj = consulta_instalacao(instalacao)
# def substituir_nome_area(data, local_, nome_antigo, nome_novo):
#     for local in data.get("locais", []):
#         if local['nome'] == local_:
#             for area in local.get("areas", []):
#                 if area.get("nome") == nome_antigo:
#                     area["nome"] = nome_novo
# substituir_nome_area(instalacaoObj, local, area_atual, area_nova)
# #grava_instalacao(instalacao, instalacaoObj)
# print(json.dumps(instalacaoObj,indent=4))
# exit()
