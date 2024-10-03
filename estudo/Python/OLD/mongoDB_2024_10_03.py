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
        "locais": [
            { 
                "1_andar": {                    
                    "areas": [  
                        { 
                            "Home Theater": {
                                "espectros": [
                                    {
                                        "1" : {
                                            "a": 45,
                                            "b": 40,
                                            "c": 50,
                                            "d": 52
                                        }
                                    },
                                    {
                                        "2" : {
                                            "a": 30,
                                            "b": 40,
                                            "c": 34,
                                            "d": 22
                                        }
                                    },
                                    {
                                        "3" : {
                                            "a": 70,
                                            "b": 40,
                                            "c": 24,
                                            "d": 12
                                        }
                                    },
                                    {
                                        "4" : {
                                            "a": 80,
                                            "b": 60,
                                            "c": 54,
                                            "d": 72
                                        }
                                    }
                                    
                                ]
                            },
                            "Sala Estar": {
                                "espectros": [
                                    {
                                        "1" : {
                                            "a": 450,
                                            "b": 40,
                                            "c": 50,
                                            "d": 52
                                        }
                                    },
                                    {
                                        "2" : {
                                            "a": 30,
                                            "b": 40,
                                            "c": 34,
                                            "d": 22
                                        }
                                    },
                                    {
                                        "3" : {
                                            "a": 10,
                                            "b": 40,
                                            "c": 94,
                                            "d": 22
                                        }
                                    },
                                    {
                                        "4" : {
                                            "a": 70,
                                            "b": 90,
                                            "c": 14,
                                            "d": 22
                                        }
                                    }
                                ]
                            }
                        }
                    ]
                },
                "2_andar": {                    
                    "areas": [  
                        { 
                            "area_1": {
                                "espectros": [
                                    {
                                        1 : {
                                            "a": 45,
                                            "b": 40,
                                            "c": 50,
                                            "d": 52
                                        }
                                    },
                                    {
                                        2 : {
                                            "a": 30,
                                            "b": 40,
                                            "c": 34,
                                            "d": 22
                                        }
                                    },
                                    {
                                        3 : {
                                            "a": 70,
                                            "b": 40,
                                            "c": 24,
                                            "d": 12
                                        }
                                    },
                                    {
                                        4 : {
                                            "a": 80,
                                            "b": 60,
                                            "c": 54,
                                            "d": 72
                                        }
                                    }
                                    
                                ]
                            },
                            "area_2": {
                                "espectros": [
                                    {
                                        1 : {
                                            "a": 450,
                                            "b": 40,
                                            "c": 50,
                                            "d": 52
                                        }
                                    },
                                    {
                                        2 : {
                                            "a": 30,
                                            "b": 40,
                                            "c": 34,
                                            "d": 22
                                        }
                                    },
                                    {
                                        3 : {
                                            "a": 10,
                                            "b": 40,
                                            "c": 94,
                                            "d": 22
                                        }
                                    },
                                    {
                                        4 : {
                                            "a": 70,
                                            "b": 90,
                                            "c": 14,
                                            "d": 22
                                        }
                                    }
                                ]
                            }
                        }
                    ]
                }
            }
        ]
    }

obj1 = {
        "instalacao": 1,
        "locais": [{ 
                "nome" :"1_andar",
                "areas": [{
                    "nome": "Home Theaer",
                    "espectos": [
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
                    "espectos": [{
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

# Inserir um documento
def inserir_instalacao():
    documento = {
        "instalacao": 1,
        "locais": [
            { 
                "nome": "principal",
                "areas": [  
                    { 
                        "nome": "Home Theater",
                        "espectros": [
                            {
                                1 : {
                                    "a": 450,
                                    "b": 40,
                                    "c": 50,
                                    "d": 52
                                }
                            },
                            {
                                2 : {
                                    "a": 30,
                                    "b": 40,
                                    "c": 34,
                                    "d": 22
                                }
                            }
                        ]
                    }
                ]
            }
        ]
    }
    collection.insert_one(documento)
    print("Documento inserido com sucesso!")



# Alterar um documento
def alterar_instalacao(instalacao_id, nova_area):
    collection.update_one(
        {"instalacao": instalacao_id},
        {"$set": {"locais.0.areas": nova_area}}
    )
    print("Documento alterado com sucesso!")

def consulta_instalacao(instalacao_id):
    documento = collection.find_one({"instalacao": instalacao_id})
    del documento['_id']
    return documento

def atualiza_locais(instalacao_id, obj):
    locais = obj['locais']
    collection.update_one(
        {"instalacao": instalacao_id},
        {"$set": {"locais": locais}}
    )

# Exemplo de uso
#inserir_instalacao()
#consultar_instalacao(1)
# alterar_instalacao(1, [
#     {
#         "nome": "Sala de Estar",
#         "espectros": [
#             {
#                 "dbssid_code": 3,
#                 "a": 100,
#                 "b": 200,
#                 "c": 150,
#                 "d": 75
#             }
#         ]
#     }
# ])


# - Localizando a área ---------------------------------------------------------------------------

def numero_entre_intervalos(num, espectro):
    return min(espectro["a"], espectro["b"]) <= num <= max(espectro["a"], espectro["b"]) and \
           min(espectro["c"], espectro["d"]) <= num <= max(espectro["c"], espectro["d"])

# def encontrar_numero_old(dados, numero):
#     resultado = []    
#     for local in dados["locais"]:
#         for local_nome, conteudo in local.items():
#             for area_info in conteudo["areas"]:
#                 for area, espectro_info in area_info.items():
#                     for espectro in espectro_info["espectros"]:
#                         for espectro_id, valores in espectro.items():
#                             if numero_entre_intervalos(numero, valores):
#                                 resultado.append({
#                                     "local": local_nome,
#                                     "area": area,
#                                     "espectro_id": espectro_id
#                                 })
#     return resultado

def encontrar_numero(instalacao, sinal):
    resultado = []
    for local in instalacao['locais']:    
        for area in local['areas']:            
            for especto in area['espectos']:
                valores = {chave: especto[chave] for chave in ['a', 'b', 'c', 'd']}
                print(local['nome'],area['nome'],especto['ssid'],valores)
                if numero_entre_intervalos(sinal, valores):
                    resultado.append({
                        "local": local['nome'],
                        "area": area['nome'],
                        "espectro_id": especto['ssid']
                    })
    return resultado

atualiza_locais(1,obj1)
sinal = 35
instalacao = consulta_instalacao(1)
print(json.dumps(instalacao,indent=4))
resultado = encontrar_numero(instalacao, sinal)
for res in resultado:
    print(res)
    print(f"Localização : {res['local']} : {res['area']} ! espectro {res['espectro_id']}.")
