#include <math.h>

static void cross(double *v1, double *v2, double *res){
    res[0] = v1[1]*v2[2] - v1[2]*v2[1];
    res[1] = v1[2]*v2[0] - v1[0]*v2[2];
    res[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

static void dot(double *v1, double *v2, double *res){
    *res = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

static void norm2(double *u, double *res){
    *res = u[0]*u[0] + u[1]*u[1] + u[2]*u[2];
}

static void length(double * u, double *v, double * res){
    double d0,d1,d2;
    d0 = v[0] - u[0];
    d1 = v[1] - u[1];
    d2 = v[2] - u[2];
    *res = sqrt( d0*d0 + d1*d1 + d2*d2 );
}

static void positive_area(double *a, double *b, double *c, double *res){
    double temp = 0.0;
    double t;

    t = (a[1]-c[1])*(b[2]-c[2]) - (a[2]-c[2])*(b[1]*c[1]);
    temp += t*t;
    t = (a[2]-c[2])*(b[0]-c[0]) - (a[0]-c[0])*(b[2]*c[2]);
    temp += t*t;
    t = (a[0]-c[0])*(b[1]-c[1]) - (a[1]-c[1])*(b[0]*c[0]);
    temp += t*t;

    *res = sqrt(temp)/2.0;
}

static void determinant(double *a, double *b, double *c, double *d, double *res){
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

    *res = t2-t1;
}

static void volume(double *a, double *b, double *c, double *d, double *res){
    double r;
    determinant(a,b,c,d,&r);
    *res = r/6.0;
}

double metric_determinant(double *a, double *b, double *c, double *d){
    double r;
    determinant(a,b,c,d,&r);
    return r;
}

double metric_shewchuk(double *a, double *b, double *c, double *d){
    double num, den, areas[4];
    double t1;

    positive_area(a,b,c, &areas[0]);
    positive_area(a,b,d, &areas[1]);
    positive_area(a,c,d, &areas[2]);
    positive_area(b,c,d, &areas[3]);

    num = fabs(metric_determinant(a,b,c,d)/6.0);
    dot(areas, areas, &t1);
    den = pow(t1, 3.0/4.0);
    return 6.83852117077*num/den;
}

double metric_neta(double *a, double *b, double *c, double *d){
    double num, den, l;

    volume(a,b,c,d,&num);
    num = pow(num, 2.0/3.0);

    den = 0.0;
    length(a, b, &l); den += l;
    length(a, c, &l); den += l;
    length(a, d, &l); den += l;
    length(b, c, &l); den += l;
    length(b, d, &l); den += l;
    length(c, d, &l); den += l;
    den = den*den;

    return num/den;
}
