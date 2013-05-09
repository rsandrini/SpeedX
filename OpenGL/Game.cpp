#include "Game.h"
#include "GameWindow.h"
#include "Camera.h"
#include <time.h>

CCamera objCamera; 
tVector3 buffer[20]; // The buffer complete for draw the tunnel
tVector3 color[20];

float lastEnd = -0.5f; // The deepest part of the quad
float end = 0;
bool left;
bool right;
float rotate;
float cameraSpeed=0.01f;

/* [0]- Game Run [1]- GameOver  [-1]-Pause */
int gameState; 


void Game::setup()
{
	//Define a cor de limpeza da tela
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);*/
	glEnable(GL_SMOOTH);
	gameState = 0;
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
	
	if (gameState == 0){
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


}

void Game::processLogics(float secs)
{
    //Lemos o estado das teclas
    Uint8* keys = SDL_GetKeyState(NULL);
	

	if (gameState == 1){
		objCamera.Move_Camera(cameraSpeed);
		cameraSpeed+=0.00001f;

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

		if (colision()){
			printf("GameOver\n");

		}
	}
	
	if (gameState == -1){
	
		+printf("PAUSE\n");
	}

	if (gameState == 0){
		printf("GameOver");
	}
	/*
	printf("POS [%.2f %.2f %.2f] VIEW [%.2f %.2f %.2f] UP [%.2f %.2f %.2f] \n",
		    objCamera.mPos.x,  objCamera.mPos.y,  objCamera.mPos.z,	
			objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,	
			objCamera.mUp.x,   objCamera.mUp.y,   objCamera.mUp.z);
	*/
}


bool Game::colision(){
	for (int i=0; i<20; i++){
		if (buffer[i].x > 0){
			int local = (int)buffer[0].x;
			printf("%f %f \n", buffer[i].y, objCamera.mView.z+1.5f);
			float cameraView = objCamera.mView.z+1.5f;
			switch(local){
				case 1:
					if (cameraView >= buffer[i].y && cameraView <= buffer[i].z){
						printf("COLISAO\n");
						gameStatus = 1;
					}
					break;
				/*
				case 2:
					if()
				*/
			}
		}
	}
	return false;
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
}

// Remove the quads out of view and create new's in the finish tunnel
void Game::rebuildMap() const{

	int lastInvalid = 0;
	for (int i=0; i<20; i++){
		if((objCamera.mPos.z) < buffer[i].z){
			lastInvalid = i;
		}
		else{
			if (lastInvalid > 0)
				break;
		}
	}

	if (lastInvalid > 0){
		int c = 0;
		for (int i=lastInvalid; i<20; i++){
		
			buffer[c] = buffer[i];
			color[c] = color[i];
			c++;
		}

		for (int i=c; i<20; i++){
			generateRing(i);
		}
	}
}

void Game::drawObstacle(int position) const{

	int local = buffer[position].x;
	
	// Local debug
	local = 1;
	
	float _end = buffer[position].y;
	float _endLast = buffer[position].z;

	glColor3f(color[position].x, color[position].y, color[position].z); 
	switch (local){
		
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

void Game::generateMap()  const{

	for (int i=0; i<20; i++){
		generateRing(i);
	}
}

void Game::generateRing(int i) const{
	color[i].x = (float)rand()/((float)RAND_MAX/1.0f);
	color[i].y = (float)rand()/((float)RAND_MAX/1.0f);
	color[i].z = (float)rand()/((float)RAND_MAX/1.0f);

	if( rand()/(RAND_MAX/10) >= 7 )
		buffer[i].x = (float)rand()/((float)RAND_MAX/15.0f);
	else
		buffer[i].x = 0;

	buffer[i].y = end;
	buffer[i].z = lastEnd;

	end = lastEnd;
	lastEnd -= 0.5f;
}

void Game::drawTunnel() const
{
	float _end = 0;
	float _endLast = 0;

	//glRotatef(20.0f, rotate, 0.0f, 0.0f);
	glRotatef(rotate, 0, 0, 1);
	for(int i=0; i<20; i++){
		_end = buffer[i].y;
		_endLast = buffer[i].z;

		glColor3f(color[i].x, color[i].y, color[i].z); 
		//printf("Draw in color %f %f %f \n", color[i].x, color[i].y, color[i].z);
		glBegin(GL_QUAD_STRIP);
			//1
			glVertex3f(0.15f, -0.65f, _end);
			glVertex3f(0.15f, -0.65f, _endLast);

			//2
			glVertex3f(-0.15f, -0.65f, _end);
			glVertex3f(-0.15f, -0.65f, _endLast);

			//3
			glVertex3f(-0.45f, -0.45f, _end);
			glVertex3f(-0.45f, -0.45f, _endLast);

			//4
			glVertex3f(-0.65f, -0.15f, _end);
			glVertex3f(-0.65f, -0.15f, _endLast);

			//5
			glVertex3f(-0.65f, 0.15f, _end);
			glVertex3f(-0.65f, 0.15f, _endLast);

			//6
			glVertex3f(-0.45f, 0.45f, _end);
			glVertex3f(-0.45f, 0.45f, _endLast);

			//7
			glVertex3f(-0.15f, 0.65f, _end);
			glVertex3f(-0.15f, 0.65f, _endLast);

			//8
			glVertex3f(0.15f, 0.65f, _end);
			glVertex3f(0.15f, 0.65f, _endLast);

			//9
			glVertex3f(0.45f, 0.45f, _end);
			glVertex3f(0.45f, 0.45f, _endLast);

			//10
			glVertex3f(0.65f, 0.15f, _end);
			glVertex3f(0.65f, 0.15f, _endLast);

			//11
			glVertex3f(0.65f, -0.15f, _end);
			glVertex3f(0.65f, -0.15f, _endLast);

			//12
			glVertex3f(0.45f, -0.45f,_end);
			glVertex3f(0.45f, -0.45f, _endLast);

			//13
			glVertex3f(0.15f, -0.65f, _end);
			glVertex3f(0.15f, -0.65f, _endLast);
		glEnd();

		if (buffer[i].x > 0)
			drawObstacle(i);
	}
}

void Game::teardown()
{
}