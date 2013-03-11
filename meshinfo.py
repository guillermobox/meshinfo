# Busco un nodo en particular de la red que entra al simulador
import sys

if len(sys.argv)!=2:
    print 'Uso: {0} <whatever.1.red.dat>'.format(sys.argv[0])
    exit(0)

f = open(sys.argv[1], 'r')
f.readline() # linea vacia!
nnodos = int(f.readline())

def determinante(coordenadas):
    def diff(x): return x[1]-x[0]
    (v1,v2,v3,v4) = coordenadas[:]
    r1 = map(diff, zip(v1,v2))
    r2 = map(diff, zip(v1,v3))
    r3 = map(diff, zip(v1,v4))

    temp1 = r1[0]*r2[1]*r3[2] +\
            r1[2]*r2[0]*r3[1] +\
            r1[1]*r2[2]*r3[0]

    temp2 = r1[2]*r2[1]*r3[0] +\
            r1[1]*r2[0]*r3[2] +\
            r1[0]*r2[2]*r3[1]

    return abs(temp1 - temp2)

def condicion(x, y, z, xn, yn, zn):
    if sum(map( abs, [x-xn, y-yn, z-zn])) < 1e-3:
        return True

print 'Leo nodos'
coordenadas = []
nodos = []
while 1:
    line = f.readline()
    (i, ifront, icont, line) = tuple( line.split(None, 3) )
    (x, y, z, line) = tuple( line.split(None,3) )
    (x,y,z) = map(float, (x,y,z))
    i = int(i)
    nodos.append( (x,y,z) )

    if nnodos==i+1: break

print 'Leidos', nnodos, 'nodos'

print 'Leo elementos'
dets = []
nelems = int(f.readline())
for line in f.readlines():
    vertices = map(int,line.split())
    mis_vertices = tuple( nodos[i] for i in vertices )
    det = determinante(mis_vertices)
    dets.append(det)
    if det<1e-20:
        print 'Encontrado determinante<1e-20:',det
        print 'Vertices:', vertices
        print 'Coordenadas:'
        for co in mis_vertices:
            print '%13.6e %13.6e %13.6e'%co
        print

print 'Leidos',nelems,'elementos'
f = open('determinantes.txt', 'w')
for d in dets: f.write('%13.6e\n'%d)
f.close()
