#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif



#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#define XSIDE	10.0		// length of the x side of the grid
#define X0      (-XSIDE/2.0)		// where one side starts
#define NX	1000		// how many points in x
#define DX	( XSIDE/(float)NX )	// change in x between the points

#define YGRID	0.f

#define ZSIDE	14.0		// length of the z side of the grid
#define Z0      (-ZSIDE/2.0)		// where one side starts
#define NZ	1000			// how many points in z
#define DZ	( ZSIDE/(float)NZ )	// change in z between the points

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "horse.h"

int	SphereDL, MarsDL, VenusDL, EarthDL, JupiterDL, SaturnDL, UranusDL, NeptuneDL;	// display lists
GLuint	MarsTex, VenusTex, EarthTex, JupiterTex, SaturnTex, UranusTex, NeptuneTex;	// texture object
int	NowPlanet = 1; // to show the current planet
int LightAndViewDistanceAccordingToPlanet = 5; //adjusting light and view distance based on this key


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// title of these windows:

const char* WINDOWTITLE = "OpenGL / GLUT Sample -- Manoj Graphics Project 5";
const char* GLUITITLE = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT = 4;
const int MIDDLE = 2;
const int RIGHT = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = { 0, 0,0, 1. };

// line width for the axes:

const GLfloat AXES_WIDTH = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char* ColorNames[] =
{
	(char*)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 7.f;

// for lighting:

const float	WHITE[] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 10000;		// 10000 milliseconds = 10 seconds

const int MSEC = 10000;


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
GLuint	BoxList;				// object display list
GLuint	GridDL;				// object display list
GLuint SphereList;
GLuint	CarDL;				// object display list
GLuint	LightSource;				// object display list
GLuint	CatObj;				// object display list
GLuint	DogObj;				// object display list
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
float	LightSourceColor[3] = { 1.,1.,1. };
bool  PointLight = true;
int		NowProjection;		// ORTHO or PERSP
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
GLuint WireHorseList;			// list to hold wire horse
GLuint PolygonHorseList;		// list to hold polygon horse  
int NumberOfHorses = 4;			// to define number of horses	
int InView = 0;					// to get in view
bool Frozen;

bool EnableTextureMode = true;
bool EnableLightingMode = true;



// function prototypes:

void	Animate();
void	Display();
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);

void			Axes(float);
void			HsvRgb(float[3], float[3]);
void			Cross(float[3], float[3], float[3]);
float			Dot(float[3], float[3]);
float			Unit(float[3], float[3]);
float			Unit(float[3]);


struct planet
{
	char* name;
	char* file;
	float                   scale;
	int                     displayList;
	char                    key;
	unsigned int            texObject;
};

struct planet Planets[] =
{
		{ "Venus",      "venus.bmp",     0.95f, VenusDL, 'v', VenusTex},
		{ "Earth",      "earth.bmp",     1.00f, EarthDL, 'e', EarthTex },
		{ "Mars",       "mars.bmp",      0.53f, MarsDL, 'm', MarsTex },
		{ "Jupiter",    "jupiter.bmp",  11.21f, JupiterDL, 'j', JupiterTex },
		{ "Saturn",     "saturn.bmp",    9.45f, SaturnDL, 's', SaturnTex },
		{ "Uranus",     "uranus.bmp",    4.01f, UranusDL, 'u', UranusTex },
		{ "Neptune",    "neptune.bmp",   3.88f, NeptuneDL, 'n', NeptuneTex },
};

const int NUMPLANETS = sizeof(Planets) / sizeof(struct planet);

// utility to create an array from 3 separate values:

float*
Array3(float a, float b, float c)
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float*
MulArray3(float factor, float array0[])
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float*
MulArray3(float factor, float a, float b, float c)
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

#include "setmaterial.cpp"
#include "setlight.cpp"
#include "osusphere.cpp"
#include "osucone.cpp"
#include "osutorus.cpp"
#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
#include "glslprogram.cpp"

Keytimes BallRotationInX;
Keytimes ColorKeyTimeRed;
Keytimes ColorKeyTimeGreen;
Keytimes ColorKeyTimeBlue;
Keytimes LightSourceKeyTimeRed;

Keytimes LightSourceKeyTimeGreen;
Keytimes LightSourceKeyTimeBlue;
Keytimes LightSourceUpAndDownX;
Keytimes LightSourceUpAndDownY;
Keytimes LightSourceUpAndDownZ;

Keytimes DogMotionRotation;
Keytimes DogMotionTranslation;
Keytimes CatMotionRotation;
Keytimes CatMotionTranslation;
Keytimes BallScaleX;

Keytimes EyePositionChangeInY;
Keytimes SetMaterialDogR;
Keytimes SetMaterialDogG;
Keytimes SetMaterialDogB;
Keytimes SetMaterialCatR;

Keytimes SetMaterialCatG;
Keytimes SetMaterialCatB;
Keytimes EyePositionCircle;
//Keytimes BallRotationInX;
//Keytimes BallRotationInX;
// main program:

int
main(int argc, char* argv[])
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit(&argc, argv);

	// setup all the graphics stuff:

	InitGraphics();

	// create the display lists that **will not change**:

	InitLists();

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset();

	// setup all the user interface stuff:

	InitMenus();

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow(MainWindow);
	glutMainLoop();

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate()
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// draw the complete scene:

void
Display()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow(MainWindow);

	// erase the background:
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
#ifdef DEMO_DEPTH_BUFFER
	if (DepthBufferOn == 0)
		glDisable(GL_DEPTH_TEST);
#endif


	// specify shading to be flat:

	glShadeModel(GL_FLAT);

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	int msec = glutGet(GLUT_ELAPSED_TIME) % MSEC;

	// turn that into a time in seconds:
	float nowTime = (float)msec / 1000.;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (NowProjection == ORTHO)
		glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
	else
		gluPerspective(70.f, 1.f, 0.1f, 1000.f);

	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*float radius = 7.0f;
	float angle = (EyePositionCircle.GetValue(nowTime) * 3.14 / 180.0);
	float eyeX = radius * sin(angle);
	float eyeZ = radius * cos(angle);*/
	//gluLookAt(eyeX, EyePositionChangeInY.GetValue(nowTime), eyeZ, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
	gluLookAt(0, 0, LightAndViewDistanceAccordingToPlanet - 2, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

	glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
	glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);


	//// rotate the scene:

	//glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
	//glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);

	// uniformly scale the scene:

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	// set the fog parameters:

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}


	/*if (AxesOn != 0)
	{
		glColor3fv(&Colors[NowColor][0]);
		glCallList(AxesList);
	}*/


	
	// since we are using glScalef( ), be sure the normals get unitized:


	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	/*glPushMatrix();
	glRotatef(-40 * BallRotationInX.GetValue(nowTime), 0, 0, 1);
	glTranslatef(-2.4, 1.4, 0);
	SetMaterial(ColorKeyTimeRed.GetValue(nowTime), ColorKeyTimeGreen.GetValue(nowTime), ColorKeyTimeBlue.GetValue(nowTime), 100.f);
	glScalef(1., 1 * BallScaleX.GetValue(nowTime), 1.);
	glCallList(SphereList);
	glPopMatrix();*/

	//glPushMatrix();
	////glTranslatef(-0.1, 1.8, 0.);
	//glTranslatef(3.1, DogMotionTranslation.GetValue(nowTime), 0);
	//glRotatef(-(DogMotionRotation.GetValue(nowTime)), 0, 0, 1);
	//glTranslatef(-3.1, 0, 0);
	//glPushMatrix();
	//SetMaterial(SetMaterialDogR.GetValue(nowTime), SetMaterialDogG.GetValue(nowTime), SetMaterialDogB.GetValue(nowTime), 100.f);
	//glCallList(DogObj);
	//glPopMatrix();
	//glPopMatrix();


	/*glPushMatrix();
	glTranslatef(-2.5, CatMotionTranslation.GetValue(nowTime), 0);
	glRotatef((CatMotionRotation.GetValue(nowTime)), 0, 0, 1);
	glTranslatef(3.1, 0, 0);
	glColor3f(1, 0, 0);
	SetMaterial(SetMaterialCatR.GetValue(nowTime), SetMaterialCatG.GetValue(nowTime), SetMaterialCatB.GetValue(nowTime), 100.f);
	glCallList(CatObj);
	glPopMatrix();*/


	//SetPointLight(GL_LIGHT0, 1., 2., 0., 1., 0., 0.);
	/*if (PointLight) {
		SetPointLight(GL_LIGHT0, LightSourceUpAndDownX.GetValue(nowTime), LightSourceUpAndDownY.GetValue(nowTime), LightSourceUpAndDownZ.GetValue(nowTime), LightSourceKeyTimeRed.GetValue(nowTime), LightSourceKeyTimeGreen.GetValue(nowTime), LightSourceKeyTimeBlue.GetValue(nowTime));
	}
	else {
		SetSpotLight(GL_LIGHT0, LightSourceUpAndDownX.GetValue(nowTime), LightSourceUpAndDownY.GetValue(nowTime), LightSourceUpAndDownZ.GetValue(nowTime), 0., -1, 0., LightSourceKeyTimeRed.GetValue(nowTime), LightSourceKeyTimeGreen.GetValue(nowTime), LightSourceKeyTimeBlue.GetValue(nowTime));
	}*/
	glPushMatrix();
	glRotatef(360.f * Time, 0., 1., 0.);
	if (PointLight) {
		SetPointLight(GL_LIGHT0, 0., 0., LightAndViewDistanceAccordingToPlanet, 1., 1., 1.);
	}
	else {
		SetSpotLight(GL_LIGHT0, 0., 0., LightAndViewDistanceAccordingToPlanet, 0., 0., -LightAndViewDistanceAccordingToPlanet, 1., 1., 1.);
	}
	glTranslatef(0., 0., LightAndViewDistanceAccordingToPlanet);
	glColor3f(1., 1., 1.);
	glCallList(LightSource);
	glPopMatrix();


	if (EnableTextureMode)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	
	if (EnableLightingMode)
	{
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	else
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}

	
	glCallList(Planets[NowPlanet].displayList);


	glDisable(GL_TEXTURE_2D);

#ifdef DEMO_Z_FIGHTING
	if (DepthFightingOn != 0)
	{
		glPushMatrix();
		glRotatef(90.f, 0.f, 1.f, 0.f);
		glCallList(BoxList);
		glPopMatrix();
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex
	glDisable(GL_LIGHTING);
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.f, 100.f, 0.f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.f, 1.f, 1.f);
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers();

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();
}


void
DoAxesMenu(int id)
{
	AxesOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoColorMenu(int id)
{
	NowColor = id - RED;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDebugMenu(int id)
{
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoInsideView(int id) {
	InView = id;
	glutSetWindow(MainWindow);
	InitMenus();
	glutPostRedisplay();

}


void
DoDepthBufferMenu(int id)
{
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthFightingMenu(int id)
{
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthMenu(int id)
{
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// main menu callback:

void
DoMainMenu(int id)
{
	switch (id)
	{
	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
	NowProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:

void
DoRasterString(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString(float x, float y, float z, float ht, char* s)
{
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(float));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int insideviewmenu = glutCreateMenu(DoInsideView);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int projmenu = glutCreateMenu(DoProjectMenu);
	if (InView == 1) {
		glutAddMenuEntry("Perspective", PERSP);
	}
	else {
		glutAddMenuEntry("Orthographic", ORTHO);
		glutAddMenuEntry("Perspective", PERSP);
	}

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Axes", axesmenu);
	glutAddSubMenu("Axis Colors", colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu("Depth Buffer", depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu("Depth Fighting", depthfightingmenu);
#endif

	glutAddSubMenu("Depth Cue", depthcuemenu);
	glutAddSubMenu("Inside View", insideviewmenu);
	glutAddMenuEntry("Reset", RESET);
	glutAddSubMenu("Debug", debugmenu);
	glutAddMenuEntry("Quit", QUIT);
	glutAddSubMenu("Projection", projmenu);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);


	//For Rotating in Semi Circle
	BallRotationInX.Init();
	BallRotationInX.AddTimeValue(0.0, 0.000);
	BallRotationInX.AddTimeValue(1.0, 0.75);
	BallRotationInX.AddTimeValue(2.0, 1.25);
	BallRotationInX.AddTimeValue(4.0, 2.5);
	BallRotationInX.AddTimeValue(5.0, 3.0);
	BallRotationInX.AddTimeValue(6.0, 2.5);
	BallRotationInX.AddTimeValue(8.0, 1.5);
	BallRotationInX.AddTimeValue(9.0, 1.);
	BallRotationInX.AddTimeValue(10.0, 0.000);

	//For Sphere Material Color
	ColorKeyTimeRed.Init();
	ColorKeyTimeRed.AddTimeValue(0.0, 1.0);
	ColorKeyTimeRed.AddTimeValue(1.0, 1.0);
	ColorKeyTimeRed.AddTimeValue(4.9, 1.0);
	ColorKeyTimeRed.AddTimeValue(5.0, 0.0);
	ColorKeyTimeRed.AddTimeValue(8.0, 1.0);
	ColorKeyTimeRed.AddTimeValue(9.9, 0.0);
	ColorKeyTimeRed.AddTimeValue(10.0, 1.0);

	ColorKeyTimeGreen.Init();
	ColorKeyTimeGreen.AddTimeValue(0.0, 1.0);
	ColorKeyTimeGreen.AddTimeValue(1.0, 0.0);
	ColorKeyTimeGreen.AddTimeValue(4.9, 1.0);
	ColorKeyTimeGreen.AddTimeValue(5.0, 1.0);
	ColorKeyTimeGreen.AddTimeValue(8.0, 1.0);
	ColorKeyTimeGreen.AddTimeValue(9.9, 1.0);
	ColorKeyTimeGreen.AddTimeValue(10.0, 1.0);

	ColorKeyTimeBlue.Init();
	ColorKeyTimeBlue.AddTimeValue(0.0, 1.0);
	ColorKeyTimeBlue.AddTimeValue(1.0, 0.5);
	ColorKeyTimeBlue.AddTimeValue(4.9, 1.0);
	ColorKeyTimeBlue.AddTimeValue(5.0, 0.0);
	ColorKeyTimeBlue.AddTimeValue(8.0, 1.0);
	ColorKeyTimeBlue.AddTimeValue(9.9, 0.0);
	ColorKeyTimeBlue.AddTimeValue(10.0, 1.0);

	//For Light Source Color
	LightSourceKeyTimeRed.Init();
	LightSourceKeyTimeRed.AddTimeValue(0.0, 1.0);
	LightSourceKeyTimeRed.AddTimeValue(1.0, 1.0);
	LightSourceKeyTimeRed.AddTimeValue(2.0, 0.0);
	LightSourceKeyTimeRed.AddTimeValue(5.0, 0.0);
	LightSourceKeyTimeRed.AddTimeValue(8.0, 1.0);
	LightSourceKeyTimeRed.AddTimeValue(10.0, 1.0);

	LightSourceKeyTimeGreen.Init();
	LightSourceKeyTimeGreen.AddTimeValue(0.0, 1.0);
	LightSourceKeyTimeGreen.AddTimeValue(1.0, 0.0);
	LightSourceKeyTimeGreen.AddTimeValue(2.0, 1.0);
	LightSourceKeyTimeGreen.AddTimeValue(5.0, 0.0);
	LightSourceKeyTimeGreen.AddTimeValue(8.0, 1.0);
	LightSourceKeyTimeGreen.AddTimeValue(10.0, 1.0);

	LightSourceKeyTimeBlue.Init();
	LightSourceKeyTimeBlue.AddTimeValue(0.0, 1.0);
	LightSourceKeyTimeBlue.AddTimeValue(1.0, 0.0);
	LightSourceKeyTimeBlue.AddTimeValue(2.0, 0.0);
	LightSourceKeyTimeBlue.AddTimeValue(5.0, 1.0);
	LightSourceKeyTimeBlue.AddTimeValue(8.0, 0.0);
	LightSourceKeyTimeBlue.AddTimeValue(10.0, 1.0);

	//For Light Source Motion - Up and Down
	LightSourceUpAndDownX.Init();
	LightSourceUpAndDownX.AddTimeValue(0.0, -2.0);
	LightSourceUpAndDownX.AddTimeValue(2.0, 2.);
	LightSourceUpAndDownX.AddTimeValue(4.0, 2.);
	LightSourceUpAndDownX.AddTimeValue(6.0, -2.);
	LightSourceUpAndDownX.AddTimeValue(8.0, -2.);
	LightSourceUpAndDownX.AddTimeValue(10.0, -2.0);

	LightSourceUpAndDownY.Init();
	LightSourceUpAndDownY.AddTimeValue(0.0, 3.0);
	LightSourceUpAndDownY.AddTimeValue(2.0, 3.);
	LightSourceUpAndDownY.AddTimeValue(4.0, 3.);
	LightSourceUpAndDownY.AddTimeValue(6.0, 3.);
	LightSourceUpAndDownY.AddTimeValue(8.0, 3.);
	LightSourceUpAndDownY.AddTimeValue(10.0, 3.0);

	LightSourceUpAndDownZ.Init();
	LightSourceUpAndDownZ.AddTimeValue(0.0, 2.0);
	LightSourceUpAndDownZ.AddTimeValue(2.0, 2.);
	LightSourceUpAndDownZ.AddTimeValue(4.0, -2.);
	LightSourceUpAndDownZ.AddTimeValue(6.0, -2.);
	LightSourceUpAndDownZ.AddTimeValue(8.0, 2.);
	LightSourceUpAndDownZ.AddTimeValue(10.0, 2.0);

	//For Dog Rotation and Translation
	DogMotionRotation.Init();
	DogMotionRotation.AddTimeValue(0.0, 0.0);
	DogMotionRotation.AddTimeValue(1.0, 0.);
	DogMotionRotation.AddTimeValue(2.0, 0.);
	DogMotionRotation.AddTimeValue(4., 30.);
	DogMotionRotation.AddTimeValue(5.0, 0.);
	DogMotionRotation.AddTimeValue(8.0, 0.);
	DogMotionRotation.AddTimeValue(10.0, 0.0);


	DogMotionTranslation.Init();
	DogMotionTranslation.AddTimeValue(0.0, 0.0);
	DogMotionTranslation.AddTimeValue(1.0, 0.00);
	DogMotionTranslation.AddTimeValue(2.0, 0.00);
	DogMotionTranslation.AddTimeValue(5.0, 0.2);
	DogMotionTranslation.AddTimeValue(7.0, 0.00);
	DogMotionTranslation.AddTimeValue(8.0, 0.00);
	DogMotionTranslation.AddTimeValue(10.0, 0.0);

	//For Cat Rotation and Translation
	CatMotionRotation.Init();
	CatMotionRotation.AddTimeValue(0.0, 30.0);
	CatMotionRotation.AddTimeValue(1.0, 0.);
	CatMotionRotation.AddTimeValue(2.0, 0.);
	CatMotionRotation.AddTimeValue(4., 0);
	CatMotionRotation.AddTimeValue(5.0, 0.);
	CatMotionRotation.AddTimeValue(8.0, 0.);
	CatMotionRotation.AddTimeValue(10.0, 30.0);


	CatMotionTranslation.Init();
	CatMotionTranslation.AddTimeValue(0.0, 0.2);
	CatMotionTranslation.AddTimeValue(1.0, 0.10);
	CatMotionTranslation.AddTimeValue(2.0, 0.00);
	CatMotionTranslation.AddTimeValue(5.0, 0.0);
	CatMotionTranslation.AddTimeValue(7.0, 0.00);
	CatMotionTranslation.AddTimeValue(8.0, 0.10);
	CatMotionTranslation.AddTimeValue(10.0, 0.2);

	//For Scaling of the ball
	BallScaleX.Init();
	BallScaleX.AddTimeValue(0.0, 0.8);
	BallScaleX.AddTimeValue(0.5, 0.9);
	BallScaleX.AddTimeValue(1.0, 1.0);
	BallScaleX.AddTimeValue(4.9, 1.0);
	BallScaleX.AddTimeValue(5.0, 0.8);
	BallScaleX.AddTimeValue(5.5, 0.9);
	BallScaleX.AddTimeValue(6.0, 1.0);
	BallScaleX.AddTimeValue(9.9, 1.0);
	BallScaleX.AddTimeValue(10.0, 0.8);

	//Eye Position for Look at to move up and down
	EyePositionChangeInY.Init();
	EyePositionChangeInY.AddTimeValue(0.0, 3.0);
	EyePositionChangeInY.AddTimeValue(2., 2.5);
	EyePositionChangeInY.AddTimeValue(3.5, 2.0);
	EyePositionChangeInY.AddTimeValue(5.0, 1.0);
	EyePositionChangeInY.AddTimeValue(7.5, 2.0);
	EyePositionChangeInY.AddTimeValue(8., 2.5);
	EyePositionChangeInY.AddTimeValue(10.0, 3.0);

	//Dog Material Color
	SetMaterialDogR.Init();
	SetMaterialDogR.AddTimeValue(0.0, 1.00);
	SetMaterialDogR.AddTimeValue(2.0, 1.0);
	SetMaterialDogR.AddTimeValue(5.0, 0.00);
	SetMaterialDogR.AddTimeValue(7.0, 0.00);
	SetMaterialDogR.AddTimeValue(9.0, 1.0);
	SetMaterialDogR.AddTimeValue(10.0, 1.0);

	SetMaterialDogG.Init();
	SetMaterialDogG.AddTimeValue(0.0, 1.0);
	SetMaterialDogG.AddTimeValue(2.0, 0.0);
	SetMaterialDogG.AddTimeValue(5.0, 1.0);
	SetMaterialDogG.AddTimeValue(7.0, 0.0);
	SetMaterialDogG.AddTimeValue(9.0, 1.0);
	SetMaterialDogG.AddTimeValue(10.0, 0.0);

	SetMaterialDogB.Init();
	SetMaterialDogB.AddTimeValue(0.0, 1.0);
	SetMaterialDogB.AddTimeValue(2.0, 0.0);
	SetMaterialDogB.AddTimeValue(5.0, 0.0);
	SetMaterialDogB.AddTimeValue(7.0, 1.0);
	SetMaterialDogB.AddTimeValue(9.0, 0.0);
	SetMaterialDogB.AddTimeValue(10.0, 1.0);

	//Cat Material Color
	SetMaterialCatR.Init();
	SetMaterialCatR.AddTimeValue(0.0, 1.0);
	SetMaterialCatR.AddTimeValue(2.0, 0.5);
	SetMaterialCatR.AddTimeValue(5.0, 1.0);
	SetMaterialCatR.AddTimeValue(7.0, 0.6);
	SetMaterialCatR.AddTimeValue(9.0, 0.6);
	SetMaterialCatR.AddTimeValue(10.0, 1.0);

	SetMaterialCatG.Init();
	SetMaterialCatG.AddTimeValue(0.0, 0.5);
	SetMaterialCatG.AddTimeValue(2.0, 0.5);
	SetMaterialCatG.AddTimeValue(5.0, 0.5);
	SetMaterialCatG.AddTimeValue(7.0, 0.4);
	SetMaterialCatG.AddTimeValue(9.0, 0.4);
	SetMaterialCatG.AddTimeValue(10.0, 1.0);

	SetMaterialCatB.Init();
	SetMaterialCatB.AddTimeValue(0.0, 0.0);
	SetMaterialCatB.AddTimeValue(2.0, 1.0);
	SetMaterialCatB.AddTimeValue(5.0, 0.5);
	SetMaterialCatB.AddTimeValue(7.0, 0.2);
	SetMaterialCatB.AddTimeValue(9.0, 0.2);
	SetMaterialCatB.AddTimeValue(10.0, 1.0);

	//Look at motion in circle
	EyePositionCircle.Init();
	EyePositionCircle.AddTimeValue(0.0, 0.0);
	EyePositionCircle.AddTimeValue(1.0, 36.0);
	EyePositionCircle.AddTimeValue(2.0, 72.0);
	EyePositionCircle.AddTimeValue(3.0, 108.0);
	EyePositionCircle.AddTimeValue(4.0, 144.0);
	EyePositionCircle.AddTimeValue(5.0, 180.0);
	EyePositionCircle.AddTimeValue(6.0, 216.0);
	EyePositionCircle.AddTimeValue(7.0, 252.0);
	EyePositionCircle.AddTimeValue(8.0, 288.0);
	EyePositionCircle.AddTimeValue(9.0, 324.0);
	EyePositionCircle.AddTimeValue(10.0, 360.0);

	for (int i = 0; i < NUMPLANETS; i++)
	{
		int width, height;
		char* file = (char*)Planets[i].file;
		unsigned char* texture = BmpToTexture(file, &width, &height);
		if (texture == NULL)
			fprintf(stderr, "Cannot open texture '%s'\n", file);
		else
			fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", file, width, height);

		glGenTextures(1, &Planets[i].texObject);
		glBindTexture(GL_TEXTURE_2D, Planets[i].texObject);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	}

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc(Animate);

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )



void
InitLists()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow(MainWindow);

	BoxList = glGenLists(1);
	glNewList(BoxList, GL_COMPILE);
	glPushMatrix();
	SetMaterial(0.6, 0.6, 0.6, 100);
	glNormal3f(0., 1., 0.);
	for (int i = 0; i < NZ; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < NX; j++)
		{
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 0));
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 1));
		}
		glEnd();
	}
	glPopMatrix();
	glEndList();


	SphereList = glGenLists(1);
	glNewList(SphereList, GL_COMPILE);
	glPushMatrix();
	OsuSphere(0.3, 100, 100);
	glPopMatrix();
	glEndList();

	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(1.5);
	glLineWidth(1.);
	glEndList();

	LightSource = glGenLists(1);
	glNewList(LightSource, GL_COMPILE);
	glPushMatrix();
	OsuSphere(0.05, 100, 100);
	glPopMatrix();
	glEndList();

	CatObj = glGenLists(1);
	glNewList(CatObj, GL_COMPILE);
	glPushMatrix();
	glTranslatef(-3., 0.0, 0.);
	glRotatef(180, 0, 1, 0);
	glColor3f(0.98, 0.227, 0.0);
	glScalef(0.25, 0.25, 0.25);
	LoadObjFile((char*)"cat.obj");
	glPopMatrix();
	glEndList();

	DogObj = glGenLists(1);
	glNewList(DogObj, GL_COMPILE);
	glPushMatrix();

	glTranslatef(3.1, 0.0, 0.);
	glRotatef(270, 0, 1, 0);
	//glColor3f(0.969, 0.776, 0.42);
	glScalef(0.4, 0.4, 0.4);
	LoadObjFile((char*)"dog.obj");
	glPopMatrix();
	glEndList();

	SphereDL = glGenLists(1);
	glNewList(SphereDL, GL_COMPILE);
	OsuSphere(1., 100 , 100 );
	glEndList();

	for (int i = 0; i < NUMPLANETS; i++)
	{
		Planets[i].displayList = glGenLists(1);
		glNewList(Planets[i].displayList, GL_COMPILE);
		
		glBindTexture(GL_TEXTURE_2D, Planets[i].texObject);
		glPushMatrix();
		SetMaterial(1., 1., 1., 100.f);
		glScalef(Planets[i].scale, Planets[i].scale, Planets[i].scale);	// scale of mars sphere, from the table
		glCallList(SphereDL);		// a dl can call another dl that has been previously created
		glPopMatrix();
		glEndList();
	}

	//PolygonHorse();
}


// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'o':
	case 'O':
		NowProjection = ORTHO;
		break;

	case 'p':
	case 'P':
		PointLight = !PointLight;
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler
	case 'f':
	case 'F':
		Frozen = !Frozen;
		if (Frozen)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(Animate);
		break;
	case 'w':
	case 'W':
		LightSourceColor[0] = 1., LightSourceColor[1] = 1., LightSourceColor[2] = 1.;

		break;
	case 'r':
	case 'R':
		LightSourceColor[0] = 1., LightSourceColor[1] = 0., LightSourceColor[2] = 0.;
		break;
	case 'g':
	case 'G':
		LightSourceColor[0] = 0., LightSourceColor[1] = 1., LightSourceColor[2] = 0.;
		break;
	case 'b':
	case 'B':
		LightSourceColor[0] = 0., LightSourceColor[1] = 0., LightSourceColor[2] = 1.;
		break;
	case 'y':
	case 'Y':
		LightSourceColor[0] = 1., LightSourceColor[1] = 1., LightSourceColor[2] = 0.;
		break;
	case 't':
	case 'T':
		EnableTextureMode = !EnableTextureMode;
		break;
	case 'l':
	case 'L':
		EnableLightingMode = !EnableLightingMode;
		break;
	case 'v':
	case  'V' :
			NowPlanet = 0;
			LightAndViewDistanceAccordingToPlanet = 5;
		break;
	case 'e':
	case 	'E' :
			NowPlanet = 1;
			LightAndViewDistanceAccordingToPlanet = 5;
		break;
	case 'm':
	case 	'M' :
			NowPlanet = 2;
			LightAndViewDistanceAccordingToPlanet = 4;
		break;
	case 'j':
	case 'J' :
			NowPlanet = 3;
			LightAndViewDistanceAccordingToPlanet = 28;
		break;
	case 's':
	case 'S' :
			NowPlanet = 4;
			LightAndViewDistanceAccordingToPlanet = 24;
		break;
	case 'u':
	case 'U' :
			NowPlanet = 5;
			LightAndViewDistanceAccordingToPlanet = 12;
		break;
	case 'n':
	case 'N' :
			NowPlanet = 6;
			LightAndViewDistanceAccordingToPlanet = 12;
		break;
	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	case SCROLL_WHEEL_UP:
		Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	case SCROLL_WHEEL_DOWN:
		Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}

	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
	InView = 0;
	Frozen = false;
	NowPlanet = 1;
}


// called when user resizes the window:

void
Resize(int width, int height)
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// handle a change to the window's visibility:

void
Visibility(int state)
{
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = { 0.f, 1.f, 0.f, 1.f };

static float xy[] = { -.5f, .5f, .5f, -.5f };

static int xorder[] = { 1, 2, -3, 4 };

static float yx[] = { 0.f, 0.f, -.5f, .5f };

static float yy[] = { 0.f, .6f, 1.f, 1.f };

static int yorder[] = { 1, 2, 3, -2, 4 };

static float zx[] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes(float length)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb(float hsv[3], float rgb[3])
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;

	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = (float)floor(h);
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - (s * (1.f - f)));

	float r = 0., g = 0., b = 0.;			// red, green, blue
	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit(float v[3])
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
