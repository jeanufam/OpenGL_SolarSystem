
/************************************************************************************

	File: 			screensaver.c

	Description:	An open GL program that displays a screen saver with various functionality, morphing, sparkles,
					and bonus effects. This is built with opengl and freeglut.

	Author:			Michael Northorp (heavily modifed the point.c from course website as well as other examples in class)

*************************************************************************************/

/* include the library header files */
// Freeglut header
#include <GL\freeglut.h>
#include <GL\Gl.h>
#include <windows.h>
#include <math.h>

/* Global variables */

//  Ratio of the circumference to the diameter of a circle
#define PI 3.14159265f

// Conversion multiplier for converting from degrees to Radians for some calculations
#define DEG_TO_RAD PI/180.0f

// Window size parameters
#define WIN_HEIGHT 640.0f
#define WIN_WIDTH 640.0f

// Define the number of stars
#define NUM_STARS 2500

// Defines an x,y,z point
//
typedef GLfloat point3[3];

// Defines a RGB color
typedef GLfloat color3[3];

// Keep track of current camera position and set the default
GLfloat cameraPosition[] = {0, 500, 2000, 0, 0, 0};

// This is a star struct
typedef struct
{
	point3 starPoints;
} star;

// Create a list of stars
star stars[NUM_STARS];

// Set up display list for stars
GLuint starField = 0;

// This struct defines a planet with a orbit rate, theta value, radius, color, and distance from
// sun
typedef struct
{
    // Rate of Orbit
    GLfloat rateOrbit;
    // Theta
    GLfloat theta;
    // Radius
    GLfloat radius;
    // Color
    color3 color;
    // Distance from Sun
    GLfloat distance;
} planet;

// Initialize an array for all the planets of struct planet
planet planets[10];

// Draw a planet with a radius and color
void drawPlanet(GLfloat radius, color3 color) {
	glColor3fv(color);
	glutSolidSphere(radius, 50, 50);
}

// This function sets up 9 planets including the sun
void setUpPlanets() {
	// The sun
	planets[0].rateOrbit = 0.0f;
	planets[0].theta = 0.0f;
	planets[0].radius = 200.0f;
	planets[0].color[0] = 1.0f;
	planets[0].color[1] = 1.0f;
	planets[0].color[2] = 0.0f;
	planets[0].distance = 0.0f;

	// Mercury
	planets[1].rateOrbit = 0.01f;
	planets[1].theta = 0.0f;
	planets[1].radius = 20.0f;
	planets[1].color[0] = 1.0f;
	planets[1].color[1] = 0.0f;
	planets[1].color[2] = 0.0f;
	planets[1].distance = planets[0].distance + planets[0].radius + planets[1].radius + 40.0f;

	// Venus
	planets[2].rateOrbit = 0.0065f;
	planets[2].theta = 0.0f;
	planets[2].radius = 40.0f;
	planets[2].color[0] = 0.0f;
	planets[2].color[1] = 0.0f;
	planets[2].color[2] = 1.0f;
	planets[2].distance = planets[1].distance + planets[1].radius + planets[2].radius + 50.0f;

	// Earth
	planets[3].rateOrbit = 0.0025f;
	planets[3].theta = 0.0f;
	planets[3].radius = 50.0f;
	planets[3].color[0] = 0.0f;
	planets[3].color[1] = 1.0f;
	planets[3].color[2] = 0.0f;
	planets[3].distance = planets[2].distance + planets[2].radius + planets[3].radius + 20.0f;

	// Mars
	planets[4].rateOrbit = 0.002f;
	planets[4].theta = 0.0f;
	planets[4].radius = 30.0f;
	planets[4].color[0] = 1.0f;
	planets[4].color[1] = 0.0f;
	planets[4].color[2] = 0.0f;
	planets[4].distance = planets[3].distance + planets[3].radius + planets[4].radius + 20.0f;

	// Jupiter
	planets[5].rateOrbit = 0.001f;
	planets[5].theta = 0.0f;
	planets[5].radius = 100.0f;
	planets[5].color[0] = 1.0f;
	planets[5].color[1] = 0.1f;
	planets[5].color[2] = 0.0f;
	planets[5].distance = planets[4].distance + planets[4].radius + planets[5].radius + 20.0f;

	// Saturn
	planets[6].rateOrbit = 0.003f;
	planets[6].theta = 0.0f;
	planets[6].radius = 80.0f;
	planets[6].color[0] = 1.0f;
	planets[6].color[1] = 0.6f;
	planets[6].color[2] = 0.0f;
	planets[6].distance = planets[5].distance + planets[5].radius + planets[6].radius + 20.0f;

	// Uranus
	planets[7].rateOrbit = 0.007f;
	planets[7].theta = 0.0f;
	planets[7].radius = 60.0f;
	planets[7].color[0] = 0.0f;
	planets[7].color[1] = 0.2f;
	planets[7].color[2] = 1.0f;
	planets[7].distance = planets[6].distance + planets[6].radius + planets[7].radius + 20.0f;

	// Neptune
	planets[8].rateOrbit = 0.005f;
	planets[8].theta = 0.0f;
	planets[8].radius = 70.0f;
	planets[8].color[0] = 0.0f;
	planets[8].color[1] = 0.7f;
	planets[8].color[2] = 1.0f;
	planets[8].distance = planets[7].distance + planets[7].radius + planets[8].radius + 20.0f;

	// Pluto
	planets[9].rateOrbit = 0.0009f;
	planets[9].theta = 0.0f;
	planets[9].radius = 10.0f;
	planets[9].color[0] = 1.0f;
	planets[9].color[1] = 0.0f;
	planets[9].color[2] = 0.3f;
	planets[9].distance = planets[8].distance + planets[8].radius + planets[9].radius + 20.0f;
}

// Draw planets
void drawPlanets() {
	int i;

	// Go through the 10 planets and draw them all and translate them
	// Based on their theta value and rate of orbit and distance from the sun
	for (i=0;i<10;i++) {
		// Do translations and rotations
		glPushMatrix();
		// Get the degree of the rotation
		glRotatef(planets[i].theta * 360.0f, 0.0f, 1.0f, 0.0f);
		// Translate to a distance from the sun
		glTranslatef(planets[i].distance, 0.0f, 0.0f);
		// Draw each planet
		drawPlanet(planets[i].radius, planets[i].color);
		glPopMatrix();
	}
}

// Increase the theta values of each planet
void rotatePlanets() {
	int i;
	// Increase each planets theta value or roll it over
	for(i=0;i<10;i++) {
		// Check if the theta should roll over
		if(planets[i].theta >= 1.0f) {
			planets[i].theta = 0.0f;
		}
		// Increase the theta value of the planet
		planets[i].theta += planets[i].rateOrbit;
	}
}

// This sets up the stars with random points
void setUpStars() {
	int i;
	for(i=0;i<NUM_STARS;i++) {
		// Set up stars with random points on the 1000.0 Z axis
		stars[i].starPoints[0] = rand() % 10000 - 5000;
		stars[i].starPoints[1] = rand() % 10000 - 5000;
		stars[i].starPoints[2] = -1000.0f;
	}

	// Puts the stars in a display list
	starField = glGenLists(1);
  	glNewList(starField, GL_COMPILE);

  	// Draw the star field once
   	glPointSize(1.0f);
	glBegin(GL_POINTS);
		for(i=0;i<NUM_STARS;i++) {
				glVertex3fv(stars[i].starPoints);
		}
	glEnd();

	// End the list
   	glEndList();
}

// This draws stars in the background
void drawStars() {
	// Call the list previously stored with a random color
	glColor3f((float)(rand()/(float)RAND_MAX), (float)(rand()/(float)RAND_MAX), (float)(rand()/(float)RAND_MAX));
	glCallList(starField);
}

/************************************************************************

	Function:		init

	Description:	This performs the main setup for openGl, it sets the
	projection mod to gl_projection, and the coordinate system.

*************************************************************************/
void init(void)
{
	// Enable the depth test
	glEnable(GL_DEPTH_TEST);

	// Set the background to be black
	glClearColor(0, 0, 0, 1.0);

	// Set to projection mode so we can use camera functions
    glMatrixMode(GL_PROJECTION);

	// Load identity matrix onto projection matrix
    glLoadIdentity();

    // gluPerspective(fovy, aspect, near, far)
    gluPerspective(90, 1, 0.1, 10000);

    // change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);

	// Set up the planet values
	setUpPlanets();

	// Set up the star values
	setUpStars();
}

/************************************************************************

	Function:		myIdle

	Description:	This runs whenever the program is idle. It handles most
	of the dynamic functionality of the program. This includes scales, angles
	changing, and vertexs to interpolate between for the sparkle.

*************************************************************************/
void myIdle(void)
{

	// Idle stuff like increasing thetas or scales
	rotatePlanets();
	// Force a redraw in OpenGL
	glutPostRedisplay();
}

/************************************************************************

	Function:		display

	Description:	Draws initial static openGL. Sets up buttons, menu,
	drawing of letters, calls mouse functionality, runs all animations.

*************************************************************************/
void display(void)
{

	// Clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load the indentity matrix
	glLoadIdentity();

	// Set up the camera position
	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2], cameraPosition[3], cameraPosition[4], cameraPosition[5], 0, 1, 0);

	// Call draw functions here
	drawPlanets();
	drawStars();

	// Swap the drawing buffers here
	glutSwapBuffers();
}


/************************************************************************

	Function:		main

	Description:	Sets up the openGL rendering context and the windowing
					system, then begins the display loop. Display mode is
					set to double buffering, and idle function and mouse
					event listener are set up.

*************************************************************************/
void main(int argc, char** argv)
{
	// initialize the toolkit
	glutInit(&argc, argv);
	// set display mode
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	// set window size
	glutInitWindowSize(WIN_HEIGHT, WIN_WIDTH);
	// open the screen window
	glutCreateWindow("Solar System");
	//initialize the rendering context
	init();
	// register the idle function
	glutIdleFunc(myIdle);
	// register redraw function
	glutDisplayFunc(display);
	// go into a perpetual loop
	glutMainLoop();
}
