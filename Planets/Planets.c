
/************************************************************************************

	File: 			screensaver.c

	Description:	An open GL program that displays a screen saver with various functionality, morphing, sparkles,
					and bonus effects. This is built with opengl and freeglut.

	Author:			Michael Northorp (heavily modifed the point.c from course website as well as other examples in class)

*************************************************************************************/

/* include the library header files */
// Freeglut header
#define _CRT_SECURE_NO_DEPRECATE
#include <GL\freeglut.h>
#include <GL\Gl.h>
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

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

// Define the number of planets
#define NUM_PLANETS 11

// Define the number of corona lines
#define NUM_CORONAS 360

// Define the number of klingon ships to generate
// Higher than 10 may cause app to lag
#define NUM_KLINGON 8

// Defines an x,y,z point
typedef GLfloat point3[3];

// Defines a RGB color
typedef GLfloat color3[3];

// Define starting points for klingon x/y/z
point3 klingonPoints[NUM_KLINGON];

// Keep track of current camera position and set the default
GLfloat cameraPosition[] = {0, 500, 2500, 0, 0, 0};

// Keep track of ship position
GLfloat shipPosition[] = {0, 300, 2200};

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
planet planets[NUM_PLANETS];

// This is an array of all the vertices for the enterprise
point3 shipVertices[1201];

// This is an array of all the vertices for the klingon ship
point3 klingonVertices[1610];

// Set up display list for the klingon ship
GLuint klingonShip = 0;

// Set up display list for the klingon ship fleet
GLuint klingonShipFleet = 0;

// Set up display list for the ship
GLuint theShip = 0;

// Set an interp variable for the klingon ship to bring it close to user
GLfloat klingonInterp = 0;

// This controls the color of the shield
GLfloat shieldInterp = 0.5;

// Keep track of the shield interp if it is increasing or decreasing
int shieldInterpFlip = 0;

// This toggles all the different keys
int ringsToggle = 0;
int starsToggle = 0;
int coronaToggle = 0;
int shieldToggle = 0;
int bonusToggle = 0;

// Toggles for directions key pressed and not pressed
int upPressed = 0;
int downPressed = 0;
int rightPressed = 0;
int leftPressed = 0;
int forwardPressed = 0;
int backwardPressed = 0;

// This controls normal keys for toggling features of the program
void normalKeys(unsigned char key, int x, int y) {
	switch(key) {
		// Toggle the rings for orbits
		case 'r':
			if(ringsToggle == 1) {
				ringsToggle = 0;
			} else {
				ringsToggle = 1;
			}
			break;
		// Toggle the stars
		case 's':
			if(starsToggle == 1) {
				starsToggle = 0;
			} else {
				starsToggle = 1;
			}
			break;
		// Toggle the corona
		case 'c':
			if(coronaToggle == 1) {
				coronaToggle = 0;
			} else {
				coronaToggle = 1;
			}
			break;
		// Toggle the shields
		case 'k':
			if(shieldToggle == 1) {
				shieldToggle = 0;
			} else {
				shieldToggle = 1;
			}
			break;
		// Toggle the bonus of the klingons attacking
		case 'b':
			if(bonusToggle == 1) {
				bonusToggle = 0;
			} else {
				klingonInterp = 0.0f;
				bonusToggle = 1;
			}
			break;
		default:
			break;
	}
}

// This reads in special keys for controlling the ship
void specialKeys(int key, int x, int y) {
		// Move up
		if(key == GLUT_KEY_UP) {
			// Set the key to be pressed
			upPressed = 1;
		}
		// Move down
		if(key == GLUT_KEY_DOWN) {
			// Set the key to be pressed
			downPressed = 1;
		}
		// Move right
		if(key == GLUT_KEY_RIGHT) {
			// Set the key to be pressed
			rightPressed = 1;
		}
		// Move left
		if(key == GLUT_KEY_LEFT) {
			// Set the key to be pressed
			leftPressed = 1;
		}
		// Move forward
		if(key == GLUT_KEY_PAGE_UP) {
			// Set the key to be pressed
			forwardPressed = 1;
		}
		// Move down
		if(key == GLUT_KEY_PAGE_DOWN) {
			// Set the key to be pressed
			backwardPressed = 1;
		}
}

// This checks when a special key is released for tilting
void specialKeysReleased(int key, int x, int y) {
		if(key == GLUT_KEY_UP) {
			// Set the key to be depressed
			upPressed = 0;
		}
		// Move down
		if(key == GLUT_KEY_DOWN) {
			// Set the key to be depressed
			downPressed = 0;
		}
		// Move right
		if(key == GLUT_KEY_RIGHT) {
			// Set the key to be depressed
			rightPressed = 0;
		}
		// Move left
		if(key == GLUT_KEY_LEFT) {
			// Set the key to be depressed
			leftPressed = 0;
		}
		// Move forward
		if(key == GLUT_KEY_PAGE_UP) {
			// Set the key to be depressed
			forwardPressed = 0;
		}
		// Move down
		if(key == GLUT_KEY_PAGE_DOWN) {
			// Set the key to be depressed
			backwardPressed = 0;
		}
}

// This function prints out all the keys used by the program
void printOutControls() {
	printf("Scene Controls\n--------------\n");
	printf("r: toggle orbit rings\n");
	printf("s: toggle stars\n");
	printf("c: toggle the sun's corona\n");
	printf("k: toggle shields\n\n");
	printf("b: toggle bonus klingon attack\n\n");
	printf("Camera Controls\n---------------\n");
	printf("Up    Arrow: move up\n");
	printf("Down  Arrow: move down\n");
	printf("Right Arrow: move right\n");
	printf("Left  Arrow: move left\n");
	printf("PAGE  UP   : forward\n");
	printf("PAGE  DOWN : backward\n");
}

// This sets up the enemey klingon ship
void setUpKlingonShip() {
	int i = 0;
	int j = 1;
	int randx = 0;
	int randy = 0;

	// Ship faces to draw as a triangle
	int face1;
	int face2;
	int face3;

	// Set up a file
	FILE * fileStream;
	// Char array to store
	char string[100];
	fileStream = fopen("enemyship.txt", "rt");

	// Make sure the file stream is not null
	if (fileStream != NULL)
	{
		// Puts the ship in a display list
		klingonShip = glGenLists(1);
	  	glNewList(klingonShip, GL_COMPILE);

		// Read each file line while it is not null, store in char array
		while(fgets(string, 100, fileStream) != NULL)
		{
			// Store the ship vertices as it reads the file
			sscanf(string, "v %f %f %f ", &klingonVertices[i][0], &klingonVertices[i][1], &klingonVertices[i][2]);

			// Check if it read in a face and draw it and store in a display list
			if(sscanf(string, "f %d %d %d ", &face1, &face2, &face3) == 3) {
				glColor3f(j/(float)3191, j/(float)3191, j/(float)3191);
				glBegin(GL_TRIANGLES);
					glVertex3f(klingonVertices[face1-1][0], klingonVertices[face1-1][1], klingonVertices[face1-1][2]);
					glVertex3f(klingonVertices[face2-1][0], klingonVertices[face2-1][1], klingonVertices[face2-1][2]);
					glVertex3f(klingonVertices[face3-1][0], klingonVertices[face3-1][1], klingonVertices[face3-1][2]);
				glEnd();
				j++;
			}

			// Increase i for reading in vertices
			i++;
		}

		// End the display list
		glEndList();

	}
	fclose (fileStream);

	// Set a new random seed value
	srand (time(0));
	// Generate random starting points for klingon ships
	for(i=0;i<NUM_KLINGON;i++) {
		// Random starting x and y value with time as seed
		randx = rand() % (600 + 600) - 600;
		randy = rand() % (1000 - 100) + 100;

		// Assign values to the klingon ships
		klingonPoints[i][0] = randx;
		klingonPoints[i][1] = randy;
		klingonPoints[i][2] = -3000;
	}

	// Puts the ship fleet  in a display list
	klingonShipFleet = glGenLists(1);
  	glNewList(klingonShipFleet, GL_COMPILE);

	// Set up another display list to store a fleet of klingon ships
	for(i=0;i<NUM_KLINGON;i++) {
		// Push the matrix
		glPushMatrix();

		// Translate to a random point in the back and interp
		glTranslatef(klingonPoints[i][0], klingonPoints[i][1], klingonPoints[i][2]);

		// Rotate the ship so it is facing the user
		glRotatef(-90, 1.0f, 0.0f, 0.0f);

		// Scale the ships to same size as enterprise
		glScalef(100.0f, 100.0f, 100.0f);

		// Draw the klingonShip
		glCallList(klingonShip);

		// Pop the matrix
		glPopMatrix();
	}

	// End the fleet list
	glEndList();
}

// This sets up the main ship by reading it in from a text file
void setUpShip() {
	int i = 0;
	int j = 1;

	// Ship faces to draw as a triangle
	int face1;
	int face2;
	int face3;

	// Set up a file
	FILE * fileStream;
	// Char array to store
	char string[100];
	fileStream = fopen("ship.txt", "rt");

	// Make sure the file stream is not null
	if (fileStream != NULL)
	{
		// Puts the ship in a display list
		theShip = glGenLists(1);
	  	glNewList(theShip, GL_COMPILE);

		// Read each file line while it is not null, store in char array
		while(fgets(string, 100, fileStream) != NULL)
		{
			// Store the ship vertices as it reads the file
			sscanf(string, "v %f %f %f ", &shipVertices[i][0], &shipVertices[i][1], &shipVertices[i][2]);

			// Check if it read in a face and draw it and store in a display list
			if(sscanf(string, "f %d %d %d ", &face1, &face2, &face3) == 3) {
				glColor3f(j/(float)1988, j/(float)1988, j/(float)1988);
				glBegin(GL_TRIANGLES);
					glVertex3f(shipVertices[face1-1][0], shipVertices[face1-1][1], shipVertices[face1-1][2]);
					glVertex3f(shipVertices[face2-1][0], shipVertices[face2-1][1], shipVertices[face2-1][2]);
					glVertex3f(shipVertices[face3-1][0], shipVertices[face3-1][1], shipVertices[face3-1][2]);
				glEnd();
				j++;
			}

			// Increase i for reading in vertices
			i++;
		}

		// End the display list
		glEndList();

	}
	fclose (fileStream);
}

// This draws the main ship
void drawShip() {
	glPushMatrix();

	// Translate it to the ships global position
	glTranslatef(shipPosition[0], shipPosition[1], shipPosition[2]);
	// Check if we should rotate a certain way depending on where the ship is moving
	if(upPressed) {
		// Update camera position and ship position
		cameraPosition[1] += 20;
		cameraPosition[4] += 20;
		shipPosition[1] += 20;

		// Add tilt to ship
		glRotatef(20, 1.0f, 0.0f, 0.0f);
	}

	if(downPressed) {
		// Update camera position and ship position
		cameraPosition[1] -= 20;
		cameraPosition[4] -= 20;
		shipPosition[1] -= 20;

		// Add tilt to ship
		glRotatef(-20, 1.0f, 0.0f, 0.0f);
	}

	if(rightPressed) {
		// Update camera position and ship position
		cameraPosition[0] += 20;
		cameraPosition[3] += 20;
		shipPosition[0] += 20;

		// Add tilt to ship
		glRotatef(-20, 0.0f, 0.0f, 1.0f);
	}

	if(leftPressed) {
		// Update camera position and ship position
		cameraPosition[0] -= 20;
		cameraPosition[3] -= 20;
		shipPosition[0] -= 20;

		// Add tilt to ship
		glRotatef(20, 0.0f, 0.0f, 1.0f);
	}

	if(forwardPressed) {
		// Update camera position and ship position
		cameraPosition[2] -= 20;
		cameraPosition[5] -= 20;
		shipPosition[2] -= 20;

		// Add tilt to ship
		glRotatef(-10, 1.0f, 0.0f, 0.0f);
	}

	if(backwardPressed) {
		// Update camera position and ship position
		cameraPosition[2] += 20;
		cameraPosition[5] += 20;
		shipPosition[2] += 20;

		// Add tilt to ship
		glRotatef(10, 1.0f, 0.0f, 0.0f);
	}
	glScalef(100.0f, 100.0f, 100.0f);
	glCallList(theShip);
	glPopMatrix();
}

// Draw the enemy klingon ship
void drawKlingonShip() {
	// Push the matrix
	glPushMatrix();

	// Dont translate anything if the interp is greater than 1.0, so they pause, then have them vanish
	if(klingonInterp >= 1.0 && klingonInterp < 2.0) {
		// Translate the fleet based on the interp on the Z axis
		glTranslatef(0.0f, 0.0f, 4000.0);
		// Draw the fleet of ships
		glCallList(klingonShipFleet);
	} else if(klingonInterp >= 2.0) {
		// Don't draw anything because they vanished
	} else {
		// Translate the fleet based on the interp on the Z axis
		glTranslatef(0.0f, 0.0f, (1-klingonInterp) * klingonPoints[0][2] + klingonInterp * 4000.0);
		// Draw the fleet of ships
		glCallList(klingonShipFleet);
	}

	// Pop the matrix
	glPopMatrix();
}


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

	// Earth moon
	planets[10].rateOrbit = 0.01f;
	planets[10].theta = 0.0f;
	planets[10].radius = 20.0f;
	planets[10].color[0] = 1.0f;
	planets[10].color[1] = 1.0f;
	planets[10].color[2] = 1.0f;
	planets[10].distance = 70.0f;
}

// Draw planets
void drawPlanets() {
	int i;

	// Go through the 10 planets and draw them all and translate them
	// Based on their theta value and rate of orbit and distance from the sun
	for (i=0;i<NUM_PLANETS;i++) {
		// Do translations and rotations
		glPushMatrix();
		// If it is the moon for the earth, translate it out from the earth
		if(i==10) {
			// Rotate to where the earth is
			glRotatef(planets[3].theta * 360.0f, 0.0f, 1.0f, 0.0f);
			// Translate to the earth
			glTranslatef(planets[3].distance, 0.0f, 0.0f);
			// Rotate around the earth now
			glRotatef(planets[i].theta * 360.0f, 0.0f, 1.0f, 0.0f);
			// Move away from earth
			glTranslatef(planets[i].distance, 0.0f, 0.0f);
		} else { // If it is a normal planet
			// Get the degree of the rotation
			glRotatef(planets[i].theta * 360.0f, 0.0f, 1.0f, 0.0f);
			// Translate to a distance from the sun
			glTranslatef(planets[i].distance, 0.0f, 0.0f);
		}

		// Draw each planet
		drawPlanet(planets[i].radius, planets[i].color);
		glPopMatrix();
	}
}

// Increase the theta values of each planet
void rotatePlanets() {
	int i;
	// Increase each planets theta value or roll it over
	for(i=0;i<NUM_PLANETS;i++) {
		// Check if the theta should roll over
		if(planets[i].theta >= 1.0f) {
			planets[i].theta = 0.0f;
		}
		// Increase the theta value of the planet
		planets[i].theta += planets[i].rateOrbit;
	}
}

// This draws the orbits of the planets
void drawOrbits() {
	int i;
	int j;

	// Go through each planets
	for(i=1;i<NUM_PLANETS;i++) {
		// If moon translate to position of earth before drawing orbit line
		if(i==10) {
			glPushMatrix();
			// Get the degree of the rotation
			glRotatef(planets[3].theta * 360.0f, 0.0f, 1.0f, 0.0f);
			// Translate to a distance from the sun
			glTranslatef(planets[3].distance, 0.0f, 0.0f);
		}
		glLineWidth(1.0f);
		// Begin a line loop for orbit lines
		glBegin(GL_LINE_LOOP);
		// Go from 1 to 360 degrees and draw the orbit lines, which then connect
		for(j=0;j<360;j++) {
			// Set color to white
			glColor3f(1.0f, 1.0f, 1.0f);
			// If drawing orbit for moon
			if(i==10) {
				// Get x and z values, y is at 0
				glVertex3f(planets[i].distance * cos(j * DEG_TO_RAD), 0, planets[i].distance * sin(j * DEG_TO_RAD));
			} else {
				// Get x and z values, y is at 0
				glVertex3f(planets[i].distance * cos(j * DEG_TO_RAD), 0, planets[i].distance * sin(j * DEG_TO_RAD));
			}
		}
		glEnd();

		// Pop matrix if drawing the moon
		if(i==10) {
			glPopMatrix();
		}
	}
}

// This sets up the stars with random points
void setUpStars() {
	int i;
	for(i=0;i<NUM_STARS;i++) {
		// Set up stars with random points on the 1000.0 Z axis
		stars[i].starPoints[0] = (rand() % (25000+25000)) - 15000;
		stars[i].starPoints[1] = (rand() % (25000+25000)) - 15000;
		stars[i].starPoints[2] = -5000.0f;
	}

	// Puts the stars in a display list
	starField = glGenLists(1);
  	glNewList(starField, GL_COMPILE);

  	// Draw the star field once
   	glPointSize(1.0f);
	glBegin(GL_POINTS);
		// For each star draw the star point generated and put into display list
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

// Draw the shield using a wireframe
void drawShield() {
	// Push the matrix
	glPushMatrix();

	// Translate the shield to the ship position
	glTranslatef(shipPosition[0], shipPosition[1], shipPosition[2]);

	// Check if we should rotate a certain way depending on where the ship is moving
	if(upPressed) {
		// Add tilt to the shield
		glRotatef(20, 1.0f, 0.0f, 0.0f);
	}

	if(downPressed) {
		// Add tilt to the shield
		glRotatef(-20, 1.0f, 0.0f, 0.0f);
	}

	if(rightPressed) {
		// Add tilt to the shield
		glRotatef(-20, 0.0f, 0.0f, 1.0f);
	}

	if(leftPressed) {
		// Add tilt to the shield
		glRotatef(20, 0.0f, 0.0f, 1.0f);
	}

	if(forwardPressed) {
		// Add tilt to the shield
		glRotatef(-10, 1.0f, 0.0f, 0.0f);
	}

	if(backwardPressed) {
		// Add tilt to the shield
		glRotatef(10, 1.0f, 0.0f, 0.0f);
	}

	// Scale the z axis to stretch the shield a bit
	glScalef(1.0f, 1.0f, 1.5f);

	// Pulse the color based on interp value
	glColor3f(0.0f, shieldInterp, 1-shieldInterp);
	// Draw the shield using a wireframe
	glutWireSphere(70, 40, 40);

	// Pop the matrix
	glPopMatrix();
}

// This changes the color of the shield so it pulses
void calculateShieldColor() {
	// Increase or decrease the color value of the shield
	if(shieldInterpFlip == 1) {
		shieldInterp -= 0.02;
	} else {
		shieldInterp += 0.02;
	}

	// Control if the shield interp is increasing or decreasing
	if(shieldInterp >= 1.0) {
		shieldInterpFlip = 1;
	} else if(shieldInterp <= 0.5) {
		shieldInterpFlip = 0;
	}
}

// This changes the interp value of the klingon ships
void calculateKlingonInterp() {
	// Increase the klingon interp and reset if it hits three
	if(klingonInterp >= 3.0) {
		klingonInterp = 0.0;
	} else {
		klingonInterp += 0.005;
	}
}

// Draw the suns corona
void drawCorona() {
	float j;
	float i;
	int startNum = 200;
	int randNum;

	// Push the matrix
	glPushMatrix();

	// Increase the line width
	glLineWidth(1.5f);
	// Enable blending
	glEnable(GL_BLEND);
	// Set blending mode for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for(i=0;i<360;i++) {
		// Generate a random num to decide if the corona should be drawn on a z axis
		randNum = (rand() % (300-200)) + 200;

		// Choose to draw random coronas on certain degrees of the sun
		if(randNum > 290) {
			// Rotate in 3d for the lines
			glRotatef(i, 1.0f, 0.0f, 0.0f);

			// Draw all of the lines at random positions on that axis
			for(j=0;j<360;j+=0.5) {
				// Generate a random line length
				randNum = (rand() % (300-200)) + 200;

				// Randomly draw lines on the corona for that z axis
				if(randNum > 296) {
					// Rotate along Z using theta for degrees as a global variable
					glRotatef(j, 0.0, 0.0, 1.0);
					glBegin(GL_LINES);
						// Set the color to yellow with no transparency
						glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
						// Start value of the line is on the radius of the sun
						glVertex3f((float)startNum, 0.0f, 0.0f);
						// Set the color to orange with some transparency
						glColor4f(1.0f, 0.6f, 0.0f, 0.7f);
						// End value of the line is a random number so the lines flicker
						glVertex3f((float)randNum, 0.0f, 0.0f);
					glEnd();
				}
			}
		}
	}
	// Pop the matrix
	glPopMatrix();
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
    gluPerspective(90, 1, 0.1, 40000);

    // change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);

	// Print out the controls
	printOutControls();

	// Set up the planet values
	setUpPlanets();

	// Set up the star values
	setUpStars();

	// Set up the ship to be drawn
	setUpShip();

	// Set up the klingon ship to be drawn
	setUpKlingonShip();
}

/************************************************************************

	Function:		myIdle

	Description:	This runs whenever the program is idle. It handles most
	of the dynamic functionality of the program. This includes scales, angles
	changing, and vertexs to interpolate between for the sparkle.

*************************************************************************/
void myIdle(void)
{
	// Rotate all of the planets
	rotatePlanets();

	// Change the color of the shield so it pulses
	calculateShieldColor();

	// Increase the interp value of the klingon
	calculateKlingonInterp();

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

	// Draw the planets
	drawPlanets();

	// Draw the corona
	if(coronaToggle == 1) {
		drawCorona();
	}

	// Draw the orbits
	if(ringsToggle == 1) {
		drawOrbits();
	}

	// Draw the stars
	if(starsToggle == 1) {
		drawStars();
	}

	// Draw the shield
	if(shieldToggle == 1) {
		drawShield();
	}

	// Draw klingon ships for bonus
	if(bonusToggle == 1) {
		drawKlingonShip();
	}

	// Draw the ship
	drawShip();

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
	// This handles keyboard input for normal keys
	glutKeyboardFunc(normalKeys);
	// This handles keyboard input for special keys
	glutSpecialFunc(specialKeys);
	// This checks when a special key is released
	glutSpecialUpFunc(specialKeysReleased);
	// register redraw function
	glutDisplayFunc(display);
	// go into a perpetual loop
	glutMainLoop();
}
