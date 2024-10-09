import os
import sys
import json
sys.path.insert(1, '../')

from comum   import p, pt, pj
from comumDB import consulta_instalacao, grava_instalacao
from comumDB import get_locais, atualiza_nome_local, add_local, apaga_local
from comumDB import get_areas, atualiza_nome_area, add_area, apaga_area

local   = os.environ.get('LOCAL')
area    = os.environ.get('AREA')

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
    print('S = Show')
    print('L = Local')
    print('A = Area')
    print('E = Espectros')
    print('X = Fim')
    return input('Entre com a opção: ').upper()

def menu(modulo_):
    os.system('cls')
    pt('Local',os.environ.get('LOCAL'))
    pt('Area',os.environ.get('AREA'))
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

def criar(modulo_, data):
    os.system('cls')
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

instalacao_id = 1
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
    if modulo == 'S':
        print(json.dumps(instalacao,indent=4))
        enter = input('ENTER para continuar')        