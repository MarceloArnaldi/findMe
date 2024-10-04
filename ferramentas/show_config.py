import os
import sys
import json
from pymongo import MongoClient

client = MongoClient("mongodb+srv://marceloarnaldi:BITh5VIzm3vY3Eoc@clusterdev.syoui.mongodb.net/?retryWrites=true&w=majority&appName=ClusterDev")
db = client['ClusterDev']
collection = db['instalacoes']

def consulta_instalacao(instalacao_id):
    documento = collection.find_one({"instalacao": instalacao_id})
    del documento['_id']
    return documento

os.system('cls')

if len(sys.argv) <= 1:  
    instalacao = 1
else:
    instalacao = int(sys.argv[1])

instalacaoObj = consulta_instalacao(instalacao)
print(json.dumps(instalacaoObj,indent=4))