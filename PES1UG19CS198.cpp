/*
PES1UG19CS198
Justin James

Right-click and move mouse for camera movement
Left-click on object to select object
Left-click on background to de-select object

Only 1 object selectable at a time

Selecting another object deselects current selected object

Keyboard commands(after selection)

Translation:
Right arrow key to translate object in +X
Left arrow key to translate object in -X
Up arrow key to translate object in +Y
Down arrow key to translate object in -Y
w key to translate object in +Z
s key to translate object in -Z

Rotation:
k = rotate along x axis
l = rotate along y axis
o = rotate along z axis

Scaling:
x = scale up in X dimension
c = scale down in X dimension

y = scale up in Y dimension
u = scale down in Y dimension

z = scale up in Z dimension
a = scale down in Z dimension


*/

#include <windows.h>
#include <random>
#include <gl/glut.h>
#include <cstdlib>
#include <cmath>
#include <iostream>


using namespace std;
constexpr auto PI = 3.14;

float initial_I_size = 4;
float I_scaleX = 1, I_scaleY = 1, I_scaleZ = 1;
float IC_X = initial_I_size*0.525731112119133606 ;
float IC_Z = initial_I_size*0.850650808352039932;
float min_I_scale = 0.5;
//float min_IC_X = 1* 0.525731112119133606;
//float min_IC_Z = 1* 0.850650808352039932;

float min_Cb_scale = 0.5;
float Cb_scaleX = 1, Cb_scaleY = 1, Cb_scaleZ = 1;

float min_Cub_scale = 0.5;
float Cub_scaleX = 1, Cub_scaleY = 1, Cub_scaleZ = 1;

constexpr auto RADIUS = 20;

struct GLPoint {
	GLfloat x, y;
};

void normalize(float v[3]) {
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (d == 0.0) {
		//error("zero length vector");
		return;
	}
	v[0] /= d; v[1] /= d; v[2] /= d;
}

void normcrossprod(float v1[3], float v2[3], float out[3])
{
	GLint i, j;
	GLfloat length;

	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];
	normalize(out);
}

GLfloat I_vertex_colors[20][3];
GLfloat Cb_vertex_colors[12][3];
GLfloat Cub_vertex_colors[12][3];

void init_I_vertex_colors() {
	float r, g, b;
	srand(time(nullptr));
	for (int k = 0; k < 20; k++) {
		float r = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		float g = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		float b = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		I_vertex_colors[k][0] = r;
		I_vertex_colors[k][1] = g;
		I_vertex_colors[k][2] = b;
		
	}
}

void init_Cb_vertex_colors() {
	float r, g, b;
	srand(time(nullptr));
	for (int k = 0; k < 12; k++) {
		float r = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		float g = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		float b = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		Cb_vertex_colors[k][0] = r;
		Cb_vertex_colors[k][1] = g;
		Cb_vertex_colors[k][2] = b;

	}
}

void init_Cub_vertex_colors() {
	float r, g, b;
	//srand(time(nullptr));
	for (int k = 0; k < 12; k++) {
		float r = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		float g = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		float b = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		Cub_vertex_colors[k][0] = r;
		Cub_vertex_colors[k][1] = g;
		Cub_vertex_colors[k][2] = b;

	}
}

GLfloat I_vdata[12][3] = {
   {-IC_X, 0.0, IC_Z}, {IC_X, 0.0, IC_Z}, {-IC_X, 0.0, -IC_Z}, {IC_X, 0.0, -IC_Z},
   {0.0, IC_Z, IC_X}, {0.0, IC_Z, -IC_X}, {0.0, -IC_Z, IC_X}, {0.0, -IC_Z, -IC_X},
   {IC_Z, IC_X, 0.0}, {-IC_Z, IC_X, 0.0}, {IC_Z, -IC_X, 0.0}, {-IC_Z, -IC_X, 0.0}
};
GLfloat Cb_vdata[8][3] = {
	{0,0,0},{2,0,0},{2,0,2},{0,0,2},
	{0,2,0},{2,2,0},{2,2,2},{0,2,2}
};

GLfloat Cub_vdata[8][3] = {
	{0,0,0},{2,0,0},{2,0,3},{0,0,3},
	{0,5,0},{2,5,0},{2,5,3},{0,5,3}
};

GLfloat I_current_vertices[12][3]; 


void init_I_vertex_positions() {
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 3; j++) {
			I_current_vertices[i][j] = I_vdata[i][j];
		}
	}
}
GLuint I_tindices[20][3] = {
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

GLuint Cb_tindices[12][3] = {
	{0,1,2},{2,3,0},{1,5,6},{6,2,1},
	{5,4,7},{7,6,5},{4,0,3},{3,7,4},
	{3,2,6},{6,7,3},{1,0,4},{4,5,1}
};

GLuint Cub_tindices[12][3] = {
	{0,1,2},{2,3,0},{1,5,6},{6,2,1},
	{5,4,7},{7,6,5},{4,0,3},{3,7,4},
	{3,2,6},{6,7,3},{1,0,4},{4,5,1}
};


int i;

float zoom = 2; float cam_alpha = 0; float cam_theta = 0;
float camX = 21, camZ = 0, camY = 0;
float upX = 0, upY = 1, upZ = 1;
float mX = 0, mY = 0;
float axisX = 1, axisY = 0, axisZ = 0;
float tX = 0, tY = 0, tZ = 0;

float I_theta = 0;
float I_x = 0, I_y = 0, I_z = 0;
float I_direction[3] = {0.5,0.5,0.5};
float I_R_x = 1, I_R_y = 0, I_R_z = 0;
int I_select = 0; int I_not_collided = 0;
int I_randomiser = 0;

float Cb_theta = 0;
float Cb_x = 6, Cb_y = 6, Cb_z = 6;
float Cb_direction[3] = { 0.5,-0.6,-0.3 };
float Cb_R_x = 0, Cb_R_y = 1, Cb_R_z = 0;
int Cb_select = 0; int Cb_not_collided = 0;
int Cb_randomiser = 0;

float Cub_theta = 0;
float Cub_x = -8, Cub_y = -8, Cub_z = -8;
float Cub_direction[3] = { -0.5,+0.2,-0.5 };
float Cub_R_x = 0, Cub_R_y = 1, Cub_R_z = 0;
int Cub_select = 0; int Cub_not_collided = 0;
int Cub_randomiser = 0;


void init_I_rotation() {
	float x = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	float y = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	float z = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	I_R_x = x;
	I_R_y = y;
	I_R_z = z;
}

void init_Cb_rotation() {
	float x = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	float y = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	float z = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	Cb_R_x = x;
	Cb_R_y = y;
	Cb_R_z = z;
}

void init_Cub_rotation() {
	float x = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	float y = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	float z = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	Cub_R_x = x;
	Cub_R_y = y;
	Cub_R_z = z;
}

void change_direction(float direction[3],int shape) {
	if (shape == 1) {
		I_randomiser += 1;
		if (I_randomiser >= 10) {
			//srand(time(nullptr));
			direction[0] = -I_x * 0.1;
			direction[1] = -I_y * 0.1;
			direction[2] = -I_z * 0.1;
			I_randomiser = 0;
			return;
		}
	}
	else if (shape == 2) {
		Cb_randomiser += 1;
		if (Cb_randomiser >= 10) {
			//srand(time(nullptr));
			direction[0] = -Cb_x * 0.1;
			direction[1] = -Cb_y * 0.1;
			direction[2] = -Cb_z * 0.1;
			Cb_randomiser = 0;
			return;
		}
	}
	else if (shape == 3) {
		Cub_randomiser += 1;
		if (Cub_randomiser >= 10) {
			//srand(time(nullptr));
			direction[0] = -Cub_x * 0.1;
			direction[1] = -Cub_y * 0.1;
			direction[2] = -Cub_z * 0.1;
			Cub_randomiser = 0;
			return;
		}
	}
	else{
	}

	float x = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	float y = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);
	float z = (-1.0 + ((float)rand() / (float)RAND_MAX) * 2);

	if (x == 0.0 && y == 0.0 && z == 0) {
		change_direction(direction,shape);
		return;
	}

	direction[0] = x;
	direction[1] = y;
	direction[2] = z;
	//printf("\nDirection Change (%f %f %f) \n", direction[0], direction[1], direction[2]);
	normalize(direction);
	//init_I_rotation();
	if (shape == 1) {
		I_not_collided = 0;
	}
	else if (shape == 2) {
		Cb_not_collided = 0;
	}
	else if (shape == 3) {
		Cub_not_collided = 0;
	}
	else {

	}

}
// a callback function that is executed at regular intervals of time registered under 'glutIdleFunc'
void timerCallBack()
{
	// HINT: the camera is moving on a curcumference of a 2D circle 
	// the points on the circle will be the position of the camera (camx,0,camy)
	// I_x+=0.01; I_y+=0.01; I_z+=0.01;
	
	float radT = cam_theta * PI / 180;
	float radA = cam_alpha * PI / 180;
	float t = 21 * cos(radA);
	camY = 21 * sin(radA);
	camX = t * cos(radT);
	camZ = t * sin(radT);
	float I_d; float Cb_d; float Cub_d;
	float x, y, z;
	int I_cd_flag = 0,Cb_cd_flag = 0,Cub_cd_flag = 0;

	if(I_select == 0)
	{
		I_not_collided += 1;;
		//printf("%d \n", I_not_collided);
		if (I_not_collided > 500) {
			change_direction(I_direction,1);
			init_I_rotation();
			//printf("\n Premptive dc \n");
		}
		I_theta++;// I_x+=0.01; I_y+=0.01; I_z+=0.01;
		if (I_theta == 360.0) {
			I_theta = 0.0;
		}
	for (int t = 0; t < 12; t++) {
		x = I_scaleX * I_vdata[t][0] + I_x + I_direction[0] * 0.1;
		y = I_scaleY * I_vdata[t][1] + I_y + I_direction[1] * 0.1;
		z = I_scaleZ * I_vdata[t][2] + I_z + I_direction[2] * 0.1;

		I_d = sqrtf(x * x + y * y + z * z);
		//printf("%f ",I_d);
		if (I_d >= RADIUS) {
			I_cd_flag = 1;
			break;
		}
	}
	if (I_cd_flag) {
		change_direction(I_direction,1);
		//printf("\nDc (%f,%f,%f) \n", I_direction[0], I_direction[1], I_direction[2]);
		I_x += 0.2 * I_direction[0];
		I_y += 0.2 * I_direction[1];
		I_z += 0.2 * I_direction[2];

	}
	else {
		I_x += 0.25 * I_direction[0];
		I_y += 0.25 * I_direction[1];
		I_z += 0.25 * I_direction[2];
	}

	}

	if (Cb_select == 0)
	{
		Cb_not_collided += 1;;
		//printf("%d \n", I_not_collided);
		if (Cb_not_collided > 500) {
			change_direction(Cb_direction,2);
			init_Cb_rotation();
			//printf("\n Premptive dc \n");
		}
		Cb_theta++;// I_x+=0.01; I_y+=0.01; I_z+=0.01;
		if (Cb_theta == 360.0) {
			Cb_theta = 0.0;
		}
		for (int t = 0; t < 8; t++) {
			x = Cb_scaleX * Cb_vdata[t][0] + Cb_x + Cb_direction[0] * 0.1;
			y = Cb_scaleY * Cb_vdata[t][1] + Cb_y + Cb_direction[1] * 0.1;
			z = Cb_scaleZ * Cb_vdata[t][2] + Cb_z + Cb_direction[2] * 0.1;

			Cb_d = sqrtf(x * x + y * y + z * z);
			//printf("%f ",I_d);
			if (Cb_d >= RADIUS) {
				Cb_cd_flag = 1;
				break;
			}
		}
		if (Cb_cd_flag) {
			change_direction(Cb_direction,2);
			//printf("\nDc (%f,%f,%f) \n", I_direction[0], I_direction[1], I_direction[2]);
			Cb_x += 0.2 * Cb_direction[0];
			Cb_y += 0.2 * Cb_direction[1];
			Cb_z += 0.2 * Cb_direction[2];

		}
		else {
			Cb_x += 0.25 * Cb_direction[0];
			Cb_y += 0.25 * Cb_direction[1];
			Cb_z += 0.25 * Cb_direction[2];
		}

	}
	if (Cub_select == 0)
	{
		Cub_not_collided += 1;;
		//printf("%d \n", I_not_collided);
		if (Cub_not_collided > 500) {
			change_direction(Cub_direction, 3);
			init_Cub_rotation();
			//printf("\n Premptive dc \n");
		}
		Cub_theta++;// I_x+=0.01; I_y+=0.01; I_z+=0.01;
		if (Cub_theta == 360.0) {
			Cub_theta = 0.0;
		}
		for (int t = 0; t < 8; t++) {
			x = Cub_scaleX * Cub_vdata[t][0] + Cub_x + Cub_direction[0] * 0.1;
			y = Cub_scaleY * Cub_vdata[t][1] + Cub_y + Cub_direction[1] * 0.1;
			z = Cub_scaleZ * Cub_vdata[t][2] + Cub_z + Cub_direction[2] * 0.1;

			Cub_d = sqrtf(x * x + y * y + z * z);
			//printf("%f ",I_d);
			if (Cub_d >= RADIUS) {
				Cub_cd_flag = 1;
				break;
			}
		}
		if (Cub_cd_flag) {
			change_direction(Cub_direction, 3);
			//printf("\nDc (%f,%f,%f) \n", I_direction[0], I_direction[1], I_direction[2]);
			Cub_x += 0.2 * Cub_direction[0];
			Cub_y += 0.2 * Cub_direction[1];
			Cb_z += 0.2 * Cub_direction[2];

		}
		else {
			Cub_x += 0.25 * Cub_direction[0];
			Cub_y += 0.25 * Cub_direction[1];
			Cub_z += 0.25 * Cub_direction[2];
		}

	}
	Sleep(10);

	//redisplay after the paramters are changed.
	glutPostRedisplay();
}

void setMaterial(GLfloat ambientR, GLfloat ambientG, GLfloat ambientB, GLfloat diffuseR, GLfloat diffuseG, GLfloat diffuseB, GLfloat specularR, GLfloat specularG, GLfloat specularB, int shine) {
	float ambient[] = { ambientR,ambientG,ambientB };
	float diffuse[] = { diffuseR,diffuseG,diffuseB };
	float specular[] = { specularR,specularG,specularB };
	glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);

}

void drawSun() {
	float colors[] = { 0.93,0.96,0.14,1};
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, colors);
	
	glTranslatef(30, 30, 30);
	glColor3f(0.93,0.96,0.14);
	glutSolidSphere(3, 10, 10);
	glPopMatrix();

}

void draw_icosahedron(){
	glBegin(GL_TRIANGLES);
	//srand(time(nullptr));
	for (i = 0; i < 20; i++) {
		
		
		//float r = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		//float g = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		//float b = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		GLfloat d1[3], d2[3], norm[3];
		for (int j = 0; j < 3; j++) {
			d1[j] = I_vdata[I_tindices[i][0]][j] - I_vdata[I_tindices[i][1]][j];
			d2[j] = I_vdata[I_tindices[i][1]][j] - I_vdata[I_tindices[i][2]][j];
		}
		normcrossprod(d2,d1, norm);
		glNormal3fv(norm);
		glColor3f(I_vertex_colors[i][0], I_vertex_colors[i][1], I_vertex_colors[i][2]);
		glVertex3fv(&I_vdata[I_tindices[i][0]][0]);
		glVertex3fv(&I_vdata[I_tindices[i][1]][0]);
		glVertex3fv(&I_vdata[I_tindices[i][2]][0]);
		//printf("\nDrew the shape . Scale = %f ",I_scale);
	}
	glEnd();
}

void draw_cube() {
	glBegin(GL_TRIANGLES);
	//srand(time(nullptr));
	for (i = 0; i < 12; i++) {


		//float r = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		//float g = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		//float b = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		GLfloat d1[3], d2[3], norm[3];
		for (int j = 0; j < 3; j++) {
			d1[j] = Cb_vdata[Cb_tindices[i][0]][j] - Cb_vdata[Cb_tindices[i][1]][j];
			d2[j] = Cb_vdata[Cb_tindices[i][1]][j] - Cb_vdata[Cb_tindices[i][2]][j];
		}
		normcrossprod(d1, d2, norm);
		glNormal3fv(norm);
		glColor3f(Cb_vertex_colors[i][0], Cb_vertex_colors[i][1], Cb_vertex_colors[i][2]);
		glVertex3fv(&Cb_vdata[Cb_tindices[i][0]][0]);
		glVertex3fv(&Cb_vdata[Cb_tindices[i][1]][0]);
		glVertex3fv(&Cb_vdata[Cb_tindices[i][2]][0]);
		//printf("\nDrew the shape . Scale = %f ",I_scale);
	}
	glEnd();
}

void draw_cuboid() {
	glBegin(GL_TRIANGLES);
	//srand(time(nullptr));
	for (i = 0; i < 12; i++) {


		//float r = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		//float g = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		//float b = (0.0 + ((float)rand() / (float)RAND_MAX) * 1.0);
		GLfloat d1[3], d2[3], norm[3];
		for (int j = 0; j < 3; j++) {
			d1[j] = Cub_vdata[Cub_tindices[i][0]][j] - Cub_vdata[Cub_tindices[i][1]][j];
			d2[j] = Cub_vdata[Cub_tindices[i][1]][j] - Cub_vdata[Cub_tindices[i][2]][j];
		}
		normcrossprod(d1, d2, norm);
		glNormal3fv(norm);
		glColor3f(Cub_vertex_colors[i][0], Cub_vertex_colors[i][1], Cub_vertex_colors[i][2]);
		glVertex3fv(&Cub_vdata[Cub_tindices[i][0]][0]);
		glVertex3fv(&Cub_vdata[Cub_tindices[i][1]][0]);
		glVertex3fv(&Cub_vdata[Cub_tindices[i][2]][0]);
		//printf("\nDrew the shape . Scale = %f ",I_scale);
	}
	glEnd();

}
void project_display() {
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	
	
	glMatrixMode(GL_MODELVIEW);

	//clearing the model view matrix
	glLoadIdentity();

	//setting up the position for camara based on the calculation in timeCallBack()
	//gluLookAt(zoom * camX, zoom * camY, zoom * camZ, 0, 0, 0, 0, 1, 0);
	gluLookAt(camX,camY, camZ, 0, 0, 0, upX, upY, upZ);

	//drawSun();

	setMaterial(0.2, 0.2, 0.2, 0.4, 0.4, 0.4, 0.8, 0.8, 0.8, 10);
	glPushMatrix();
	
	glTranslatef(I_x, I_y, I_z);
	
	glRotatef(I_theta, I_R_x, I_R_y, I_R_z);
	
	glScalef(I_scaleX, I_scaleY, I_scaleZ);
	glStencilFunc(GL_ALWAYS, 1, -1);
	draw_icosahedron();
	glPopMatrix();

	glPushMatrix();

	glTranslatef(Cb_x, Cb_y, Cb_z);

	glRotatef(Cb_theta, Cb_R_x, Cb_R_y, Cb_R_z);

	glScalef(Cb_scaleX, Cb_scaleY,Cb_scaleZ);
	glStencilFunc(GL_ALWAYS, 2, -1);
	draw_cube();
	glPopMatrix();


	glPushMatrix();

	glTranslatef(Cub_x, Cub_y, Cub_z);

	glRotatef(Cub_theta, Cub_R_x, Cub_R_y, Cub_R_z);

	glScalef(Cub_scaleX, Cub_scaleY, Cub_scaleZ);
	glStencilFunc(GL_ALWAYS, 3, -1);
	draw_cuboid();
	glPopMatrix();
	
	glFlush();
}
void display()
{
	// Clear your Window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Colored Hexagon with triangles
	/*glBegin(GL_TRIANGLES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2i(160, 110);
		glVertex2i(320, 10);
		glVertex2i(480, 110);

		glColor3f(0.0, 1.0, 0.0);
		glVertex2i(160, 500);
		glVertex2i(320, 600);
		glVertex2i(480, 500);

		glColor3f(0.0, 0.0, 1.0);
		glVertex2i(160, 110);
		glVertex2i(480, 110);
		glVertex2i(160, 500);

		glColor3f(1.0, 0.0, 1.0);
		glVertex2i(160, 500);
		glVertex2i(480, 500);
		glVertex2i(480, 110);

	glEnd();*/
	//selecting the Model View Matrix for making modification to the view
	glMatrixMode(GL_MODELVIEW);

	//clearing the model view matrix
	glLoadIdentity();

	//setting up the position for camara based on the calculation in timeCallBack()
	gluLookAt(zoom*camX, zoom*camY, zoom*camZ, 0, 0, 0, 0, 1, 0);
	
	//draw the scene
	//glPushMatrix();
	//glScalef(0.5, 0.5, 0.5);
	//glTranslatef(-1, 0, 0);
	//coloredCube();
	//glPopMatrix();


	glColor3f(1, 0, 0);
	

	glColor3f(1, 0, 0);
	glPushMatrix();

	//set material
	setMaterial(0.2,0.2,0.2,0.4,0.4,0.4,1,1,1,100);
	glTranslatef(tX, tY, tZ);
	//glRotatef(I_theta, axisX,axisY,axisZ);
	glutSolidTeapot(0.5);
	glPopMatrix();
	
	// Flush the drawing routines to the window
	glFlush();
	//glSwapBuffers();
}

void initLight() {

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	float ambient[] = { 0.2,0.2,0.2,1 };
	float diffuse[] = { 0.8,0.8,0.8,1 };
	float specular[] = { 0.7,0.7,0.7,1 };
	float position[] = {0,0,0,1};
	float position2[] = { 30,30,30,1 };
	glLightfv(GL_LIGHT0, GL_LIGHT_MODEL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	//glMaterialf(GL_FRONT_FACE, GL_SHININESS, shine);
	glLightfv(GL_LIGHT0, GL_POSITION, position);


	
	

}

void myKeyboardFunc(unsigned char key, int x, int y) {
	switch (key) {
	//case '-': zoom--; break;
	//case '=': zoom++; break;
	case 'k':
		if (I_select == 1) {
			I_R_x = 1;
			I_R_y = 0;
			I_R_z = 0;
			I_theta++;
			if (I_theta == 360.0) {
				I_theta = 0;
			}
			break;
		}
		else if (Cb_select == 1) {
			Cb_R_x = 1;
			Cb_R_y = 0;
			Cb_R_z = 0;
			Cb_theta++;
			if (Cb_theta == 360.0) {
				Cb_theta = 0;
			}
			break;
		}
		else if (Cub_select == 1) {
			Cub_R_x = 1;
			Cub_R_y = 0;
			Cub_R_z = 0;
			Cub_theta++;
			if (Cub_theta == 360.0) {
				Cub_theta = 0;
			}
			break;
		}
		else
			break;
	case 'l':
		if (I_select == 1) {
			I_R_x = 0;
			I_R_y = 1;
			I_R_z = 0;
			I_theta++;
			if (I_theta == 360.0) {
				I_theta = 0;
			}
			break;
		}
		else if (Cb_select == 1) {
			Cb_R_x = 0;
			Cb_R_y = 1;
			Cb_R_z = 0;
			Cb_theta++;
			if (Cb_theta == 360.0) {
				Cb_theta = 0;
			}
			break;
		}
		else if (Cub_select == 1) {
			Cub_R_x = 0;
			Cub_R_y = 1;
			Cub_R_z = 0;
			Cub_theta++;
			if (Cub_theta == 360.0) {
				Cub_theta = 0;
			}
			break;
		}
		else
			break;
	case 'o':
		if (I_select == 1) {
			I_R_x = 0;
			I_R_y = 0;
			I_R_z = 1;
			I_theta++;
			if (I_theta == 360.0) {
				I_theta = 0;
			}
			break;
		}
		else if (Cb_select == 1) {
			Cb_R_x = 0;
			Cb_R_y = 0;
			Cb_R_z = 1;
			Cb_theta++;
			if (Cb_theta == 360.0) {
				Cb_theta = 0;
			}
			break;
		}
		else if (Cub_select == 1) {
			Cub_R_x = 0;
			Cub_R_y = 0;
			Cub_R_z = 1;
			Cub_theta++;
			if (Cub_theta == 360.0) {
				Cub_theta = 0;
			}
			break;
		}
		else
			break;
	case 'x':
		if (I_select == 1) {
			float temp = I_scaleX;
			I_scaleX += 0.2;
			int flag = 0;
			for (int k = 0; k < 12; k++) {
				float x = I_scaleX * I_vdata[k][0] + I_x;
				float y = I_scaleY*I_vdata[k][1] + I_y;
				float z = I_scaleZ*I_vdata[k][2] + I_z;

				float d = sqrtf(x * x + y * y + z * z);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag==1) {
				I_scaleX = temp;
				break;
			}

			break;
		}
		else if (Cb_select == 1) {
			float temp = Cb_scaleX;
			Cb_scaleX += 0.2;
			int flag = 0;
			for (int k = 0; k < 8; k++) {
				float x = Cb_scaleX * Cb_vdata[k][0] + Cb_x;
				float y = Cb_scaleY * Cb_vdata[k][1] + Cb_y;
				float z = Cb_scaleZ * Cb_vdata[k][2] + Cb_z;

				float d = sqrtf(x * x + y * y + z * z);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				Cb_scaleX = temp;
				break;
			}

			break;
		}
		else if (Cub_select == 1) {
			float temp = Cub_scaleX;
			Cub_scaleX += 0.2;
			int flag = 0;
			for (int k = 0; k < 8; k++) {
				float x = Cub_scaleX * Cub_vdata[k][0] + Cub_x;
				float y = Cub_scaleY * Cub_vdata[k][1] + Cub_y;
				float z = Cub_scaleZ * Cub_vdata[k][2] + Cub_z;

				float d = sqrtf(x * x + y * y + z * z);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				Cub_scaleX = temp;
				break;
			}

			break;
		}
		else
			break;
	case 'c':
		if (I_select == 1) {
			if (I_scaleX - 0.2 < min_I_scale) {
				break;
			}
			else {
				I_scaleX -= 0.2;
			}
		}
		else if (Cb_select == 1) {
			if (Cb_scaleX - 0.2 < min_Cb_scale) {
				break;
			}
			else {
				Cb_scaleX -= 0.2;
			}
		}
		else if (Cub_select == 1) {
			if (Cub_scaleX - 0.2 < min_Cub_scale) {
				break;
			}
			else {
				Cub_scaleX -= 0.2;
			}
		}
		else
			break;
	case 'y':
		if (I_select == 1) {
			float temp = I_scaleY;
			I_scaleY += 0.2;
			int flag = 0;
			for (int k = 0; k < 12; k++) {
				float x = I_scaleX * I_vdata[k][0] + I_x;
				float y = I_scaleY * I_vdata[k][1] + I_y;
				float z = I_scaleZ * I_vdata[k][2] + I_z;

				float d = sqrtf(x * x + y * y + z * z);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				I_scaleY = temp;
				break;
			}

			break;
		}
		else if (Cb_select == 1) {
			float temp = Cb_scaleY;
			Cb_scaleY += 0.2;
			int flag = 0;
			for (int k = 0; k < 8; k++) {
				float x = Cb_scaleX * Cb_vdata[k][0] + Cb_x;
				float y = Cb_scaleY * Cb_vdata[k][1] + Cb_y;
				float z = Cb_scaleZ * Cb_vdata[k][2] + Cb_z;

				float d = sqrtf(x * x + y * y + z * z);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				Cb_scaleY = temp;
				break;
			}

			break;
		}
		else if (Cub_select == 1) {
			float temp = Cub_scaleY;
			Cub_scaleY += 0.2;
			int flag = 0;
			for (int k = 0; k < 8; k++) {
				float x = Cub_scaleX * Cub_vdata[k][0] + Cub_x;
				float y = Cub_scaleY * Cub_vdata[k][1] + Cub_y;
				float z = Cub_scaleZ * Cub_vdata[k][2] + Cub_z;

				float d = sqrtf(x * x + y * y + z * z);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				Cub_scaleY = temp;
				break;
			}

			break;
		}
		else
			break;

	case 'u':
		if (I_select == 1) {
			if (I_scaleY - 0.2 < min_I_scale) {
				break;
			}
			else {
				I_scaleY -= 0.2;
			}
		}
		else if (Cb_select == 1) {
			if (Cb_scaleY - 0.2 < min_Cb_scale) {
				break;
			}
			else {
				Cb_scaleY -= 0.2;
			}
		}
		else if (Cub_select == 1) {
			if (Cub_scaleY - 0.2 < min_Cub_scale) {
				break;
			}
			else {
				Cub_scaleY -= 0.2;
			}
		}
		else
			break;
	case 'z':
		if (I_select == 1) {
			float temp = I_scaleZ;
			I_scaleZ += 0.2;
			int flag = 0;
			for (int k = 0; k < 12; k++) {
				float x = I_scaleX * I_vdata[k][0] + I_x;
				float y = I_scaleY * I_vdata[k][1] + I_y;
				float z = I_scaleZ * I_vdata[k][2] + I_z;

				float d = sqrtf(x * x + y * y + z * z);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				I_scaleZ = temp;
				break;
			}

			break;
		}
		else if (Cb_select == 1) {
			float temp = Cb_scaleZ;
			Cb_scaleZ += 0.2;
			int flag = 0;
			for (int k = 0; k < 8; k++) {
				float x = Cb_scaleX * Cb_vdata[k][0] + Cb_x;
				float y = Cb_scaleY * Cb_vdata[k][1] + Cb_y;
				float z = Cb_scaleZ * Cb_vdata[k][2] + Cb_z;

				float d = sqrtf(x * x + y * y + z * z);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				Cb_scaleZ = temp;
				break;
			}

			break;
		}
		else if (Cub_select == 1) {
			float temp = Cub_scaleZ;
			Cub_scaleZ += 0.2;
			int flag = 0;
			for (int k = 0; k < 8; k++) {
				float x = Cub_scaleX * Cub_vdata[k][0] + Cub_x;
				float y = Cub_scaleY * Cub_vdata[k][1] + Cub_y;
				float z = Cub_scaleZ * Cub_vdata[k][2] + Cub_z;

				float d = sqrtf(x * x + y * y + z * z);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				Cub_scaleZ = temp;
				break;
			}

			break;
		}
		else
			break;
	case 'a':
		if (I_select == 1) {
			if (I_scaleZ - 0.2 < min_I_scale) {
				break;
			}
			else {
				I_scaleZ -= 0.2;
			}
		}
		else if (Cb_select == 1) {
			if (Cb_scaleZ - 0.2 < min_Cb_scale) {
				break;
			}
			else {
				Cb_scaleZ -= 0.2;
			}
		}
		else if (Cub_select == 1) {
			if (Cub_scaleZ - 0.2 < min_Cub_scale) {
				break;
			}
			else {
				Cub_scaleZ -= 0.2;
			}
		}
		else
			break;
	//case 'y': axisX = 0; axisY = 1; axisZ = 0; break;
	//case 'z': axisX = 0; axisY = 0; axisZ = 1; break;
	//case 'f':tZ = tZ + 0.2; break;
	//case 'b':tZ = tZ - 0.2; break;
	//case 'i':alpha = alpha - 1;
	//case 'k':alpha = alpha + 1;
	//case 'j':I_theta = I_theta - 1;
	//case 'l':I_theta = I_theta + 1;
	case 'w':
		if (I_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 12; t++) {
				x = I_scaleX * I_vdata[t][0] + I_x;
				y = I_scaleY * I_vdata[t][1] + I_y;
				z = I_scaleZ * I_vdata[t][2] + I_z + 0.1;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			I_z = I_z + 0.1;
			break;
		}
		else if (Cb_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cb_scaleX * Cb_vdata[t][0] + Cb_x;
				y = Cb_scaleY * Cb_vdata[t][1] + Cb_y;
				z = Cb_scaleZ * Cb_vdata[t][2] + Cb_z + 0.1;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cb_z = Cb_z + 0.1;
			break;
		}
		else if (Cub_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cub_scaleX * Cub_vdata[t][0] + Cub_x;
				y = Cub_scaleY * Cub_vdata[t][1] + Cub_y;
				z = Cub_scaleZ * Cub_vdata[t][2] + Cub_z + 0.1;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cub_z = Cub_z + 0.1;
			break;
		}
		else
			break;

	case 's':
		if (I_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 12; t++) {
				x = I_scaleX * I_vdata[t][0] + I_x ;
				y = I_scaleY * I_vdata[t][1] + I_y;
				z = I_scaleZ * I_vdata[t][2] + I_z -0.1;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			I_z = I_z - 0.1;
			break;
		}
		else if (Cb_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cb_scaleX * Cb_vdata[t][0] + Cb_x;
				y = Cb_scaleY * Cb_vdata[t][1] + Cb_y;
				z = Cb_scaleZ * Cb_vdata[t][2] + Cb_z - 0.1;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cb_z = Cb_z - 0.1;
			break;
		}
		else if (Cub_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cub_scaleX * Cub_vdata[t][0] + Cub_x;
				y = Cub_scaleY * Cub_vdata[t][1] + Cub_y;
				z = Cub_scaleZ * Cub_vdata[t][2] + Cub_z - 0.1;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cub_z = Cub_z - 0.1;
			break;
		}
		else
			break;
	}
	glutPostRedisplay();
}

void mySpecialFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		if (I_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x ,y,z;
			float d;
			float flag = 0;
			
			for (int t = 0; t < 12; t++) {
				x = I_scaleX * I_vdata[t][0] + I_x ;
				y = I_scaleY * I_vdata[t][1] + I_y+0.1;
				z = I_scaleZ * I_vdata[t][2] + I_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			I_y = I_y+0.1;
			break;
		}
		else if (Cb_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cb_scaleX * Cb_vdata[t][0] + Cb_x;
				y = Cb_scaleY * Cb_vdata[t][1] + Cb_y + 0.1;
				z = Cb_scaleZ * Cb_vdata[t][2] + Cb_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cb_y = Cb_y + 0.1;
			break;
		}
		else if (Cub_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cub_scaleX * Cub_vdata[t][0] + Cub_x;
				y = Cub_scaleY * Cub_vdata[t][1] + Cub_y + 0.1;
				z = Cub_scaleZ * Cub_vdata[t][2] + Cub_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cub_y = Cub_y + 0.1;
			break;
		}
		else
			break;
	case GLUT_KEY_DOWN:
		if (I_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag=0;

			for (int t = 0; t < 12; t++) {
				x = I_scaleX * I_vdata[t][0] + I_x;
				y = I_scaleY * I_vdata[t][1] + I_y - 0.1;
				z = I_scaleZ * I_vdata[t][2] + I_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			I_y = I_y-0.1;
			break;
		}
		else if (Cb_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cb_scaleX * Cb_vdata[t][0] + Cb_x;
				y = Cb_scaleY * Cb_vdata[t][1] + Cb_y - 0.1;
				z = Cb_scaleZ * Cb_vdata[t][2] + Cb_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cb_y = Cb_y - 0.1;
			break;
		}
		else if (Cub_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cub_scaleX * Cub_vdata[t][0] + Cub_x;
				y = Cub_scaleY * Cub_vdata[t][1] + Cub_y - 0.1;
				z = Cub_scaleZ * Cub_vdata[t][2] + Cub_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cub_y = Cub_y - 0.1;
			break;
		}
		else
			break;
	case GLUT_KEY_LEFT:
		if (I_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;

			for (int t = 0; t < 12; t++) {
				x = I_scaleX * I_vdata[t][0] + I_x-0.1;
				y = I_scaleY * I_vdata[t][1] + I_y;
				z = I_scaleZ * I_vdata[t][2] + I_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			I_x = I_x-0.1;
			break;
		}
		else if (Cb_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cb_scaleX * Cb_vdata[t][0] + Cb_x - 0.1;
				y = Cb_scaleY * Cb_vdata[t][1] + Cb_y;
				z = Cb_scaleZ * Cb_vdata[t][2] + Cb_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cb_x = Cb_x - 0.1;
			break;
		}
		else if (Cub_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cub_scaleX * Cub_vdata[t][0] + Cub_x - 0.1;
				y = Cub_scaleY * Cub_vdata[t][1] + Cub_y;
				z = Cub_scaleZ * Cub_vdata[t][2] + Cub_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cub_x = Cub_x - 0.1;
			break;
		}
		else
			break;
	case GLUT_KEY_RIGHT: 
		if (I_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag=0;
			for (int t = 0; t < 12; t++) {
				x = I_scaleX * I_vdata[t][0] + I_x + 0.1;
				y = I_scaleY * I_vdata[t][1] + I_y ;
				z = I_scaleZ * I_vdata[t][2] + I_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			I_x = I_x+0.1;
			break;
		}
		else if (Cb_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cb_scaleX * Cb_vdata[t][0] + Cb_x + 0.1;
				y = Cb_scaleY * Cb_vdata[t][1] + Cb_y;
				z = Cb_scaleZ * Cb_vdata[t][2] + Cb_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cb_x = Cb_x + 0.1;
			break;
		}
		else if (Cub_select == 1) {
			//axisX = 1; axisY = 0; axisZ = 0;
			float x, y, z;
			float d;
			float flag = 0;
			for (int t = 0; t < 8; t++) {
				x = Cub_scaleX * Cub_vdata[t][0] + Cub_x + 0.1;
				y = Cub_scaleY * Cub_vdata[t][1] + Cub_y;
				z = Cub_scaleZ * Cub_vdata[t][2] + Cub_z;

				d = sqrtf(x * x + y * y + z * z);
				//printf("%f ",I_d);
				if (d >= RADIUS) {
					flag = 1;
					break;
				}
			}
			if (flag) {
				break;
			}
			Cub_x = Cub_x + 0.1;
			break;
		}
		else
			break;
	}
	glutPostRedisplay();
}

void myMouseFunc(int button,int state,int x,int y) {
	//buttons: GLUT_LEFT_BUTTON GLUT_RIGHT_BUTTON
	//states: GLUT_UP, GLUT_DOWN
	//if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//zoom=zoom-0.1;
	//}

	//else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		//zoom = zoom + 0.1;
	//}
	//else {

	//}
	int yPixel,xPixel;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		yPixel = glutGet(GLUT_WINDOW_HEIGHT) - y;
		xPixel = x;
		int index;
		//Index1
		glReadPixels(xPixel, yPixel, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
		printf("\nindex value %d \n", index);
		if (index == 1) {
			I_select = 1;
			Cb_select = 0;
			Cub_select = 0;
			printf("\nselected %d \n", I_select);
		}
		else if (index == 2) {
			I_select = 0;
			Cb_select = 1;
			Cub_select = 0;
			printf("\nselected %d \n", Cb_select);
		}
		else if (index == 3) {
			I_select = 0;
			Cb_select = 0;
			Cub_select = 1;
			printf("\nselected %d \n", Cub_select);
		}
		else {
			if (I_select == 1 ) {
				I_select = 0;
				change_direction(I_direction,1);
				init_I_rotation();
			}
			if (Cb_select == 1) {
				Cb_select = 0;
				change_direction(Cb_direction,2);
				init_Cb_rotation();
			}
			if (Cub_select == 1) {
				Cub_select = 0;
				change_direction(Cub_direction, 3);
				init_Cub_rotation();
			}
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		mX = x;
		mY = y;
	}
	glutPostRedisplay();
}

void myMotionFunc(int x,int y){
	float dx, dy;
	float gain = 10;
	dx = gain * (x - mX) / 900;
	dy = gain * (y - mY) / 900;

	cam_theta += dx;
	cam_alpha += -dy;
	if(cam_alpha>90){
		cam_alpha = 90;
		cam_theta += 180;
	}
	else if (cam_alpha < -90) {
		cam_alpha = -90;
		cam_theta += 180;
	}
	else{}
	//printf("\nChange in x : %f Change in y : %f \n Cam theta %f  Cam alpha %f", dx, dy, cam_theta, cam_alpha);
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	// Step1:  initialize GLUT using the command line parameters
	glutInit(&argc, argv);

	// Step2: Setup the size of the opengl window, display mode
	glutInitWindowSize(900, 900);
	glutInitWindowPosition(10, 10);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);

	

	// Step3: creating the window
	glutCreateWindow("Hello World! Display a Point");
	glEnable(GL_NORMALIZE);
	//Set up Lighting
	initLight();
	
	init_I_vertex_colors();
	init_I_vertex_positions();
	init_I_rotation();

	init_Cb_vertex_colors();
	//init_Cb_vertex_positions();
	init_Cb_rotation();

	init_Cub_vertex_colors();
	init_Cub_rotation();
	//Allow us to change the colors of the material
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_DOUBLEBUFFER);
	//set window background color
	
	glClearColor(0.40, 0.70, 1, 0);


	// Step4: defining a callback function for looping into the rasterizer
	glutDisplayFunc(project_display);

	//define a timer callback function
	glutIdleFunc(timerCallBack);

	glutKeyboardFunc(myKeyboardFunc);

	//for up down keys etc
	glutSpecialFunc(mySpecialFunc);

	//mouse
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(myMotionFunc);

	//Setup projection window
	glMatrixMode(GL_PROJECTION); // Select Projection Matrix
	glLoadIdentity();
	//Orthographic view for 2D
	//gluOrtho2D(0, 640, 0, 640);

	//ortho graphic view for 3D
	//glOrtho(-8, 8, -8, 8, -8, 8);

	gluPerspective(90, 1, 0.15, 1000000);

	//defines size of the point
	//glPointSize(3.0);

	// Step5: Telling the GLUT to loop into the callback
	glutMainLoop();
}
