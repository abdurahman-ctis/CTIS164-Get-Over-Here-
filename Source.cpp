/*********
CTIS164 - Template Source Program
----------
STUDENT : Abdurahman Atakishiyev	
SECTION : 02
HOMEWORK: #3
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
	-> Name of the game is derived from famous kunai attack of Scorpion from Mortal Kombat, from which I got inspiration.
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <vector>
using namespace std;

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 750

#define TIMER_PERIOD      16  // Period for the timer of movements and rotations.
#define REAL_TIMER_PERIOD 10 // Period for the actual timer showing time.
#define TIMER_ON          1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

						  //
						  // to draw circle, center at (x,y)
						  // radius r
						  //

/* Stuff for this project */
#define START 0
#define GAME  1
typedef struct {
	float x, y;
}point_t;

typedef struct {
	float angle = 0;
	bool active = false;
	int speed = 0, dir = 1;
}player_t;

typedef struct {
	point_t center;
	float angle;
	int speed, direction;
	bool visible = true;
}enemy_t;

typedef struct {
	char name[50], time[50];
}score_t;

enemy_t e[3];
player_t p;
int state = START,
    hit = 0,
	scoreSize = 0;
int min1 = 0, min2 = 0, sec1 = 0, sec2 = 0, mSec1 = 0, mSec2 = 0;
bool enterName = false;
char name[50] = "";
//vector < pair<char*, char*>> scores;
score_t scores[10];

void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
	glLineWidth(1);
}

void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void drawGradient(int x1, int y1, int w, int h, float r, float g, float b) {
	glBegin(GL_QUADS);
	glColor3ub(r, g, b);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glColor3ub(r + 102, g + 102, b + 102);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();
	glColor3f(0.1, 0.1, 0.1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();
}

void gRec() {
	glBegin(GL_QUADS);
	//red color
	glColor3f(0, 0.0, 0.0);
	glVertex2f(-300,300);
	glVertex2f(-300, -300);
	//blue color
	glColor3f(0.1, 0.1, 0.1);
	glVertex2f(300, -300);
	glVertex2f(300, 300);
	glEnd();
}

void basis() {
	drawGradient(-500, 375, 1000, 750, 95, 39, 205);
	glColor3f(1, 1, 1);
	circle_wire(0, 0, 360);
	circle_wire(0, 0, 290);
	circle_wire(0, 0, 220);
	glBegin(GL_LINES);
	glVertex2f(0, 400);
	glVertex2f(0, -400);
	glVertex2f(500, 0);
	glVertex2f(-500, 0);
	glEnd();
}

//x = x*cos(a) - y*sin(a)
//y = x*sin(a) + y*cos(a)
void player() {
	//Body of kunai
	glBegin(GL_QUADS);
	glColor3ub(234, 32, 39);
	glVertex2f(4 * cos(p.angle*D2R) - 4 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), 4 * sin(p.angle*D2R) + 4 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
	glVertex2f(4 * cos(p.angle*D2R) + 4 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), 4 * sin(p.angle*D2R) - 4 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
	glVertex2f(25 * cos(p.angle*D2R) + 4 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), 25 * sin(p.angle*D2R) - 4 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
	glVertex2f(25 * cos(p.angle*D2R) - 4 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), 25 * sin(p.angle*D2R) + 4 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
	glEnd();

	//Lines around body
	glColor3ub(30, 30, 30);
	glLineWidth(2);
	glBegin(GL_LINES);
	for (int i = 8; i < 25; i += 4) {
		glVertex2f(i * cos(p.angle*D2R) - 4 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), i * sin(p.angle*D2R) + 4 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
		glVertex2f(i * cos(p.angle*D2R) + 4 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), i * sin(p.angle*D2R) - 4 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
	}
	glEnd();
	glLineWidth(1);

	//"Hand" of kunai
	circle(0.000001*cos(p.angle*D2R) + p.speed*cos(p.angle*D2R), 0.000001 *sin(p.angle*D2R) + p.speed*sin(p.angle*D2R), 8);

	//"Head" of kunai
	glColor3ub(30, 30, 30);
	glBegin(GL_POLYGON);
	glVertex2f(25 * cos(p.angle*D2R) - 4 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), 25 * sin(p.angle*D2R) + 4 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
	glVertex2f(35 * cos(p.angle*D2R) - 10 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), 35 * sin(p.angle*D2R) + 10 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
	glVertex2f(60 * cos(p.angle*D2R) - 0.0001 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), 60 * sin(p.angle*D2R) + 0.0001 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
	glVertex2f(35 * cos(p.angle*D2R) + 10 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), 35 * sin(p.angle*D2R) - 10 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
	glVertex2f(25 * cos(p.angle*D2R) + 4 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R), 25 * sin(p.angle*D2R) - 4 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R));
	glEnd();

	//Chains
	if (p.active) {
		glColor3f(0, 0, 0);
		glLineWidth(3);
		int j = 0.0001*cos(p.angle*D2R) + p.speed*cos(p.angle*D2R);
		glBegin(GL_LINES);
		glVertex2f(0.0001*cos(p.angle*D2R) + p.speed*cos(p.angle*D2R), 0.0001 *sin(p.angle*D2R) + p.speed*sin(p.angle*D2R));
		glVertex2f(0, 0);
		glEnd();
		glLineWidth(1);
	}
	circle_wire(0, 0, 8);
	circle(0, 0, 3);

	if (!p.active) {
		//Angle
		glColor3f(1, 1, 1);
		vprint2(380 * cos(p.angle*D2R), 365 * sin(p.angle*D2R), 0.1, "%.0f", p.angle);
	}
}

void enemy(enemy_t e) {
	glColor3f(0, 0, 0);
	circle(e.center.x * cos(e.angle*D2R), e.center.y * sin(e.angle*D2R), 30);
	glColor3ub(255, 227, 159);
	circle(e.center.x * cos(e.angle*D2R), e.center.y * sin(e.angle*D2R), 26);
	glColor3f(1, 0, 0);
	circle(e.center.x * cos(e.angle*D2R), e.center.y * sin(e.angle*D2R), 20);
	glColor3f(0, 0, 0);
	circle(e.center.x * cos(e.angle*D2R), e.center.y * sin(e.angle*D2R), 5);	
	//Angle
	glColor3f(0.9, 0.9, 0.9);
	circle(e.center.x * cos(e.angle*D2R), e.center.y * sin(e.angle*D2R), 15);
	glColor3f(0, 0, 0);
	vprint2(e.center.x * cos(e.angle*D2R) - 9, e.center.y * sin(e.angle*D2R) - 4, 0.08, "%d", (abs)((int)e.angle % 360));
}

bool collision(enemy_t e) { 
	float x = 60 * cos(p.angle*D2R) - 0.0001 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R);
	float y = 60 * sin(p.angle*D2R) + 0.0001 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R);
	float dx = fabs(x - e.center.x* cos(e.angle*D2R));
	float dy = fabs(y - e.center.y* sin(e.angle*D2R));
	float d = sqrtf(dx*dx + dy*dy);
	return dx + dy <= 30;
}

void drawStart() {
	glColor3f(1, 1, 0);
	vprint2(-300, 100, 0.5, "Get Over Here !!!");
	vprint2(-150, -50, 0.25, "Click to start.");
	for (int i = 0; i < scoreSize; i++)
		vprint2(-150, -100 - i*40, 0.25, "%-10s%s", scores[i].name, scores[i].time);
}

void drawGame() {
	basis();
	glColor3f(0, 0, 0);
	glRectf(-500, 400, -400, 330);
	glColor3f(0, 1, 0);
	vprint(-485, 350, GLUT_BITMAP_9_BY_15, "%d%d:%d%d:%d%d", min1, min2, sec1, sec2, mSec1, mSec2);
	for (int i = 0; i < 3; i++)
		if (e[i].visible)
			enemy(e[i]);
	player();
	if (enterName)
	{
		gRec();
		glColor3f(1, 1, 1);
		vprint2(-250, 100, 0.4, "Enter your name : ", name);
		vprint2(-250, 0, 0.4, "<Space> to finish", name);
		glColor3f(1, 1, 1);
		vprint2(-240, -120, 0.5, "%s", name);
	}
}

//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) {
	case START:
		drawStart();
		break;
	case GAME:
		drawGame();
		break;
	}
	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	char c[2] = "";
	c[0] = key;
	if (key == 27)
		exit(0);
	if (enterName && *c >= 'a' && *c <= 'z')
		strcat(name, c);
	if (key == 32 && enterName)
	{
		char time[50];
		score_t temp;
		sprintf(time, "%d%d:%d%d:%d%d", min1, min2, sec1, sec2, mSec1, mSec2);
		strcpy(temp.time, time);
		strcpy(temp.name, name);
		enterName = false;
		scores[scoreSize++] = temp;
		//sort(scores.begin(), scores.end());
		state = START;
		sec2 = sec1 = mSec1 = mSec2 = min1 = min2 = hit = 0;
		for (int i = 0; i < 3; i++) {
			e[i].center.x = 360 - i * 70;
			e[i].center.y = e[i].center.x;
			e[i].angle = rand() % 360;
			e[i].speed = rand() % 3 + 1;
			e[i].direction = rand() % 2 * 2 - 1;
			e[i].visible = true;
		}
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && state == START)
	{
		name[0] = '\0';
		state = GAME;
	}

	else if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && state == GAME && hit != 3)
		p.active = true;


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.
	if (hit == 3)
	{
		p.angle = 0;
		p.active = false;
	}
	else if (!p.active) {
		x -= winWidth / 2;
		y = winHeight / 2 - y;
		p.angle = atan2f(y, x) / D2R;
		if (p.angle < 0)
			p.angle += 360;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void timerFoo(int v) {
	glutTimerFunc(REAL_TIMER_PERIOD, timerFoo, 0);
	if (state == GAME && hit != 3) {
		if (mSec2 == 9) {
			mSec1++;
			mSec2 = 0;

			if (mSec1 == 10) {
				sec2++;
				mSec1 = 0;
			}
		}
		if (sec2 == 10) {
			sec1++;
			sec2 = 0;

			if (sec1 == 6) {
				min2++;
				sec1 = 0;
			}
		}
		mSec2++;
	}
}
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	
	// Drawing enemies
	for (int i = 0; i < 3; i++) {
		e[i].speed = 0;
		e[i].angle += e[i].speed * e[i].direction;
		if (e[i].angle > 360)
			e[i].angle -= 360;
	}

	// Fire mechanics
	if (p.active) {
		p.speed += 15 * p.dir;
		if (fabs(60 * cos(p.angle*D2R) - 0.0001 * sin(p.angle*D2R) + p.speed*cos(p.angle*D2R)) > 500 ||
			fabs(60 * sin(p.angle*D2R) + 0.0001 * cos(p.angle*D2R) + p.speed*sin(p.angle*D2R)) > 500) {
			p.dir = -1;
		}
		if (p.dir == -1 && p.speed == 0) {
			p.active = false;
			p.dir = 1;
		}
	}

	for (int i = 0; i < 3; i++)
		if (collision(e[i]) == 1)
		{
			if(e[i].visible)
				hit++;
			e[i].visible = false;
		}
	if (hit == 3 && p.speed == 0)
		enterName = true;
	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	srand(time(NULL));
	for (int i = 0; i < 3; i++) {
		e[i].center.x = 360 - i * 70;
		e[i].center.y = e[i].center.x;
		e[i].angle = rand() % 360;
		e[i].speed = rand() % 3 + 1;
		e[i].direction = rand() % 2 * 2 - 1;
	}

}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("\"Get Over Here !!!\" by Abdurahman Atakishiyev");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	glutTimerFunc(REAL_TIMER_PERIOD, timerFoo, 0);
#endif

	Init();

	glutMainLoop();
}
