//Computer Graphics HW3
//Ming-Chang Chiu
//2015.5.17
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

#define far  1.0	  //far plane
#define near  -1.0	  //near plane
#define left  -1.0	  //left plane
#define right  1.0	  //right plane
#define top  1.0	  //top plane
#define bottom  -1.0  //bottom plane

#ifndef max
# define max(a,b) (((a)>(b))?(a):(b))
# define min(a,b) (((a)<(b))?(a):(b))
#endif
GLuint program;
// Shader attributes
GLint iLocPosition;
GLint iLocMVP, iLocModMat4, iLocinvTransposeModMat3;
GLint iLocNormal;//Store Location of shader vars
GLint iLocMDiffuse[40],iLocMSpecular[40],iLocMShininess[40],iLocMAmbient[40] ;
GLint iLocLAmbient_0, iLocLDiffuse_0, iLocLSpecular_0, iLocLConsAtt_0, iLocLLinStt_0, iLocLPosition_0, iLocLSpotDir_0, iLocLSpotCosCutOff_0, iLocLSpotCutOff_0,iLocLSpotExp_0;
GLint iLocLAmbient_1, iLocLDiffuse_1, iLocLSpecular_1, iLocLConsAtt_1, iLocLLinStt_1, iLocLPosition_1, iLocLSpotDir_1, iLocLSpotCosCutOff_1, iLocLSpotCutOff_1,iLocLSpotExp_1;
GLint iLocLAmbient_2, iLocLDiffuse_2, iLocLSpecular_2, iLocLConsAtt_2, iLocLLinStt_2, iLocLPosition_2, iLocLSpotDir_2, iLocLSpotCosCutOff_2, iLocLSpotCutOff_2,iLocLSpotExp_2;
GLint iLocRotMat,iLocinvViewMat4;
GLint iLocEyeDir, iLocEyePos;
GLint iLocdirOn , iLocpointOn , iLocspotOn ;
GLfloat *normals;//Do not forget to malloc() them.
Matrix4 m, v, p,m2,m3;
//directional light
GLfloat light0_position[4]         = {0.0f, 0.0f, 1.0f, 0.0f};
GLfloat light0_ambient[4]         = {0.5f, 0.5f, 0.5f, 1.0f};
//GLfloat light0_ambient[4]         = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat light0_diffuse[4]           = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat light0_specular[4]        = {1.0f, 1.0f, 1.0f, 1.0f};
//point light
GLfloat light1_position[4]         = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat light1_ambient[4]         = {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat light1_diffuse[4]           = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat light1_specular[4]        = {1.0f, 1.0f, 1.0f, 1.0f};
//spot light
GLfloat light2_position[4]         = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat light2_ambient[4]         = {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat light2_diffuse[4]           = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat light2_specular[4]        = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat light2_spot_dir[3]         = {0.0f, 0.0f, -1.0f};
GLfloat light2_spot_cos_cut_off   = 0.96f; //cosf(8.1301f / 180.0f * M_PI * 2.0f);
GLfloat light2_spot_cut_off   = 8.1301f; //cosf(8.1301f / 180.0f * M_PI * 2.0f);
GLfloat light2_spot_exp            = 2.0f;
GLfloat material_ambient[40][4],eyeDir[4],eyePos[4];
GLfloat material_diffuse[40][4],material_tambient[4],tModel[9];
GLfloat material_specular[40][4],material_shininess[40];
GLMmodel* OBJ;
GLMmodel* OBJ2;
GLMmodel* OBJ3;
GLMgroup* group  ;
GLfloat aMVP[16],aM3[9];
GLfloat aM[16],aV[16];
bool isFreeing = false;
bool isGeometrical = true, isViewing = false;
bool isScaling = false,isTranslating = false,isRotating = false; 
bool eyeChanging = false, centerChanging = false, upChanging = false;
bool ctrl1 = true, ctrl2 = false, ctrl3 = false;
bool dirOn = true, pointOn = false, spotOn = false;
bool mouseLeftDown, mouseRightDown, mouseWheelUp, mouseWheelDown;
float mouseX, mouseY;
char filename[40] = "NormalModels/Medium/duck4KN.obj";
int groupNum = 0;
int groupTriangleNum[100];
//char filename[40] = "NormalModels/High/brain18KN.obj";
char filename0[] =  "NormalModels/High/brain18KN.obj";
char filename2[] =  "NormalModels/Medium/teapot4KN.obj";
char filename3[] =  "NormalModels/Low/boxN.obj";
char filename1[] =  "NormalModels/High/elephant16KN.obj";
char filename4[] =  "NormalModels/Low/shuttleN.obj";
int mode = GL_FILL;
float eyeX = 0.0, eyeY = 0.0, eyeZ = 2.0;
float cenX = 0.0, cenY = 0.0, cenZ = 0.0;
float upX = 0.0, upY = 1.0, upZ = 0.0;
float *vertex ;
float *triangle_vertex;

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
	GLuint v, f;
	char *vs = NULL;
	char *fs = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	//vs = textFileRead("shader2.vert");
	vs = textFileRead("sample.vert");
	fs = textFileRead("sample.frag");

	glShaderSource(v, 1, (const GLchar**)&vs, NULL);
	glShaderSource(f, 1, (const GLchar**)&fs, NULL);

	free(vs);
	free(fs);

	// compile vertex shader
	glCompileShader(v);
	GLint vShaderCompiled;
	showShaderCompileStatus(v, &vShaderCompiled);
	if(!vShaderCompiled) system("pause"), exit(123);
	else printf("vert success\n");

	// compile fragment shader
	glCompileShader(f);
	GLint fShaderCompiled;
	showShaderCompileStatus(f, &fShaderCompiled);
	if(!fShaderCompiled) system("pause"), exit(456);
	else printf("frag success\n");

	program = glCreateProgram();

	// bind shader
	glAttachShader(program, f);
	glAttachShader(program, v);

	// link program
	glLinkProgram(program);
	iLocdirOn = glGetUniformLocation(program, "isDir");
	iLocpointOn = glGetUniformLocation(program, "isPoint");
	iLocspotOn = glGetUniformLocation(program, "isSpot");	
	iLocPosition = glGetAttribLocation (program, "av4position");
	iLocNormal   = glGetAttribLocation (program, "av3normal");	
	iLocEyePos = glGetUniformLocation(program, "eyePos");
	/*iLocPosition = glGetAttribLocation (program, "v_coord");
	iLocNormal   = glGetAttribLocation (program, "v_normal");*/

	iLocMVP		= glGetUniformLocation(program,"mvp");
	//iLocinvViewMat4		= glGetUniformLocation(program,"v_inv");
	for (int i = 0; i< 40;i++){
		iLocMAmbient[i] = glGetUniformLocation(program, "Material.ambient");
		iLocMDiffuse[i]  = glGetUniformLocation(program, "Material.diffuse");
		iLocMSpecular[i] = glGetUniformLocation(program, "Material.specular");
		iLocMShininess[i] = glGetUniformLocation(program, "Material.shininess");
	}
	iLocModMat4 = glGetUniformLocation(program, "m");
	//iLocinvTransposeModMat3 = glGetUniformLocation(program, "m_3x3_inv_transp");
		
	iLocLAmbient_0 = glGetUniformLocation(program, "LightSource[0].ambient");	
	iLocLAmbient_1 = glGetUniformLocation(program, "LightSource[1].ambient");
	iLocLAmbient_2 = glGetUniformLocation(program, "LightSource[2].ambient");
	iLocLDiffuse_0 = glGetUniformLocation(program, "LightSource[0].diffuse");
	iLocLDiffuse_1 = glGetUniformLocation(program, "LightSource[1].diffuse");
	iLocLDiffuse_2 = glGetUniformLocation(program, "LightSource[2].diffuse");
	iLocLSpecular_0 = glGetUniformLocation(program, "LightSource[0].specular");
	iLocLSpecular_1 = glGetUniformLocation(program, "LightSource[1].specular");
	iLocLSpecular_2 = glGetUniformLocation(program, "LightSource[2].specular");	
	iLocLPosition_0 = glGetUniformLocation(program, "LightSource[0].position");
	iLocLPosition_1 = glGetUniformLocation(program, "LightSource[1].position");
	iLocLPosition_2 = glGetUniformLocation(program, "LightSource[2].position");	
	iLocLSpotDir_2 = glGetUniformLocation(program, "LightSource[2].spotDirection");
	iLocLSpotCosCutOff_2= glGetUniformLocation(program, "LightSource[2].spotCosCutoff");
	iLocLSpotCutOff_2 = glGetUniformLocation(program, "LightSource[2].spotCutoff");		
	iLocLSpotExp_2 = glGetUniformLocation(program, "LightSource[2].spotExponent");
	glUseProgram(program);
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
	/*for(int i = 0;i< 16;i++)
		printf("%lf\n",m[i]);
	printf("pos: %lf\n",OBJ->position[0]);
	printf("pos: %lf\n",OBJ->position[1]);
	printf("pos: %lf\n",OBJ->position[2]);
	*/
	OBJ->position[0] = 0.0;
	OBJ->position[1] = 0.0;
	OBJ->position[2] = 0.0;
	
	
	while(group)
	{			
		material_ambient[groupNum][0] = OBJ->materials[group->material].ambient[0];
		material_ambient[groupNum][1] = OBJ->materials[group->material].ambient[1];
		material_ambient[groupNum][2] = OBJ->materials[group->material].ambient[2];
		material_ambient[groupNum][3] = OBJ->materials[group->material].ambient[3];
		material_diffuse[groupNum][0] = OBJ->materials[group->material].diffuse[0];
		material_diffuse[groupNum][1] = OBJ->materials[group->material].diffuse[1];
		material_diffuse[groupNum][2] = OBJ->materials[group->material].diffuse[2];
		material_diffuse[groupNum][3] = OBJ->materials[group->material].diffuse[3];
		material_specular[groupNum][0] = OBJ->materials[group->material].specular[0];
		material_specular[groupNum][1] = OBJ->materials[group->material].specular[1];
		material_specular[groupNum][2] = OBJ->materials[group->material].specular[2];
		material_specular[groupNum][3] = OBJ->materials[group->material].specular[3];
		material_shininess[groupNum] = OBJ->materials[group->material].shininess;
		
		groupTriangleNum[groupNum] = group->numtriangles;
		for(int i = 0; i<(int)group->numtriangles; i++) 
		{

			//Triangle index
			int triangleID = group->triangles[i];

			//the index of each vertices			
			int indv1 = OBJ->triangles[triangleID].vindices[0];
			int indv2 = OBJ->triangles[triangleID].vindices[1];
			int indv3 = OBJ->triangles[triangleID].vindices[2];
			//Assign the original coordinates to the triangle_vertex			
			triangle_vertex[k+0] = vertex[indv1*3+0];
			triangle_vertex[k+1] = vertex[indv1*3+1];
			triangle_vertex[k+2] = vertex[indv1*3+2];
			triangle_vertex[k+3] = vertex[indv2*3+0];
			triangle_vertex[k+4] = vertex[indv2*3+1];
			triangle_vertex[k+5] = vertex[indv2*3+2];		
			triangle_vertex[k+6] = vertex[indv3*3+0];
			triangle_vertex[k+7] = vertex[indv3*3+1];
			triangle_vertex[k+8] = vertex[indv3*3+2];
			//the index of the face normal of the triangle i
			
			//the index of each normals
			int indn1 = OBJ->triangles[triangleID].nindices[0];
			int indn2 = OBJ->triangles[triangleID].nindices[1];
			int indn3 = OBJ->triangles[triangleID].nindices[2];						
			
			// the vertex normal
			normals[k+0] = OBJ->normals[indn1*3];
			normals[k+1] = OBJ->normals[indn1*3+1];
			normals[k+2] = OBJ->normals[indn1*3+2];
			normals[k+3] = OBJ->normals[indn2*3];
			normals[k+4] = OBJ->normals[indn2*3+1];
			normals[k+5] = OBJ->normals[indn2*3+2];
			normals[k+6] = OBJ->normals[indn3*3];
			normals[k+7] = OBJ->normals[indn3*3+1];
			normals[k+8] = OBJ->normals[indn3*3+2];			
			k += 9;	

		}	
		groupNum++;
		group = group->next;
	}
	glUniform4fv( iLocLAmbient_0, 1, light0_ambient);		
	glUniform4fv( iLocLDiffuse_0, 1, light0_diffuse);
	glUniform4fv(iLocLSpecular_0, 1, light0_specular);	
	glUniform4fv(iLocLPosition_0, 1, light0_position);		
	glUniform4fv( iLocLAmbient_1, 1, light1_ambient);
	glUniform4fv( iLocLDiffuse_1, 1, light1_diffuse);
	glUniform4fv(iLocLSpecular_1, 1, light1_specular);	
	glUniform4fv(iLocLPosition_1, 1, light1_position);	
	glUniform4fv( iLocLAmbient_2, 1, light2_ambient);		
	glUniform4fv( iLocLDiffuse_2, 1, light2_diffuse);
	glUniform4fv(iLocLSpecular_2, 1, light2_specular);	
	glUniform4fv(iLocLPosition_2, 1, light2_position);	
	glUniform1f(iLocLSpotCutOff_2, light2_spot_cut_off);
	glUniform1f(iLocLSpotCosCutOff_2, light2_spot_cos_cut_off);
	glUniform1f(iLocLSpotExp_2, light2_spot_exp);
	glUniform3fv(iLocLSpotDir_2, 1, light2_spot_dir);

}
void loadOBJModel()
{	
	OBJ = glmReadOBJ(filename);
	group = OBJ->groups;
	vertex = (float*) malloc(sizeof(float) * 9 * (OBJ->numvertices));	
	triangle_vertex = (float*) malloc (sizeof(float) * 9 * (OBJ->numtriangles));
	normals = (float*) malloc (sizeof(float) * 9 * (OBJ->numtriangles));
	// traverse the color model
	colorModel();
}
void renderScene(void)
{
	Matrix4 MVP = p * (v * m);		
		
	aM[0] = m[0];	aM[4] = m[4];	aM[8] = m[8] ;	aM[12] = m[12];
	aM[1] = m[1];	aM[5] = m[5];	aM[9] = m[9] ;	aM[13] = m[13];
	aM[2] = m[2];	aM[6] = m[6];	aM[10]= m[10];	aM[14] = m[14];
	aM[3] = m[3];	aM[7] = m[7];	aM[11]= m[11];	aM[15] = m[15];	

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Matrix4 invV = v.invert();
	aV[0] = invV[0];	aV[4] = invV[4];	aV[8] = invV[8] ;	aV[12] = invV[12];
	aV[1] = invV[1];	aV[5] = invV[5];	aV[9] = invV[9] ;	aV[13] = invV[13];
	aV[2] = invV[2];	aV[6] = invV[6];	aV[10]= invV[10];	aV[14] = invV[14];
	aV[3] = invV[3];	aV[7] = invV[7];	aV[11]= invV[11];	aV[15] = invV[15];	
	/*
	Matrix4 invTransM = (m.invert()).transpose();
	aM3[0] = invTransM[0];	aM3[3] = invTransM[4];	aM3[6] = invTransM[8] ;
	aM3[1] = invTransM[1];	aM3[4] = invTransM[5];	aM3[7] = invTransM[9] ;
	aM3[2] = invTransM[2];	aM3[5] = invTransM[6];	aM3[8] =invTransM[10];*/

	aMVP[0] = MVP[0];	aMVP[4] = MVP[4];	aMVP[8] = MVP[8] ;	aMVP[12] = MVP[12];
	aMVP[1] = MVP[1];	aMVP[5] = MVP[5];	aMVP[9] = MVP[9] ;	aMVP[13] = MVP[13];
	aMVP[2] = MVP[2];	aMVP[6] = MVP[6];	aMVP[10] =MVP[10];	aMVP[14] = MVP[14];
	aMVP[3] = MVP[3];	aMVP[7] = MVP[7];	aMVP[11] =MVP[11];	aMVP[15] = MVP[15];	

	eyeDir[0] = eyeX-cenX; eyeDir[1] = eyeY-cenY; eyeDir[2] = eyeZ-cenZ; eyeDir[3] = 0.0;
	eyePos[0] = eyeX; eyePos[1] = eyeY; eyePos[2] = eyeZ; eyePos[3] = 0.0;
	glEnableVertexAttribArray(iLocPosition);
	glEnableVertexAttribArray(iLocNormal);
	for(int i = 0; i < groupNum-1; i++){	
		glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, triangle_vertex);
		glVertexAttribPointer(  iLocNormal, 3, GL_FLOAT, GL_FALSE, 0, normals);
		glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, aMVP);
		glUniformMatrix4fv(iLocModMat4, 1, GL_FALSE,aM);
		glUniform4fv(iLocEyePos, 1,  eyePos);
		//glUniformMatrix4fv(iLocinvViewMat4, 1, GL_FALSE,aV);
		//glUniformMatrix3fv(iLocinvTransposeModMat3, 1, GL_FALSE,aM3);
						
			
		glUniform4fv( iLocMDiffuse[i] ,1, material_diffuse[i]);
		glUniform4fv( iLocMAmbient[i] ,1, material_ambient[i]);
		glUniform4fv(iLocMSpecular[i] ,1, material_specular[i]);
		glUniform1f(iLocMShininess[i] ,  material_shininess[i]);
		
		glDrawArrays(GL_TRIANGLES, 0, 3*groupTriangleNum[i]);
	}
	glutSwapBuffers();
}
void processMouse(int button, int state, int x, int y)
{
	mouseX = x;
    mouseY = y;
	glGetUniformfv(program,iLocinvTransposeModMat3,tModel);
	/*for (int i  = 0;i<9;i++)
		printf("%f\n",tModel[i]);
	

	printf("positinoX: %lf\n",OBJ->position[0]);
	printf("positinoY: %lf\n",OBJ->position[1]);
	printf("positinoZ: %lf\n",OBJ->position[2]);*/
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
	if(mouseLeftDown){
		if(ctrl1){
			m.translate( dx/400.0 ,-1.0 * dy / 400.0 , 0.0);
			OBJ->position[0] += dx / 400.0;
			OBJ->position[1] -= dy / 400.0;
		}				
        mouseX = x;
        mouseY = y;
    }
	
    if(mouseRightDown){
		if(ctrl1){
			m.translate( -1.0 * OBJ->position[0], -1.0 * OBJ->position[1], -1.0 * OBJ->position[2]);			
			m.rotateX( dy / 10.0);
			m.rotateY( dx / 10.0);
			m.translate( OBJ->position[0], OBJ->position[1], OBJ->position[2]);
		}		
		mouseX = x;
        mouseY = y;
    }
	glutPostRedisplay();
}
void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {

		case 97://a
			light0_position[0] -= 0.5;
			printf("Directinoal light shift left by 0.5\n");
			glUniform4fv(iLocLPosition_0, 1, light0_position);
			break;
		case 119://w
			light1_position[0] -= 0.5;
			printf("Point light shift left by 0.5\n");
			glUniform4fv(iLocLPosition_1, 1, light1_position);
			break;
		case 100://d
			light2_position[0] -= 0.5;
			printf("Spotlight shift left by 0.5\n");
			glUniform4fv(iLocLPosition_2, 1, light2_position);
			break;
		case 65://A
			light0_position[0] += 0.5;
			printf("Directinoal light shift right by 0.5\n");
			glUniform4fv(iLocLPosition_0, 1, light0_position);
			break;
		case 87://W
			light1_position[0] += 0.5;
			printf("Point light shift right by 0.5\n");
			glUniform4fv(iLocLPosition_1, 1, light1_position);
			break;
		case 68://D
			light2_position[0] += 0.5;
			printf("Spotlight shift right by 0.5\n");
			glUniform4fv(iLocLPosition_2, 1, light2_position);
			break;

		case 52://4
			light2_spot_exp -= 0.5;
			printf("Spot Exponent: %lf\n",light2_spot_exp);
			glUniform1f(iLocLSpotExp_2, light2_spot_exp);
			break;
		case 54://6
			light2_spot_exp += 0.5;
			printf("Spot Exponent: %lf\n",light2_spot_exp);
			glUniform1f(iLocLSpotExp_2, light2_spot_exp);
			break;
		case 55://7
			if (dirOn) glUniform1i(iLocdirOn , 0),dirOn = false,printf("Directional light off\n");
			else glUniform1i(iLocdirOn , 1),dirOn = true,printf("Directional light On\n");
			break;
		case 57://9
			if (spotOn) glUniform1i(iLocspotOn , 0),spotOn = false,printf("Spot light off\n");
			else glUniform1i(iLocspotOn , 1),spotOn = true,printf("Spotlight On\n");
			break;
		case 56://8
			if (pointOn) glUniform1i(iLocpointOn , 0),pointOn = false,printf("Point light off\n");
			else glUniform1i(iLocpointOn , 1),pointOn = true,printf("Point light On\n");
			break;
		case 27: /* the Esc key */ 
			printf("Farewell!");
			exit(0); 
			break;
		case 72: //H key
			printf("Hit\n7 to turn on or off directional light\n8 to turn on or off point light\n9 to turn on or off spotlight\n");
			printf("Hit\n4 to decrese spolight exponent\n6 to increase\n");
			printf("Hit a/A, w/W, d/D to manipulate the position of direcational light, point light, spotlight respectively. \nLowercase to shift left, uppercase to shift right");
			printf("\n\nClick mouse \nleft button to translate the model\nright button to rotate the model\nScroll middle to scale the model\n");
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
	printf("Displayed models are: duck4KN\n");
	// set up shaders here
	setShaders();
	glUniform1i(iLocdirOn , 1);
	// load obj models through glm
	loadOBJModel();
	
	//Set default polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, mode);
	
	
	// register glut callback functions
	glutDisplayFunc (renderScene);
	glutIdleFunc    (idle);
	glutKeyboardFunc(processNormalKeys);
	glutMouseFunc   (processMouse);
	glutMotionFunc  (processMouseMotion);

	glEnable(GL_DEPTH_TEST);
	
	// main loop
	glutMainLoop();

	// free
	glmDelete(OBJ);
	glmDelete(OBJ2);
	glmDelete(OBJ3);
	return 0;
}

