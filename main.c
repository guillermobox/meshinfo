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
        result[i] = (*metric.fun)( &data[0], &data[3], &data[6], &data[9]);
        data = &data[12];
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
        &metric_neta,
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
    printf("%13s %13s  %13s  %13s\n","METRIC","MIN","MEAN","MAX");
    printf("%13s %13.6e  %13.6e  %13.6e\n", m.name, min, mean, max);
}

struct st_fileparser * find_fileparser(char * filename, struct st_fileparser * fps, int ifps){
    regex_t r;
    struct st_fileparser * fp;
    int i, err;

    for(i=0; i<ifps; i++){
        fp = fps + i;
        printf("Trying: %s [%s]\n", fp->name, fp->regex_string);
        err = regcomp( &r, fp->regex_string, 0);
        if( err ){
            fprintf(stderr, "Regex compilation failed!");
            continue;
        }
        err = regexec(&r, filename, 0, NULL, 0);
        regfree(&r);
        if( err ){
            continue;
        }
        printf("Match!\n");
        return fp;
    }

    return NULL;
}

int main(int argc, char** argv){

    if( argc<2 ){
        printf("Usage: %s <meshfile> [<meshfile> ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct st_fileparser f[] = {
        {
            &parse_gmsh, 
            strdup("GMSH Mesh file"),
            strdup(".*\\.msh$")
        },
        {
            &parse_red,
            strdup("PREPROCESOR Mesh file"),
            strdup(".*\\.red\\.dat$")
        }
    };
    
    char * filename;
    struct st_fileparser * fp;
    int argi;
    for( argi=1; argi<argc; argi++ ){
        filename = argv[argi];
        fp = find_fileparser( filename, f, 2);
        if( fp==NULL ){
            fprintf(stderr, "I don't know this file format: %s\n", filename);
            continue;
        }
        parse_file(filename, *fp);
    }

    return 0;
}
