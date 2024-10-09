import os 
import sys
sys.path.insert(1, '../')
from comumDB import consulta_instalacao, recupera_todos_espectros, registra_espectros, grava_instalacao

os.system('cls')

if os.environ.get('LOCAL') == None:
    print('informe o local. SET LOCAL=andar_1')
    exit()
local   = os.environ.get('LOCAL')

if len(sys.argv) <= 1:  
    if os.environ.get('AREA') == None:
        print('informe o area. SET AREA=Sala de Estar')
        exit()
    area = os.environ.get('AREA')  
else:
    area = int(sys.argv[1])    

instalacao_id = 1
instalacao = consulta_instalacao(instalacao_id)

espectros_para_apagar = []
espectros = recupera_todos_espectros(instalacao, local, area)
for espectro in espectros:
    ok = False
    if not (espectro['n'] == espectro['s'] or espectro['n'] == espectro['l'] or espectro['n'] == espectro['o']):
        if not (espectro['s'] == espectro['l'] or espectro['s'] == espectro['o']):
            if espectro['l'] != espectro['o']:
                ok = True
    if not ok:
        espectros_para_apagar.append(espectro['bssid'])
espectros_filtrados = [item for item in espectros if item["bssid"] not in espectros_para_apagar]
print('LOCAL ............... :', local)
print('AREA ................ :', area)
print('espectros ........... :', len(espectros))
print('espectros validos ... :', len(espectros_filtrados))
resposta = input('atualiza os espectros (s/n) ? ')
if resposta.lower() == 's':
    registra_espectros(instalacao, local, area, espectros_filtrados)
    grava_instalacao(instalacao_id, instalacao)
exit()

home_theater = [
                        {
                            "ssid": "yic",
                            "bssid": "1a:eb:b6:c8:8d:72",
                            "n": "29",
                            "s": "29",
                            "l": "29",
                            "o": "29"
                        },
                        {
                            "ssid": "SSID Desconhecido",
                            "bssid": "78:8c:b5:51:99:92",
                            "n": "99",
                            "s": "94",
                            "l": "98",
                            "o": "92"
                        },
                        {
                            "ssid": "SSID Desconhecido",
                            "bssid": "78:8c:b5:51:ad:b2",
                            "n": "75",
                            "s": "83",
                            "l": "67",
                            "o": "85"
                        },
                        {
                            "ssid": "SSID Desconhecido",
                            "bssid": "78:8c:b5:51:ad:b3",
                            "n": "35",
                            "s": "35",
                            "l": "35",
                            "o": "35"
                        },
                        {
                            "ssid": "Pat&Victor Twibi",
                            "bssid": "24:fd:0d:e3:dd:e6",
                            "n": "31",
                            "s": "18",
                            "l": "18",
                            "o": "60"
                        },
                        {
                            "ssid": "Pat&Victor Twibi",
                            "bssid": "24:fd:0d:e3:dd:ee",
                            "n": "82",
                            "s": "82",
                            "l": "82",
                            "o": "82"
                        },
                        {
                            "ssid": "SUPER FAMILIA 5G TP-Link_EXT",
                            "bssid": "cc:32:e5:a5:e3:59",
                            "n": "18",
                            "s": "18",
                            "l": "18",
                            "o": "18"
                        },
                        {
                            "ssid": "FAMILIA NISIGUCHE",
                            "bssid": "c6:3d:d9:74:93:a7",
                            "n": "24",
                            "s": "24",
                            "l": "24",
                            "o": "24"
                        },
                        {
                            "ssid": "SUPER FAMILIA 2G TP-Link",
                            "bssid": "bc:c3:42:be:51:d0",
                            "n": "70",
                            "s": "50",
                            "l": "81",
                            "o": "72"
                        },
                        {
                            "ssid": "SUPER FAMILIA 2G TP-Link",
                            "bssid": "d4:6e:0e:b2:0e:9f",
                            "n": "67",
                            "s": "67",
                            "l": "67",
                            "o": "67"
                        },
                        {
                            "ssid": "[LG_Wall-Mount A/C]264d",
                            "bssid": "a6:36:c7:c5:26:4d",
                            "n": "82",
                            "s": "82",
                            "l": "82",
                            "o": "82"
                        },
                        {
                            "ssid": "Arnaldi",
                            "bssid": "7a:8c:b5:71:ad:b3",
                            "n": "20",
                            "s": "40",
                            "l": "26",
                            "o": "57"
                        },
                        {
                            "ssid": "Arnaldi",
                            "bssid": "7a:8c:b5:21:ad:b2",
                            "n": "84",
                            "s": "72",
                            "l": "70",
                            "o": "85"
                        },
                        {
                            "ssid": "Arnaldi",
                            "bssid": "7a:8c:b5:71:99:93",
                            "n": "92",
                            "s": "90",
                            "l": "95",
                            "o": "88"
                        },
                        {
                            "ssid": "AD_201_F_V7_Y___W.A__ECON_a1dc",
                            "bssid": "76:40:be:ff:a1:dc",
                            "n": "80",
                            "s": "80",
                            "l": "80",
                            "o": "80"
                        },
                        {
                            "ssid": "Rede WiFi de Barbosa",
                            "bssid": "dc:a4:ca:ee:5f:5a",
                            "n": "62",
                            "s": "62",
                            "l": "40",
                            "o": "40"
                        },
                        {
                            "ssid": "CUNHA",
                            "bssid": "48:2c:d0:8e:3a:e8",
                            "n": "86",
                            "s": "81",
                            "l": "90",
                            "o": "85"
                        },
                        {
                            "ssid": "PAT&VICTOR_5G",
                            "bssid": "68:02:b8:f9:bf:43",
                            "n": "70",
                            "s": "70",
                            "l": "70",
                            "o": "70"
                        },
                        {
                            "ssid": "ArnaldiNET",
                            "bssid": "a4:33:d7:01:65:c8",
                            "n": "98",
                            "s": "88",
                            "l": "87",
                            "o": "88"
                        },
                        {
                            "ssid": "[LG_CeilingCassette A/C]2a10",
                            "bssid": "a6:36:c7:9d:2a:10",
                            "n": "88",
                            "s": "88",
                            "l": "88",
                            "o": "88"
                        },
                        {
                            "ssid": "Sala de TV.o,",
                            "bssid": "fa:8f:ca:6e:63:9e",
                            "n": "85",
                            "s": "94",
                            "l": "94",
                            "o": "86"
                        },
                        {
                            "ssid": "DIRECT-9C-EPSON-9E2EBC",
                            "bssid": "52:57:9c:9e:ae:bc",
                            "n": "57",
                            "s": "46",
                            "l": "81",
                            "o": "65"
                        }
                    ]

sala_de_jantar = [
                        {
                            "ssid": "[washer] Samsung",
                            "bssid": "70:2c:1f:6f:8e:c9",
                            "n": "65",
                            "s": "65",
                            "l": "43",
                            "o": "43"
                        },
                        {
                            "ssid": "Magdalenos 2.4G",
                            "bssid": "86:ea:ea:28:f0:c8",
                            "n": "18",
                            "s": "18",
                            "l": "13",
                            "o": "13"
                        },
                        {
                            "ssid": "Magdalenos 5G",
                            "bssid": "94:ea:ea:28:f0:c8",
                            "n": "20",
                            "s": "20",
                            "l": "11",
                            "o": "11"
                        },
                        {
                            "ssid": "Arashiro",
                            "bssid": "30:de:4b:ed:1d:72",
                            "n": "50",
                            "s": "50",
                            "l": "53",
                            "o": "53"
                        },
                        {
                            "ssid": "SSID Desconhecido",
                            "bssid": "78:8c:b5:51:ad:b3",
                            "n": "57",
                            "s": "57",
                            "l": "72",
                            "o": "65"
                        },
                        {
                            "ssid": "SSID Desconhecido",
                            "bssid": "78:8c:b5:51:99:92",
                            "n": "89",
                            "s": "90",
                            "l": "95",
                            "o": "92"
                        },
                        {
                            "ssid": "SSID Desconhecido",
                            "bssid": "78:8c:b5:51:ad:b2",
                            "n": "85",
                            "s": "85",
                            "l": "75",
                            "o": "82"
                        },
                        {
                            "ssid": "[LG_Wall-Mount A/C]265a",
                            "bssid": "a6:36:c7:c5:26:5a",
                            "n": "31",
                            "s": "31",
                            "l": "31",
                            "o": "31"
                        },
                        {
                            "ssid": "Escrit\u00c3\u0192\u00c2\u00b3rio_5.0",
                            "bssid": "cc:32:e5:fe:17:3f",
                            "n": "26",
                            "s": "26",
                            "l": "26",
                            "o": "24"
                        },
                        {
                            "ssid": "Clarinha_2.4G",
                            "bssid": "86:0a:62:36:14:7e",
                            "n": "13",
                            "s": "24",
                            "l": "24",
                            "o": "13"
                        },
                        {
                            "ssid": "SUPER FAMILIA 5G TP-Link",
                            "bssid": "d4:6e:0e:b2:0e:9e",
                            "n": "24",
                            "s": "18",
                            "l": "18",
                            "o": "18"
                        },
                        {
                            "ssid": "ASSESCON-5G",
                            "bssid": "84:0b:bb:9f:1b:9f",
                            "n": "20",
                            "s": "18",
                            "l": "18",
                            "o": "26"
                        },
                        {
                            "ssid": "SUPER FAMILIA 2G TP-Link",
                            "bssid": "d4:6e:0e:b2:0e:9f",
                            "n": "72",
                            "s": "53",
                            "l": "65",
                            "o": "62"
                        },
                        {
                            "ssid": "Celia2g",
                            "bssid": "74:3a:ef:5b:cf:3d",
                            "n": "62",
                            "s": "50",
                            "l": "50",
                            "o": "50"
                        },
                        {
                            "ssid": "Arnaldi",
                            "bssid": "7a:8c:b5:21:ad:b2",
                            "n": "84",
                            "s": "81",
                            "l": "80",
                            "o": "82"
                        },
                        {
                            "ssid": "Arnaldi",
                            "bssid": "7a:8c:b5:71:ad:b3",
                            "n": "57",
                            "s": "60",
                            "l": "26",
                            "o": "67"
                        },
                        {
                            "ssid": "Arnaldi",
                            "bssid": "7a:8c:b5:21:99:92",
                            "n": "90",
                            "s": "91",
                            "l": "95",
                            "o": "92"
                        },
                        {
                            "ssid": "Arnaldi",
                            "bssid": "7a:8c:b5:71:99:93",
                            "n": "92",
                            "s": "87",
                            "l": "84",
                            "o": "91"
                        },
                        {
                            "ssid": "SUPER FAMILIA 2G TP-Link_EXT",
                            "bssid": "cc:32:e5:a5:e3:5a",
                            "n": "46",
                            "s": "82",
                            "l": "81",
                            "o": "62"
                        },
                        {
                            "ssid": "Rede Wi\u00c3\u00a2\u00e2\u201a\u00ac\u00e2\u20ac\u02dcFi de Barbosa",
                            "bssid": "dc:a4:ca:ee:5f:5a",
                            "n": "50",
                            "s": "50",
                            "l": "50",
                            "o": "65"
                        },
                        {
                            "ssid": "SUPER FAMILIA 5G TP-Link_EXT",
                            "bssid": "cc:32:e5:a5:e3:59",
                            "n": "18",
                            "s": "14",
                            "l": "8",
                            "o": "14"
                        },
                        {
                            "ssid": "FAMILIA NISIGUCHE",
                            "bssid": "c6:3d:d9:74:93:a7",
                            "n": "24",
                            "s": "82",
                            "l": "33",
                            "o": "46"
                        },
                        {
                            "ssid": "FAMILIA NISIGUCHE",
                            "bssid": "c0:3d:d9:74:93:a0",
                            "n": "60",
                            "s": "86",
                            "l": "70",
                            "o": "70"
                        },
                        {
                            "ssid": "Pat&Victor Twibi",
                            "bssid": "24:fd:0d:e3:dd:e6",
                            "n": "46",
                            "s": "22",
                            "l": "18",
                            "o": "20"
                        },
                        {
                            "ssid": "Pat&Victor Twibi",
                            "bssid": "24:fd:0d:e3:dd:e3",
                            "n": "70",
                            "s": "75",
                            "l": "75",
                            "o": "75"
                        },
                        {
                            "ssid": "Pat&Victor Twibi",
                            "bssid": "24:fd:0d:e3:dd:ee",
                            "n": "81",
                            "s": "81",
                            "l": "70",
                            "o": "82"
                        },
                        {
                            "ssid": "Pat&Victor Twibi",
                            "bssid": "24:fd:0d:e3:dd:eb",
                            "n": "96",
                            "s": "85",
                            "l": "88",
                            "o": "91"
                        },
                        {
                            "ssid": "FAMILIA NISIGUCHE-5G",
                            "bssid": "c0:3d:d9:74:93:a7",
                            "n": "31",
                            "s": "82",
                            "l": "50",
                            "o": "43"
                        },
                        {
                            "ssid": "Vieira",
                            "bssid": "86:0b:bb:9f:d0:4f",
                            "n": "13",
                            "s": "13",
                            "l": "22",
                            "o": "22"
                        },
                        {
                            "ssid": "DIRECT-9C-EPSON-9E2EBC",
                            "bssid": "52:57:9c:9e:ae:bc",
                            "n": "75",
                            "s": "80",
                            "l": "80",
                            "o": "80"
                        },
                        {
                            "ssid": "Clarinha_5G",
                            "bssid": "90:0a:62:36:14:7e",
                            "n": "13",
                            "s": "26",
                            "l": "24",
                            "o": "11"
                        },
                        {
                            "ssid": "CUNHA",
                            "bssid": "48:2c:d0:8e:3a:ec",
                            "n": "83",
                            "s": "72",
                            "l": "72",
                            "o": "80"
                        },
                        {
                            "ssid": "CUNHA",
                            "bssid": "48:2c:d0:8e:3a:e8",
                            "n": "84",
                            "s": "72",
                            "l": "81",
                            "o": "87"
                        },
                        {
                            "ssid": "CUNHA-Iot",
                            "bssid": "48:2c:d0:8e:3a:ea",
                            "n": "75",
                            "s": "35",
                            "l": "80",
                            "o": "87"
                        },
                        {
                            "ssid": "PAT&VICTOR_5G",
                            "bssid": "68:02:b8:f9:bf:43",
                            "n": "75",
                            "s": "50",
                            "l": "70",
                            "o": "70"
                        },
                        {
                            "ssid": "[LG_Wall-Mount A/C]264d",
                            "bssid": "a6:36:c7:c5:26:4d",
                            "n": "35",
                            "s": "35",
                            "l": "57",
                            "o": "65"
                        },
                        {
                            "ssid": "ArnaldiNET",
                            "bssid": "a6:33:d7:01:65:cf",
                            "n": "89",
                            "s": "85",
                            "l": "80",
                            "o": "87"
                        },
                        {
                            "ssid": "ArnaldiNET",
                            "bssid": "a4:33:d7:01:65:c8",
                            "n": "85",
                            "s": "85",
                            "l": "82",
                            "o": "85"
                        },
                        {
                            "ssid": "[LG_CeilingCassette A/C]bff6",
                            "bssid": "a6:36:c7:ff:bf:f6",
                            "n": "72",
                            "s": "80",
                            "l": "80",
                            "o": "60"
                        },
                        {
                            "ssid": "[LG_CeilingCassette A/C]2a10",
                            "bssid": "a6:36:c7:9d:2a:10",
                            "n": "85",
                            "s": "81",
                            "l": "81",
                            "o": "84"
                        },
                        {
                            "ssid": "PAT&VICTOR_2G",
                            "bssid": "68:02:b8:f9:bf:42",
                            "n": "87",
                            "s": "82",
                            "l": "82",
                            "o": "91"
                        },
                        {
                            "ssid": "Sala de TV.o,",
                            "bssid": "fa:8f:ca:6e:63:9e",
                            "n": "87",
                            "s": "84",
                            "l": "86",
                            "o": "90"
                        }
                    ]