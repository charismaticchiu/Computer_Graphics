#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <freeglut/glut.h>
#include "textfile.h"
#include "GLM.h"

#ifndef GLUT_WHEEL_UP
# define GLUT_WHEEL_UP   0x0003
# define GLUT_WHEEL_DOWN 0x0004
#endif

#ifndef max
# define max(a,b) (((a)>(b))?(a):(b))
# define min(a,b) (((a)<(b))?(a):(b))
#endif

// Shader attributes
GLint iLocPosition;
GLint iLocColor;

char filename[40] = "ColorModels/brain18KC.obj";

char filename0[] = "ColorModels/brain18KC.obj";
char filename1[] = "ColorModels/teapot4KC.obj";
char filename2[] = "ColorModels/boxC.obj";
char filename3[] = "ColorModels/elephant16KC.obj";
char filename4[] = "ColorModels/tigerC.obj";

bool isFreeing = false;
int mode = GL_LINE;
GLMmodel* OBJ;

float *vertex ;
float *color ;
float *triangle_vertex;

void colorModel()
{
	//int i;
	int k = 0;	
	float distance=0.0;	
	float dx,dy,dz;
	




	//Calculate norm
	float minx = 10000000, miny = 10000000, minz = 10000000;
	float maxx = -10000000, maxy = -10000000, maxz = -10000000;

	for (int i = 1; i <= (int)OBJ->numvertices; i++){
		maxx = max(OBJ->vertices[i * 3 + 0], maxx);
		maxy = max(OBJ->vertices[i * 3 + 1], maxy);
		maxz = max(OBJ->vertices[i * 3 + 2], maxz);
		minx = min(OBJ->vertices[i * 3 + 0], minx);
		miny = min(OBJ->vertices[i * 3 + 1], miny);
		minz = min(OBJ->vertices[i * 3 + 2], minz);
	}
	dx = maxx - minx;
	dy = maxy - miny;
	dz = maxz - minz;

	//printf("%f %f %f %f %f %f\n", minx, maxx, miny, maxy, minz, maxz);

	OBJ->position[0] = (maxx + minx) / 2.0f;
	OBJ->position[1] = (maxy + miny) / 2.0f;
	OBJ->position[2] = (maxz + minz) / 2.0f;
/*
	printf("OBJ->position[0]= %f\n",OBJ->position[0]);
	printf("OBJ->position[1]= %f\n",OBJ->position[1]);
	printf("OBJ->position[2]= %f\n",OBJ->position[2]);*/

	//Centralize Vertices
	for(int i = 1; i<= (int) OBJ->numvertices; i++){
		vertex[i*3+0] =(float) ((OBJ->vertices[i * 3 + 0]) - (OBJ->position[0]));
		vertex[i*3+1] =(float) ((OBJ->vertices[i * 3 + 1]) - (OBJ->position[1]));
		vertex[i*3+2] =(float) ((OBJ->vertices[i * 3 + 2]) - (OBJ->position[2]));
		//printf("origin x=%f y=%f z=%f\n",OBJ->vertices[i*3+0],OBJ->vertices[i*3+1],OBJ->vertices[i*3+2]);
		//printf("center x=%f y=%f z=%f\n",vertex[i*3+0],vertex[i*3+1],vertex[i*3+2]);
	}
	

	//distance=sqrt(dx*dx+dy*dy+dz*dz)/2.0;
	distance = max(dx,max(dy,dz))/2.0;

	float _max=0.0;
	for(int i=0; i<(int)OBJ->numtriangles; i++) //NOTICE: vertex index starts from 1.
	{
		// the index of each vertex		
		int indv1 = OBJ->triangles[i].vindices[0];
		int indv2 = OBJ->triangles[i].vindices[1];
		int indv3 = OBJ->triangles[i].vindices[2];
											
		// the index of each color
		int indc1 = indv1;
		int indc2 = indv2;
		int indc3 = indv3;
		// vertices			
		//printf("i=%d k= %d\n",i,k);
		triangle_vertex[k+0] = vertex[indv1*3+0]/distance;		
		triangle_vertex[k+1] = vertex[indv1*3+1]/distance;
		triangle_vertex[k+2] = vertex[indv1*3+2]/distance;			
		triangle_vertex[k+3] = vertex[indv2*3+0]/distance;		
		triangle_vertex[k+4] = vertex[indv2*3+1]/distance;		
		triangle_vertex[k+5] = vertex[indv2*3+2]/distance;			
		triangle_vertex[k+6] = vertex[indv3*3+0]/distance;		
		triangle_vertex[k+7] = vertex[indv3*3+1]/distance;		
		triangle_vertex[k+8] = vertex[indv3*3+2]/distance;		

		//printf("v1 x=%f y=%f z=%f\n",triangle_vertex[k+0],triangle_vertex[k+1],triangle_vertex[k+2]);
		//printf("x=%f y=%f z=%f\n",triangle_vertex[k+0],triangle_vertex[k+1],triangle_vertex[k+2]);
		color[k+0] = OBJ->colors[indv1*3+0];
		color[k+1] = OBJ->colors[indv1*3+1];
		color[k+2] = OBJ->colors[indv1*3+2];			
		color[k+3] = OBJ->colors[indv2*3+0];
		color[k+4] = OBJ->colors[indv2*3+1];
		color[k+5] = OBJ->colors[indv2*3+2];		
		color[k+6] = OBJ->colors[indv3*3+0];
		color[k+7] = OBJ->colors[indv3*3+1];
		color[k+8] = OBJ->colors[indv3*3+2];
		
		k += 9;		
	}
	//printf("max after normalize=%f",_max);
}

void loadOBJModel()
{
	// read an obj model here
	OBJ = glmReadOBJ(filename);
	printf("%s\n", filename);
	vertex = (float*) malloc(sizeof(float) * 9 * (OBJ->numvertices));
	color = (float*) malloc(sizeof(float) * 9 * (OBJ->numtriangles));
	triangle_vertex = (float*) malloc (sizeof(float) * 9 * (OBJ->numtriangles));
	//printf("numvertices= %d ;numbertriangles= %d\n",OBJ->numvertices,OBJ->numtriangles);
	// traverse the color model
	colorModel();
}

void idle()
{
	glutPostRedisplay();
}

void renderScene(void)
{
	if(isFreeing!=true){
		// clear canvas
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnableVertexAttribArray(iLocPosition);
		glEnableVertexAttribArray(iLocColor);
		
		glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, triangle_vertex);
		glVertexAttribPointer(   iLocColor, 3, GL_FLOAT, GL_FALSE, 0, color);
	
		// draw the array we just bound
		glDrawArrays(GL_TRIANGLES, 0, 3*(OBJ->numtriangles));

		glutSwapBuffers();
	}
}

void showShaderCompileStatus(GLuint shader, GLint *shaderCompiled){
	glGetShaderiv(shader, GL_COMPILE_STATUS, shaderCompiled);
	if(GL_FALSE == (*shaderCompiled))
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character.
		GLchar *errorLog = (GLchar*) malloc(sizeof(GLchar) * maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		fprintf(stderr, "%s", errorLog);

		glDeleteShader(shader);
		free(errorLog);
	}
}

void setShaders()
{
	GLuint v, f, p;
	char *vs = NULL;
	char *fs = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("shader.vert");
	fs = textFileRead("shader.frag");

	glShaderSource(v, 1, (const GLchar**)&vs, NULL);
	glShaderSource(f, 1, (const GLchar**)&fs, NULL);

	free(vs);
	free(fs);

	// compile vertex shader
	glCompileShader(v);
	GLint vShaderCompiled;
	showShaderCompileStatus(v, &vShaderCompiled);
	if(!vShaderCompiled) system("pause"), exit(123);

	// compile fragment shader
	glCompileShader(f);
	GLint fShaderCompiled;
	showShaderCompileStatus(f, &fShaderCompiled);
	if(!fShaderCompiled) system("pause"), exit(456);

	p = glCreateProgram();

	// bind shader
	glAttachShader(p, f);
	glAttachShader(p, v);

	// link program
	glLinkProgram(p);

	iLocPosition = glGetAttribLocation (p, "av4position");
	iLocColor    = glGetAttribLocation (p, "av3color");

	glUseProgram(p);
}


void processMouse(int who, int state, int x, int y)
{
	printf("(%d, %d) ", x, y);

	switch(who){
	case GLUT_LEFT_BUTTON:   printf("left button   "); break;
	case GLUT_MIDDLE_BUTTON: printf("middle button "); break;
	case GLUT_RIGHT_BUTTON:  printf("right button  "); break; 
	case GLUT_WHEEL_UP:      printf("wheel up      "); break;
	case GLUT_WHEEL_DOWN:    printf("wheel down    "); break;
	default:                 printf("%-14d", who);     break;
	}

	switch(state){
	case GLUT_DOWN:          printf("start ");         break;
	case GLUT_UP:            printf("end   ");         break;
	}

	printf("\n");
	glutPostRedisplay();
}

void processMouseMotion(int x, int y){  // callback on mouse drag
	printf("(%d, %d) mouse move\n", x, y);
	glutPostRedisplay();
}

void processNormalKeys(unsigned char key, int x, int y) {
	printf("(%d, %d) \n", x, y);
	switch(key) {
		case 27: /* the Esc key */ 
			exit(0); 
			break;
		case 72: //h key
			printf("PRESS S for image switching\nPRESS C for frame mode switching\n");
			break;
		case 67://c
			if (mode == GL_LINE)
			{
				mode = GL_FILL;
			}
			else{
				mode = GL_LINE;
			}
			glPolygonMode(GL_FRONT_AND_BACK, mode);
			break;
		case 83://s
			if(strcmp(filename, filename0) == 0){
				strcpy(filename, filename1);
			}
			else if(strcmp(filename, filename1) == 0){
				strcpy(filename, filename2);
			}
			else if(strcmp(filename, filename2) == 0){
				strcpy(filename, filename3);
			}
			else if(strcmp(filename, filename3) == 0){
				strcpy(filename, filename4);
			}
			else if(strcmp(filename, filename4) == 0){
				strcpy(filename, filename0);
			}
			isFreeing = true;

			free(vertex);
			free(color);
			free(triangle_vertex);

			isFreeing = false;
			loadOBJModel();			
			break;
	}
	printf("\n");
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	// glut init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// create window
	glutInitWindowPosition(460, 40);
	glutInitWindowSize(800, 800);
	glutCreateWindow("10320 CS550000 CG HW1 Ming-Chang Chiu");

	glewInit();
	if(glewIsSupported("GL_VERSION_2_0")){
		printf("Ready for OpenGL 2.0\n");
	}else{
		printf("OpenGL 2.0 not supported\n");
		system("pause");
		exit(1);
	}
	printf("==================\nPRESS H for usage\n==================\n");
	printf("Available models are:\n(1)brain18KC\n(2)teapot4KC\n(3)shuttleC\n(4)elephant16KC\n(5)tigerC\n");
	// load obj models through glm
	loadOBJModel();
	glPolygonMode(GL_FRONT_AND_BACK, mode);
	
	
	// register glut callback functions
	glutDisplayFunc (renderScene);
	glutIdleFunc    (idle);
	glutKeyboardFunc(processNormalKeys);
	glutMouseFunc   (processMouse);
	glutMotionFunc  (processMouseMotion);

	glEnable(GL_DEPTH_TEST);

	// set up shaders here
	setShaders();

	// main loop
	glutMainLoop();

	// free
	glmDelete(OBJ);
	//delete [] vertex;
	return 0;
}

