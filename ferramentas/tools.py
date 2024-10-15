import os
import sys
import json
os.system('cls')
sys.path.insert(1, '../')

from comum     import p, pt, pj, it, set_env_bat, pausa
from comumDB   import consulta_instalacao, grava_instalacao
from comumDB   import get_locais, atualiza_nome_local, add_local
from comumDB   import get_areas, atualiza_nome_area, add_area
from comumDB   import existe_area, exclui_todos_espectros, registra_espectros
from comumWIFI import scanner_media, monta_espectros

if os.environ.get('INSTALACAO') == None:
    id = it('informe o Instalação')
    os.environ['INSTALACAO'] = id
    set_env_bat()

instalacao_id = int(os.environ.get('INSTALACAO'))
local         = os.environ.get('LOCAL')
area          = os.environ.get('AREA')

pt('INSTALACAO',instalacao_id)
pt('LOCAL',local)
pt('AREA',area)

def get_modulo(opcao_):
    if opcao_ == 'L':
        return 'LOCAL'
    else:
        if opcao_ == 'A':
            return 'AREA'
        else:
            if opcao_ == 'E':
                return 'ESPECTRO'
    return 'indefiinido'

def main_menu():
    os.system('cls')
    pt('Instalacao',os.environ.get('INSTALACAO'))
    pt('Local',os.environ.get('LOCAL'))
    pt('Area',os.environ.get('AREA'))
    p('-')
    print('C = Configuração Completa')
    print('S = Configuração sem Espectros')
    print('L = Local')
    print('A = Area')
    print('E = Espectros')
    print('O = ScannerOne')
    print('X = Fim')
    return input('Entre com a opção: ').upper()

def menu(modulo_):
    os.system('cls')
    pt('Instalacao',os.environ.get('INSTALACAO'))
    pt('Local',os.environ.get('LOCAL'))
    pt('Area',os.environ.get('AREA'))
    p('-')
    pt('Módulo',get_modulo(modulo_))
    p('-')
    print('L = Listar')
    print('A = Alterar')
    print('C = Crar')
    print('E = Excluir')
    print('X = Volta')
    return input('Entre com a opção: ').upper()

def alterar(modulo_, data):
    os.system('cls')
    instalacao_id = os.environ.get('INSTALACAO')
    local = os.environ.get('LOCAL')
    area  = os.environ.get('AREA')
    pt('Local',local)
    pt('Area',area)
    pt('Módulo',get_modulo(modulo_))
    p('-')    
    pt('Alterar',area)
    p('-')    
    novo_atual = ''
    if modulo_ == 'L': novo_atual = local
    if modulo_ == 'A': novo_atual = area
    if modulo_ == 'E': return 'ESPECTRO'
    novo_nome = input('Entre com novo nome : ')
    sn = input(f'confirma alterar o nome do {get_modulo(modulo_)} : "{novo_atual}" por "{novo_nome}"? (s/n) ').lower()
    if sn == 's':
        if modulo_ == 'L': 
            atualiza_nome_local(data, local, novo_nome)
            os.environ['LOCAL'] = novo_nome
        if modulo_ == 'A': 
            atualiza_nome_area(data, local, area, novo_nome)
            os.environ['AREA'] = novo_nome
        grava_instalacao(instalacao_id, data)

def criar(modulo_, data):
    os.system('cls')
    instalacao_id = os.environ.get('INSTALACAO')
    local = os.environ.get('LOCAL')
    area  = os.environ.get('AREA')
    pt('Local',local)
    pt('Area',area)
    pt('Módulo',get_modulo(modulo_))
    p('-')    
    p('Criar')
    p('-')    
    nome = input('Entre com novo nome : ')
    sn = input(f'confirma CRIAR : {get_modulo(modulo_)} : "{nome}" ? (s/n) ').lower()
    if sn == 's':
        if modulo_ == 'L': 
            add_local(data, nome)
            os.environ['LOCAL'] = nome
            os.environ['AREA']  = ''
        if modulo_ == 'A': 
            add_area(data, local, nome)
            os.environ['AREA'] = nome
        grava_instalacao(instalacao_id, data)

def listar(modulo_, data):
    os.system('cls')
    local = os.environ.get('LOCAL')
    area  = os.environ.get('AREA')
    pt('Local',local)
    pt('Area',area)
    pt('Módulo',get_modulo(modulo_))
    p('-')    
    pt('Listar',get_modulo(modulo_))
    p('-')
    retorno = []
    if modulo_ == 'L':
        retorno = get_locais(data)
        print(retorno)
    else:
        if modulo_ == 'A':
            retorno = get_areas(data,local)
            print(retorno)
        else:
            if modulo_ == 'E':
                return 'ESPECTRO'
    p('-')
    enter = input('ENTER para continuar ou index para setar : ')
    if enter != '':
        try:
            enter = int(enter)
        except:
            enter = ''
            raise
        if type(enter) is int:
            if enter < len(retorno):
                if modulo_ == 'L':
                    os.environ['LOCAL'] = retorno[enter]
                else:
                    if modulo_ == 'A':
                        os.environ['AREA'] = retorno[enter]            

instalacao = consulta_instalacao(instalacao_id)
sai = False
while not sai:  
    opcao = 'L'
    modulo = main_menu()
    if modulo == 'X' or modulo == '': sai = True
    if modulo in ['L','A','E']:
        print('modulo',modulo)
        while opcao != 'X' and opcao != '' :
            opcao = menu(modulo)            
            if opcao == 'L':
                listar(modulo, instalacao)
            if opcao == 'A':
                alterar(modulo, instalacao)
            if opcao == 'C':
                criar(modulo, instalacao)
    if modulo == 'C':
        print(json.dumps(instalacao,indent=4))
        enter = input('ENTER para continuar')        
    if modulo == 'S':
        os.system('cls')
        pt('Instalacao',os.environ.get('INSTALACAO'))
        pt('Local',os.environ.get('LOCAL'))
        pt('Area',os.environ.get('AREA'))
        p('-')
        for local in instalacao['locais']:
            print('\t',local['nome'])
            for area in local['areas']:
                print('\t\t',area['nome'])
        enter = input('ENTER para continuar')
    if modulo == 'O':
        local = os.environ.get('LOCAL')
        area  = os.environ.get('AREA')
        sai = False
        while not sai:
            area = input(f'Entre com a AREA ({area}): ').strip() or area
            if area.lower() == 'sai':
                sai = True
                break
            print('LOCAL    :',local)
            print('AREA     :',area)
            # - verifica se area existe ---------------------------------------------
            print('')
            existe, _, _ = existe_area(instalacao, local, area)
            if (existe):
                print('area existe!')
                pausa()
            else:
                print('area nao existe!')
                resposta = input(f'Confirma a criação da area "{area}" (s/n) ? ')
                if resposta.lower() == 's':
                    p(f'criando area ... "{area}" em "{local}"')
                    add_area(instalacao, local, area)
                    grava_instalacao(instalacao_id, instalacao)
                    instalacao = consulta_instalacao(instalacao_id)
                    pj(instalacao)
                    pausa('',5)
                else:
                    p('area nao criada, scanner cancelado!')
                break
            os.system('cls')
            # - scanner -------------------------------------------------------------
            redes = scanner_media()
            espectros = []
            espectros = monta_espectros(espectros, redes, 'c')
            exclui_todos_espectros(instalacao, local, area)
            registra_espectros(instalacao, local, area, espectros)
            grava_instalacao(instalacao_id, instalacao)
            pj(instalacao)
set_env_bat()