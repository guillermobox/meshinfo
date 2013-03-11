# !/usr/bin/env python
#
# Mesh analyzer
# Analice the mesh, apply metrics and print a summary
#
# Guillermo Indalecio Fernandez, March-2013

import sys
import numpy

def metric__determinant(v1, v2, v3, v4):
    'Determinant of the element'

    r1 = v2 - v1
    r2 = v3 - v1
    r3 = v4 - v1

    temp1 = r1[0]*r2[1]*r3[2] +\
            r1[2]*r2[0]*r3[1] +\
            r1[1]*r2[2]*r3[0]

    temp2 = r1[2]*r2[1]*r3[0] +\
            r1[1]*r2[0]*r3[2] +\
            r1[0]*r2[2]*r3[1]

    return abs(temp1 - temp2)

def parsefile_msh(filename):
    fh = open(filename, 'r')
    assert fh.readline().strip()=='$MeshFormat'
    assert fh.readline().startswith('2.2')
    assert fh.readline().strip()=='$EndMeshFormat'
    assert fh.readline().strip()=='$Nodes'
    nnodes = int(fh.readline())
    nodes = [None]*nnodes
    i=0
    while i < nnodes:
        line = fh.readline()
        (ind,x,y,z) = line.split()
        position = numpy.array(map(float, (x,y,z)))
        nodes[i] = position
        i += 1

    assert fh.readline().strip()=='$EndNodes'
    assert fh.readline().strip()=='$Elements'

    total_nelem = int(fh.readline())
    tetra_nelem = 0
    elems = []
    i_elem = 0
    while i_elem < total_nelem:
        i_elem += 1
        line = fh.readline()
        (i, sides, tmp1, tmp2, tmp3, line) = tuple(line.split(None, 5))
        if int(sides)==4:
            tetra_nelem += 1
            vert = map(int, line.split())
            elems.append( (nodes[v-1] for v in vert) )

    assert fh.readline().strip()=='$EndElements'
    fh.close()
    return elems

def parsefile_red(filename):
    fh = open(filename, 'r')
    fh.readline()
    nnodes = int(fh.readline())

    nodes = []
    i=0
    while i+1 < nnodes:
        line = fh.readline()
        (i, ifront, icont, line) = tuple( line.split(None, 3) )
        (x, y, z, line) = tuple( line.split(None,3) )
        position = numpy.array( map(float, (x,y,z)) )
        i = int(i)
        nodes.append( position )

    nelems = int(fh.readline())
    coords = [None]*nelems
    for (i, line) in enumerate(fh.readlines()):
        vert = map(int, line.split())
        coords[i] = (nodes[v] for v in vert)

    fh.close()
    return coords

def process(filename, metrics):
    print
    print filename.center(80,'=')
    print

    if filename.endswith('.red.dat'):
        coords = parsefile_red(filename)
    elif filename.endswith('.msh'):
        coords = parsefile_msh(filename)
    else:
        print 'Ups! I cant recognize that format!'
        return

    results = {}
    for (k,f) in metrics.iteritems():
        results[k] = map(lambda c:f(*c), coords)

    print '%15s %13s %13s %13s %13s'%('metric','mean', 'std', 'min', 'max')
    for (key, array) in results.iteritems():
        array = numpy.array(array)
        print '%15s %13.6e %13.6e %13.6e %13.6e'%(key, array.mean(), array.std(), array.min(), array.max())

def main():
    if len(sys.argv)<2:
        print 'Usage: {0} <meshfile> [<meshfile> ...]'.format(sys.argv[0])
        print
        exit(1)

    metrics = {}
    for name in globals().keys():
        if name.startswith('metric__'):
            print 'Found metric (%s): %s'%(name[8:], globals()[name].__doc__)
            metrics[name[8:]] = globals()[name] 

    print
    for filename in sys.argv[1:]: 
        process(filename, metrics)

if __name__=='__main__':
    main()
