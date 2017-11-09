#include <glut/glut.h>
#include <math.h>

#include <vector>
using namespace std;

void vector4_copy(GLfloat dest[], GLfloat vector[]);
void translate_vect(double dx, double dy, double dz, GLfloat inp[4], GLfloat out[4]);
void mult_matrix(GLfloat matrix[4][4], GLfloat vertex[4], GLfloat temp[4]);

void x_rot(double ang, GLfloat vect[4], GLfloat temp[4]);
void y_rot(double ang, GLfloat vect[4], GLfloat temp[4]);
void z_rot(double ang, GLfloat vect[4], GLfloat temp[4]);

void scale_vect(double sx, double sy, double sz, GLfloat inp[4], GLfloat out[4]);

void cross_prod(GLfloat a[], GLfloat b[], GLfloat ret[]);
void normalize_vect(GLfloat a[], GLfloat ret[]);
void add_vect(GLfloat a[], GLfloat b[], GLfloat ret[]);
void subtract_vect(GLfloat a[], GLfloat b[], GLfloat ret[]);
void divide_vect(GLfloat a[], GLfloat b[], GLfloat ret[]);

GLfloat dot_vect(GLfloat a[], GLfloat b[]);
void scalar_mult(GLfloat a[], GLfloat b, GLfloat ret[]);

//void translate_cpp_vect(double dx, double dy, double dz, vector<GLfloat> v);

