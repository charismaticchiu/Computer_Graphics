//Computer Graphics HW2
//Ming-Chang Chiu
//2015.4.23
//For convenience, transformation, viewing, projection matrix is mostly modified in processNormalKeys(), and processMouse()
//Rotation is done by first translating the model to the origin, rotate, and then translate back to the center of the model
//Scaling is similar to rotation.
//When translating, I will modified the center of the model accordingly for future rotation and scaling.
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <freeglut/glut.h>
#include "textfile.h"
#include "GLM.h"
#include "MAT.h"
#include "math.h"
#include "Matrices.h"
#ifndef GLUT_WHEEL_UP
# define GLUT_WHEEL_UP   0x0003
# define GLUT_WHEEL_DOWN 0x0004
#endif

#define far  3.0	  //far plane
#define near  1.0	  //near plane
#define left  -1.0	  //left plane
#define right  1.0	  //right plane
#define top  1.0	  //top plane
#define bottom  -1.0  //bottom plane

#ifndef max
# define max(a,b) (((a)>(b))?(a):(b))
# define min(a,b) (((a)<(b))?(a):(b))
#endif

// Shader attributes
GLint iLocPosition;
GLint iLocColor;
GLint iLocMVP;
Matrix4 m, v, p, m2,  m3;
GLMmodel* OBJ;
GLMmodel* OBJ2;
GLMmodel* OBJ3;
GLMmodel* OBJ4[3];
GLfloat aMVP[16];
GLfloat aMVP2[16];
GLfloat aMVP3[16];
bool isFreeing = false;
bool isGeometrical = true;
bool isViewing = false;
bool isScaling = false;
bool isTranslating = false;
bool isRotating = false;
bool eyeChanging = false;
bool centerChanging = false;
bool upChanging = false;
bool ctrl1 = true;
bool ctrl2 = false;
bool ctrl3 = false;
bool mouseLeftDown;
bool mouseRightDown;
bool mouseWheelUp;
bool mouseWheelDown;
float mouseX, mouseY;
char filename[40] = "ColorModels/brain18KC.obj";
char filename0[] = "ColorModels/brain18KC.obj";
char filename2[] = "ColorModels/teapot4KC.obj";
char filename3[] = "ColorModels/boxC.obj";
char filename1[] = "ColorModels/elephant16KC.obj";
char filename4[] = "ColorModels/tigerC.obj";
int mode = GL_LINE;
float eyeX = 0.0;
float eyeY = 0.0;
float eyeZ = 2.0;
float cenX = 0.0;
float cenY = 0.0;
float cenZ = 0.0;
float upX = 0.0;
float upY = 1.0;
float upZ = 0.0;
float *vertex ;
float *vertex2 ;
float *vertex3 ;
float *color ;
float *color2 ;
float *color3 ;
float *triangle_vertex;
float *triangle_vertex2;
float *triangle_vertex3;
void idle()
{
	glutPostRedisplay();
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
	iLocMVP		 = glGetUniformLocation(p, "mvp");

	glUseProgram(p);
}
void colorModel()
{	
	int k = 0, a = 0, b = 0;	
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

	OBJ->position[0] = (maxx + minx) / 2.0f;
	OBJ->position[1] = (maxy + miny) / 2.0f;
	OBJ->position[2] = (maxz + minz) / 2.0f;	

	for (int i = 1; i<= (int) OBJ->numvertices; i++)//NOTICE: vertex index starts from 1.
	{
		vertex[i*3+0] =(float) (OBJ->vertices[i * 3 + 0]) ;
		vertex[i*3+1] =(float) (OBJ->vertices[i * 3 + 1]) ;
		vertex[i*3+2] =(float) (OBJ->vertices[i * 3 + 2]) ;
	}

	//scale factor
	distance =(float) 2.0 / max(dx,max(dy,dz)); // normalize to [-1,1]
	
	
	m.translate( -1.0 * OBJ->position[0]  ,  -1.0 * (OBJ->position[1]) ,   -1.0 * (OBJ->position[2])   );
	m.scale(distance,distance,distance);
	m.translate( 0.0 ,  0.0 ,   0.0  );//why this step to see the model
	
	OBJ->position[0] = 0.0;
	OBJ->position[1] = 0.0;
	OBJ->position[2] = 0.0;
	//Assign the original coordinates to the triangle_vertex
	for(int i=0; i<(int)OBJ->numtriangles; i++) 
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
		triangle_vertex[k+0] = vertex[indv1*3+0];
		triangle_vertex[k+1] = vertex[indv1*3+1];
		triangle_vertex[k+2] = vertex[indv1*3+2];
		triangle_vertex[k+3] = vertex[indv2*3+0];
		triangle_vertex[k+4] = vertex[indv2*3+1];
		triangle_vertex[k+5] = vertex[indv2*3+2];		
		triangle_vertex[k+6] = vertex[indv3*3+0];
		triangle_vertex[k+7] = vertex[indv3*3+1];
		triangle_vertex[k+8] = vertex[indv3*3+2];
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

	///load second model
	minx = 10000000; 
	miny = 10000000; 
	minz = 10000000;
	maxx = -10000000; 
	maxy = -10000000; 
	maxz = -10000000;
	for (int i = 1; i <= (int)OBJ2->numvertices; i++){
		maxx = max(OBJ2->vertices[i * 3 + 0], maxx);
		maxy = max(OBJ2->vertices[i * 3 + 1], maxy);
		maxz = max(OBJ2->vertices[i * 3 + 2], maxz);
		minx = min(OBJ2->vertices[i * 3 + 0], minx);
		miny = min(OBJ2->vertices[i * 3 + 1], miny);
		minz = min(OBJ2->vertices[i * 3 + 2], minz);
	}
	dx = maxx - minx;
	dy = maxy - miny;
	dz = maxz - minz;
	
	OBJ2->position[0] = (maxx + minx) / 2.0f;
	OBJ2->position[1] = (maxy + miny) / 2.0f;
	OBJ2->position[2] = (maxz + minz) / 2.0f;	
	for (int i = 1; i<= (int) OBJ2->numvertices; i++)//NOTICE: vertex index starts from 1.
	{
		vertex2[i*3+0] =(float) (OBJ2->vertices[i * 3 + 0]) ;
		vertex2[i*3+1] =(float) (OBJ2->vertices[i * 3 + 1]) ;
		vertex2[i*3+2] =(float) (OBJ2->vertices[i * 3 + 2]) ;
	}
	//scale factor
	distance =(float) max(dx,max(dy,dz))/2.0;
	
	m2.translate( -1.0 * (OBJ2->position[0])  ,  -1.0 * (OBJ2->position[1]) ,   -1.0 * (OBJ2->position[2])   );
	m2.scale(1.0/distance,1.0/distance,1.0/distance);	
	
	OBJ2->position[0] = 0.0;
	OBJ2->position[1] = 0.0;
	OBJ2->position[2] = 0.0;
	for(int i=0; i<(int)OBJ2->numtriangles; i++) 
	{
		// the index of each vertex		
		int indv1 = OBJ2->triangles[i].vindices[0];
		int indv2 = OBJ2->triangles[i].vindices[1];
		int indv3 = OBJ2->triangles[i].vindices[2];											
		// the index of each color
		int indc1 = indv1;
		int indc2 = indv2;
		int indc3 = indv3;
		// vertices					
		triangle_vertex2[a+0] = vertex2[indv1*3+0];
		triangle_vertex2[a+1] = vertex2[indv1*3+1];
		triangle_vertex2[a+2] = vertex2[indv1*3+2];
		triangle_vertex2[a+3] = vertex2[indv2*3+0];
		triangle_vertex2[a+4] = vertex2[indv2*3+1];
		triangle_vertex2[a+5] = vertex2[indv2*3+2];		
		triangle_vertex2[a+6] = vertex2[indv3*3+0];
		triangle_vertex2[a+7] = vertex2[indv3*3+1];
		triangle_vertex2[a+8] = vertex2[indv3*3+2];
		color2[a+0] = OBJ2->colors[indv1*3+0];
		color2[a+1] = OBJ2->colors[indv1*3+1];
		color2[a+2] = OBJ2->colors[indv1*3+2];
		color2[a+3] = OBJ2->colors[indv2*3+0];
		color2[a+4] = OBJ2->colors[indv2*3+1];
		color2[a+5] = OBJ2->colors[indv2*3+2];
		color2[a+6] = OBJ2->colors[indv3*3+0];
		color2[a+7] = OBJ2->colors[indv3*3+1];
		color2[a+8] = OBJ2->colors[indv3*3+2];		
		a += 9;	
	}	

	///load third model
	minx = 10000000; 
	miny = 10000000; 
	minz = 10000000;
	maxx = -10000000; 
	maxy = -10000000; 
	maxz = -10000000;
	for (int i = 1; i <= (int)OBJ3->numvertices; i++){
		maxx = max(OBJ3->vertices[i * 3 + 0], maxx);
		maxy = max(OBJ3->vertices[i * 3 + 1], maxy);
		maxz = max(OBJ3->vertices[i * 3 + 2], maxz);
		minx = min(OBJ3->vertices[i * 3 + 0], minx);
		miny = min(OBJ3->vertices[i * 3 + 1], miny);
		minz = min(OBJ3->vertices[i * 3 + 2], minz);
	}
	dx = maxx - minx;
	dy = maxy - miny;
	dz = maxz - minz;
	
	OBJ3->position[0] = (maxx + minx) / 2.0f;
	OBJ3->position[1] = (maxy + miny) / 2.0f;
	OBJ3->position[2] = (maxz + minz) / 2.0f;	
	for (int i = 1; i<= (int) OBJ3->numvertices; i++)//NOTICE: vertex index starts from 1.
	{
		vertex3[i*3+0] =(float) (OBJ3->vertices[i * 3 + 0]) ;
		vertex3[i*3+1] =(float) (OBJ3->vertices[i * 3 + 1]) ;
		vertex3[i*3+2] =(float) (OBJ3->vertices[i * 3 + 2]) ;
	}
	//scale factor
	distance =(float) max(dx,max(dy,dz))/2.0;
	
	m3.translate( -1.0 * (OBJ3->position[0])  ,  -1.0 * (OBJ3->position[1]) ,   -1.0 * (OBJ3->position[2])   );
	m3.scale(1.0/distance,1.0/distance,1.0/distance);	
	
	OBJ3->position[0] = 0.0;
	OBJ3->position[1] = 0.0;
	OBJ3->position[2] = 0.0;	
	for(int i=0; i<(int)OBJ3->numtriangles; i++) 
	{
		// the index of each vertex		
		int indv1 = OBJ3->triangles[i].vindices[0];
		int indv2 = OBJ3->triangles[i].vindices[1];
		int indv3 = OBJ3->triangles[i].vindices[2];											
		// the index of each color
		int indc1 = indv1;
		int indc2 = indv2;
		int indc3 = indv3;
		// vertices					
		triangle_vertex3[b+0] = vertex3[indv1*3+0];
		triangle_vertex3[b+1] = vertex3[indv1*3+1];
		triangle_vertex3[b+2] = vertex3[indv1*3+2];
		triangle_vertex3[b+3] = vertex3[indv2*3+0];
		triangle_vertex3[b+4] = vertex3[indv2*3+1];
		triangle_vertex3[b+5] = vertex3[indv2*3+2];		
		triangle_vertex3[b+6] = vertex3[indv3*3+0];
		triangle_vertex3[b+7] = vertex3[indv3*3+1];
		triangle_vertex3[b+8] = vertex3[indv3*3+2];
		color3[b+0] = OBJ3->colors[indv1*3+0];
		color3[b+1] = OBJ3->colors[indv1*3+1];
		color3[b+2] = OBJ3->colors[indv1*3+2];
		color3[b+3] = OBJ3->colors[indv2*3+0];
		color3[b+4] = OBJ3->colors[indv2*3+1];
		color3[b+5] = OBJ3->colors[indv2*3+2];
		color3[b+6] = OBJ3->colors[indv3*3+0];
		color3[b+7] = OBJ3->colors[indv3*3+1];
		color3[b+8] = OBJ3->colors[indv3*3+2];		
		b += 9;	
	}	
}
void loadOBJModel()
{	
	OBJ = glmReadOBJ(filename);
	//strcpy(filename,filename1);
	OBJ2 = glmReadOBJ(filename1);
	//strcpy(filename,filename2);
	OBJ3 = glmReadOBJ(filename2);
	printf("Object1 %s\nObject2 %s\nObject3 %s\n", filename,filename2,filename3);
	vertex = (float*) malloc(sizeof(float) * 9 * (OBJ->numvertices));
	vertex2 = (float*) malloc(sizeof(float) * 9 * (OBJ->numvertices));
	vertex3 = (float*) malloc(sizeof(float) * 9 * (OBJ->numvertices));
	color = (float*) malloc(sizeof(float) * 9 * (OBJ->numtriangles));
	color2 = (float*) malloc(sizeof(float) * 9 * (OBJ2->numtriangles));
	color3 = (float*) malloc(sizeof(float) * 9 * (OBJ3->numtriangles));
	triangle_vertex = (float*) malloc (sizeof(float) * 9 * (OBJ->numtriangles));
	triangle_vertex2 = (float*) malloc (sizeof(float) * 9 * (OBJ2->numtriangles));
	triangle_vertex3 = (float*) malloc (sizeof(float) * 9 * (OBJ3->numtriangles));	
	// traverse the color model
	colorModel();
}
void renderScene(void)
{
	Matrix4 MVP = p * (v * m);
	Matrix4 MVP2 = p * (v * m2);
	Matrix4 MVP3 = p * (v * m3);	
		
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw first model
	glEnableVertexAttribArray(iLocPosition);
	glEnableVertexAttribArray(iLocColor);
		
	// Move example to left by 0.5 is to set aMVP[12] as -0.5
	aMVP[0] = MVP[0];	aMVP[4] = MVP[4];	aMVP[8] = MVP[8] ;	aMVP[12] = MVP[12];
	aMVP[1] = MVP[1];	aMVP[5] = MVP[5];	aMVP[9] = MVP[9] ;	aMVP[13] = MVP[13];
	aMVP[2] = MVP[2];	aMVP[6] = MVP[6];	aMVP[10] =MVP[10];	aMVP[14] = MVP[14];
	aMVP[3] = MVP[3];	aMVP[7] = MVP[7];	aMVP[11] =MVP[11];	aMVP[15] = MVP[15];	

	glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, triangle_vertex);
	glVertexAttribPointer(   iLocColor, 3, GL_FLOAT, GL_FALSE, 0, color);

	glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, aMVP);

	// draw the array we just bound
	glDrawArrays(GL_TRIANGLES, 0, 3*(OBJ->numtriangles));


	//draw second model
	glEnableVertexAttribArray(iLocPosition);
	glEnableVertexAttribArray(iLocColor);
		
	// Move example to left by 0.5 is to set aMVP[12] as -0.5
	aMVP2[0] = MVP2[0];	aMVP2[4] = MVP2[4];	aMVP2[8] = MVP2[8] ;	aMVP2[12] = MVP2[12];
	aMVP2[1] = MVP2[1];	aMVP2[5] = MVP2[5];	aMVP2[9] = MVP2[9] ;	aMVP2[13] = MVP2[13];
	aMVP2[2] = MVP2[2];	aMVP2[6] = MVP2[6];	aMVP2[10] =MVP2[10];	aMVP2[14] = MVP2[14];
	aMVP2[3] = MVP2[3];	aMVP2[7] = MVP2[7];	aMVP2[11] =MVP2[11];	aMVP2[15] = MVP2[15];	

	glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, triangle_vertex2);
	glVertexAttribPointer(   iLocColor, 3, GL_FLOAT, GL_FALSE, 0, color2);

	glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, aMVP2);

	// draw the array we just bound
	glDrawArrays(GL_TRIANGLES, 0, 3*(OBJ2->numtriangles));
	//draw third model
	glEnableVertexAttribArray(iLocPosition);
	glEnableVertexAttribArray(iLocColor);
		
	// Move example to left by 0.5 is to set aMVP[12] as -0.5
	aMVP3[0] = MVP3[0];	aMVP3[4] = MVP3[4];	aMVP3[8] = MVP3[8] ;	aMVP3[12] = MVP3[12];
	aMVP3[1] = MVP3[1];	aMVP3[5] = MVP3[5];	aMVP3[9] = MVP3[9] ;	aMVP3[13] = MVP3[13];
	aMVP3[2] = MVP3[2];	aMVP3[6] = MVP3[6];	aMVP3[10] =MVP3[10];	aMVP3[14] = MVP3[14];
	aMVP3[3] = MVP3[3];	aMVP3[7] = MVP3[7];	aMVP3[11] =MVP3[11];	aMVP3[15] = MVP3[15];	

	glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, triangle_vertex3);
	glVertexAttribPointer(   iLocColor, 3, GL_FLOAT, GL_FALSE, 0, color3);

	glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, aMVP3);

	// draw the array we just bound
	glDrawArrays(GL_TRIANGLES, 0, 3*(OBJ3->numtriangles));
	glutSwapBuffers();
}
void processMouse(int button, int state, int x, int y)
{
	mouseX = x;
    mouseY = y;

    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;
    }

    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }
	else if (button == GLUT_WHEEL_UP)
	{
		if(ctrl1){		
			m.translate( -1.0 * OBJ->position[0], -1.0 * OBJ->position[1], -1.0 * OBJ->position[2]);
			m.scale(1.01,1.01,1.01);				
			m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
		}
		else if (ctrl2){	
			m2.translate( -1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1], -1.0 * OBJ2->position[2]);
			m2.scale(1.01,1.01,1.01);		
			m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
		}
		else if (ctrl3){	
			m3.translate( -1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1], -1.0 * OBJ3->position[2]);
			m3.scale(1.01,1.01,1.01);				
			m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
		}	
	}
	else if (button == GLUT_WHEEL_DOWN)
	{
		if(ctrl1){		
			m.translate( -1.0 * OBJ->position[0], -1.0 * OBJ->position[1], -1.0 * OBJ->position[2]);
			m.scale(0.99,0.99,0.99);		
			m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
		}
		else if (ctrl2){	
			m2.translate( -1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1], -1.0 * OBJ2->position[2]);
			m2.scale(0.99,0.99,0.99);		
			m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
		}
		else if (ctrl3){	
			m3.translate( -1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1], -1.0 * OBJ3->position[2]);
			m3.scale(0.99,0.99,0.99);		
			m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
		}		
	}
	
	glutPostRedisplay();
}
void processMouseMotion(int x, int y){  // callback on mouse drag
	float dx = (float) (x - mouseX);
	float dy = (float) (y - mouseY);
	if(mouseLeftDown)
    {
		if(ctrl1){
			m.translate( dx/400.0 ,-1.0 * dy / 400.0 , 0.0);
			OBJ->position[0] += dx / 400.0;
			OBJ->position[1] -= dy / 400.0;
		}
		else if (ctrl2){	
			m2.translate( dx/400.0 ,-1.0 * dy / 400.0 , 0.0);
			OBJ2->position[0] += dx / 400.0;
			OBJ2->position[1] -= dy / 400.0;
		}
		else if (ctrl3)	{
			m3.translate( dx/400.0 ,-1.0 * dy / 400.0 , 0.0);
			OBJ3->position[0] += dx / 400.0;
			OBJ3->position[1] -= dy / 400.0;
		}
        mouseX = x;
        mouseY = y;
    }
    if(mouseRightDown)
    {
		if(ctrl1){
			m.translate( -1.0 * OBJ->position[0], -1.0 * OBJ->position[1], -1.0 * OBJ->position[2]);			
			m.rotateX( dy / 10.0);
			m.rotateY( dx / 10.0);
			m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
		}
		else if (ctrl2)	{
			m2.translate( -1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1], -1.0 * OBJ2->position[2]);
			m2.rotateX( dy / 10.0);
			m2.rotateY( dx / 10.0);
			m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
		}
		else if (ctrl3)	{
			m3.translate( -1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1], -1.0 * OBJ3->position[2]);
			m3.rotateX( dy / 10.0);
			m3.rotateY( dx / 10.0);
			m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
		}
		mouseX = x;
        mouseY = y;
    }
	glutPostRedisplay();
}
void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 49: /* 1 */ 
			printf("Controlling %s\n",filename0);
			ctrl1 = true;
			ctrl2 = false;
			ctrl3 = false;
			break;
		case 50: /* 2 */ 
			printf("Controlling %s\n",filename1);
			ctrl1 = false;
			ctrl2 = true;
			ctrl3 = false;
			break;
		case 51: /* 3 */ 
			printf("Controlling %s\n",filename2);
			ctrl1 = false;
			ctrl2 = false;
			ctrl3 = true;
			break;
		case 27: /* the Esc key */ 
			printf("Farewell!");
			exit(0); 
			break;
		case 72: //H key
			printf("PRESS m for frame mode switching\n");
			printf("PRESS o for Orthogonal Projection\nPress p for Perspective Projection\n");
			printf("PRESS 1, 2, or 3 to select model you want to play.\n");
			printf("PRESS g for Geometrical Transformation and then\n\tPress s for Scaling\n\tPress t for Translation\n\tPress r for Rotation\n\tPress X/x, Y/y, Z/z to increase/decrease the index value\n");
			printf("PRESS v for Geometrical Transformation and then\n\tPress e to change the EYE position\n\tPress c to change the Center of object\n\tPress u for UP Vector Changing\n\tPress X/x, Y/y, Z/z to increase/decrease the index value\n");
			break;		
		case 109://m
			if (mode == GL_LINE) mode = GL_FILL;
			else mode = GL_LINE;
			glPolygonMode(GL_FRONT_AND_BACK, mode);
			break;			
		case 103://g
			printf("Performing Transformation\n");
			isGeometrical = true;
			isViewing = false;
			break;
		case 118://v
			printf("Performing Viewing Transformation\n");
			isGeometrical = false;
			isViewing = true;
			break;
		case 115://s
			if (isGeometrical){ 
				printf("Performing Geometrical Scaling\n");
				isScaling = true;
				isRotating = false;
				isTranslating = false;
			}
			break;
		case 116://t
			if (isGeometrical){ 
				printf("Performing Geometrical Translating\n");
				isScaling = false;
				isRotating = false;
				isTranslating = true;
			}
			break;
		case 114://r
			if (isGeometrical){ 
				printf("Performing Geometrical Rotating\n");
				isScaling = false;
				isRotating = true;
				isTranslating = false;
			}
			break;
		case 101://e
			if (isViewing){ 
				printf("Changing Eye Position\n");
				eyeChanging = true;
				centerChanging = false;
				upChanging = false;
			}
			break;
		case 99://c 
			if (isViewing) {
				printf("Changing Center Position\n");
				eyeChanging = false;
				centerChanging = true;
				upChanging = false;
			}
			break;
		case 117://u 
			if (isViewing){ 
				printf("Changing Up Vector\n");
				eyeChanging = false;
				centerChanging = false;
				upChanging = true;
			}
			break;
		case 120://x 
			printf("X index decreasing \n");
			if (isGeometrical && isScaling){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.scale(0.99,1.0,1.0);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.scale(0.99,1.0,1.0);		
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.scale(0.99,1.0,1.0);		
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}	
			}
			else if(isGeometrical && isRotating){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.rotateX(-0.1);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.rotateX(-0.1);	
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.rotateX(-0.1);	
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}
			}			
						
			else if(isGeometrical && isTranslating){
				if(ctrl1){		  m.translate(-0.01,0.0,0.0); OBJ->position[0] -= 0.01;}
				else if (ctrl2){ m2.translate(-0.01,0.0,0.0);OBJ2->position[0] -= 0.01;}
				else if (ctrl3){ m3.translate(-0.01,0.0,0.0);OBJ3->position[0] -= 0.01;}
			}				
			else if(isViewing && eyeChanging){
				eyeX -= 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && centerChanging){
				cenX -= 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && upChanging){	
				upX -= 0.01;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			break;
		case 121://y 
			printf("Y index decreasing \n");
			if (isGeometrical && isScaling){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.scale(1.0,0.99,1.0);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.scale(1.0,0.99,1.0);		
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.scale(1.0,0.99,1.0);		
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}	
			}
			else if(isGeometrical && isRotating){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.rotateY(-0.1);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.rotateY(-0.1);	
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.rotateY(-0.1);	
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}
			}			
						
			else if(isGeometrical && isTranslating){
				if(ctrl1){		  m.translate(0.0,-0.01,0.0); OBJ->position[1] -= 0.01;}
				else if (ctrl2){ m2.translate(0.0,-0.01,0.0);OBJ2->position[1] -= 0.01;}
				else if (ctrl3){ m3.translate(0.0,-0.01,0.0);OBJ3->position[1] -= 0.01;}
			}
			else if(isViewing && eyeChanging){			
				eyeY -= 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && centerChanging){
				cenY -= 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && upChanging){
				upY -= 0.01;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			break;
		case 122://z
			printf("Z index decreasing \n");
			if (isGeometrical && isScaling){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.scale(1.0,1.0,0.99);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.scale(1.0,1.0,0.99);		
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.scale(1.0,1.0,0.99);		
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}	
			}
			else if(isGeometrical && isRotating){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.rotateZ(-0.1);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.rotateZ(-0.1);	
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.rotateZ(-0.1);	
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}
			}			
						
			else if(isGeometrical && isTranslating){
				if(ctrl1){		  m.translate(0.0,0.0,-0.01); OBJ->position[2] -= 0.01;}
				else if (ctrl2){ m2.translate(0.0,0.0,-0.01);OBJ2->position[2] -= 0.01;}
				else if (ctrl3){ m3.translate(0.0,0.0,-0.01);OBJ3->position[2] -= 0.01;}
			}
			else if(isViewing && eyeChanging){			
				eyeZ -= 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && centerChanging){
				cenZ -= 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && upChanging){
				upZ -= 0.01;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			break;
		case 88://X 
			printf("X index increasing \n");
			if (isGeometrical && isScaling){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.scale(1.01,1.0,1.0);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.scale(1.01,1.0,1.0);		
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.scale(1.01,1.0,1.0);		
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}	
			}
			else if(isGeometrical && isRotating){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.rotateX(0.1);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.rotateX(0.1);	
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.rotateX(0.1);	
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}
			}			
						
			else if(isGeometrical && isTranslating){
				if(ctrl1){		  m.translate(0.01,0.0,0.0); OBJ->position[0] += 0.01;}
				else if (ctrl2){ m2.translate(0.01,0.0,0.0);OBJ2->position[0] += 0.01;}
				else if (ctrl3){ m3.translate(0.01,0.0,0.0);OBJ3->position[0] += 0.01;}
			}
			else if(isViewing && eyeChanging){
				eyeX += 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && centerChanging){
				cenX += 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && upChanging){
				upX += 0.01;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -1.0 * eyeX;
				t[1][3] = -1.0 * eyeY;
				t[2][3] = -1.0 * eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			break;
		case 89://Y 
			printf("Y index increasing \n");
			if (isGeometrical && isScaling){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.scale(1.0,1.01,1.0);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.scale(1.0,1.01,1.0);		
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.scale(1.0,1.01,1.0);		
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}	
			}
			else if(isGeometrical && isRotating){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.rotateY(0.1);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.rotateY(0.1);	
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.rotateY(0.1);	
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}
			}			
						
			else if(isGeometrical && isTranslating){
				if(ctrl1){		  m.translate(0.0,0.01,0.0); OBJ->position[1] += 0.01;}
				else if (ctrl2){ m2.translate(0.0,0.01,0.0);OBJ2->position[1] += 0.01;}
				else if (ctrl3){ m3.translate(0.0,0.01,0.0);OBJ3->position[1] += 0.01;}
			}
			else if(isViewing && eyeChanging){
				eyeY += 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && centerChanging){
				cenY += 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && upChanging){
				upY += 0.01;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}			
			}
			break;
		case 90://Z
			printf("Z index increasing \n");
			if (isGeometrical && isScaling){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.scale(1.0,1.0,1.01);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.scale(1.0,1.0,1.01);		
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.scale(1.0,1.0,1.01);		
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}	
			}
			else if(isGeometrical && isRotating){
				if(ctrl1){							
					m.translate(-1.0 * OBJ->position[0], -1.0 * OBJ->position[1] , -1.0 * OBJ->position[2]);
					m.rotateZ(0.1);		
					m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
				}
				else if (ctrl2){		
					m2.translate(-1.0 * OBJ2->position[0], -1.0 * OBJ2->position[1] , -1.0 * OBJ2->position[2]);
					m2.rotateZ(0.1);	
					m2.translate( OBJ2->position[0], OBJ2->position[1], OBJ2->position[2]);
				}	
				else if (ctrl3){		
					m3.translate(-1.0 * OBJ3->position[0], -1.0 * OBJ3->position[1] , -1.0 * OBJ3->position[2]);
					m3.rotateZ(0.1);	
					m3.translate( OBJ3->position[0], OBJ3->position[1], OBJ3->position[2]);
				}
			}			
						
			else if(isGeometrical && isTranslating){
				if(ctrl1){		  m.translate(0.0,0.0,0.01); OBJ->position[2] += 0.01;}
				else if (ctrl2){ m2.translate(0.0,0.0,0.01);OBJ2->position[2] += 0.01;}
				else if (ctrl3){ m3.translate(0.0,0.0,0.01);OBJ3->position[2] += 0.01;}
			}
			else if(isViewing && eyeChanging){
				eyeZ += 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;				
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && centerChanging){
				cenZ += 0.05;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}
			else if(isViewing && upChanging){
				upZ += 0.01;
				MAT r(4),t(4),view(4);				
				Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
				for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
				t[0][3] = -eyeX;
				t[1][3] = -eyeY;
				t[2][3] = -eyeZ;
				
				p1p2.x = eyeX - cenX;
				p1p2.y = eyeY - cenY;
				p1p2.z = eyeZ - cenZ;				
				Rz.x = r[2][0] = p1p2.x/p1p2.length();
				Rz.y = r[2][1] = p1p2.y/p1p2.length();
				Rz.z = r[2][2] = p1p2.z/p1p2.length();
				up.x = upX;
				up.y = upY;
				up.z = upZ;
				p1p2p3 = up.cross(p1p2);
				Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
				Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
				Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
				Ry = Rz.cross(Rx);
				r[1][0]  = Ry.x;
				r[1][1]	 = Ry.y;
				r[1][2]	 = Ry.z;
				view = r*t;
				for (int i = 0;i<4;i++){
					for (int j = 0;j<4;j++)
						v[4*i+j] = view[j][i];	
				}
			}	
			break;
		case 111://o	projection matrix is modified here
			printf("Orthogonal Projection\n");
			p = p.identity();//refresh projection matrix
			p[0] = 2.0/(right - left);
			p[5] = 2.0/(top -bottom); 
			p[12] = -1.0 * (right + left) / (right - left);
			p[13] = -1.0 * (top + bottom) / (top - bottom);
			p[10] = -2.0/(far - near);//
			p[14] = -1.0 * (far + near) / (far - near);
			break; 
		case 112://p	projection matrix is modified here
			printf("Perspective Projection\n");
			p = p.identity();//refresh projection matrix
			p[0] = (2.0 * near)/(right - left);
			p[5] = (2.0 * near)/(top - bottom);
			p[8] = (right + left) / (right - left);
			p[9] = (top + bottom) / (top - bottom);
			p[10] = (-1.0 * (far + near)) /(far - near);
			p[14] = (-2.0 * far * near) / (far - near);
			p[11] = -1.0;// remember to set to -1
			p[15] = 0.0;//remember to set to 0
			break;
	}
	printf("\n");
	glutPostRedisplay();
}
int main(int argc, char **argv) {
	{	//initial projection and viewing matrices	
		p[0] = 2.0/(right - left);
		p[5] = 2.0/(top -bottom); 
		p[12] = -1.0 * (right + left) / (right - left);
		p[13] = -1.0 * (top + bottom) / (top - bottom);
		p[10] = -2.0/(far - near);
		p[14] = -1.0 * (far + near) / (far - near);
		MAT r(4),t(4),view(4);				
		Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
		for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
		t[0][3] = (-1.0) * eyeX;		
		t[1][3] = (-1.0) * eyeY;		
		t[2][3] = (-1.0) * eyeZ;	
		
		p1p2.x = eyeX - cenX;
		p1p2.y = eyeY - cenY;
		p1p2.z = eyeZ - cenZ;				
		Rz.x = r[2][0] = p1p2.x/p1p2.length();
		Rz.y = r[2][1] = p1p2.y/p1p2.length();
		Rz.z = r[2][2] = p1p2.z/p1p2.length();
		up.x = upX;
		up.y = upY;
		up.z = upZ;
		p1p2p3 = up.cross(p1p2);
		Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
		Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
		Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
		Ry = Rz.cross(Rx);
		r[1][0]  = Ry.x;
		r[1][1]	 = Ry.y;
		r[1][2]	 = Ry.z;
		
		view = r*t;
		for (int i = 0;i<4;i++){
			for (int j = 0;j<4;j++){
				v[4*i+j] = view[j][i];	
				printf("%f ",view[i][j]);
			}
			printf("\n");
		}		
	}


	// glut init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// create window
	glutInitWindowPosition(460, 40);
	glutInitWindowSize(800, 800);
	glutCreateWindow("10320 CS550000 CG HW2 100060007");

	glewInit();
	if(glewIsSupported("GL_VERSION_2_0")){
		printf("Ready for OpenGL 2.0\n");
	}else{
		printf("OpenGL 2.0 not supported\n");
		system("pause");
		exit(1);
	}
	printf("==================\nPRESS H for usage\n==================\n");
	printf("Displayed models are:\n(1)brain18KC\n(2)elephant16KC\n(3)teapot4KC\n");
	// load obj models through glm
	loadOBJModel();
	//initialize projection mode to Orthogonal mode

	//Set default polygon mode
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

	return 0;
}

