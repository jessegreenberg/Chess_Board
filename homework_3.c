/*
 * HOMEWORK ASSIGNMENT 3
 * Jesse Greenberg
 * CSCI 4229
 * 
 * RENDER A 3D SCENE WITH TEXTURES AND LIGHTING
 * Building on the chess set from last week.
 * 
 * */


#include "CSCIx229.h"

/* State Variables */
int th = 0; // Azimuthal angle
int ph = 30; // Elevation of View
int zh = 0; // Azimuth of light
double ypos = 20; // yposition of light;
double dim = 90; // dimension of view box
int fov = 55; // Field of view for perspective mode
double asp = 1; // Aspect ratio
int white = 0; // Variable for piece color
int black = 1; // Variable for piece clor
int move = 1; // State of light movement
unsigned int piecetexture[2]; // Holds piece textures
unsigned int boardtexture[3]; // holds textures for the board


/* CHESS PIECE BUILDING BLOCKS */

/*
 * Draw a Vertex in polar coordinates and
 * bind a texture coordinate
 * */
static void polarVertex(double th, double ph) {
	double x = -Sin(th)*Cos(ph);
	double y = Cos(th)*Cos(ph);
	double z = Sin(ph);
	glNormal3d(x, y, z);
	glTexCoord2d(th/360.0, ph/180.0+0.5);
	glVertex3d(x, y, z);
}

/* Draw a pyramid at (x, y, z)
 *  Define texture coordinates and normals for lighting
 *  Order of vertices for face culling
 * */
static void drawPyramid(double x, double y, double z) {
	glPushMatrix();
	glTranslated(x, y, z);
	glBegin(GL_TRIANGLES);
	glNormal3d(-1, 2, 0);
	glTexCoord2d(0.5, 1.0); glVertex3f(0, 2, 0);;
	glTexCoord2d(1.0, 0.0); glVertex3f(-1, 0, -1);
	glTexCoord2d(0.0, 0.0); glVertex3f(-1, 0, 1);
	glNormal3d(0, 2, -1);
	glTexCoord2d(0.5, 1.0); glVertex3f(0, 2, 0);;
	glTexCoord2d(0.0, 1.0); glVertex3f(1, 0, -1);
	glTexCoord2d(1.0, 0.0); glVertex3f(-1, 0, -1);
	glNormal3d(1, 2, 0);
	glTexCoord2d(0.5, 1.0); glVertex3f(0, 2, 0);
	glTexCoord2d(1.0, 0.0); glVertex3f(1, 0, 1);;
	glTexCoord2d(0.0, 1.0); glVertex3f(1, 0, -1);
	glNormal3d(0, 2, 1);
	glTexCoord2d(1.0, 0.0); glVertex3f(1, 0, 1);;
	glTexCoord2d(0.5, 1.0); glVertex3f(0, 2, 0);;
	glTexCoord2d(0.0, 1.0); glVertex3f(-1, 0, 1);
	glEnd();
	glPopMatrix();
}

/*
 * Draw a Rectangular Box at (x,y,z) with dimensions (dx,dy,dz)
 * */
static void drawCube(double x, double y, double z, double dx, double dy, double dz)
{
	// Save the transformation
	glPushMatrix();
	// Apply the offset
	glTranslated(x, y, z);
	glScaled(dx, dy, dz);
	// Draw the rectangular prism
	glBegin(GL_QUADS);
	// Front
	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0); glVertex3f(-1, -1, 1);
	glTexCoord2d(0, 1); glVertex3f(1, -1, 1);
	glTexCoord2d(1, 1); glVertex3f(1, 1, 1);
	glTexCoord2d(1, 0); glVertex3f(-1, 1, 1);
	// Back
	glNormal3d(0, 0, -1);
	glTexCoord2d(0, 0); glVertex3f(1, -1, -1);
	glTexCoord2d(0, 1); glVertex3f(-1, -1, -1);
	glTexCoord2d(1, 1); glVertex3f(-1, 1, -1);
	glTexCoord2d(1, 0);glVertex3f(1, 1, -1);
	// Right
	glNormal3d(1, 0, 0);
	glTexCoord2d(0, 0);glVertex3f(1, -1, 1);
	glTexCoord2d(0, 1);glVertex3f(1, -1, -1);
	glTexCoord2d(1, 0);glVertex3f(1, 1, -1);
	glTexCoord2d(1, 1);glVertex3f(1, 1, 1);
	// Left
	glNormal3d(-1, 0, 0);
	glTexCoord2d(0, 0);glVertex3f(-1, -1, -1);
	glTexCoord2d(0, 1);glVertex3f(-1, -1, 1);
	glTexCoord2d(1, 0);glVertex3f(-1, 1, 1);
	glTexCoord2d(1, 1);glVertex3f(-1, 1, -1);
	// Top
	glNormal3d(0, 1, 0);
	glTexCoord2d(0, 0);glVertex3f(-1, 1, 1);
	glTexCoord2d(0, 1);glVertex3f(1, 1, 1);
	glTexCoord2d(1, 1);glVertex3f(1, 1, -1);
	glTexCoord2d(1, 0);glVertex3f(-1, 1, -1);
	// Botton
	glNormal3d(0, -1, 0);
	glTexCoord2d(0, 0);glVertex3f(-1, -1, -1);
	glTexCoord2d(0, 1);glVertex3f(1, -1, -1);
	glTexCoord2d(1, 1);glVertex3f(1, -1, 1);
	glTexCoord2d(1, 0);glVertex3f(-1, -1, 1);
	// End
	glEnd();
	// Undo transformations
	glPopMatrix();
}

/*
 *  Draw a sphere at (x, y, z) with radius (r)
 */
static void drawSphere(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
			polarVertex(th,ph+d); // This vertex first for face culling
			polarVertex(th,ph);
      }
      glEnd();
   }
   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a hemisphere at (x, y, z) with radius (r)
 */
static void drawHemisphere(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glRotated(270, 0, 0, 1);
   glScaled(r,r,r);
   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=180;th+=d)
      {
		 polarVertex(th,ph+d); // This vertex first for face culling
         polarVertex(th,ph);
      }
      glEnd();
   }
   //  Undo transformations
   glPopMatrix();
}

/* Draw a Cylinder of radius r and height y */
void drawCylinder(double r, double y) {
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glScaled(r, r, y);
	int i;
	/* Top of the Cylinder */
	glNormal3f(0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5, 0.5);
	glVertex3f(0, 0, 1);
	for(i = 0; i <=360; i+=10)
	{
		glTexCoord2f(0.5*Cos(i)+0.5, 0.5*Sin(i)+0.5);
		glVertex3f(Cos(i), Sin(i), 1);
	}
	glEnd();
	/* Bottom of the Cylinder */
	glNormal3f(0, 0, -1);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5, 0.5);
	glVertex3f(0, 0, -1);
	for(i = 0; i <= 360; i +=10)
	{
		glTexCoord2f(0.5*Cos(i)+0.5, 0.5*Sin(i)+0.5);
		glVertex3f(-1*Cos(i), Sin(i), -1);
	}
	glEnd();
	/* Side of Cylinder */
	glBegin(GL_QUADS);
	double sidediv = 45;
	for(i = 0; i <= 360; i+=sidediv)
	{
		glNormal3f(Cos(i), Sin(i), 0);
		glTexCoord2f(1, 1); glVertex3f(Cos(i+sidediv), Sin(i+sidediv), +1);
		glTexCoord2f(1, 0); glVertex3f(Cos(i), Sin(i), +1);
		glTexCoord2f(0, 0); glVertex3f(Cos(i), Sin(i), -1);		
		glTexCoord2f(0, 1); glVertex3f(Cos(i+sidediv), Sin(i+sidediv), -1);
	}
	glEnd();
	glPopMatrix();
}
		
/*
 * Draw a curved cone (hat-like shape)
 * eccentricity (ecc) varies the severity of the curve
 * height (height) varies the height of the base
 * This will act as a base for many pieces
 * */
void drawCurvedBase(double ecc, double height){
	// Save the transformation
	glPushMatrix();
	glRotated(180, 1, 0, 0);
	// Apply the offset
	double i; // iteration variable
	/* Time Step */
	double dt = 0.2;
	/* Draw several cylinders with radii of the function
	 * (ecc^x) to create a curved survace */
	for(i = dt; i < height; i = i+dt){
		glTranslated(0, .2, 0);
		double radius = pow(ecc, i);
		drawCylinder(radius, .2);
	}
	glPopMatrix();
	glutPostRedisplay();
}

/* BUILDING THE CHESS PIECES*/
/*
 * Draw a pawn at location (x, y, z) with color (color)
 * */
void drawPawn(double x, double y, double z, int color) {
	/* Set up gl textures */
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, piecetexture[color]);
	// Save the transformation matrix
	glPushMatrix();
		glTranslated(x, y, z);
		glPushMatrix();
			glScaled(1, 4, 1);
			/* Translate the piece up by its height to set the base at 0 */
			glTranslated(0, 2, 0); 	
			// Draw the base of the pawn
			drawCurvedBase(2, 2);
		glPopMatrix();
		/* Draw the Head of the Pawn */
		/* Translate the head up to the height * the yscale */
		glTranslated(0, 8, 0);
		drawSphere(0, 0, 0, 1.7);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
/* Draw a Rook at location (x, y, z) with color (color)
 * */
void drawRook(double x, double y, double z, int color) {
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, piecetexture[color]);
	glPushMatrix();
	glTranslated(x, y, z);
	glScaled(2.2, 4, 2.2);
	// Draw the base of the rook
	/* Translate the base up to 0 */
	glTranslated(0, 2.5, 0);
	drawCurvedBase(1.1, 2.5);
	// Draw the top
	drawCylinder(1.5, 0.6);
	// draw the battlements
	int i;
	for(i = 0; i < 360; i = i+60){
		drawCube(1.5*Cos(i), 0.4, 1.5*Sin(i), .3, .7, .3);
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
/* Draw a Bishop at location (x, y, z) with color (color)
 * */
void drawBishop(double x, double y, double z, int color) {
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, piecetexture[color]);

	glPushMatrix();
	glTranslated(x, y, z);
	glScaled(0.6, 3, 0.6);
	/* Translate the base up to 0 */
	glTranslated(0, 2.6, 0);
	// Draw the base of the bishop
	drawCurvedBase(2, 2.6);
	// Draw the stand of the bishop
	glTranslated(0, -1.8, 0);
	drawCylinder(1.3, 0.7);
	// Draw the collar of the bishop
	glTranslated(0, 2, 0);
	drawCylinder(2, 0.2);
	glScaled(2.8, 0.8, 2.8);
	drawSphere(0, 1, 0, 0.8);
	drawSphere(0, 1.8, 0, 0.4);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

/* Draw a Queen at location (x, y, z) with color (color)
 * */
void drawQueen(double x, double y, double z, int color) {
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, piecetexture[color]);
	glPushMatrix();
	glTranslated(x, y, z);
	
	glScaled(1, 4, 1);
	/* Translate the base up to 0 */
	glTranslated(0, 3, 0);
	// Draw the base of the queen
	drawCurvedBase(1.3, 3);

	// Draw the collar of the queen
	drawCylinder(2, 0.2);

	// Draw the Queen's crown
	glScaled(1, 0.4, 1);
	double i; // iteration variable
	for(i = 0.2; i < 1.5; i = i+0.05){
		glTranslated(0, 0.05, 0);
		drawCylinder(i, 0.05);
	}
	for(i = 1.5; i > 0; i = i-0.05){
		glTranslated(0, 0.02, 0);
		drawCylinder(i, 0.05);
	}
	drawSphere(0, 0.15, 0, 0.25);
	for(i = 0; i < 360; i = i+60){
		drawSphere(1.5*Cos(i), 0, 1.5*Sin(i), 0.15);
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
/* Draw a King at location (x, y, z) with color (color)
 * */
void drawKing(double x, double y, double z, int color) {
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, piecetexture[color]);
	glPushMatrix();
		glTranslated(x, y, z);
		/* Translate the base up to 0 */
		glTranslated(0, 12.8, 0);
		glScaled(1.3, 4, 1.3);
		// Draw the base of the King
		drawCurvedBase(1.2, 3.2);
		// Draw the king's collar
		drawCylinder(1.5, 0.3);
		// Draw the King's crown
		double i; // iteration variable
		for(i = 0.8; i < 1; i = i+0.02){
			glTranslated(0, 0.1, 0);
			drawCylinder(i, 0.1);
		}
		for(i = 1; i > 0; i = i-0.05){
			glTranslated(0, 0.02, 0);
			drawCylinder(i, 0.05);
		}
		// Draw a cross on top of the crown
		glScaled(1, 0.4, 1);
		drawCube(0, 0.5, 0, 0.1, 0.5, 0.1);
		drawCube(0, 0.6, 0, 0.3, 0.1, 0.1);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
/* Draw a knight at the location (x, y, z) with color (color)
 * */
void drawKnight(double x, double y, double z, int color) {
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, piecetexture[color]);
	glPushMatrix();
		glTranslated(x, y, z);
		glTranslated(0, 6.75, 0);
		glRotated(180, 0, 1, 0);
		glPushMatrix();
			// Draw the Nose
			glTranslated(0, 3, 0);
			glRotated(245, 1, 0, 0);
			glScaled(1.95, 4, 1.95);
			drawHemisphere(0, 0, 0, 0.6);
		glPopMatrix();
		glPushMatrix();
			// Draw the head
			glTranslated(0, 3, 0);
			drawSphere(0, 0, 0, 1.3);
		glPopMatrix();
		// Draw the ears of the horse
		glPushMatrix();
			glTranslated(0, 3, 0);
			glRotated(-30, 1, 0, 0);
			glScaled(0.2, 0.9, 0.2);
			drawPyramid(2.4, 0.8, 0.4);
			drawPyramid(-2.4, 0.8, 0.4);
		glPopMatrix();
		// draw the body of the horse
		double neck = 2.5; // iteration variables
		int ang = 50;
		glPushMatrix();
			glTranslated(0, -6, 0);
			glRotated(-90, 0, 1, 0);
			glScaled(0.5, 0.5, 0.5);
			// Draw the base of the knight
			/* Curvature of the neck follows Sin */
			drawCylinder(7, 1.5);
			while(ang < 280){
				glTranslated(Sin(ang)/15, .15, 0);
				ang = ang + 2;
				drawCylinder(neck+Sin(ang), 1);
			}
		glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

/* Set up one side of the chess board */
void drawSet(int color)
{
	/* Set up a row of pawns */
	int i;
	int spacing = 10;
	for(i = 0; i <8; i++){ 
		drawPawn((i-4)*spacing, 0, 10, color);
	}
	/* Set the rooks */
	drawRook(-4*spacing, 0, 0, color);
	drawRook(3*spacing, 0, 0, color);
	/* Set the knights */
	drawKnight(-3*spacing, 0, 0, color);
	drawKnight(2*spacing, 0, 0, color);
	/* Set the bishops*/
	drawBishop(-2*spacing, 0, 0, color);
	drawBishop(1*spacing, 0, 0, color);
	/* Set the King and Queen */
	if(color == 0) {
		drawQueen(-spacing, 0, 0, color);
		drawKing(0, 0, 0, color);
	}
	else{
		drawQueen(0, 0, 0, color);
		drawKing(-spacing, 0, 0, color);
	}
}

/* Draw a chess board for the pieces. */
void drawBoard()
{
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	int color = 1;
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	int i, k;
	int spacing = 10; // Size of each checker
	/* Render the 64 checkers */
	for(i = -4*spacing; i <= 3*spacing; i += spacing) {
		for(k = -4*spacing; k <= 3*spacing; k += spacing) {
			glBindTexture(GL_TEXTURE_2D, boardtexture[color]);
			drawCube(i, k, 0, spacing/2, spacing/2, 0.5);
			color = 1 - color; // Alternate checker color
		}
		color = 1 - color; // Alternate checker color
	}
	// place a border around the board
	glBindTexture(GL_TEXTURE_2D, boardtexture[2]);
	for(i = -5*spacing; i <= 4*spacing; i += spacing){
		for(k = -5*spacing; k <= 4*spacing; k += spacing) {
			drawCube(i, k, 0, spacing/2, spacing/2, 0.5);
		}
	}
	glPopMatrix();
}
	 
/*
 * Glut will call this to display the view
 * Basic structures are not yet drawn in the correct direction to enable face
 * culling.
 * */
void display()
{
	// Clear the window and set the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Enable face culling 
	glEnable(GL_CULL_FACE);
	// Undo previous transformations
	glLoadIdentity();
	// Set up position of the eye for gluLookAt
	double Ex = -3*dim*Sin(th)*Cos(ph);
	double Ey = 3*dim*Sin(ph);
	double Ez = 3*dim*Cos(th)*Cos(ph);
	gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
	// Set up decent viewing scale
	glPushMatrix();
	glScaled(3, 3, 3);
	// Initialize light position
	float Position[] = {60*Cos(zh), ypos, 60*Sin(zh), 1};
	// Draw light position as a small sphere
	glColor3f(1, 1, 1);
	drawSphere(Position[0], Position[1], Position[2], 0.5);
	// Translate material intensity to color vectors
	float Ambient[]   = {0.3,0.3,0.3,1.0};
	float Diffuse[]   = {1,1,1,1};
	float Specular[]  = {0.5,0.5,0,0.5};
	float White_V[]     = {1,1,1,1};
	// Enable lighting and vector normaliztion
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	//  glColor sets ambient and diffuse color materials
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	//  Enable light0
	glEnable(GL_LIGHT0);
	// Set ambient, diffuse, specular components and position of light0
	glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
	glLightfv(GL_LIGHT0,GL_POSITION,Position);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0f);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, White_V);
	
	// draw the chess board
	drawBoard();
	// draw the white pieces on the board
	glPushMatrix();
	glTranslated(0, 0, -40);
	drawSet(white);
	glPopMatrix();
	// draw the black pieces on the board
	glPushMatrix();
	glTranslated(-10, 0, 30);
	glRotated(180, 0, 1, 0);
	drawSet(black);
	glPopMatrix();
	
	// Render the scene and make it visible
	glFlush();
	glPopMatrix();
	glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the program is idel
 *  animates the position of the light
**/
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(30*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

// Handles key presses
void key(unsigned char ch, int x, int y)
{
	// Conveniently exit on escape
	if (ch == 27) {
		exit(0);
	}
	// Easily reset view angle
	else if (ch == '0') {
		th = 0;
		ph = 30;
	}

	// Increase the field of view (zoom in)
	else if (ch == '-') {
		fov++;
	}
	// Decrease the field of view (zoom out)
	else if (ch == '+') {
		fov--;
	}
	// Increase the y position of the light
	else if(ch == 'u' || ch == 'U'){
		ypos ++;
	}
	// Decrease the y position of the light
	else if(ch == 'j' || ch == 'J'){
		ypos --;
	}
	 //  Toggle light movement
	else if (ch == 'm' || ch == 'M') {
		move = 1-move;
	}
   //  Move light
   else if (ch == '<'){
      zh += 1;
	}
   else if (ch == '>') {
      zh -= 1;
	}
	
	// Update the projection
	Project(fov, asp, dim);
	// Tell GLUT to animate the light position when idle
	glutIdleFunc(move?idle:NULL);
	// Tell GLUT to update the view
	glutPostRedisplay();
}

// Handles arrow presses to change the view angle
// Mostly taken from example 6 in course material
void special(int key, int x, int y)
{
	// Right arrow key - increase azimuth by 5 degrees
	if (key == GLUT_KEY_RIGHT){
		th -= 5;
	}
	else if(key == GLUT_KEY_LEFT) {
		th += 5;
	}
	else if(key == GLUT_KEY_UP) {
		ph += 5;
	}
	else if (key == GLUT_KEY_DOWN) {
		ph -= 5;
	}
	// Keep angles bounded by 360 in magnitude
	th %= 360;
	ph %= 360;
	// Update the projection 
	Project(fov, asp, dim); 
	// Tell GLUT to update the screen
	glutPostRedisplay();
}

/*
 * GLUT will call this to handle window resizing
 * */
 void reshape(int width, int height) {
	// Ratio of the width to the height of the window
	asp = (height>0) ? (double)width/height : 1;
	// Set the viewport to the entire window
	glViewport(0, 0, width, height);
	// Reset the projection
	Project(fov, asp, dim);
}

/*
 * Start up GLUT and tell it to handle all displays
 * */
int main(int argc, char* argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);
	// Request a double buffered, true color window with z-buffering
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	// Create the window
	glutCreateWindow("Jesse Greenberg Homework 3");
	// Register function to display the scene
	glutDisplayFunc(display);
	// Tell GLUT to handle window resizing
	glutReshapeFunc(reshape);
	// Tell GLUT to handle key input
	glutKeyboardFunc(key);
	// Tell GLUT to handle input from special keys
	glutSpecialFunc(special);
	// Tell GLUT to animate the light position when idle
	glutIdleFunc(idle);
	// Load textures
	piecetexture[0] = LoadTexBMP("whitemarble.bmp");
	piecetexture[1] = LoadTexBMP("blackmarble.bmp");
	boardtexture[0] = LoadTexBMP("white_wood.bmp");
	boardtexture[1] = LoadTexBMP("black_wood.bmp");
	boardtexture[2] = LoadTexBMP("red_wood.bmp");
	// Pass control to GLUT for any events
	ErrCheck("init");
	glutMainLoop();
	// Return to OS
	return 0;
}
	
	
	
	
	
