#include "Game.h"
#include "GameWindow.h"
#include "Camera.h"
#include <time.h>

CCamera objCamera; 
tVector3 buffer[20][27]; // The buffer complete for draw the tunnel
tVector3 ring[27]; // Temp var with mount buffer

tVector3 bufferObstacles[20]; // The buffer of barrers

float lastEnd = -0.5f; // The deepest part of the quad
float end = 0;
bool left;
bool right;
float rotate;


void Game::setup()
{
	//Define a cor de limpeza da tela
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);*/
	glEnable(GL_SMOOTH);
	
	rotate = 0.0f;
	// Definição de luz 
	/*glEnable(GL_LIGHTING);
	float ambientLight[] = {0.55f, 0.55f, 0.55f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	*/
	glMatrixMode(GL_PROJECTION);
		gluPerspective(45, GAMEWINDOW.getRatio(), 0.1, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	objCamera.Position_Camera(0, -0.5f, 2,  0, 0.0f, 0,   0, 0, 0.5);
	srand(time(0));
	generateMap();

}

void Game::processEvents(const SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_QUIT:
            exit = true;
            break;
    }
	// KEYBOARD
	
	if(GetKeyState(VK_UP) & 0x80) 
	{	
		objCamera.Move_Camera(CAMERASPEED);
	}

	if(GetKeyState(VK_DOWN) & 0x80) 
	{
		objCamera.Move_Camera(-CAMERASPEED);
	}
	
	if(GetKeyState(VK_LEFT) & 0x80) 
	{	
		left = !left; // Chifre do bode
		if (right)
			right = false;
	}

	if(GetKeyState(VK_RIGHT) & 0x80) 
	{
		right = !right; // Chifre do bode
		if (left)
			left = false;
	}



}

void Game::processLogics(float secs)
{
    //Lemos o estado das teclas
    Uint8* keys = SDL_GetKeyState(NULL);
	
	objCamera.Move_Camera(CAMERASPEED);
	
	rebuildMap();

	// because rotation is clockwise 
	if (left)
		rotate = rotate - 3.0f;
		if (rotate < 0)
			rotate = 360.0f;
	if (right)
		rotate += 3.0f;
		if (rotate > 360)
			rotate = 0;

	/*
	printf("POS [%.2f %.2f %.2f] VIEW [%.2f %.2f %.2f] UP [%.2f %.2f %.2f] \n",
		    objCamera.mPos.x,  objCamera.mPos.y,  objCamera.mPos.z,	
			objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,	
			objCamera.mUp.x,   objCamera.mUp.y,   objCamera.mUp.z);
	*/
}

void Game::draw() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									// Reset The Current Modelview Matrix
	//*********/**/**/ CAMERA /**/**/**/***********//
	// use this function for opengl target camera
	
	gluLookAt(objCamera.mPos.x,  objCamera.mPos.y,  objCamera.mPos.z,	
			  objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,	
			  objCamera.mUp.x,   objCamera.mUp.y,   objCamera.mUp.z);
	
	//glLoadIdentity();
	drawTunnel();
	drawQuad();
}

// Remove the quads out of view and create new's in the finish tunnel
void Game::rebuildMap() const{
	/*
	// Value before a mView cam
	float min = objCamera.mView.z;
	// Value for max view 
	float max = objCamera.mView.z - 2.0f;
	*/
	/* Teste */

	int lastInvalid = 0;
	for (int i=1; i<=20; i++){
		//printf("Camera %f =!= bufferPosition %f \n", objCamera.mPos.z, buffer[i][2].z);
		if((objCamera.mPos.z) < buffer[i][2].z){
			/* REALLOC THIS POSITION AND GENERATE A NEW IN END BUFFER */
			lastInvalid = i;
			//printf("Position is out of view - %i\n", lastInvalid);
		}
		/*else
			break;*/
	}
	
	if (lastInvalid > 0){
		//printf("Realloc\n");
		realocPosition(lastInvalid);
	}
	
}

void Game::realocPosition(int position) const{
	int k=0;
	int iO=0;

	int invObs = 0;
	// Search a last obstacle out of view
	for(int b=0; b<=20; b++){
		if (bufferObstacles[b].z >= buffer[position][2].z)
			invObs = b;
	}

	//Remove all obstacles in back of view
	for (int b=0; b<=20; b++){
		if(b > position){
			bufferObstacles[k] = bufferObstacles[k]; 
		}
	}
	// Create a new obstacles
	for (int i=k; i<=20; i++){
		// Test with in this rings exist obstacle (>=30%)
		int randd = rand()/(RAND_MAX/10);
		if( randd >= 7 ){
			bufferObstacles[iO].x = (float)rand()/((float)RAND_MAX/12.0f);
			bufferObstacles[iO].y = end; 
			bufferObstacles[iO].z = lastEnd;
			iO ++;
		}
	}

	// Complete bufferObstacles with 0.0f (invalid)
	for(int i=iO; i<=20; i++){
		bufferObstacles[i].x = 0.0f;
	}

	// reset a var
	k = 0;

	// Remove all position invalid in buffer original 
	for (int i=0; i<=20; i++){
		// [i] represent a out of view ring - remove all after this
		if (i > position ){
			for(int j=0; j<27; j++){
				buffer[k][j] = buffer[i][j]; 
			}
			k++;
		}
	}
	
	// Create a new rings
	for (int i=k; i<=20; i++){
		// rewrite a new rings on the last positions
		generateOneRing();
		for (int j=0; j<27; j++){
			buffer[i][j] = ring[j];
		}
	}
	
	/*
	for (int i=0; i<=20; i++){
		for (int j=0; j<27; j++){
			//printf("BufferTemp[%i][%i] - (%f, %f, %f)\n", i, x, bufferTemp[i][x].x, bufferTemp[i][x].y, bufferTemp[i][x].z);
			tVector3 vT(bufferTemp[i][j].x,bufferTemp[i][j].y, bufferTemp[i][j].z);
			//tVector3 vT(0, 0, 0);
			//printf("%f, %f, %f\n", vT.x, vT.y, vT.z);
			buffer[i][j] = vT;
		}
	}
	*/
}

void Game::drawQuad() const{

	for(int i=0; i<=20; i++){
		// Test if end of obstacles in vector
		if (bufferObstacles[i].x == 0.0f){
			break;
		}
		
		float _end = bufferObstacles[i].y;
		float _endLast = bufferObstacles[i].z;

		glColor3f( 1.0, 1.0, 1.0 );  // COLOR
		switch ((int)bufferObstacles[i].x){
		
			case 1:
				// FRONT
				glBegin(GL_POLYGON);
					
					glNormal3f( 0.0, 0.0, -1.0);	
					glVertex3f( 0.15, -0.65, _end );
					glVertex3f( 0.15, -0.35, _end );
					glVertex3f( -0.15, -0.35, _end );
					glVertex3f( -0.15, -0.65, _end );
				glEnd();

				// LEFT
				glBegin(GL_POLYGON);
					glNormal3f( -1.0, 0.0, 0.0);
					glVertex3f( -0.15, -0.65, _end );
					glVertex3f( -0.15, -0.65, _endLast );
					glVertex3f( -0.15, -0.35, _endLast );
					glVertex3f( -0.15, -0.35, _end );
				glEnd();

				// RIGHT
				glBegin(GL_POLYGON);
					glNormal3f( 1.0, 0.0, 0.0);
					glVertex3f( 0.15, -0.65, _end );
					glVertex3f( 0.15, -0.65, _endLast );
					glVertex3f( 0.15, -0.35, _endLast );
					glVertex3f( 0.15, -0.35, _end );
				glEnd();

				// TOP
				glBegin(GL_POLYGON);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( 0.15, -0.35, _end );
					glVertex3f( 0.15, -0.35, _endLast );
					glVertex3f( -0.15, -0.35, _endLast );
					glVertex3f( -0.15, -0.35, _end );
				glEnd();
				break;
		
			case 2:
				glBegin(GL_TRIANGLE_FAN);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( -0.1, -0.3, _end );
					glVertex3f( -0.15, -0.65, _end );
					glVertex3f( -0.15, -0.65, _endLast );
					glVertex3f( -0.45, -0.45, _endLast );
					glVertex3f( -0.45, -0.45, _end );
					glVertex3f( -0.15, -0.65, _end );
				glEnd();
				break;

			case 3:
				glBegin(GL_TRIANGLE_FAN);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( 0, 0, _end );
					glVertex3f( -0.45, -0.45, _end );
					glVertex3f( -0.45, -0.45, _endLast );
					glVertex3f( -0.65, -0.15, _endLast );
					glVertex3f( -0.65, -0.15, _end);
					glVertex3f( -0.45, -0.45, _end );
				glEnd();
				break;

			case 4:
				// FRONT
				glBegin(GL_POLYGON);
					glColor3f( 1.0, 1.0, 1.0 );
					glNormal3f( 0.0, 0.0, -1.0);	
					glVertex3f( -0.65, -0.15, _end );
					glVertex3f( -0.35, -0.15, _end );
					glVertex3f( -0.35, 0.15, _end );
					glVertex3f( -0.65, 0.15, _end );
				glEnd();
	
				// LEFT
				glBegin(GL_POLYGON);
					glNormal3f( -1.0, 0.0, 0.0);
					glVertex3f( -0.35, 0.15, _end );
					glVertex3f( -0.35, 0.15, _endLast );
					glVertex3f( -0.65, 0.15, _endLast );
					glVertex3f( -0.65, 0.15, _end );
				glEnd();

				// RIGHT
				glBegin(GL_POLYGON);
					glNormal3f( 1.0, 0.0, 0.0);
					glVertex3f( -0.35, -0.15, _end );
					glVertex3f( -0.35, -0.15, _endLast );
					glVertex3f( -0.65, -0.15, _endLast );
					glVertex3f( -0.65, -0.15, _end );
				glEnd();

				// TOP
				glBegin(GL_POLYGON);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( -0.35, -0.15, _end );
					glVertex3f( -0.35, -0.15, _endLast );
					glVertex3f( -0.35, 0.15, _endLast );
					glVertex3f( -0.35, 0.15, _end );
				glEnd();
				break;

			case 5:
				glBegin(GL_TRIANGLE_FAN);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( 0, 0, _end );
					glVertex3f( -0.65, 0.15, _end );
					glVertex3f( -0.65, 0.15, _endLast );
					glVertex3f( -0.45, 0.45, _endLast );
					glVertex3f( -0.45, 0.45, _end );
					glVertex3f( -0.65, 0.15, _end );
				glEnd();
				break;

			case 6: 
				glBegin(GL_TRIANGLE_FAN);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( -0.1, 0.3, _end );
					glVertex3f( -0.45, 0.45, _end );
					glVertex3f( -0.45, 0.45, _endLast );
					glVertex3f( -0.15, 0.65, _endLast );
					glVertex3f( -0.15, 0.65, _end);
					glVertex3f( -0.45, 0.45, _end );
				glEnd();
				break;

			case 7:
				glBegin(GL_POLYGON);
					glColor3f( 1.0, 1.0, 1.0 );
					glNormal3f( 0.0, 0.0, -1.0);	
					glVertex3f( -0.15, 0.65, _end );
					glVertex3f( 0.15, 0.65, _end );
					glVertex3f( 0.15, 0.35, _end );
					glVertex3f( -0.15, 0.35, _end );
				glEnd();

				// LEFT
				glBegin(GL_POLYGON);
					glNormal3f( -1.0, 0.0, 0.0);
					glVertex3f( 0.15, 0.35, _end );
					glVertex3f( 0.15, 0.35, _endLast );
					glVertex3f( 0.15, 0.65, _endLast );
					glVertex3f( 0.15, 0.65, _end );
				glEnd();

				// RIGHT
				glBegin(GL_POLYGON);
					glNormal3f( 1.0, 0.0, 0.0);
					glVertex3f( -0.15, 0.35, _end );
					glVertex3f( -0.15, 0.35, _endLast );
					glVertex3f( -0.15, 0.65, _endLast );
					glVertex3f( -0.15, 0.65, _end );
				glEnd();

				// TOP
				glBegin(GL_POLYGON);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( 0.15, 0.35, _end );
					glVertex3f( 0.15, 0.35, _endLast );
					glVertex3f( -0.15, 0.35, _endLast );
					glVertex3f( -0.15, 0.35, _end );
				glEnd();
				break;

			case 8:
				glBegin(GL_TRIANGLE_FAN);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( 0, 0, _end );
					glVertex3f( 0.45, 0.45, _end );
					glVertex3f( 0.45, 0.45, _endLast );
					glVertex3f( 0.15, 0.65, _endLast );
					glVertex3f( 0.15, 0.65, _end );
					glVertex3f( 0.45, 0.45, _end );
				glEnd();
				break;

			case 9:
				glBegin(GL_TRIANGLE_FAN);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( 0.3, 0.1, _end );
					glVertex3f( 0.65, 0.15, _end );
					glVertex3f( 0.65, 0.15, _endLast );
					glVertex3f( 0.45, 0.45, _endLast );
					glVertex3f( 0.45, 0.45, _end);
					glVertex3f( 0.65, 0.15, _end );
				glEnd();
				break;

			case 10:
				// FRONT
				glBegin(GL_POLYGON);
					glColor3f( 1.0, 1.0, 1.0 );
					glNormal3f( 0.0, 0.0, -1.0);	
					glVertex3f( 0.65, -0.15, _end );
					glVertex3f( 0.65, 0.15, _end );
					glVertex3f( 0.35, 0.15, _end );
					glVertex3f( 0.35, -0.15, _end );
				glEnd();

				// LEFT
				glBegin(GL_POLYGON);
					glNormal3f( -1.0, 0.0, 0.0);
					glVertex3f( 0.65, -0.15, _end );
					glVertex3f( 0.65, -0.15, _endLast );
					glVertex3f( 0.35, -0.15, _endLast );
					glVertex3f( 0.35, -0.15, _end );
				glEnd();

				// RIGHT
				glBegin(GL_POLYGON);
					glNormal3f( 1.0, 0.0, 0.0);
					glVertex3f( 0.65, 0.15, _end );
					glVertex3f( 0.65, 0.15, _endLast );
					glVertex3f( 0.35, 0.15, _endLast );
					glVertex3f( 0.35, 0.15, _end );
				glEnd();

				// TOP
				glBegin(GL_POLYGON);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( 0.35, -0.15, _end );
					glVertex3f( 0.35, -0.15, _endLast );
					glVertex3f( 0.35, 0.15, _endLast );
					glVertex3f( 0.35, 0.15, _end );
				glEnd();
				break;

			case 11:
				glBegin(GL_TRIANGLE_FAN);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( 0, 0, _end );
					glVertex3f( 0.65, -0.15, _end );
					glVertex3f( 0.65, -0.15, _endLast );
					glVertex3f( 0.45, -0.45, _endLast );
					glVertex3f( 0.45, -0.45, _end );
					glVertex3f( 0.65, -0.15, _end );
				glEnd();
				break;

			case 12:
				glBegin(GL_TRIANGLE_FAN);
					glNormal3f( 0.0, 0.0, -1.0);
					glVertex3f( 0.1, -0.3, _end );
					glVertex3f( 0.45, -0.45, _end );
					glVertex3f( 0.45, -0.45, _endLast );
					glVertex3f( 0.15, -0.65, _endLast );
					glVertex3f( 0.15, -0.65, _end);
					glVertex3f( 0.45, -0.45, _end );
				glEnd();
				break;
			case 13:

				// FRONT
				glBegin(GL_POLYGON);
					glColor3f( 1.0, 1.0, 1.0 );
					glNormal3f( 0.0, 0.0, -1.0);	
					glVertex3f( 0.15, -0.65, _end );
					glVertex3f( 0.15, 0.65, _end );
					glVertex3f( -0.15, 0.65, _end );
					glVertex3f( -0.15, -0.65, _end );
				glEnd();

				// LEFT
				glBegin(GL_POLYGON);
					glNormal3f( -1.0, 0.0, 0.0);
					glVertex3f( -0.15, -0.65, _end );
					glVertex3f( -0.15, -0.65, _endLast );
					glVertex3f( -0.15, 0.65, _endLast );
					glVertex3f( -0.15, 0.65, _end );
				glEnd();

				// RIGHT
				glBegin(GL_POLYGON);
					glNormal3f( 1.0, 0.0, 0.0);
					glVertex3f( 0.15, -0.65, _end );
					glVertex3f( 0.15, -0.65, _endLast );
					glVertex3f( 0.15, 0.65, _endLast );
					glVertex3f( 0.15, 0.65, _end );
				glEnd();
				break;

			case 14:
				// TOP
				glBegin(GL_POLYGON);
					glColor3f( 1.0, 1.0, 1.0 );
					glNormal3f( 0.0, 0.0, -1.0);	
					glVertex3f( -0.65, -0.15, _end );
					glVertex3f( 0.65, -0.15, _end );
					glVertex3f( 0.65, 0.15, _end );
					glVertex3f( -0.65, 0.15, _end );
				glEnd();
	
				// LEFT
				glBegin(GL_POLYGON);
					glNormal3f( -1.0, 0.0, 0.0);
					glVertex3f( 0.65, 0.15, _end );
					glVertex3f( 0.65, 0.15, _endLast );
					glVertex3f( -0.65, 0.15, _endLast );
					glVertex3f( -0.65, 0.15, _end );
				glEnd();

				// RIGHT
				glBegin(GL_POLYGON);
					glNormal3f( 1.0, 0.0, 0.0);
					glVertex3f( 0.65, -0.15, _end );
					glVertex3f( 0.65, -0.15, _endLast );
					glVertex3f( -0.65, -0.15, _endLast );
					glVertex3f( -0.65, -0.15, _end );
				glEnd();
				break;
		}
	}
}

void Game::generateOneRing() const{
	/*
	// Value before a mView cam
	float min = objCamera.mView.z;
	// Value for max view 
	float max = objCamera.mView.z - 2.0f;
	lastEnd =- 0.05f;
	float end = min;
	*/

	tVector3 vT0((float)rand()/((float)RAND_MAX/1.0f),
				(float)rand()/((float)RAND_MAX/1.0f),
				(float)rand()/((float)RAND_MAX/1.0f));
	//Generate a Random color
	ring[0] = vT0;
		
	//1
	tVector3 vT1(0.15f, -0.65f, end);
	ring[1] = vT1;
	vT1.z = lastEnd;
	ring[2] = vT1;

	//2
	tVector3 vT2(-0.15f, -0.65f, end);
	ring[3] = vT2;
	vT2.z = lastEnd;
	ring[4] = vT2;

	//3
	tVector3 vT3(-0.45f, -0.45f, end);
	ring[5] = vT3;
	vT3.z = lastEnd;
	ring[6] = vT3;

	//4
	tVector3 vT4(-0.65f, -0.15f, end);
	ring[7] = vT4;
	vT4.z = lastEnd;
	ring[8] = vT4;

	//5
	tVector3 vT5(-0.65f, 0.15f, end);
	ring[9] = vT5;
	vT5.z = lastEnd;
	ring[10] = vT5;

	//6
	tVector3 vT6(-0.45f, 0.45f, end);
	ring[11] = vT6;
	vT6.z = lastEnd;
	ring[12] = vT6;

	//7
	tVector3 vT7(-0.15f, 0.65f, end);
	ring[13] = vT7;
	vT7.z = lastEnd;
	ring[14] = vT7;

	//8
	tVector3 vT8(0.15f, 0.65f, end);
	ring[15] = vT8;
	vT8.z = lastEnd;
	ring[16] = vT8;

	//9
	tVector3 vT9(0.45f, 0.45f, end);
	ring[17] = vT9;
	vT9.z = lastEnd;
	ring[18] = vT9;

	//10
	tVector3 vT10(0.65f, 0.15f, end);
	ring[19] = vT10;
	vT10.z = lastEnd;
	ring[20] = vT10;

	//11
	tVector3 vT11(0.65f, -0.15f, end);
	ring[21] = vT11;
	vT11.z = lastEnd;
	ring[22] = vT11;

	//12
	tVector3 vT12(0.45f, -0.45f, end);
	ring[23] = vT12;
	vT12.z = lastEnd;
	ring[24] = vT12; 

	//13
	tVector3 vT13(0.15f, -0.65f, end);
	ring[25] = vT13;
	vT13.z = lastEnd;
	ring[26] = vT13;

	end = lastEnd;
	lastEnd -= 0.5f;

}

void Game::generateMap()  const{
	int iO=0;
	for (int i=0; i<=20; i++){
		generateOneRing();
		for (int x=0; x<27; x++){
			buffer[i][x] = ring[x];
		}
		// Test with in this rings exist obstacle (>=30%)
		int randd = rand()/(RAND_MAX/10);
		if( randd >= 7 ){
			bufferObstacles[iO].x = (float)rand()/((float)RAND_MAX/12.0f);
			bufferObstacles[iO].y = end; 
			bufferObstacles[iO].z = lastEnd;
			iO ++;
		}
	}

	// Complete bufferObstacles with 0.0f (invalid)
	for(int i=iO; i<=20; i++){
		bufferObstacles[i].x = 0.0f;
	}
}

void Game::drawTunnel() const
{
	/*
	float minList[3];
	float medList[25];
	*/


	//glRotatef(20.0f, rotate, 0.0f, 0.0f);
	glRotatef(rotate, 0 ,0, 1);
	for(int i=0; i<=20; i++){
		glBegin(GL_QUAD_STRIP);
		
		for (int j=0; j<27; j++){
			//printf("[%i][%i] %.1f %.2f %.2f \n", i, j, buffer[i][j].x, buffer[i][j].y, buffer[i][j].z);

			// Represent a First space in vector, is a color of quad
			if(j==0){ glColor3f(buffer[i][j].x, buffer[i][j].y, buffer[i][j].z); }

			else{ glVertex3f(buffer[i][j].x, buffer[i][j].y, buffer[i][j].z); }
		}
		glEnd();
		//printf("Endfor\n\n");
	}
}

void Game::teardown()
{
}