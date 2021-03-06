#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#define BUFFSIZE 1024

static int notoverflow(long n){
    return (n!=LONG_MAX) && (n!=LONG_MIN);
}

int parse_gmsh(char * filename, double ** data, int * nelem){

    FILE *f = fopen(filename, "r");

    if( f==NULL ){
        perror("parse_gmsh");
        return 1;
    }

    char buffer[BUFFSIZE], * pbuffer;
    int lineno;
    int nnodes=0, nelems=0, i;
    int nodenum=1, elemnum=1, position=0, tetranum=1;
    double *nodes=NULL, *elems=NULL;

    lineno = 0;
    while( fgets(buffer, BUFFSIZE, f)!=NULL ){
        if( lineno==1 ){
            pbuffer = buffer;
            assert( strncmp(pbuffer, "2.2", 3)==0 );
        }else if( lineno==4 ){
            pbuffer = buffer;
            nnodes = strtol(pbuffer, NULL, 10);
            nodes = malloc( nnodes*3*sizeof(double) );
        }else if( lineno > 4 && lineno <= 4+nnodes ){
            pbuffer = buffer;
            assert( notoverflow(strtol(pbuffer, &pbuffer, 10)) );
            nodes[ 3*(nodenum-1) + 0 ] = strtod(pbuffer, &pbuffer);
            nodes[ 3*(nodenum-1) + 1 ] = strtod(pbuffer, &pbuffer);
            nodes[ 3*(nodenum-1) + 2 ] = strtod(pbuffer, &pbuffer);
            nodenum += 1;
        }else if( lineno == 4 + nnodes + 3 ){
            pbuffer = buffer;
            nelems = strtol(pbuffer, NULL, 10);
            elems = malloc( 12 * nelems * sizeof(double) );
        }else if( lineno > 4 + nnodes + 3 && lineno <= 4 + nnodes + 3 + nelems ){
            pbuffer = buffer;
            assert( strtol(pbuffer, &pbuffer, 10) == elemnum );
            if( strtol(pbuffer, &pbuffer, 10) == 4 ){
                assert( notoverflow(strtol(pbuffer, &pbuffer, 10)) );
                assert( notoverflow(strtol(pbuffer, &pbuffer, 10)) );
                assert( notoverflow(strtol(pbuffer, &pbuffer, 10)) );
                for( i=0; i<4; i++){
                    position = strtol(pbuffer, &pbuffer, 10);
                    memcpy( elems + 12*(tetranum-1) + 3*i, nodes + 3*(position-1), 3*sizeof(double));
                }
                tetranum += 1;
            }
            elemnum += 1;
        }else if( lineno == 4 + nnodes + 3 + nelems + 1){
            assert( strncmp(buffer, "$EndElements", 12)==0 );
        }
        lineno += 1;
    }

    free(nodes);
    fclose(f);

    *data = elems;
    *nelem = tetranum-1;
    return 0;
}


int parse_red(char * filename, double ** data, int * nelem){
    FILE * f = fopen(filename, "r");

    if( f==NULL ){
        perror("parse_red");
        return 1;
    }

    char buffer[BUFFSIZE], * pbuffer;
    int lineno;
    int nnodes=0, nelems=0, i;
    int nodenum=0, elemnum=0, position=0;
    double * nodes=NULL, * elems=NULL;

    lineno = 0;
    while( fgets(buffer, BUFFSIZE, f)!=NULL ){
        if( lineno==1 ){
            pbuffer = buffer;
            nnodes = strtol(pbuffer, NULL, 10);
            nodes = malloc( nnodes*3*sizeof(double) );

        }else if( lineno > 1 && lineno <= 1+nnodes ){
            pbuffer = buffer;
            assert( strtol(pbuffer, &pbuffer, 10) == nodenum );
            assert( notoverflow(strtol(pbuffer, &pbuffer,10)) );
            assert( notoverflow(strtol(pbuffer, &pbuffer,10)) );
            nodes[ 3*nodenum + 0 ] = strtod(pbuffer, &pbuffer);
            nodes[ 3*nodenum + 1 ] = strtod(pbuffer, &pbuffer);
            nodes[ 3*nodenum + 2 ] = strtod(pbuffer, &pbuffer);
            nodenum += 1;

        }else if( lineno == nnodes + 2 ){
            pbuffer = buffer;
            nelems = strtol(pbuffer, &pbuffer, 10);
            elems = malloc ( nelems*12*sizeof(double) );

        }else if( lineno > nnodes + 2 && lineno <= nnodes + 2 + nelems){
            pbuffer = buffer;
            for( i=0; i<4; i++){
                position = strtol(pbuffer, &pbuffer, 10);
                memcpy( elems + 12*elemnum + 3*i, nodes + 3*position, 3*sizeof(double));
            }
            elemnum += 1;
        }
        lineno += 1;
    }

    free(nodes);
    fclose(f);
    *data = elems;
    *nelem = elemnum;
    return 0;
}
