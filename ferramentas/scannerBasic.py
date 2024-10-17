import os
import sys
os.system('cls')
sys.path.insert(1, '../')

from comum      import p, pj, pausa
from comumWIFI  import scanner_basic

redes = scanner_basic()
pj(redes)