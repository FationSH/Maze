/*
	compile with g++ maze.cpp -lglut -lGL -lGLU -lSOIL
	or just make
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "SOIL.h"

using namespace std;

static int g_Width = 600; // platos parathirou
static int g_Height = 400; // ipsos parathirou

int L, N, K; // number of levels, NxN megethos maze, K sfiria paikth
int score, current_level, moves, hammers; // score, trexon epipedo, kiniseis kai sfiria tou paikth
/*
	pinakas 3 diastasewn maze pou perigrafei ton labirintho:
	0 = E	1 = R	2 = G	3 = B	4 = T1	5 = T2	6 = T3
*/
int maze[4][16][16];

// coords gia tin gluLookAt
float eyeX, eyeY, eyeZ; // gia tin gluLookAt
float angle = 0.0f; // gwnia
float xrot = 0.0f, yrot = 0.0f; // x, y rotations gia tin mouse motion
float lastx, lasty; // lasy x, y gia tin mouse motion
float xpos = 0, ypos = 0, zpos = 0;

string msg; // message string gia to score

GLuint texture[3]; // T1, T2, T3 textures sta 0, 1, 2 indexes antistoixa

GLfloat light_ambient[] = {1.0f, 1.0f, 1.0f}; // set AmbientLight to the specified values
GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0}; //set the material to white
GLfloat mShininess[] = {64}; // set the shininess of the material

int oldMouseX;
int oldMouseY;

// keys pressed
int vPressed = 0;
int ePressed = 0;
int gameover = 0;

void readMaz(const char *filename); // diabazei arxeia tipou file.maz
void renderScene(); // h display function pou xrisimopoioume
void processNormalKeys(unsigned char key, int x, int y); // sinartisi gia ta normal keys
void motion(int mouseX, int mouseY); // mouse motion gia ton elegxo kameras me topontiki tipou fps
void changeSize(int w, int h); // sinartisi reshape
void init(); // sinartisi gia tis arxikopoihseis

/*
	Reads .maz files
*/
void readMaz(const char *filename) {
	ifstream mazFile;

	int levels, i, j;
	string inputLine;

	mazFile.open(filename, ios::in);
	inputLine = "";

	mazFile >> inputLine; // read L
	mazFile >> inputLine; // read =
	mazFile >> L; // read L = number

	mazFile >> inputLine; // read N
	mazFile >> inputLine; // read =
	mazFile >> N; // read N = number

	mazFile >> inputLine; // read K
	mazFile >> inputLine; // read =
	mazFile >> K; // read K = number

	/*
		0 - keni thesi E
		1 - kokino R
		2 - prasino G
		3 - ble B
		4 - texture 1 T1
		5 - texture 2 T2
		6 - texture 3 T3
	*/
	for (levels = 0; levels < L; levels++) {
		mazFile >> inputLine; // Level
		mazFile >> inputLine; // number of level
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				mazFile >> inputLine;
				if (inputLine == "E") {
					maze[levels][i][j] = 0;
				} else if (inputLine == "R") {
					maze[levels][i][j] = 1;
				} else if (inputLine == "G") {
					maze[levels][i][j] = 2;
				} else if (inputLine == "B") {
					maze[levels][i][j] = 3;
				} else if (inputLine == "T1") {
					maze[levels][i][j] = 4;
				} else if (inputLine == "T2") {
					maze[levels][i][j] = 5;
				} else if (inputLine == "T3") {
					maze[levels][i][j] = 6;
				}
			}
		}
	}
}

/*
	Display function
*/
void renderScene() {

	int i, j, k;
	int y, z, x;
	char buffer[64];

	// clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset transformations
	glLoadIdentity();

	// change the light accordingly
	// mia ekswteriki pigi fwtismou
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

	// set the camera
	if (vPressed == 1) { // an patisi V kai den exei teliwsei to paixnidi na borei na kinite
		if (gameover == 0) {
			glRotatef(xrot, 1.0, 0.0, 0.0); // rotate aristera-deksia
			glRotatef(yrot, 0.0, 1.0, 0.0); // rotate panw-katw
			glTranslated(-xpos, -ypos, -zpos); // emfanise to simio otan allaksei
		} else {
			gluLookAt(eyeX, eyeY, eyeZ, /* eye */
				0.0f, 0.0f,  0.0f,      /* center is at (0,0,0) */
				0.0f, 1.0f,  0.0f);     /* up is in positive Y direction */
		}
	} else { // alliws na fenete apo panw o labirinthos
		gluLookAt(eyeX, eyeY, eyeZ, /* eye */
			0.0f, 0.0f,  0.0f,      /* center is at (0,0,0) */
			0.0f, 1.0f,  0.0f);     /* up is in positive Y direction */
	}

	for (y = -2; y < 2; y++) {
		for (z = -8; z < 8; z++) {
			for (x = -8; x < 8; x++) {
				glPushMatrix();
				if (maze[y+2][z+8][x+8] == 0) { // 0 = E
					glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
					glTranslated(x, y, z);
					glutSolidCube(1);
				} else if (maze[y+2][z+8][x+8] == 1) { // 1 = R
					glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
					glTranslated(x, y, z);
					glutSolidCube(1);
				} else if (maze[y+2][z+8][x+8] == 2) { // 2 = G
					glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
					glTranslated(x, y, z);
					glutSolidCube(1);
				} else if (maze[y+2][z+8][x+8] == 3) { // 3 = B
					glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
					glTranslated(x, y, z);
					glutSolidCube(1);
				} else if (maze[y+2][z+8][x+8] == 4) { // 4 = T1
					glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
					glEnable(GL_TEXTURE_2D);
					glTranslated(x, y, z);
					glBindTexture(GL_TEXTURE_2D, texture[0]);
					glBegin(GL_QUADS);
						// Front Face
						glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,  0.5f,  0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.5f,  0.5f,  0.5f);  // Top Left Of The Texture and Quad
						// Back Face
						glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f,  0.5f, -0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f,  0.5f, -0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);  // Bottom Left Of The Texture and Quad
						// Top Face
						glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.5f,  0.5f, -0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,  0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f,  0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,  0.5f, -0.5f);  // Top Right Of The Texture and Quad
						// Bottom Face
						glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f, -0.5f, -0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f, -0.5f, -0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						// Right face
						glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,  0.5f, -0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f,  0.5f,  0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						// Left Face
						glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f,  0.5f,  0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.5f,  0.5f, -0.5f);  // Top Left Of The Texture and Quad
					glEnd();
					glDisable(GL_TEXTURE_2D);
				} else if (maze[y+2][z+8][x+8] == 5) { // 5 = T2
					glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
					glEnable(GL_TEXTURE_2D);
					glTranslated(x, y, z);
					glBindTexture(GL_TEXTURE_2D, texture[1]);
					glBegin(GL_QUADS);
						// Front Face
						glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,  0.5f,  0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.5f,  0.5f,  0.5f);  // Top Left Of The Texture and Quad
						// Back Face
						glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f,  0.5f, -0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f,  0.5f, -0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);  // Bottom Left Of The Texture and Quad
						// Top Face
						glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.5f,  0.5f, -0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,  0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f,  0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,  0.5f, -0.5f);  // Top Right Of The Texture and Quad
						// Bottom Face
						glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f, -0.5f, -0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f, -0.5f, -0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						// Right face
						glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,  0.5f, -0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f,  0.5f,  0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						// Left Face
						glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f,  0.5f,  0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.5f,  0.5f, -0.5f);  // Top Left Of The Texture and Quad
					glEnd();
					glDisable(GL_TEXTURE_2D);
				} else if (maze[y+2][z+8][x+8] == 6) { // 6 = T3
					glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
					glEnable(GL_TEXTURE_2D);
					glTranslated(x, y, z);
					glBindTexture(GL_TEXTURE_2D, texture[2]);
					glBegin(GL_QUADS);
						// Front Face
						glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,  0.5f,  0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.5f,  0.5f,  0.5f);  // Top Left Of The Texture and Quad
						// Back Face
						glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f,  0.5f, -0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f,  0.5f, -0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);  // Bottom Left Of The Texture and Quad
						// Top Face
						glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.5f,  0.5f, -0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,  0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f,  0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,  0.5f, -0.5f);  // Top Right Of The Texture and Quad
						// Bottom Face
						glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f, -0.5f, -0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f, -0.5f, -0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						// Right face
						glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,  0.5f, -0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f,  0.5f,  0.5f);  // Top Left Of The Texture and Quad
						glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);  // Bottom Left Of The Texture and Quad
						// Left Face
						glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);  // Bottom Left Of The Texture and Quad
						glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);  // Bottom Right Of The Texture and Quad
						glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f,  0.5f,  0.5f);  // Top Right Of The Texture and Quad
						glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.5f,  0.5f, -0.5f);  // Top Left Of The Texture and Quad
					glEnd();
					glDisable(GL_TEXTURE_2D);
				}
			glPopMatrix();
			}
		}
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	if (ePressed == 0) {
			if (current_level >= L) { // an o paikths ftasi panw apo to epipedo L
				// na min borei na xeiristi ton paikth
				vPressed = 0;
				//ePressed = -1;
				gameover = 1;
				//glPushMatrix();
				glPopMatrix();
				eyeX = 30.0f;
				eyeY = 30.0f;
				eyeZ = 30.0f;
				score = N*N - moves*10 - hammers*50;
				sprintf(buffer, "Max level reached! Score: %d", score);
				msg = buffer;
				glRasterPos3f(2.0f, 21.0f, 10.0f);
				for (i = 0; i < msg.length(); i++) {
					glutBitmapCharacter(GLUT_BITMAP_9_BY_15, msg[i]);
				}
				glPopMatrix();
			}
	} else if (ePressed == 1) { // alliws an patisei to e na termatizei
		// na min borei na xeiristi ton paikth
		if (current_level <= L) {
			score = 0;
		}
		glPushMatrix();
		eyeX = 30.0f;
		eyeY = 30.0f;
		eyeZ = 30.0f;
		sprintf(buffer, "Game ended! Score: %d", score);
		msg = buffer;
		glRasterPos3f(4.0f, 21.0f, 10.0f);
		for (i = 0; i < msg.length(); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, msg[i]);
		}
		glPopMatrix();
	}

	glFlush();
	glutSwapBuffers(); // swap the buffers
}

/*
	Normal keys function
*/
void processNormalKeys(unsigned char key, int x, int y) {

	int x2, y2, z2;
	float xrotrad, yrotrad;

	switch (key) {
		case 27: // esc key
			exit(0);
			break;
		case 'r': // r key
		case 'R':
			if (gameover == 0) {
				if (vPressed == 0) {
					eyeX = 30 * cos(angle);
					eyeY = 30.0f;
					eyeZ = 30 * sin(angle);
					angle += 0.01;
				}
			}
			break;
		case 'v': // v key
		case 'V':
			if (gameover == 0) { // an den teliwse to paixnidi
				if (vPressed == 0) { // an patisi V
					vPressed = 1;
				} else { // alliws prepei h kamera na deixnei apo panw(ginete stin display me antistoixo elegxo)
					vPressed = 0;
				}
			}
			break;
		case 's': // s key
		case 'S':
			if (gameover == 0) {
				if (vPressed == 1) {
					moves++;
					yrotrad = (yrot / 180 * 3.141592654f);
					xrotrad = (xrot / 180 * 3.141592654f); 
					xpos += float(sin(yrotrad));
					zpos -= float(cos(yrotrad));
					//ypos -= float(sin(xrotrad)); // de thelw na paei pros ta panw akoma kai an koitaei h camera
				}
			}
			break;
		case 'w': // w key
		case 'W':
			if (gameover == 0){
				if (vPressed == 1) {
					moves++;
					yrotrad = (yrot / 180 * 3.141592654f);
					xrotrad = (xrot / 180 * 3.141592654f);
					xpos -= float(sin(yrotrad));
					zpos += float(cos(yrotrad));
					//ypos += float(sin(xrotrad)); // de thelw na paei pros ta panw akoma kai an koitaei h camera
				}
			}
			break;
		case 'h':
		case 'H':
			if (gameover == 0) {
				if (hammers < K) {
					if (maze[(int)xpos+8][(int)ypos][(int)zpos+8] != 0) { // an den einai keno tote borei na to spaei
						maze[(int)xpos+8][(int)ypos][(int)zpos+8] = 0;
						hammers++;
					}
				}
			}
			break;
		case 32: // spacebar key
			if (gameover == 0){
				if (vPressed == 1) {
					if (current_level+1 <= L) {
						current_level++;
						ypos -= 1;
					}
				}
			}
			break;
		case 'e': // e key
		case 'E':
			if (ePressed == 0) {
				if (gameover == 0) { // an den exei teliwsei to paixnidi
					ePressed = 1;
					gameover = 1;
				}
			}
			break;
	}

	glutPostRedisplay(); // request display update
}

/*
	mouse motion function
*/
void motion(int x, int y) {

	int diffx, diffy;

	if (vPressed == 1) {
		diffx = x-lastx; // diafora x apo to proigoumeno x
		diffy = y-lasty; // diafora y apo to proigoumeno y
		lastx = x; // proigoumeno x iso me trexon x
		lasty = y; // proigoumeno y iso me trexon y
		xrot += (float) diffy; // prosthesi sto xrot oti eixe + to y pos
		yrot += (float) diffx; // prosthesi sto yrot oti eixe + to x pos
	}
}

/*
	Reshape function
*/
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;
	float ratio = 1.0* w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

    // Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 1, 1000);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

/*
	some initializations
*/
void init() {

	int randi, randj;

	score = 0; // score 0 arxika
	current_level = 0; // arxizei apo to 0 level
	moves = 0; // den exei kanei kamia kinisi akoma
	hammers = 0; // posa sfiria xrisimopoihse

	readMaz("file.maz");

	/*
		SOIL_load_OGL_texture parameters:
		T*.png: filename
		SOIL_LOAD_AUTO: na fortwsei tin eikona opws einai
		SOIL_CREATE_NEW_ID: na exw ksexwristo id gia kathe texture
		SOIL_FLAG_INVERT_Y: kanonika einai upper left h eikona alla h opengl kanei define apo lower left gia auto thelei invert
	*/
	texture[0] = SOIL_load_OGL_texture (
    	"images/T1.png",
    	SOIL_LOAD_AUTO,
    	SOIL_CREATE_NEW_ID,
    	SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture[1] = SOIL_load_OGL_texture (
    	"images/T2.png",
    	SOIL_LOAD_AUTO,
    	SOIL_CREATE_NEW_ID,
    	SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture[2] = SOIL_load_OGL_texture (
    	"images/T3.png",
    	SOIL_LOAD_AUTO,
    	SOIL_CREATE_NEW_ID,
    	SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// ipologizw tixaia mia thesi gia na arxizei o paikths arkei to tetragwno na einai keno(E) kai na einai to prwto epipedo
	randi = rand() % N;
	randj = rand() % N;

	while (maze[0][randi][randj] != 0) { // ama h tixaia thesh tou paith den einai keno, thelw alla i, j
		randi = rand() % N;
		randj = rand() % N;
	}

	xpos = randj-8.0;
	ypos = 1.0f;
	zpos = randi-8.0;

	eyeX = 30 * cos(angle);
	eyeY = 30.0f;
	eyeZ = 30 * sin(angle);

	//qobj = gluNewQuadric();
	//gluQuadricNormals(qobj, GLU_SMOOTH);

	glClearDepth(1); // clears the depth buffer
	glEnable(GL_DEPTH_TEST); // enable Z-buffer depth test

	// gia to alpha stin glColor4f xriazomai blend func
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// arxikopoihseis gia ton fwtismo
    glEnable (GL_LIGHTING); // energopoioume ton fwtismo
    glEnable (GL_LIGHT0); // enable LIGHT0
    glEnable (GL_LIGHT1); // enable LIGHT1
    glShadeModel (GL_SMOOTH); // set the shader to smooth shader
	// arxikopoihseis gia ta materials
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}

/*
	Main function
*/
int main(int argc, char** argv) {

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); // 3d | omales allages | xrwmata kai alpha
	glutInitWindowPosition(100, 100); // thesi parathirou
	glutInitWindowSize(g_Width, g_Height); // platos kai ipsos
	glutCreateWindow("Maze game"); // onoma parathirou

	init(); // kanw tis arxikopoihseis

	glutDisplayFunc(renderScene); // display function
	glutIdleFunc(renderScene); // idle function
	glutReshapeFunc(changeSize); // reshape function

	glutKeyboardFunc(processNormalKeys); // set the normal keyboard function
	glutPassiveMotionFunc(motion); // set the mouse motion function
	glutMainLoop(); // enter glut event processing cycle

	return 0;
}

