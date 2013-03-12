#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include "fileparsers.h"
#include "metrics.h"

struct st_metric {
    double (*fun)(double*, double*, double*, double*);
    char * name;
    char * desc;
};

struct st_fileparser {
    int (*fun)(char *, double**, int*);
    char * name;
    char * regex_string;
};

void apply_metric(int ndata, double * data, struct st_metric metric, double *result){
    int i;
    printf("Aplying metric: %s\n", metric.name);
    printf("Description: %s\n", metric.desc);

    for( i=0; i<ndata; i++){
        result[i] = (*metric.fun)( &data[12*i], &data[12*i+3], &data[12*i+6], &data[12*i+9]);
    }
}

void parse_file(char *filename, struct st_fileparser parser){
    double *data;
    double * res;
    int nelem;
    int ret;

    printf("Parsing file: %s\n", filename);
    printf("Using parser: %s\n", parser.name);
    printf("With regex: %s\n", parser.regex_string);

    ret = (*parser.fun)(filename, &data, &nelem);
    res = malloc(nelem*sizeof(double));

    struct st_metric m = {
        &metric_determinant,
        strdup("Determinant"),
        strdup("Calculates the determinant of the function, just saying!") };

    apply_metric(nelem, data, m, res);
    double min, max, mean;
    min = res[0];
    max = res[0];
    mean = 0.0;
    int i;
    for(i=0; i<nelem; i++){
        if( res[i]<min ) min = res[i];
        if( res[i]>max ) max = res[i];
        mean += res[i];
    }
    mean /= nelem;
    printf("min   mean  max\n");
    printf("%f    %f   %f\n", min, mean, max);
}

int main(int argc, char** argv){

    if( argc<2 ){
        printf("Usage: %s <meshfile> [<meshfile> ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct st_fileparser f = {
        &parse_gmsh, 
        strdup("GMSH Mesh file"),
        strdup(".*\\.msh$")
        };
    
    /*
    regex_t r;
    printf("%d\n", regcomp( &r, ".*\\.msh$", 0));
    switch(regexec(&r, "galleta.msh", 0, NULL, 0)){
        case 0:
            printf("Match!");
            break;
        default:
            printf("mismatch!");
            break;
    }
    regfree(&r);
    */
    parse_file(argv[1], f);

    return 0;
}
