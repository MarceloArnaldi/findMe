import re
import json
import subprocess
from comum import pj
from pymongo import MongoClient

# - rotinas mongo ---------------------------------------------------------------------------------
client = MongoClient("mongodb+srv://marceloarnaldi:BITh5VIzm3vY3Eoc@clusterdev.syoui.mongodb.net/?retryWrites=true&w=majority&appName=ClusterDev")
db = client['ClusterDev']
collection = db['instalacoes']
# - INSTALACAO ------------------------------------------------------------------------------------
def consulta_instalacao(instalacao_id):
    documento = collection.find_one({"instalacao": instalacao_id})
    del documento['_id']
    return documento
def grava_instalacao(instalacao_id, obj):
    locais = obj['locais']
    collection.update_one(
        {"instalacao": instalacao_id},
        {"$set": {"locais": locais}}
    )
# - LOCAL -----------------------------------------------------------------------------------------
def existe_local(data, local_):
    for x, local in enumerate(data.get("locais", [])):
        if local['nome'] == local_:
            return True, x
    return False, -1
def get_local(data, local_):
    existe, index = existe_local(data, local_)
    if existe:
        return data['locais'][index]
    else:
        return None
def atualiza_nome_local(data, local_, novo_nome):
    existe, index = existe_local(data, local_)
    if existe:
        data['locais'][index]['nome'] = novo_nome
    return True
def add_local(data, local_):
    existe, _ = existe_local(data, local_)
    if not existe:
        data['locais'].append({
                'nome': local_,
                'areas': []
            })
    return True
def apaga_local(data, local_):
    existe, index = existe_local(data, local_)
    if existe:
        del data['locais'][index]
    return True  
# - AREA ------------------------------------------------------------------------------------------
def existe_area(data, local_, area_):
    for l, local in enumerate(data.get("locais", [])):
        if local['nome'] == local_:
            for a, area in enumerate(local.get("areas",[])):
                if area.get("nome") == area_:
                    return True, a, l
    return False, -1, -1
def get_area(data, local_, area_):
    existe, index, local_index = existe_area(data, local_, area_)
    if existe:
        return data['locais'][local_index]['areas'][index]
    else:
        return None
def atualiza_nome_area(data, local_, area_, novo_nome):
    existe, index, local_index = existe_area(data, local_, area_)
    if existe:
        data['locais'][local_index]['areas'][index]['nome'] = novo_nome
    return True
def add_area(data, local_, area_):
    existe, _, local_index = existe_area(data, local_, area_)
    if not existe:
        data['locais'][local_index]['areas'].append({
                    'nome': area_,
                    "espectros": []
                })
    return True
def apaga_area(data, local_, area_):
    existe, index, local_index = existe_area(data, local_, area_)
    if existe:
        del data['locais'][local_index]['areas'][index]
    return True
# - ESPECTRO --------------------------------------------------------------------------------------
def espectro_existe(data, local_, area_, bssid_):
    area_existe, area_index, local_index = existe_area(data, local_, area_)
    if area_existe:
        area = get_area(data, local_, area_)
        for x, espectro in enumerate(area.get("espectros",[])):
            if espectro['bssid'] == bssid_:
                return True, x, area_index, local_index
    return False, -1, -1, -1
def get_ssid_name(data, local_, area_, bssid_):
    existe, index, area_index, local_index = espectro_existe(data, local_, area_, bssid_)
    if existe:
        return data['locais'][local_index]['areas'][area_index]['espectros'][index]['ssid']
    else:
        return None
def get_ssid_sinais(data, local_, area_, bssid_):
    existe, index, area_index, local_index = espectro_existe(data, local_, area_, bssid_)
    if existe:
        espectro = data['locais'][local_index]['areas'][area_index]['espectros'][index]
        return espectro
    else:
        return None
def apaga_espectro(data, local_, area_, bssid_):
    existe, index, area_index, local_index = espectro_existe(data, local_, area_, bssid_)
    if existe:
        print('exluir ->',data['locais'][local_index]['areas'][area_index]['espectros'][index])
        del data['locais'][local_index]['areas'][area_index]['espectros'][index]    
    return True
# - Exclui TODOS espectro ---------------------------------------------------------------------------
def exclui_todos_espectros(data, local_, area_):
    existe, area_index, local_index = existe_area(data, local_, area_)
    if existe:
        data['locais'][local_index]['areas'][area_index]['espectros'] = []
    return True
# - recupera TODOS espectros-------------------------------------------------------------------------
def recupera_todos_espectros(data, local_, area_):
    existe_area_, area_index, local_index = existe_area(data, local_, area_)
    if existe_area_:
        return data['locais'][local_index]['areas'][area_index]['espectros']
    else:
        return None
# - registra TODOS espectros-------------------------------------------------------------------------
def registra_espectros(data, local_, area_, espectros_):
    existe, area_index, local_index = existe_area(data, local_, area_)
    if existe:
        data['locais'][local_index]['areas'][area_index]['espectros'] = espectros_
    return True
# - registra o sinal dos SSID de cada ponto ---------------------------------------------------------
def registra_espectro(data, local_, area_, espectros_sinal_, posicao_):
    instalacao_id = data['instalacao']
    local_index = next((i for i, d in enumerate(data['locais']) if d["nome"] == local_), -1)
    if local_index > -1:        
        existe_area_, area_index = existe_area(data, local_, area_)
        if existe_area_:
            for espectro in espectros_sinal_:
                espectro_existe_, espectro_index = espectro_existe(data, local_, area_, espectro['bssid'])
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