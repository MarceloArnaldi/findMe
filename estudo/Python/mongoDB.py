from pymongo import MongoClient

# Conectar ao MongoDB
client = MongoClient("mongodb+srv://marceloarnaldi:<senha>@clusterdev.syoui.mongodb.net/?retryWrites=true&w=majority&appName=ClusterDev")
db = client['ClusterDev']
collection = db['instalacoes']

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
                                "dbssid_code": 1,
                                "a": 450,
                                "b": 40,
                                "c": 50,
                                "d": 52
                            },
                            {
                                "dbssid_code": 2,
                                "a": 30,
                                "b": 40,
                                "c": 34,
                                "d": 22
                            }
                        ]
                    }
                ]
            }
        ]
    }
    collection.insert_one(documento)
    print("Documento inserido com sucesso!")

# Consultar um documento
def consultar_instalacao(instalacao_id):
    documento = collection.find_one({"instalacao": instalacao_id})
    print("Documento encontrado:", documento)

# Alterar um documento
def alterar_instalacao(instalacao_id, nova_area):
    collection.update_one(
        {"instalacao": instalacao_id},
        {"$set": {"locais.0.areas": nova_area}}
    )
    print("Documento alterado com sucesso!")

# Exemplo de uso
inserir_instalacao()
consultar_instalacao(1)
alterar_instalacao(1, [
    {
        "nome": "Sala de Estar",
        "espectros": [
            {
                "dbssid_code": 3,
                "a": 100,
                "b": 200,
                "c": 150,
                "d": 75
            }
        ]
    }
])
consultar_instalacao(1)
