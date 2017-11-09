#include "vecops.h"


void vector4_copy(GLfloat dest[], GLfloat vector[]){
	int i;
	for (i = 0; i < 4; i++){
		dest[i] = vector[i];
	}
}


void translate_vect(double dx, double dy, double dz, GLfloat inp[4], GLfloat out[4]){
	GLfloat trans_matrix[][4] = {
		{1, 0, 0, dx},
		{0, 1, 0, dy},
		{0, 0, 1, dz},
		{0, 0, 0, 1}
	};
	mult_matrix(trans_matrix, inp, out);
}



void mult_matrix(GLfloat matrix[4][4], GLfloat vertex[4], GLfloat temp[4]){
	int row;
	int col;
	double sum;
	for (col = 0; col < 4; col++){
		sum = 0;
		for (row = 0; row < 4; row++){
			sum += matrix[col][row] * vertex[row];
		}
		temp[col] = sum;
	}
}


void x_rot(double ang, GLfloat vect[4], GLfloat temp[4]){
	GLfloat x_trans_matrix[][4] = {
	{1, 0, 0, 0},
	{0, cos(ang), -sin(ang), 0},
	{0, sin(ang), cos(ang), 0},
	{0, 0, 0, 1}};
	mult_matrix(x_trans_matrix, vect, temp);
}

void y_rot(double ang, GLfloat vect[4], GLfloat temp[4]){
	GLfloat y_trans_matrix[][4] = {
	{cos(ang), 0, sin(ang), 0},
	{0, 1, 0, 0},
	{-sin(ang), 0, cos(ang), 0},
	{0, 0, 0, 1}};
	mult_matrix(y_trans_matrix, vect, temp);
}

void z_rot(double ang, GLfloat vect[4], GLfloat temp[4]){
	GLfloat z_trans_matrix[][4] = {
	{cos(ang), -sin(ang), 0, 0},
	{sin(ang), cos(ang), 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 1}};
	mult_matrix(z_trans_matrix, vect, temp);
}

void scale_vect(double sx, double sy, double sz, GLfloat inp[4], GLfloat out[4]){
	GLfloat trans_matrix[][4] = {
		{sx, 0, 0, 0},
		{0, sy, 0, 0},
		{0, 0, sz, 0},
		{0, 0, 0, 1}
	};
	mult_matrix(trans_matrix, inp, out);
}

void cross_prod(GLfloat a[], GLfloat b[], GLfloat ret[]){
	ret[0] = (a[1]*b[2]) - (a[2]*b[1]);
	ret[1] = (a[2]*b[0]) - (a[0]*b[2]);
	ret[2] = (a[0]*b[1]) - (a[1]*b[0]);
	ret[3] = 1;
}

void normalize_vect(GLfloat a[], GLfloat ret[]){
	double denom = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
	
	ret[0] = a[0]/denom;
	ret[1] = a[1]/denom;
	ret[2] = a[2]/denom;
	ret[3] = 1;
}

void add_vect(GLfloat a[], GLfloat b[], GLfloat ret[]){
	ret[0] = a[0]+b[0];
	ret[1] = a[1]+b[1];
	ret[2] = a[2]+b[2];
	ret[3] = 1;
}

void subtract_vect(GLfloat a[], GLfloat b[], GLfloat ret[]){
	ret[0] = a[0]-b[0];
	ret[1] = a[1]-b[1];
	ret[2] = a[2]-b[2];
	ret[3] = 1;
}

void divide_vect(GLfloat a[], GLfloat b[], GLfloat ret[]){
	ret[0] = (b[0] == 0) ? a[0]: a[0]/b[0];
	ret[1] = (b[1] == 0) ? a[1]: a[1]/b[1];
	ret[2] = (b[2] == 0) ? a[2]: a[2]/b[2];
	ret[3] = 1;
}

GLfloat dot_vect(GLfloat a[], GLfloat b[]){
	GLfloat ret = 0;
	int i;
	for (i = 0; i < 4; i++){
		ret += a[i]*b[i];
	}
	return ret;
}

void scalar_mult(GLfloat a[], GLfloat b, GLfloat ret[]){
	int i;
	for (i = 0; i < 4; i++){
		ret[i] = a[i]*b;
	}
}

