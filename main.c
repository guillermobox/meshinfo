#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include "fileparsers.h"
#include "metrics.h"

int flag_save;

struct st_metric {
    double (*fun)(double*, double*, double*, double*);
    char * name;
    char * desc;
};
#define METRIC(a,b,c) {&a, strdup(b), strdup(c)}

struct st_fileparser {
    int (*fun)(char *, double**, int*);
    char * name;
    char * regex_string;
};
#define FILEPARSER(a,b,c) {&a, strdup(b), strdup(c)}

static void printcenter(char * str, char sep){
    char line[81];
    int m, i, left, right;

    m = strlen(str);

    left = (80-m-1)/2;
    right = 80-left-m-2;

    for(i=0; i<left; i++){
        line[i] = sep;
    }
    for(i=0; i<right; i++){
        line[79-i] = sep;
    }
    line[left] = ' ';
    line[79-right] = ' ';

    line[80] = '\0';

    strncpy( &(line[left+1]), str, m);
    puts(line);
}

static void header(char *str){
    puts("");
    printcenter(str, '=');
    puts("");
}

static void apply_metrics(int ndata, double * data, int nmetrics, struct st_metric * metrics, double *result){
    int i, im;

    for( i=0; i<ndata; i++){
        for( im=0; im<nmetrics; im++ ){
            result[nmetrics*i + im] = (*(metrics[im].fun))( &data[0], &data[3], &data[6], &data[9]);
        }
        data = &data[12];
    }
}

static void process_file(char *filename, struct st_fileparser parser, int nmetrics, struct st_metric * metrics){
    double *data;
    double * res;
    int nelem;
    int ret;
    int im;

    header(filename);

    ret = (*parser.fun)(filename, &data, &nelem);

    res = malloc( nelem*nmetrics*sizeof(double) );

    apply_metrics(nelem, data, nmetrics, metrics, res);

    printf("%13s  %13s  %13s  %13s\n","METRIC","MIN","MEAN","MAX");


    for(im=0; im<nmetrics; im++){
        FILE * fh;
        char metric_filename[128];
        strcpy(metric_filename, "metric.");
        strcat(metric_filename, metrics[im].name);
        strcat(metric_filename, ".");
        strcat(metric_filename, filename);
        if( flag_save )
            fh = fopen(metric_filename, "w");

        double min, max, mean;
        int i;
        min = res[im];
        max = res[im];
        mean = 0.0;
        for(i=0; i<nelem; i++){
            if( flag_save )
                fprintf(fh, "%13.6e\n", res[i*nmetrics+im]);
            if( res[i*nmetrics+im]<min ) min = res[i*nmetrics + im];
            if( res[i*nmetrics+im]>max ) max = res[i*nmetrics + im];
            mean += res[i*nmetrics + im];
        }
        mean /= nelem;

        printf("%13s  %13.6e  %13.6e  %13.6e\n", metrics[im].name, min, mean, max);
        if( flag_save )
            fclose(fh);
    }

    free(res);
    free(data);
}

static struct st_fileparser * find_fileparser(char * filename, struct st_fileparser * fps, int ifps){
    regex_t r;
    struct st_fileparser * fp;
    int i, err;

    for(i=0; i<ifps; i++){
        fp = fps + i;
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
        return fp;
    }

    return NULL;
}

static void show_metrics(int nmetrics, struct st_metric * metrics){
    int i;
    header("METRICS DEFINED");
    for(i=0; i<nmetrics; i++)
        printf("  [%s] %s\n", metrics[i].name, metrics[i].desc);
}

static void show_parsers(int nparsers, struct st_fileparser *parsers){
    int i;
    header("PARSERS DEFINED");
    for(i=0; i<nparsers; i++)
        printf("  [%s] /%s/\n", parsers[i].name, parsers[i].regex_string);
}

int main(int argc, char** argv){
    int c;

    if( argc<2 ){
        printf("Usage: %s [options] <meshfile> [<meshfile> ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while( (c = getopt(argc, argv, "s")) != -1)
        switch(c){
            case 's':
                flag_save = 1;
                break;
            case '?':
                if ( isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf( stderr, "Unknown character '``x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }

    struct st_fileparser parsers[] = {
        FILEPARSER(parse_gmsh,  "GMSH Mesh file",           ".*\\.msh$"),
        FILEPARSER(parse_red,   "PREPROCESDOR Mesh file",   ".*\\.red\\.dat$")
    };
    int nparsers = sizeof(parsers) / sizeof(struct st_fileparser);

    struct st_metric metrics[] = {
        METRIC(metric_determinant,  "determinant",  "Calculates the determinant of the vertices"),
        METRIC(metric_shewchuk,     "shewchuk",     "Shewchuk parameter from 2002 paper"),
        METRIC(metric_dummyone,     "dummy",        "Dummy: returns 1 all the time"),
        METRIC(metric_neta,         "neta",         "Neta parameter from gmsh"),
        METRIC(metric_rho,          "rho",          "Rho parameter form gmsh"),
    };
    int nmetrics = sizeof(metrics) / sizeof(struct st_metric);

    show_metrics(nmetrics, metrics);
    show_parsers(nparsers, parsers);

    char * filename;
    struct st_fileparser * fp;
    int argi;
    for( argi = optind; argi<argc; argi++ ){
        filename = argv[argi];
        fp = find_fileparser(filename, parsers, nparsers);
        if( fp==NULL ){
            fprintf(stderr, "I don't know this file format: %s\n", filename);
            continue;
        }
        process_file(filename, *fp, nmetrics, metrics);
    }

    return 0;
}
