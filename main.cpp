#include <iostream>
#include <string>
#include <vector>
#include "main.h"
#include "vecops.h"
#define WINDOW_W 460
#define WINDOW_H 380
#define SCR_TOP -43
#define SCR_BOT 13
#define fps 1000/30

/*
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <AudioToolbox/AudioToolbox.h>
*/

int num_lights = 0;
float scroll_v = .9; // The speed at which things scroll down the screen
int _id = 0;
LITE my_lights[10];
extern GLuint tex_names[10];

using namespace std;

int crt_render_mode = GL_LINE_LOOP;
float theta_x, theta_y;


// Load all settings for the lights
void lighting_setup () {
	int i;
	GLfloat globalAmb[] = {.3, .3, .3, .1};

	// create flashlight
	GLfloat amb[] = {0.25, 0.25, 0.25, 1.0};
	GLfloat dif[] = {0.6, 0.6, 0.6, 1.0};
	GLfloat spec[] = {.8, .8, .8, 1.0};

	//enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	// reflective properites -- global ambient light
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

	// We create one light here and it'll be set above the game area
	num_lights = 1;
	vector4_copy(my_lights[0].amb, amb);
	vector4_copy(my_lights[0].diff, dif);
	vector4_copy(my_lights[0].spec, spec);
	vector4_copy(my_lights[0].pos, (GLfloat[]){14, 25, 3, 1});
	my_lights[0].dir[0] = my_lights[0].dir[1] = my_lights[0].dir[2] = 0;

	// setup properties of lighting
	for (i=0; i<=num_lights; i++) {
		glEnable(GL_LIGHT0+i);
		glLightfv(GL_LIGHT0+i, GL_AMBIENT, my_lights[i].amb);
		glLightfv(GL_LIGHT0+i, GL_DIFFUSE, my_lights[i].diff);
		glLightfv(GL_LIGHT0+i, GL_SPECULAR, my_lights[i].spec);
		glLightfv(GL_LIGHT0+i, GL_POSITION, my_lights[i].pos);
		if ((my_lights[i].dir[0] > 0) ||  (my_lights[i].dir[1] > 0) ||  (my_lights[i].dir[2] > 0)) {
			glLightf(GL_LIGHT0+i, GL_SPOT_CUTOFF, my_lights[i].angle);
			glLightfv(GL_LIGHT0+i, GL_SPOT_DIRECTION, my_lights[i].dir);
		}
	}
}


// Translate a vectors of a mesh
void translate_cpp_vect(float dx, float dy, float dz, vector<GLfloat>& v){
	v[0] += dx;
	v[1] += dy;
	v[2] += dz;
}

/************ Generic base class ***************
	Nothing more than a mesh and a position
***********************************************/
class Mesh {
	public:
	unsigned int i, j;
	char temp[5];
	char tempBuff[500];
	char c;
	int t_num;
	int v_num;
	int n_num;
	int f_num;
	double scale;
	char color;
	char entered_screen;
	string str;

	int id;

	string name;

	float x, y, z, w, h, dx, dy, dz, v;
	float rot[4];
	float sc[4];

	char loaded;
	char del;
	int life;
	int texture_name;

	vector<vector<float> > verts;
	vector<vector<int> > faces;
	vector<vector<float> > normals;
	vector<vector<int> > norm_ref;
	vector<vector<float> > tex_coords;
	vector<vector<int> > tex_ref;

	char damages_player;

	int has_mesh(){
		return loaded;
	}

	void update_scale(int vs){
		scale = vs/10.0;
	}

	void load_mesh(string s){
		unsigned int k;
		c = 1;
		s = "Meshes/"+s;

		FILE *f;
		f = fopen(s.c_str(), "rb");

		if (f != NULL){
			// Keep reading until end of file
			while (c != EOF){
				i = 0;
				memset(tempBuff, '\0', sizeof(tempBuff));

				// Keep reading until newline or end of file
				while ((c = fgetc(f)) != '\n' && c != EOF){
					// Copy characters into the buffer
					tempBuff[i] = c;
					//putchar(c);
					i++;
				}
				//temp[0] = tempBuff[0];
				//temp[1] = tempBuff[1];
				strncpy(temp, tempBuff, 3);
				// Increments the number of verts
				if (strncmp(temp, "v ", 2) == 0){
					// Empty vector. Coords are pushed here
					vector<GLfloat> vr;

					str.assign(tempBuff);

					// We'll split the string by spaces and create vertices
					string v_dat(str.substr(2, str.size()-2));
					string sub("");

					// Process the string
					int start; // Beginning of vertex data

					// Splits axes by spaces and reads until the end of a line
					for (j = 0, start = 0; j <= v_dat.size(); j++){
						// Check for a space or end of line
						if (v_dat[j] == ' ' || v_dat[j] == '\n' || (j == (v_dat.size()))) {
							// Convert to a float and push to the vector
							GLfloat conv = atof(v_dat.substr(start, (j-start)).c_str());
							vr.push_back(conv);

							//cout << conv << " ";
							start = j + 1;
						}
					}
					verts.push_back(vr);

					v_num++;
				}
				// Increments the number of norms
				if (strncmp(temp, "vn ", 3) == 0){
					// Empty vector. Coords are pushed here
					vector<GLfloat> vr;
					str.assign(tempBuff);
					// We'll split the string by spaces and create vertices
					string v_dat(str.substr(3, str.size()-2));
					string sub("");
					// Process the string
					int start; // Beginning of vertex data
					// Splits vertices by spaces and reads until the end of a line
					for (j = 0, start = 0; j <= v_dat.size(); j++){
						// Check for a space or end of line
						if (v_dat[j] == ' ' || v_dat[j] == '\n' || (j == (v_dat.size()-1))) {
							// Convert to a float and push to the vector
							GLfloat conv = atof(v_dat.substr(start, (j-start)).c_str());
							vr.push_back(conv);

							//cout << conv << " ";
							start = j + 1;
						}
					}
					normals.push_back(vr);

					n_num++;
				}

				// Gathers list of texture coordinates
				if (strncmp(temp, "vt ", 3) == 0){
					// Empty vector. Coords are pushed here
					vector<GLfloat> vr;
					str.assign(tempBuff);
					// We'll split the string by spaces and create vertices
					string v_dat(str.substr(3, str.size()-2));
					string sub("");
					// Process the string
					int start; // Beginning of vertex data
					// Splits vertices by spaces and reads until the end of a line
					for (j = 0, start = 0; j < v_dat.size(); j++){
						// Check for a space or end of line
						if (v_dat[j] == ' ' || v_dat[j] == '\n' || (j == (v_dat.size()-1))) {
							// Convert to a float and push to the vector
							GLfloat conv = atof(v_dat.substr(start, (j-start)).c_str());
							vr.push_back(conv);

							//cout << conv << " ";
							start = j + 1;
						}
					}
					tex_coords.push_back(vr);

					t_num++;
				}

				// Increments the number of faces
				if (strncmp(temp, "f ", 2) == 0){
					vector<int> vr;
					vector<int> vt;
					vector<int> vn;
					str.assign(tempBuff);
					
					// We'll split the string by spaces
					string v_dat(str.substr(2, str.size()-2));

					// Splits vertices by spaces and reads until the end of a line
					int start, start2;
					for (j = 0, start = 0; j < v_dat.size(); j++){
						// Check for a space or end of line
						if (v_dat[j] == ' ' || v_dat[j] == '\n' || (j >= (v_dat.size()-1))) {
							// Go until you find a / or reach the end of the substr
							for (k = start; k < j && v_dat[k] != '/'; k++){
							}
							// Get the substr until the first '/' or end of face and convert to int
							// This the vertex of a face
							int conv = atoi(v_dat.substr(start, (k-start)).c_str());
							vr.push_back(conv);
							//printf("%d:%d", start, k);
							//printf("%s", v_dat.substr(start, k-start).c_str());
							// Get texture coord
							start2 = k+1;
							for (k = start2; k < j && v_dat[k] != '/'; k++){
							}
							conv = atoi(v_dat.substr(start2, (k-start2)).c_str());
							vt.push_back(conv);
							// This is the normal we'll use for this vertex
							start2 = k+1;
							for (k = start2; k <= j && v_dat[k] != '/' && v_dat[k] != ' ' && v_dat[k] != '\n'; k++){
							}
							conv = atoi(v_dat.substr(start2, (k-start2)).c_str());
							vn.push_back(conv);

							start = j + 1;
						}
					}
					// Push the set of vertices to the vector of faces
					faces.push_back(vr);
					tex_ref.push_back(vt);
					norm_ref.push_back(vn);

					// increment number of faces
					f_num++;
				}
			}
		}
		fclose(f);
		// If there are faces, register it as loaded and displayable
		if (f_num){
			loaded = 1;
		}
	}

	// If a mesh was loaded, draw it
	virtual void draw_mesh(){
		if (loaded){
			if (texture_name != 0){
				glBindTexture(GL_TEXTURE_2D, tex_names[texture_name]);
				glEnable(GL_TEXTURE_2D);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			// Go through the faces
			glColor4fv(colors[(int)color]);
			for (i = 0; i < faces.size(); i++){
				glBegin(crt_render_mode);
				// Go through the vertices of the face
				// Note: each one points to a vertex name
				for (j = 0; j < faces[i].size(); j++){
					GLfloat v1[4];
					GLfloat n1[4];
					v1[0] = verts[faces[i][j]-1][0];
					v1[1] = verts[faces[i][j]-1][1];
					v1[2] = verts[faces[i][j]-1][2];
					v1[3] = 1;
					///*
					n1[0] = normals[norm_ref[i][j]-1][0];
					n1[1] = normals[norm_ref[i][j]-1][1];
					n1[2] = normals[norm_ref[i][j]-1][2];
					//*/
					// If there's a texture, draw it
					if (texture_name){
						glTexCoord2f(tex_coords[tex_ref[i][j]-1][0], tex_coords[tex_ref[i][j]-1][1]);
					}
					// Always supply normals before vertices
					glNormal3fv(n1);
					glVertex4fv(v1);
				}
				glEnd();
			}
			// Draws a line showing the object's collision boundaries
			/*
			glBegin(GL_LINE_LOOP);{
				glVertex4fv((GLfloat[]){x-w, y, z-h, 1});
				glVertex4fv((GLfloat[]){x+w, y, z+h, 1});
			}
			glEnd();
			*/
		}
		if (texture_name != 0){
			glDisable(GL_TEXTURE_2D);
		}
	}

	Mesh(){
		v_num = 0;
		t_num = 0;
		f_num = 0;
		scale = 1;
		entered_screen = 0;

		damages_player = 0;

		loaded = 0;
		color = RED;

		dx = 0;
		dy = 0;
		dz = 0;
		w = 1;
		h = 1;

		texture_name = 0;

		del = 0;
		life = 1;
		vector4_copy(rot, (GLfloat[]){0, 0, 0, 1});
		vector4_copy(sc, (GLfloat[]){1, 1, 1, 1});
	}

	void trans_mesh(float dx, float dy, float dz){
		unsigned int i;
		for (i = 0; i < verts.size(); i++){
			translate_cpp_vect(dx, dy, dz, verts[i]);
		}
	}
	void scale_mesh(float sx, float sy, float sz){
		unsigned int i;
		sc[0] *= sx;
		sc[1] *= sy;
		sc[2] *= sz;
		for (i = 0; i < verts.size(); i++){
			verts[i][0] *= sx;
			verts[i][1] *= sy;
			verts[i][2] *= sz;
		}
		for (i = 0; i < normals.size(); i++){
			normals[i][0] *= 1./sx;
			normals[i][1] *= 1./sy;
			normals[i][2] *= 1./sz;
		}
		// We adjust the width and height of the rect to fit the model
		w *= sx;
		h *= sz;
	}
	void rotate_mesh(float ang, char rx, char ry, char rz){
		unsigned int i;
		GLfloat v[4];
		GLfloat d[4];
		v[3] = 1;
		if (rx){
			for (i = 0; i < verts.size(); i++){
				v[0] = verts[i][0];
				v[1] = verts[i][1];
				v[2] = verts[i][2];
				x_rot(ang, v, d);
				verts[i][0] = d[0];
				verts[i][1] = d[1];
				verts[i][2] = d[2];
			}
			for (i = 0; i < normals.size(); i++){
				v[0] = normals[i][0];
				v[1] = normals[i][1];
				v[2] = normals[i][2];
				x_rot(ang, v, d);
				normals[i][0] = d[0];
				normals[i][1] = d[1];
				normals[i][2] = d[2];
			}
			rot[0] += ang;
		}
		else if (ry){
			for (i = 0; i < verts.size(); i++){
				v[0] = verts[i][0];
				v[1] = verts[i][1];
				v[2] = verts[i][2];
				y_rot(ang, v, d);
				verts[i][0] = d[0];
				verts[i][1] = d[1];
				verts[i][2] = d[2];
			}
			for (i = 0; i < normals.size(); i++){
				v[0] = normals[i][0];
				v[1] = normals[i][1];
				v[2] = normals[i][2];
				y_rot(ang, v, d);
				normals[i][0] = d[0];
				normals[i][1] = d[1];
				normals[i][2] = d[2];
			}
			rot[1] += ang;
		}
		else if (rz){
			for (i = 0; i < verts.size(); i++){
				v[0] = verts[i][0];
				v[1] = verts[i][1];
				v[2] = verts[i][2];
				z_rot(ang, v, d);
				verts[i][0] = d[0];
				verts[i][1] = d[1];
				verts[i][2] = d[2];
			}
			for (i = 0; i < normals.size(); i++){
				v[0] = normals[i][0];
				v[1] = normals[i][1];
				v[2] = normals[i][2];
				z_rot(ang, v, d);
				normals[i][0] = d[0];
				normals[i][1] = d[1];
				normals[i][2] = d[2];
			}
			rot[2] += ang;
		}
	}

	float distToObj(Mesh *m){
		return sqrt(pow(this->x - m->x, 2) + pow(this->z - m->z, 2));
	}

	float angToObj(Mesh *m){
		return atan2(-this->z + m->z, -this->x + m->x);
	}

	virtual void action(){
	}

	virtual void move(){
		x += dx;
		y += dy;
		z += dz;

		trans_mesh(dx, dy, dz);
	}
};

Mesh *ply;

// Generic bullet
// Takes a position, angle, and velocity
// DX/DZ is calculated by the angle
class Bullet:public Mesh{
	float ang, v;
	int timer;
	char blink;
	char cur_col;
	public:
	Bullet(float _x, float _y, float _z, float _ang, float _v, char _dp){
		x = _x;
		y = _y;
		z = _z;
		ang = _ang;
		v = _v;
		w = .5;
		h = 1;
		damages_player = _dp;
		blink = 0;


		color = MAGENTA;
		timer = 40;

		dx = v*cos(ang);
		dz = v*sin(ang);

		name = "Bullet";
		load_mesh(name+".obj");
		scale_mesh(.65, .75, .75);
		//rotate_mesh(0, 1, 0, ang);
		trans_mesh(x, y, z);
	}
	Bullet(float _x, float _y, float _z, float _ang, float _v, char _dp, char _color){
		x = _x;
		y = _y;
		z = _z;
		ang = _ang;
		v = _v;
		w = .5;
		h = 1;
		damages_player = _dp;
		blink = 1;

		color = _color;
		cur_col = _color;
		timer = 50;

		dx = v*cos(ang);
		dz = v*sin(ang);

		name = "Bullet";
		load_mesh(name+".obj");
		scale_mesh(.95, .75, .75);
		//rotate_mesh(0, 1, 0, ang);
		trans_mesh(x, y, z);
	}
	void action(){
		// Have a countdown timer to remove bullets when they're assumed to be off screen
		timer--;
		if (timer < 1){
			del = 1;
		}
		if (blink){
			if (color == cur_col){
				color = CYAN;
			}
			else {
				color = cur_col;
			}
		}
	}
};
vector < Mesh *> bullets;

// A particle that's displayed behind an object's engine
class Flame:public Mesh{
public:
	char counter;
	Mesh *fl2;
	Flame(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
		counter = 0;

		name = "Flame";
		texture_name = 3;
		color = CYAN;

		load_mesh(name+".obj");

		fl2 = new Mesh();
		fl2->load_mesh(name+".obj");
		fl2->texture_name = 3;
		fl2->color = CYAN;

		scale_mesh(1.5, 2, 2);
		rotate_mesh(-M_PI/4, 0, 0, 1);
		trans_mesh(x, y, z);
		
		fl2->scale_mesh(1.5, 2, 2);
		fl2->rotate_mesh(M_PI/4, 0, 0, 1);
		fl2->trans_mesh(x, y, z);
	}
	Flame(float _x, float _y, float _z, float sx, float sy, float sz){
		x = _x;
		y = _y;
		z = _z;
		counter = 0;

		name = "Flame";
		texture_name = 3;
		color = CYAN;

		load_mesh(name+".obj");

		fl2 = new Mesh();
		fl2->load_mesh(name+".obj");
		fl2->texture_name = 3;
		fl2->color = CYAN;

		scale_mesh(1.5*sx, 2*sy, 2*sz);
		rotate_mesh(-M_PI/4, 0, 0, 1);
		trans_mesh(x, y, z);
		
		fl2->scale_mesh(1.5*sx, 2*sy, 2*sz);
		fl2->rotate_mesh(M_PI/4, 0, 0, 1);
		fl2->trans_mesh(x, y, z);
	}
	void action(){
		counter++;
	}
	void move(){
		x += dx;
		y += dy;
		z += dz;

		fl2->x += dx;
		fl2->y += dy;
		fl2->z += dz;

		trans_mesh(dx, dy, dz);
		fl2->trans_mesh(dx, dy, dz);
	}
	// Override the draw_mesh function so that we draw the shape twice at two rotations
	void draw_mesh(){
		Mesh::draw_mesh();
		fl2->draw_mesh();
	}
};


/**************** A basic enemy *****************/
class Enemy:public Mesh{
	public:
	char kind, turn_back, activated_flip;
	float orig_dz, turn_around_timer, orig_x, flip_timer, max_flip_timer, flip_ang;
	Enemy(float _x, float _y, float _z, char _kind){
		x = _x;
		y = _y;
		z = _z;
		w = 1;
		h = .5;
		orig_x = x;
		dz = orig_dz = scroll_v;
		life = 1;
		id = _id++;

		// For pattern 2's curve
		turn_around_timer = 15;

		// For pattern 1's flip
		flip_timer = max_flip_timer = 50;
		activated_flip = 0;

		turn_back = 0;

		// The enemy AI pattern
		kind = _kind;

		color = GREY;

		name = "Enemy";
		load_mesh(name+".obj");
		scale_mesh(2, 2, 2);
		trans_mesh(x, y, z);
	}

	void action(){
		if (kind == 1){
			pattern_1();
		}
		else{
			pattern_2();
		}
	}
	void pattern_1(){
		if (distToObj(ply) < 15){
			activated_flip = 1;
		}
		if (activated_flip){
			// Fly off screen, then swoop back towards the player
			if (flip_timer > max_flip_timer/2){
				if (this->x < ply->x){
					if (this->dx > -1){
						this->dx -= .05;
						this->dz -= .1;
					}
				}
				else{
					if (this->dx < 1){
						this->dx += .05;
						this->dz -= .1;
					}
				}
			}
			else if (flip_timer == max_flip_timer/2){
				flip_ang = angToObj(ply);
				dx = 0;
				dz = 0;
			}
			else if (flip_timer < max_flip_timer/2){
				if (fabs(dx) < 2)
					dx += .05 * cos(flip_ang);
				if (fabs(dz) < 2)
					dz += .05 * sin(flip_ang);
			}

			// Rotate
			if (this->dx < 0){
				trans_mesh(-x, -y, -z);
				rotate_mesh(-.35, 0, 0, 1);
				trans_mesh(x, y, z);
			}
			else if (this->dx > 0){
				trans_mesh(-x, -y, -z);
				rotate_mesh(.35, 0, 0, 1);
				trans_mesh(x, y, z);
			}

			flip_timer--;
		}
	}
	// Pattern where enemies come straight down the screen, then turn around when
	// they're about halfway down
	void pattern_2(){
		if (z > -h*5 && !turn_back){
			turn_back = 1;
			Bullet *b;
			b = new Bullet(x, y, z, angToObj(ply), .5, 1, YELLOW);
			bullets.push_back(b);
		}
		if (turn_back == 1){
			trans_mesh(-x, -y, -z);
			dz -= orig_dz/(turn_around_timer/2);
			if (id & 1){
				dx -= .1;
				rotate_mesh(-M_PI/turn_around_timer, 0, 1, 0);
			}
			else {
				dx += .1;
				rotate_mesh(M_PI/turn_around_timer, 0, 1, 0);
			}
			if (dz < -orig_dz){
				dz = -orig_dz;
				dx = 0;
				turn_back = 0;
			}
			trans_mesh(x, y, z);
		}
	}
};

/************** Enemy that spins rapidly to do damage *************/
class Spinner:public Mesh{
public:
	char kind;
	unsigned char attack_timer, stopped;
	Spinner(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
		w = 1;
		h = 1;
		life = 2;
		stopped = 0;
		attack_timer = 40;

		dz = scroll_v;

		color = GREEN;
		texture_name = 4;
		kind = 1;

		name = "Spinner";
		load_mesh(name+".obj");
		trans_mesh(x, y, z);
	}
	void action(){
		// Reduce its speed once it's on-screen
		if (z > SCR_TOP && !entered_screen){
			entered_screen = 1;
			dz = 1.2;
		}
		if (kind == 1){
			pattern_1();
		}
	}
	void pattern_1(){
		trans_mesh(-x, -y, -z);
		rotate_mesh(.15, 0, 1, 0);
		trans_mesh(x, y, z);

		if (!this->stopped && z > SCR_TOP + 15){
			stopped = 1;
			dz = 0;
		}
		if (this->stopped && attack_timer > 0){
			if (attack_timer % 11 == 0){
				Bullet *b;
				b = new Bullet(x, y, z, angToObj(ply), .8, 1, YELLOW);
				bullets.push_back(b);
			}
			attack_timer--;
			if (attack_timer < 1){
				dx = 1.25*cos(angToObj(ply));
				dz = 1.25*sin(angToObj(ply));
			}
		}
	}
};

/************ Spins around player as central axis *************/
class Revolver:public Mesh{
public:
	float radius, angle, dr;
	char in_orbit;
	Revolver(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
		life = 5;

		dz = scroll_v;
		color = GREEN;
		name = "Revolver";
		load_mesh(name+".obj");
		trans_mesh(x, y, z);

		// Stays within this range of the player
		radius = 7;
		dr = .15;
		angle = -M_PI/2;
		in_orbit = 0;
	}
	void action(){
		if (z > SCR_TOP && !entered_screen){
			entered_screen = 1;
			dz = 1.2;
		}
		// Lock to the player once you get close
		if (!in_orbit && distToObj(ply) < radius){
			angle = angToObj(ply);
			in_orbit = 1;
			dz = 0;
		}
		else if (in_orbit){
			angle += dr;
			trans_mesh(-x, -y, -z);
			rotate_mesh(-dr, 0, 1, 0);
			x = ply->x - radius*cos(angle);
			z = ply->z - radius*sin(angle);
			trans_mesh(x, y, z);
		}
	}
};

vector < Mesh *> enemies;

/************* Non-interactive meshes of the boss **********/
class BossClone:public Mesh{
public:
	BossClone(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
		w = 3.5;
		h = 2;
		dz = .2;

		color = RED;
		name = "Boss";
		load_mesh(name+".obj");
		scale_mesh(1.6, 1.6, 1.6);
		rotate_mesh(M_PI, 0, 1, 0);
		trans_mesh(x, y, z);
	}
};

/************** Large boss **************
 Uses the BigShip as a port for launch
 ****************************************/
class Boss:public Mesh{
	public:
	char taking_off, returning, launch_complete;
	short takeoff_timer;
	char shot_timer, max_shot_timer;
	char draw_flame, flame_wave;
	unsigned char mode, mode_timer, mode_max_timer;
	char dr;
	float d_shot_ang, shot_ang, old_dz;
	Flame *fl;
	Boss(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
		w = 3.5;
		h = 2;

		taking_off = 0;
		returning = 0;
		launch_complete = 0;
		life = 165;
		draw_flame = flame_wave = 0;

		shot_timer = 0;
		max_shot_timer = 5;
		takeoff_timer = 0;

		// Fires a shot from the front of the ship if 1
		mode = 1;
		mode_timer = 0;
		mode_max_timer = 200;
		shot_ang = 0;
		d_shot_ang = 1;
		//texture_name = 6;

		dr = 1;

		dz = .15;
		old_dz = 0;

		color = RED;
		name = "Boss";
		load_mesh(name+".obj");
		scale_mesh(1.6, 1.6, 1.6);
		rotate_mesh(M_PI, 0, 1, 0);
		trans_mesh(x, y, z);

		for (int i = 1; i < 5; i++){
			BossClone *bc = new BossClone(x, y, z + 20 * i);
			enemies.push_back(bc);
		}
		fl = new Flame(x, y+1.3, z + h*3.6, 2.5, 3, 3);
		//fl->scale_mesh(2, 2, 2);
	}
	void move(){
		x += dx;
		y += dy;
		z += dz;
		trans_mesh(dx, dy, dz);

		fl->dx = dx;
		fl->dy = dy;
		fl->dz = dz;
		fl->move();
	}
	void action(){
		if (!launch_complete){
			launch();
		}
		else {
			attack();
		}
	}

	// Movement that precedes battle formation
	void launch(){
		// Have it move along under the player for a few seconds (flame on), then take off
		
		// Launch off the ship when the boss passes off the bottom
		if (z >= SCR_BOT + h*2){
			dz = -.009;
			taking_off = 1;
			draw_flame = 1;
		}
		// Accelerate beyond the top of the screen, then turn around
		if (taking_off){
			if (takeoff_timer < 300){
				if (z <= 7){
					// Store the old velocity
					if (old_dz == 0){
						old_dz = dz;
					}
					dz *= .98;
				}
				else {
					dz = -.05;
				}
				takeoff_timer++;
			}
			else {
				dz *= 1.05;
			}
			
			if (z < SCR_TOP - h*4){
				taking_off = 0;
				returning = 1;
				dz *= .85;

				draw_flame = 0;

				// Turn around to face the player and make it a bit larger
				trans_mesh(-x, -y, -z);
				//scale_mesh(1.25, 1.25, 1.25);
				rotate_mesh(-M_PI, 0, 1, 0);
				// We don't return its Y pos because we want it on the same y-plane
				trans_mesh(x, 0, z);
				y = 0;
			}
		}

		// Return to the play area and slow down
		if (returning){
			if (z < SCR_TOP){
				dz += .10;
				if (dz > 1.2)
					dz = 1;
			}
			if (z > SCR_TOP){
				dz *= .969;
				if (fabs(dz) < .1){
					dz = 0;
					returning = 0;
					launch_complete = 1;
				}
			}
		}
	}

	void attack(){
		mode_timer++;
		// Turn CW & CCW and fire a shot from the center of the ship
		if (mode == 1){
			if (rot[1] > M_PI/2){
				dr = -1;
			}
			if (rot[1] < -M_PI/2){
				dr = 1;
			}

			trans_mesh(-x, -y, -z);
			rotate_mesh((M_PI*2)/mode_max_timer*dr, 0, 1, 0);
			trans_mesh(x, y, z);

			shot_timer++;
			if (shot_timer >= max_shot_timer){
				shot_timer = 0;
				Bullet *b;
				b = new Bullet(x, y, z, -rot[1] + M_PI/2, 1.25, 1, YELLOW);
				bullets.push_back(b);
			}
			// Change to beam crossing mode
			if (mode_timer >= mode_max_timer){
				mode_timer = 0;
				mode = 0;
				max_shot_timer = 7;
			}
		}
		// Wave shots that converge and diverge
		else{
			shot_timer++;
			if (shot_timer >= max_shot_timer){
				shot_ang += .12*d_shot_ang;
				if (shot_ang >= M_PI/4){
					d_shot_ang = -1;
				}
				else if (shot_ang <= -M_PI/4){
					d_shot_ang = 1;
				}
				shot_timer = 0;
				Bullet *b;
				b = new Bullet(x+w, y, z, -rot[1] + M_PI/2 + shot_ang, 1, 1, YELLOW);
				bullets.push_back(b);
				b = new Bullet(x-w, y, z, -rot[1] + M_PI/2 - shot_ang, 1, 1, YELLOW);
				bullets.push_back(b);
			}
			mode_timer++;
			if (mode_timer >= mode_max_timer){
				mode_timer = 0;
				mode = 1;
				max_shot_timer = 5;
			}
		}
	}

	void draw_mesh(){
		Mesh::draw_mesh();
		if (draw_flame){
			flame_wave++;
			if (flame_wave & 1){
				fl->draw_mesh();
			}
		}
	}
};

/************** Huge enemy battleship ***********************/
// TODO: put big flames at the back
class BigShip:public Mesh{
	public:
	char spawned_boss, flame_wave;
	Flame *fl1, *fl2, *fl3, *fl4;
	BigShip(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
		dz = scroll_v;
		name = "BigShip";

		w = 2;
		h = 5.5;

		life = 100;

		spawned_boss = 0;
		color = DARK_RED;
		
		load_mesh(name+".obj");
		scale_mesh(12, 5, 29);
		trans_mesh(x, y, z);

		fl1 = new Flame(x + -22, y + .3*sc[1], z + 3.8*sc[2], 6, 8, 8);
		fl2 = new Flame(x + 22, y + .3*sc[1], z + 3.8*sc[2], 6, 8, 8);
		fl3 = new Flame(x + -1.1*sc[0], y + .4*sc[1], z + 5.5*sc[2], 9, 8, 12);
		fl4 = new Flame(x + 1.1*sc[0] , y + .4*sc[1], z + 5.5*sc[2], 9, 8, 12);
		
	}
	void action(){
		if (z + h > SCR_TOP){
			dz = .2;
			// Push a boss near the end of the ship
			if (!spawned_boss){
				spawned_boss = 1;
				Boss *b = new Boss(x, y + 1, z-h/5);
				b->dz = this->dz;
				enemies.push_back(b);
			}
		}
	}
	void move(){
		x += dx;
		y += dy;
		z += dz;
		trans_mesh(dx, dy, dz);

		fl1->dx = dx;
		fl1->dy = dy;
		fl1->dz = dz;
		fl1->move();

		fl2->dx = dx;
		fl2->dy = dy;
		fl2->dz = dz;
		fl2->move();

		fl3->dx = dx;
		fl3->dy = dy;
		fl3->dz = dz;
		fl3->move();

		fl4->dx = dx;
		fl4->dy = dy;
		fl4->dz = dz;
		fl4->move();
	}
	void draw_mesh(){
		Mesh::draw_mesh();
		flame_wave++;
		if (flame_wave & 1){
			fl1->draw_mesh();
			fl2->draw_mesh();
			fl3->draw_mesh();
			fl4->draw_mesh();
		}
	}
};


/************** Stars that appear in the background **************/
class Star:public Mesh{
	public:
	Star(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
		dz = 2.25;

		color = LIGHT_GREY;

		name = "Star";
		load_mesh(name+".obj");
		float size = (rand()%5)/5. + .1;
		scale_mesh(size*.65, size, size);
		trans_mesh(x, y, z);
	}
	void action(){
		float mv;
		if (z > SCR_BOT){
			mv = SCR_TOP-2-z;
			trans_mesh(0, 0, mv);
			z = SCR_TOP-2;
		}
	}
};

/************* Orb; likely a planet ********************/
class Globe:public Mesh{
public:
	Mesh *ring;
	float sx, sy, sz;
	Globe(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
		dz = .04;

		color = GREY;

		ring = new Mesh();
		ring->load_mesh("Ring.obj");
		ring->color = GREY;

		name = "Globe";
		texture_name = 4;
		load_mesh(name+".obj");
		sx = 30;
		sy = -32;
		sz = 30;
		scale_mesh(sx, sy, sz);
		ring->scale_mesh(sx * 1.5, -sy * 1.5, sz * 1.5);
		ring->trans_mesh(x, y, z);
		ring->texture_name = 5;
		trans_mesh(x, y, z);
	}
	void action(){
		trans_mesh(-x, -y, -z);
		rotate_mesh(.001, 0, 1, 0);
		trans_mesh(x, y, z);

		ring->trans_mesh(0, 0, dz);
	}
	void draw_mesh(){
		Mesh::draw_mesh();
		ring->draw_mesh();
	}
};

/************* The remnants of an explosion ******************/
class Particle:public Mesh{
	public:
	float drx, dry, drz;
	short timer;
	Particle(float _x, float _y, float _z, float _dx, float _dz){
		x = _x;
		y = _y;
		z = _z;
		dx = _dx;
		dz = _dz;

		drx = (float)(rand() % 10) / 16.0;
		dry = (float)(rand() % 3) / 4.0 - .4;
		drz = (float)(rand() % 3) / 4.0 - .4;

		color = ORANGE;
		name = "Particle";
		timer = 50;

		load_mesh(name+".obj");

		float s = rand() % 5 + 1;
		s /= 6;
		scale_mesh(s, s, s);
		trans_mesh(x, y, z);
		
	}
	// Rotate away from the origin of the explosion
	void action(){
		trans_mesh(-x, -y, -z);
		
		rotate_mesh(drx, 1, 0, 0);
		rotate_mesh(dry, 0, 1, 0);
		rotate_mesh(drz, 0, 0, 1);
		
		trans_mesh(x, y, z);

		timer--;
		if (timer < 1){
			del = 1;
		}
	}
};


/*********** Allocate various game state data *************/
vector < Mesh *> stars;
vector < Mesh *> debris;


/**************** The player *******************/
class Player:public Mesh{
	public:
	char fire_side;
	char fire_timer;
	char fire_max_timer;
	char damage_timer;
	char firing;
	float drz, rv;
	Flame *flame1;
	Flame *flame2;
	Player(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _y;
		w = .4;
		h = .5;

		life = 6;
		damage_timer = 0;

		v = .45;
		rv = .025;
		drz = 0;
		fire_side = 1;
		fire_max_timer = 2;
		fire_timer = fire_max_timer;
		firing = 0;
		color = GREY;

		// Create a flame that's displayed behind the player
		flame1 = new Flame(0, 0, 0);
		flame1->trans_mesh(x, y+.25, z+3);
		flame1->fl2->trans_mesh(x, y+.25, z+3);

		name = "Player";
		load_mesh(name+".obj");
		scale_mesh(.65, .65, .65);
		trans_mesh(x, y, z);
	}
	// Countdown to firing is performed continuously, but bullets are only fired on command
	void action(){
		if (fire_timer > 0){
			fire_timer--;
		}
		else{
			if (firing){
				fire();
			}
		}
		flame1->dx = dx;
		flame1->dy = dy;
		flame1->dz = dz;
		flame1->move();
		flame1->action();

		if (damage_timer){
			took_damage();
		}
	}
	void fire(){
		// add a bullet
		// if (fire_side > 0) add to right
		// else add to left
		fire_side = ~fire_side;
		Bullet *b;
		if (fire_side > 0)
			b = new Bullet(this->x+1, this->y-.1, this->z, -M_PI/2, 1.1, 0);
		else
			b = new Bullet(this->x-1, this->y-.1, this->z, -M_PI/2, 1.1, 0);
		bullets.push_back(b);
		fire_timer = fire_max_timer;
	}
	void move(){
		// Limit the field of movement for the player
		// We can't have them going off-screen
		if (dx < 0 && x < -10)
			dx = 0;
		else if (dx > 0 && x > 10)
			dx = 0;
		if (dz < 0 && z < -12)
			dz = 0;
		else if (dz > 0 && z > 12)
			dz = 0;

		// Rotate ship
		if (dx != 0){
			if (dx > 0)
				drz = -rv;
			else
				drz = rv;
			trans_mesh(-x, -y, -z);
			rotate_mesh(drz, 0, 0, 1);
			trans_mesh(x, y, z);
		}
		// un-rotate ship if not in motion
		if (fabs(dx) < v && fabs(rot[2]) > rv){
			trans_mesh(-x, -y, -z);
			if (rot[2] > 0){
				rotate_mesh(-rv, 0, 0, 1);
			}
			else{
				rotate_mesh(rv, 0, 0, 1);
			}
			trans_mesh(x, y, z);
		}

		x += dx;
		y += dy;
		z += dz;
		trans_mesh(dx, dy, dz);
	}

	// Blink after taking damage
	void took_damage(){
		damage_timer--;
		if (damage_timer & 1){
			color = RED;
		}
		else{
			color = GREY;
		}
	}
};

Player *pl;

void parse_level(string s){
	float x, y, z;
	char c = 0;
	x = y = z = 0;
	FILE *f;
	f = fopen(s.c_str(), "r");

	if (f != NULL){
		// Keep reading until end of file
		while (c != EOF){
			x = -12;
			// Keep reading until newline or end of file
			while ((c = fgetc(f)) != '\n' && c != EOF){
				if (c == 'E'){
					Enemy *e = new Enemy(x, y, z, 1);
					enemies.push_back(e);
				}
				else if (c == 'e'){
					Enemy *e = new Enemy(x, y, z, 0);
					enemies.push_back(e);
				}
				else if (c == 'S'){
					Spinner *e = new Spinner(x, y, z);
					enemies.push_back(e);
				}
				else if (c == 'B'){
					BigShip *n = new BigShip(0, -8, z);
					enemies.push_back(n);
				}
				else if (c == 'R'){
					Revolver *n = new Revolver(x, y, z);
					enemies.push_back(n);
				}
				x += 2;
			}
			z -= 4;
		}
	}
}

void draw_text(const char *s, float x, float y, float z, float alpha){
	int i;
	glDisable(GL_LIGHTING);
	glColor4f(.0, .7, .8, alpha);
	glRasterPos3f(x, y+1.1, z+1);
	for (i = 0; s[i] != '\0'; i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
	}
	glColor4f(.0, .4, .3, alpha);
	glRasterPos3f(x+.05, y+1.15, z+.9);
	for (i = 0; s[i] != '\0'; i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
	}
	glEnable(GL_LIGHTING);
}

void HUD(){
	int i;
	static float counter = 0;
	counter += .15;
	if (counter > .2){
		counter = 0.0;
	}
	
	glColor4f(0, .3, .5, .65-counter/2);
	glBegin(crt_render_mode);{
		glVertex4f(-10, 3.8, 13.8, 1);
		glVertex4f(-10, 0, 13.8, 1);
		glVertex4f(-3, 0, 13.8, 1);
		glVertex4f(-4, 3.8, 13.8, 1);
	}
	glEnd();
	glColor4f(0, .3, .7, .65-counter/2);
	glBegin(crt_render_mode);{
		glVertex4f(-10, 3.6, 14, 1);
		glVertex4f(-10, 0, 14, 1);
		glVertex4f(-3, 0, 14, 1);
		glVertex4f(-4, 3.6, 14, 1);
	}
	glEnd();

	// Draw text that has a slight flicker
	draw_text("Shield: ", -8.5, 2.3, 14, .6+counter);
	for (i = 0; i < pl->life; i++){
		draw_text("|", -5.5+.2*i, 2.4, 14, .6+counter);
	}
}

/**********************************************************/

// Display all entities
void draw_objects(){
	//glEnable(GL_LIGHTING);
	unsigned int i;
	//glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat[]){1.0, 1.0, 1.0, 1.0});
	/*
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cur->diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, cur->spec);
	glMaterialfv(GL_FRONT, GL_SHININESS, &cur->shine);
	*/	

	// Allows object transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (i = 0; i < stars.size(); i++){
		stars[i]->draw_mesh();
	}
	for (i = 0; i < enemies.size(); i++){
		enemies[i]->draw_mesh();
	}
	for (i = 0; i < bullets.size(); i++){
		bullets[i]->draw_mesh();
	}
	for (i = 0; i < debris.size(); i++){
		debris[i]->draw_mesh();
	}

	pl->draw_mesh();
	if (pl->flame1->counter & 1){
		pl->flame1->draw_mesh();
		//pl->flame2->draw_mesh();
	}

	HUD();
}

// Handle collision between objects
// Note: It only works with objects within 0.5 units of the first object's Y value
// This is so we're not colliding with stars or anything
int col_rect(Mesh *obj1, Mesh *obj2){
	if (fabs(obj1->y - obj2->y) < .5){
		if (obj1->x+obj1->w >= obj2->x-obj2->w && obj1->x-obj1->w <= obj2->x+obj2->w){
			if (obj1->z+obj1->h >= obj2->z-obj2->h && obj1->z-obj1->h <= obj2->z+obj2->h){
				return 1;
			}
		}
	}
	return 0;
}

void my_display(){
	/* clear the buffer */
	/* NEW: now we have to clear depth as well */
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT) ;

	glMatrixMode(GL_MODELVIEW) ;
	glLoadIdentity();
	gluLookAt(0.0, 25.0, 25.0,	// x,y,z coord of the camera 
		0.0, 0.0, 0.0,	// x,y,z coord of the origin
		0.0, 1.0, 0.0); // the direction of up (default is y-axis)

	glRotatef(theta_y,0,1,0);
	glRotatef(theta_x,1,0,0);
	draw_objects();

	/* buffer is ready */
	glutSwapBuffers();

	// Look 30 frames/sec
	//glutTimerFunc( 1000./30., game_loop, 0);
	
	return;
}

void my_reshape(int w, int h){

}

void my_mouse(int button, int state, int mousex, int mousey){

}

// Handle game controls
void my_keyboard(unsigned char key, int x, int y){
	unsigned int i;
	switch (key){
		case 'a':
			pl->dx = -pl->v;
		break;
		case 's':
			pl->dz = pl->v;
		break;
		case 'w':
			pl->dz = -pl->v;
		break;
		case 'd':
			pl->dx = pl->v;
		break;
		case ' ':
			pl->firing = 1;
		break;
		case 'e':
			if (crt_render_mode == GL_POLYGON)
				crt_render_mode = GL_LINE_LOOP;
			else
				crt_render_mode = GL_POLYGON;
		break;
	}
}

void my_keyboard_up(unsigned char key, int x, int y){
	switch (key){
		case 'a':
			pl->dx = 0;
		break;
		case 's':
			pl->dz = 0;
		break;
		case 'w':
			pl->dz = 0;
		break;
		case 'd':
			pl->dx = 0;
		break;
		case ' ':
			pl->firing = 0;
		break;
	}
}


void gl_setup(void) {
	/* specifies a background color: black in this case */
	glClearColor(0,0,0,0) ;

	/* NEW: now we have to enable depth handling (z-buffer) */
	glEnable(GL_DEPTH_TEST);
	// enable auto normalize
	glEnable(GL_NORMALIZE);

	/* NEW: setup for 3d projection */
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	// perspective view
	gluPerspective(45.0, 1.0, 1.0, 150.0);

	glShadeModel(GL_FLAT); 
	lighting_setup();
	texture_setup();
	return;
}

void my_setup(void) {
	theta_x = 0;
	theta_y = 0;
	crt_render_mode = GL_POLYGON;
	return;
}

void glut_setup(void) {

	/* specify display mode -- here we ask for a double buffer and RGB coloring */
	/* NEW: tell display we care about depth now */
	glutInitDisplayMode (GLUT_DOUBLE |GLUT_RGB |GLUT_DEPTH);

	/* make a 400x400 window with the title of "Stitcher" placed at the top left corner */
	glutInitWindowSize(WINDOW_W, WINDOW_H);
	glutInitWindowPosition(5,50);
	glutCreateWindow("GSilph");

	/*initialize callback functions */
	glutDisplayFunc(my_display);
	glutReshapeFunc(my_reshape); 
	glutMouseFunc(my_mouse);
	glutKeyboardFunc(my_keyboard);
	glutKeyboardUpFunc(my_keyboard_up);
	return;
}

// Called whenever the player dies
void restart_game(){
	unsigned int i;
	// Clear out all game data
	for (i = 0; i < bullets.size(); i++){
		delete bullets[i];
		bullets.erase(bullets.begin() + i);
		i--;
	}
	for (i = 0; i < enemies.size(); i++){
		delete enemies[i];
		enemies.erase(enemies.begin() + i);
		i--;
	}
	for (i = 0; i < debris.size(); i++){
		delete debris[i];
		debris.erase(debris.begin() + i);
		i--;
	}

	Globe *b = new Globe(20, -40, -60);
	enemies.push_back(b);


	pl->life = 5;

	parse_level("level1.txt");
}

// Game loop
void my_TimeOut(int t){
	unsigned int i, j;
	// Handle bullet actions/movement
	for (i = 0; i < bullets.size(); i++){
		bullets[i]->move();
		bullets[i]->action();
		// Damage enemies upon contact
		for (j = 0; j < enemies.size(); j++){
			if (col_rect(bullets[i], enemies[j]) == 1 && !bullets[i]->damages_player){
				enemies[j]->life--;
				bullets[i]->del = 1;
			}
		}
		if (col_rect(bullets[i], pl) && bullets[i]->damages_player && pl->damage_timer < 1){
			pl->damage_timer = 30;
			pl->life--;
			bullets[i]->del = 1;
		}
		if (bullets[i]->del){
			delete bullets[i];
			bullets.erase(bullets.begin() + i);
			i--;
		}
	}
	// Enemy actions/movement
	for (i = 0; i < enemies.size(); i++){
		enemies[i]->move();
		enemies[i]->action();
		// If enemies touch the player, hurt him
		if (col_rect(enemies[i], pl) && pl->damage_timer < 1){
			pl->damage_timer = 30;
			pl->life--;
		}
		// If enemies have no life, kill them
		if (enemies[i]->life < 1){
			enemies[i]->del = 1;
		}
		if (enemies[i]->del){
			Particle *p;
			int j, k;
			// Particles fly out from the center of destroyed objects
			for (j = -1 - enemies[i]->w; j < 2 + enemies[i]->w; j+= 2){
				for (k = -1 - enemies[i]->h; k < 2 + enemies[i]->h; k += 2){
					p = new Particle(enemies[i]->x, enemies[i]->y, enemies[i]->z, -.1*j, -.1*k);
					// Have the debris retain some of the enemy's momentum
					p->dz += enemies[i]->dz/2;
					debris.push_back(p);
				}
			}
			delete enemies[i];
			enemies.erase(enemies.begin() + i);
			i--;
		}
	}

	// Stars constantly scroll down screen
	for (i = 0; i < stars.size(); i++){
		stars[i]->move();
		stars[i]->action();
		if (stars[i]->del){
			delete stars[i];
			stars.erase(stars.begin() + i);
			i--;
		}
	}

	// Bits that display after an enemy dies
	for (i = 0; i < debris.size(); i++){
		debris[i]->move();
		debris[i]->action();
		if (debris[i]->del){
			delete debris[i];
			debris.erase(debris.begin() + i);
			i--;
		}
	}

	pl->move();
	pl->action();
	glutPostRedisplay();

	// If the player dies, restart
	if (pl->life < 1){
		restart_game();
	}

	glutTimerFunc(fps, my_TimeOut, 0);
}


/*************** Main *****************/
int main(int argc, char **argv){
	srand(10);
	unsigned int i;

	glutInit( &argc, argv );

	pl = new Player(0, 0, 0);
	ply = (Player *) pl;
	
	// Load a bunch of background stars
	for (i = 0; i < 50; i++){
		Star *s = new Star(rand()%36 - 18, -6, rand()%50 - 38);
		stars.push_back(s);
	}

	parse_level("level1.txt");

	Globe *b = new Globe(20, -40, -60);
	enemies.push_back(b);

	/* we define these setup procedures */
	my_setup();
	glut_setup();
	gl_setup();
	my_TimeOut(fps);

	/* go into the main event loop */
	glutMainLoop();

	// Make sure all enemies are deallocated
	for (i = 0; i < enemies.size(); i++){
		delete enemies[i];
	}
	for (i = 0; i < bullets.size(); i++){
		delete bullets[i];
	}
	for (i = 0; i < stars.size(); i++){
		delete stars[i];
	}
	delete pl;

	return 0;
}