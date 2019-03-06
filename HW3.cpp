/*********
CTIS164 - Template Source Program
----------
STUDENT : M. Kasým Varol
SECTION :1
HOMEWORK:3
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
It calculates last point which is calculated like below
averagepoint=(speeds of hitted enemies)/wasting time
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  700
#define WINDOW_HEIGHT 700

#define TIMER_PERIOD  25 // Period for the timer.

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, 
startgame=false; // whether game starts or not
int  winWidth, winHeight; // current Window width and height

//coordinate structure
typedef struct {
	float x, y;
} coordinate_t;

typedef struct {
	coordinate_t pos;    // position of the object
	float   angle;  // view angle 
} shooter_t;

typedef struct {
	coordinate_t pos; //position of fire
	float angle; //fire angle
	bool active; // is firing or not
} fire_t;

typedef struct {
	float r, g, b; // colour variables
} color_t;

typedef struct {
	coordinate_t pos; // position of enemies
	color_t color; //enemy random colours
	float angle; // enemy's degree on a circle
	float speed; // enemy's pace
	bool hit = false; // initially enemies are not shooted
	float radius; // enemy size
} enemy_t;

// GLOBAL VARIABLES
shooter_t p = { { 0,0 }, 0};
enemy_t planets[3];
coordinate_t mousecoordinate;
int degree;
fire_t   fire = { { 30, 0 }, 0, false };
int enemyangle[3];
int counter = 0;// counts time
double averagepoints = 0; // points will calculated as (total speed of shot enemies)/Wasting Time
double collectpoints = 0;//collect speed of shot enemies
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

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
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

//It converts mouse's coordinates into a degree

int finddegree()
{
	float angle = atan2(mousecoordinate.y, mousecoordinate.x) * 180 / PI;
	if (angle < 0)
		angle += 360;
	p.angle = angle;
	return angle;
}
// it finds enemies angle
void enemiesangle()
{
	for (int i = 0; i < 3; i++) {
		enemyangle[i] = atan2(planets[i].pos.y, planets[i].pos.x) * 180 / PI;
		if (enemyangle[i] < 0)
			enemyangle[i] += 360;
	}
}

//it is rotating shape function
void vertex(coordinate_t P, coordinate_t Tr, double angle) {
	double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
	double yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
	glVertex2d(xp, yp);
}

//drawing player while rotating
void drawPlayer(shooter_t shooter) {
	shooter.angle = finddegree();
	glColor3f(1, 1, 0);
	glLineWidth(3);
	
	glBegin(GL_TRIANGLES);
	vertex({ shooter.pos.x - 30 , shooter.pos.y+15 }, { shooter.pos }, shooter.angle*D2R);
	vertex({ shooter.pos.x -30 , shooter.pos.y -15 }, { shooter.pos }, shooter.angle*D2R);
	vertex({ shooter.pos.x +30 , shooter.pos.y }, { shooter.pos }, shooter.angle*D2R);
	glEnd();
	glLineWidth(1);
	
}
//draw fire
void drawFire(fire_t bullet) {
	if (bullet.active) {
		glColor3f(1, 1, 1);
		circle(bullet.pos.x, bullet.pos.y, 6);
	}
}
//drawing XY Coordinates
void drawXY() {
	
	glColor3f(0, 0.9, 0.9);
	glBegin(GL_LINES);
	// x axis
	glVertex2f(-350, 0);
	glVertex2f(350, 0);

	// y axis
	glVertex2f(0, -350);
	glVertex2f(0, 350);
	glEnd();
}

void display() {
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	drawXY();
	circle_wire(0, 0, 200);
	circle_wire(0, 0, 250);
	circle_wire(0, 0, 300);
	// Drawing 3 enemies
	for (int i = 0; i < 3; i++)
		if (!planets[i].hit) 
		{
			glColor3f(planets[i].color.r, planets[i].color.g, planets[i].color.b);
			circle(planets[i].pos.x, planets[i].pos.y, planets[i].radius);
			glColor3b(1, 1, 1);
			enemiesangle();
			vprint(planets[i].pos.x-10, planets[i].pos.y, GLUT_BITMAP_HELVETICA_12, "%d", enemyangle[i]);
		}
	//information
	glColor3f(1, 1, 1);
	vprint(-335, 335, GLUT_BITMAP_HELVETICA_12, "Homework III");
	vprint(-335, 320, GLUT_BITMAP_HELVETICA_12, "By M. KASIM VAROL");
	vprint(320* cos(p.angle * D2R)-15, 320 * sin(p.angle * D2R)-6, GLUT_BITMAP_HELVETICA_12, "%d", finddegree());
	//draw player
	drawPlayer(p);
	//draw fire
	drawFire(fire);
	if (!startgame) {
		glColor3f(0.1, 0.1, 0.1);
		glRectf(-150, -30, 150, 20);
		glColor3f(1, 0, 0);
		vprint(-130, 0, GLUT_BITMAP_8_BY_13, "Click Left Button to Start Game");
		glColor3f(1, 1, 0);
		vprint(-115, -15, GLUT_BITMAP_8_BY_13, "Your Last Point is = %.1f", averagepoints*100);
		
	}
	vprint(290, 320, GLUT_BITMAP_HELVETICA_18, "TIME");
	vprint(300, 300, GLUT_BITMAP_HELVETICA_18, "%d", counter/40);
	vprint(200, 280, GLUT_BITMAP_HELVETICA_18, "Points Collected");
	vprint(300, 260, GLUT_BITMAP_HELVETICA_18, "%.1f", collectpoints);
	glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onClick(int button, int stat, int x, int y)
{
	// if it clicks left button
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
	{
		if (!startgame) 
		{
			//Assign ALL random information for enemies
			for (int i = 0; i < 3; i++)
			{
				planets[i].angle = rand() % 360;
				planets[i].speed = (rand() % 50 - 20)/10.0;
				while (planets[i].speed == 0)
					planets[i].speed = (rand() % 50 - 25) / 10.0;
				 if (planets[i].speed > 0 && planets[i].speed < 1)
					planets[i].speed++;
				else if (planets[i].speed < 0 && planets[i].speed > -1)
					planets[i].speed--;
				planets[i].radius = rand() % 20 + 15;
				planets[i].hit = false;
				planets[i].color.b = rand() % 100 / 100.0;
				planets[i].color.r = rand() % 100 / 100.0;
				planets[i].color.g= rand() % 100 / 100.0;
				planets[i].pos.x = planets[i].pos.y = 0;
			}
			startgame = true; // let the game start
		}
		else if(!fire.active)
		{
			fire.pos.x = 0;
			fire.pos.y = 0;
			fire.angle = p.angle;
			fire.active = true;
		}

	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

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

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// catch mouse's coordinate
	mousecoordinate.x = x - winWidth / 2;
	mousecoordinate.y = winHeight / 2 - y;


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// when the game starts
	if (startgame)
	{
		counter++;
		for (int i = 0; i < 3; i++)
		{
			//Enemies are orbitting unless being shot
			if (!planets[i].hit)
			{
				planets[i].angle += planets[i].speed;
				planets[i].pos.x = (200 + i * 50)*cos(planets[i].angle*D2R);
				planets[i].pos.y = (200 + i * 50)*sin(planets[i].angle*D2R);
			}
		}
		if (fire.active) 
		{
			//move fire unless shooter hits the enemies
			for (int i = 0; i < 3; i++)
			{
				float distance = sqrt((fire.pos.x - planets[i].pos.x)*(fire.pos.x - planets[i].pos.x) + (fire.pos.y - planets[i].pos.y)*(fire.pos.y - planets[i].pos.y));
				if (distance <= planets[i].radius) {
					collectpoints += (fabs(planets[i].speed)/(200+i*50))*250;
					planets[i].hit = true;
				}
			}
			fire.pos.x += 10 * cos(fire.angle * D2R);
			fire.pos.y += 10 * sin(fire.angle * D2R);

			if (fire.pos.x > 370 || fire.pos.x < -370 || fire.pos.y>370 || fire.pos.y < -370) {
				fire.active = false;
			}
		}
		if (planets[0].hit == true && planets[1].hit == true && planets[2].hit == true) {
			//reset variables
			startgame = false;
			fire.active = false;
			averagepoints = collectpoints / (counter / 40);
			counter = 0;
			collectpoints = 0;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char *argv[]) {
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("HW3: Hitting Targets Orbitting | M.KASIM VAROL");
	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutPassiveMotionFunc(onMove);


	Init();

	glutMainLoop();
}