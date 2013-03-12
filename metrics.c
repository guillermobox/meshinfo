#include <math.h>

double metric_determinant(double *a, double *b, double *c, double *d){
    double t1,t2;
    int i;

    for(i=0; i<3; i++){
        a[i] -= d[i];
        b[i] -= d[i];
        c[i] -= d[i];
    }

    t1 = a[0]*b[1]*c[2] +\
         a[2]*b[0]*c[1] +\
         a[1]*b[2]*c[0];

    t2 = a[2]*b[1]*c[0] +\
         a[1]*b[0]*c[2] +\
         a[0]*b[2]*c[1];

    return t2-t1;
}
