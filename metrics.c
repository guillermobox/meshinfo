#include <math.h>

static void cross(double *v1, double *v2, double *res){
    res[0] = v1[1]*v2[2] - v1[2]*v2[1];
    res[1] = v1[2]*v2[0] - v1[0]*v2[2];
    res[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

static double dot(double *v1, double *v2){
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

static double norm2(double *u){
    return u[0]*u[0] + u[1]*u[1] + u[2]*u[2];
}

static double length(double * u, double *v){
    double d0,d1,d2;
    d0 = v[0] - u[0];
    d1 = v[1] - u[1];
    d2 = v[2] - u[2];
    return sqrt( d0*d0 + d1*d1 + d2*d2 );
}

static double positive_area(double *a, double *b, double *c){
    double temp = 0.0;
    double t;

    t = (a[1]-c[1])*(b[2]-c[2]) - (a[2]-c[2])*(b[1]*c[1]);
    temp += t*t;
    t = (a[2]-c[2])*(b[0]-c[0]) - (a[0]-c[0])*(b[2]*c[2]);
    temp += t*t;
    t = (a[0]-c[0])*(b[1]-c[1]) - (a[1]-c[1])*(b[0]*c[0]);
    temp += t*t;

    return sqrt(temp)/2.0;
}

static double determinant(double *a, double *b, double *c, double *d){
    double t1,t2;
    double t[3], u[3], v[3];
    int i;

    for(i=0; i<3; i++){
        t[i] = a[i] - d[i];
        u[i] = b[i] - d[i];
        v[i] = c[i] - d[i];
    }

    t1 = t[0]*u[1]*v[2] +\
         t[2]*u[0]*v[1] +\
         t[1]*u[2]*v[0];

    t2 = t[2]*u[1]*v[0] +\
         t[1]*u[0]*v[2] +\
         t[0]*u[2]*v[1];

    return t1-t2;
}

static double volume(double *a, double *b, double *c, double *d){
    return determinant(a,b,c,d)/6.0;
}

double metric_determinant(double *a, double *b, double *c, double *d){
    return determinant(a,b,c,d);
}

double metric_shewchuk(double *a, double *b, double *c, double *d){
    double num, den, areas[4];
    double t1;

    areas[0] = positive_area(a,b,c);
    areas[1] = positive_area(a,b,d);
    areas[2] = positive_area(a,c,d);
    areas[3] = positive_area(b,c,d);

    num = volume(a,b,c,d);
    t1 = dot(areas, areas);
    den = pow(t1, 3.0/4.0);
    return 4.835564693*num/den;
}

double metric_neta(double *a, double *b, double *c, double *d){
    double num, den, l;

    num = volume(a,b,c,d);
    num = pow(fabs(num), 2.0/3.0);

    den = 0.0;
    den += length(a, b);
    den += length(a, c);
    den += length(a, d);
    den += length(b, c);
    den += length(b, d);
    den += length(c, d);
    den = den*den;

    return num/den;
}

double metric_rho(double *a, double *b, double *c, double *d){
#define MAX(a,b) ( (a)>(b) )? (a):(b)
#define MIN(a,b) ( (a)<(b) )? (a):(b)

    double min, max;
    double l;

    l = length(a,b); min = l; max = l;

    l = length(a,c); min = MIN(min, l); max = MAX(max, l);
    l = length(a,d); min = MIN(min, l); max = MAX(max, l);
    l = length(b,c); min = MIN(min, l); max = MAX(max, l);
    l = length(b,d); min = MIN(min, l); max = MAX(max, l);
    l = length(c,d); min = MIN(min, l); max = MAX(max, l);

    return min/max;
#undef MAX
#undef MIN
}
    
double metric_gamma(double *a, double *b, double *c, double *d){

}

double metric_dummyone(double *a, double *b, double *c, double *d){
    return 1.0;
}

