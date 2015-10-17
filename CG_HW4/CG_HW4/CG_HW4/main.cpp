//HW4 Ming-Chang Chiu 
//2015.5.29
#include <stdio.h>
#include <stdlib.h>
 
#include <GL/glew.h>
#include <freeglut/glut.h>
#include "textfile.h"
#include "GLM.h"
#include "Matrices.h"
#include "Texture.h"
#include "main.h"
#include "MAT.h"
#define MAX_TEXTURE_NUM 50
#define MAXSIZE 300000
#define MAXGROUPSIZE 50
#define myMax(a,b) (((a)>(b))?(a):(b))

#pragma warning (disable: 4996)

#ifndef GLUT_WHEEL_UP
# define GLUT_WHEEL_UP   0x0003
# define GLUT_WHEEL_DOWN 0x0004
#endif

int isModelLoading = 0;
int isAutoRotating = 0;

int currentModel = 0;
#define NUM_OF_MODEL 21
char* filename[NUM_OF_MODEL] = 
{
	"TextureModels/teemo.obj",
	"TextureModels/satellitetrap.obj",
	"TextureModels/morgana_fallenangel.obj",
	"TextureModels/ahri_pop.obj",
	"TextureModels/katarina_referee.obj",

	"TextureModels/nidalee_cat_2012.obj",
	"TextureModels/duck.obj",
	"TextureModels/ball.obj",
	"TextureModels/Dog.obj",
	"TextureModels/ZEBRA.obj",

	"TextureModels/Digda.obj",
	"TextureModels/Fushigidane.obj",
	"TextureModels/Golonya.obj",
	"TextureModels/Hitokage.obj",
	"TextureModels/laurana500.obj",

	"TextureModels/Mew.obj",
	"TextureModels/Nala.obj",
	"TextureModels/Nyarth.obj",
	"TextureModels/texturedknot.obj",
	"TextureModels/Zenigame.obj",

	"TextureModels/cube.obj",
};

// textures
GLuint texNum[MAX_TEXTURE_NUM];
unsigned char image[MAX_TEXTURE_NUM][MAXSIZE]; //image data array 
FileHeader FH[MAX_TEXTURE_NUM]; //BMP FileHeader 
InfoHeader IH[MAX_TEXTURE_NUM]; //BMP 

float vertices[MAXGROUPSIZE][MAXSIZE];
float vnormals[MAXGROUPSIZE][MAXSIZE];
float vtextures[MAXGROUPSIZE][MAXSIZE];

int isTextureMapping = 1, isVertexLighting = 1;
GLint iLocTexCoord;
GLint iLocTex;
GLint texture_wrap_mode = GL_REPEAT;
GLint texture_mag_filter = GL_LINEAR;
GLint texture_min_filter = GL_LINEAR;
GLint iLocVertexLighting,iLocProjMatrix;
// window size
const unsigned int uiWidth  = 500;
const unsigned int uiHeight = 500;
int screenWidth;
int screenHeight;
bool dirOn = true, pointOn = false, spotOn = false ;
// eye position
GLfloat eyeVec[3] = {0.0f, 0.0f, 5.0f};

// option keys
#define PROJECTION_PERS 0x01
#define PROJECTION_PARA 0x02
char projectionMode = PROJECTION_PERS;

// projection matrix parameters
const float xmin = -1.0, xmax = 1.0;
const float ymin = -1.0, ymax = 1.0;
const float znear = eyeVec[2]-sqrtf(3.0f), zfar = eyeVec[2]+sqrtf(3.0f);
//const float znear = 1., zfar = -1.;
// normalization matrix
Matrix4 normalization_matrix;

// global view port matrix
Matrix4 viewPort;

// global projection matrix
Matrix4 parallelProjectionMatrix = myParallelProjectionMatrix(xmin, xmax, ymin, ymax, znear, zfar);
Matrix4 perspectiveProjectionMatrix = myFrustumMatrix(xmin, xmax, ymin, ymax, znear, zfar);

// Shader attributes
GLuint v, f, f2, p;

// Shader Locations
GLint iLocModelMatrix;

GLint iLocIsTextureMapping;
GLint iLocRotMatrix;
GLint iLocPosition;
GLint iLocColour;
GLint iLocMVP;
GLint iLocModMat4;
GLint iLocViewMat;
GLint iLocNormal;//Store Location of shader vars
GLint iLocMDiffuse[40],iLocMSpecular[40],iLocMShininess[40],iLocMAmbient[40] ;
GLint iLocLAmbient_0, iLocLDiffuse_0, iLocLSpecular_0, iLocLPosition_0;
GLint iLocLAmbient_1, iLocLDiffuse_1, iLocLSpecular_1, iLocLPosition_1;
GLint iLocLAmbient_2, iLocLDiffuse_2, iLocLSpecular_2, iLocLConsAtt_2, iLocLLinStt_2, iLocLPosition_2, iLocLSpotDir_2, iLocLSpotCosCutOff_2, iLocLSpotCutOff_2,iLocLSpotExp_2;

GLint iLocEyePos;
GLint iLocdirOn , iLocpointOn , iLocspotOn ;
// Lights settings
//directional light
GLfloat light0_position[4] = {0.0f, 0.0f, 3.0f, 1.0f};
GLfloat light0_ambient[4]  = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat light0_diffuse[4]  = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat light0_specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
//point light
GLfloat light1_position[4] = {0.0f, 0.0f, 3.0f, 1.0f};
GLfloat light1_ambient[4]  = {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat light1_diffuse[4]  = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat light1_specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
//spot light
GLfloat light2_position[4] = {0.0f, 0.0f, 3.0f, 1.0f};
GLfloat light2_ambient[4]  = {0.3f, 0.3f, 0.3f, 1.0f};
GLfloat light2_diffuse[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat light2_specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat light2_spot_dir[3] = {0.0f, 0.0f, -1.0f};
GLfloat light2_spot_cos_cut_off = 0.96f; //cosf(8.1301f / 180.0f * M_PI * 2.0f);
GLfloat light2_spot_cut_off = 8.1301f; //cosf(8.1301f / 180.0f * M_PI * 2.0f);
GLfloat light2_spot_exp = 2.0f;
GLfloat material_ambient[MAXGROUPSIZE][4],eyePos[4];
GLfloat material_diffuse[MAXGROUPSIZE][4],tModel[16];
GLfloat material_specular[MAXGROUPSIZE][4],material_shininess[MAXGROUPSIZE];
float rotateDeltaX, rotateDeltaY;
float rotateStartX, rotateStartY;
float rotateX, rotateY;
Matrix4 preM;
int isRBtnDown = 0;
int isMouseMoving = 0;
int isLBtnDown = 0;

float preTranslateX, preTranslateY;
float translateX, translateY;
float translateStartX, translateStartY;

float aMVP[16];

GLMmodel* OBJ;
GLfloat normalScale;
GLfloat scale;

// unpack bmp file
void LoadTextures(char* filename, int index)
{
	unsigned long size;
	char temp;
	FILE *file = fopen(filename, "rb");
	fread(&FH[index], sizeof(FileHeader), 1, file);
	fread(&IH[index], sizeof(InfoHeader), 1, file);
	size = IH[index].Width * IH[index].Height * 3; // IH.Width * IH.Height * 3
	//image[index] = (unsigned char*)malloc(size*sizeof(char));
	fread(image[index], size*sizeof(char), 1, file);
	fclose(file);

	//Swap channel => BRG----->RGB
	for (unsigned int i = 0; i < size; i += 3) {
		temp = image[index][i];
		image[index][i] = image[index][i+2];
		image[index][i+2] = temp;
	}
}

void initTextures(int index)
{
	glBindTexture(GL_TEXTURE_2D, texNum[index]);
	
	// TODO: Generate mipmap by using gl function NOT glu function.
	// TODO: A Text2D texture is created there.
	//2D's 2nd var can be modified
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IH[index].Width/pow(2.,0), IH[index].Height/pow(2.,0), 0, GL_RGB,GL_UNSIGNED_BYTE, image[index]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	
	// Hint: texture width x height = IH[index].Width x IH[index].Height
	//       image[index] means the texture image material used by which group
}

void setTexture() 
{
	int tex_count = 0;
	GLMgroup* group = OBJ->groups;
	while(group){
		if(strlen(OBJ->materials[group->material].textureImageName) != 0){
			char TexNameStr[200];
			sprintf(TexNameStr, "TextureModels/%s", OBJ->materials[group->material].textureImageName);
			printf("read %s\n", TexNameStr);
			LoadTextures(TexNameStr, tex_count);
		}
		tex_count++;
		group = group->next;
	}
	glGenTextures(tex_count, texNum);
	for(int i = 0; i < tex_count; i++){
		initTextures(i);
	}
}

// parse texture model and align the vertices
void TextureModel()
{
	GLMgroup* group = OBJ->groups;
	float maxx, maxy, maxz;
	float minx, miny, minz;
	float dx, dy, dz;

	maxx = minx = OBJ->vertices[3];
	maxy = miny = OBJ->vertices[4];
	maxz = minz = OBJ->vertices[5];
	for(unsigned int i=2; i<=OBJ->numvertices; i++){
		GLfloat vx, vy, vz;
		vx = OBJ->vertices[i*3+0];
		vy = OBJ->vertices[i*3+1];
		vz = OBJ->vertices[i*3+2];
		if(vx > maxx) maxx = vx;  if(vx < minx) minx = vx; 
		if(vy > maxy) maxy = vy;  if(vy < miny) miny = vy; 
		if(vz > maxz) maxz = vz;  if(vz < minz) minz = vz; 
	}
	//printf("max\n%f %f, %f %f, %f %f\n", maxx, minx, maxy, miny, maxz, minz);
	dx = maxx - minx;
	dy = maxy - miny;
	dz = maxz - minz;
	//printf("dx,dy,dz = %f %f %f\n", dx, dy, dz);
	normalScale = myMax(myMax(dx, dy), dz)/2;
	OBJ->position[0] = (maxx + minx) / 2;
	OBJ->position[1] = (maxy + miny) / 2;
	OBJ->position[2] = (maxz + minz) / 2;

	int gCount = 0;
	while(group){
		material_ambient[gCount][0] = OBJ->materials[group->material].ambient[0];
		material_ambient[gCount][1] = OBJ->materials[group->material].ambient[1];
		material_ambient[gCount][2] = OBJ->materials[group->material].ambient[2];
		material_ambient[gCount][3] = OBJ->materials[group->material].ambient[3];
		material_diffuse[gCount][0] = OBJ->materials[group->material].diffuse[0];
		material_diffuse[gCount][1] = OBJ->materials[group->material].diffuse[1];
		material_diffuse[gCount][2] = OBJ->materials[group->material].diffuse[2];
		material_diffuse[gCount][3] = OBJ->materials[group->material].diffuse[3];
		material_specular[gCount][0] = OBJ->materials[group->material].specular[0];
		material_specular[gCount][1] = OBJ->materials[group->material].specular[1];
		material_specular[gCount][2] = OBJ->materials[group->material].specular[2];
		material_specular[gCount][3] = OBJ->materials[group->material].specular[3];
		material_shininess[gCount] = OBJ->materials[group->material].shininess;
		
		for(unsigned int i=0; i<group->numtriangles; i++){
			// triangle index
			int triangleID = group->triangles[i];

			// the index of each vertex
			int indv1 = OBJ->triangles[triangleID].vindices[0];
			int indv2 = OBJ->triangles[triangleID].vindices[1];
			int indv3 = OBJ->triangles[triangleID].vindices[2];

			// vertices
			GLfloat vx, vy, vz;
			vx = OBJ->vertices[indv1*3];
			vy = OBJ->vertices[indv1*3+1];
			vz = OBJ->vertices[indv1*3+2];
			//printf("%f %f %f\n", vx, vy, vz);
			vertices[gCount][i*9+0] = vx;
			vertices[gCount][i*9+1] = vy;
			vertices[gCount][i*9+2] = vz;
			//printf("%f %f %f\n",vertices[i*9], vertices[i*9+1], vertices[i*9+2]);
			vx = OBJ->vertices[indv2*3];
			vy = OBJ->vertices[indv2*3+1];
			vz = OBJ->vertices[indv2*3+2];
			//printf("%f %f %f\n", vx, vy, vz);
			vertices[gCount][i*9+3] = vx;
			vertices[gCount][i*9+4] = vy;
			vertices[gCount][i*9+5] = vz;
			//printf("%f %f %f\n",vertices[i*9+3], vertices[i*9+4], vertices[i*9+5]);
			vx = OBJ->vertices[indv3*3];
			vy = OBJ->vertices[indv3*3+1];
			vz = OBJ->vertices[indv3*3+2];
			//printf("%f %f %f\n", vx, vy, vz);
			vertices[gCount][i*9+6] = vx;
			vertices[gCount][i*9+7] = vy;
			vertices[gCount][i*9+8] = vz;
			//printf("%f %f %f\n",vertices[i*9+6], vertices[i*9+7], vertices[i*9+8]);
			//printf("\n");
			//the index of each normals
			int indn1 = OBJ->triangles[triangleID].nindices[0];
			int indn2 = OBJ->triangles[triangleID].nindices[1];
			int indn3 = OBJ->triangles[triangleID].nindices[2];						
			
			// the vertex normal
			vnormals[gCount][i*9+0] = OBJ->normals[indn1*3];
			vnormals[gCount][i*9+1] = OBJ->normals[indn1*3+1];
			vnormals[gCount][i*9+2] = OBJ->normals[indn1*3+2];
			vnormals[gCount][i*9+3] = OBJ->normals[indn2*3];
			vnormals[gCount][i*9+4] = OBJ->normals[indn2*3+1];
			vnormals[gCount][i*9+5] = OBJ->normals[indn2*3+2];
			vnormals[gCount][i*9+6] = OBJ->normals[indn3*3];
			vnormals[gCount][i*9+7] = OBJ->normals[indn3*3+1];
			vnormals[gCount][i*9+8] = OBJ->normals[indn3*3+2];	

			int indt1 = OBJ->triangles[triangleID].tindices[0]; 
			int indt2 = OBJ->triangles[triangleID].tindices[1]; 
			int indt3 = OBJ->triangles[triangleID].tindices[2];
			
			// TODO: texture coordinates should be aligned by yourself
			vtextures[gCount][i*6+0] = OBJ->texcoords[indt1*2];
			vtextures[gCount][i*6+1] = OBJ->texcoords[indt1*2+1];
			vtextures[gCount][i*6+2] = OBJ->texcoords[indt2*2];
			vtextures[gCount][i*6+3] = OBJ->texcoords[indt2*2+1];
			vtextures[gCount][i*6+4] = OBJ->texcoords[indt3*2];
			vtextures[gCount][i*6+5] = OBJ->texcoords[indt3*2+1];
		}
		group = group->next;
		gCount++;
	}
	//Light properties
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
	
	// custom normalization matrix for each loaded model
	normalization_matrix.identity();
	normalization_matrix = normalization_matrix * myTranslateMatrix(-OBJ->position[0], -OBJ->position[1], -OBJ->position[2]);
	normalization_matrix = normalization_matrix * myScaleMatrix(1/normalScale, 1/normalScale, 1/normalScale);
}

void loadOBJModel(int index)
{
	// load model through glm parser
	OBJ = glmReadOBJ(filename[index]);
	printf("%s\n", filename[index]);

	// uncomment them only if there is no vertex normal info in your .obj file
	// glm will calculate the vertex normals for you
	if (strcmp(filename[index], "TextureModels/cube.obj") == 0){
		glmFacetNormals(OBJ);         
		glmVertexNormals(OBJ, 90.0);  
	}
	// parse texture model and align the vertices
	TextureModel();
}

void setViewPortMatrix()
{
	glViewport(0, 0, screenWidth, screenHeight);
	viewPort.identity();
	float ratio = (float)screenWidth / (float)screenHeight;
	if(ratio >= 1){
		viewPort = viewPort * myScaleMatrix(1.0f/ratio, 1, 1);
	}else{
		viewPort = viewPort * myScaleMatrix(1, ratio, 1);
	}
}

void loadModel(int i)
{
	isModelLoading = 1;
	loadOBJModel(i);
	setTexture();
	isModelLoading = 0;
}

// on window size changed callback
void changeSize(int w, int h) 
{
    screenWidth = w;
	screenHeight = h;

	// view port matrix
	setViewPortMatrix();	
}

Matrix4 myTranslateMatrix(float tx, float ty, float tz)
{
	Matrix4 mat;
	mat.identity();
	mat[12] = tx;
	mat[13] = ty;
	mat[14] = tz;
	return mat;
}

Matrix4 myScaleMatrix(float sx, float sy, float sz)
{
	Matrix4 mat;
	mat.identity();
	mat[0] = sx;
	mat[5] = sy;
	mat[10] = sz;
	return mat;
}

Matrix4 myRotateMatrix(float angle, float x, float y, float z)
{
    float c = cosf(angle / 180.0f * M_PI);    // cos
    float s = sinf(angle / 180.0f * M_PI);    // sin
    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float yy = y * y;
    float yz = y * z;
    float zz = z * z;

    Matrix4 m;
    m[0] = xx * (1 - c) + c;
    m[1] = xy * (1 - c) - z * s;
    m[2] = xz * (1 - c) + y * s;
    m[3] = 0;
    m[4] = xy * (1 - c) + z * s;
    m[5] = yy * (1 - c) + c;
    m[6] = yz * (1 - c) - x * s;
    m[7] = 0;
    m[8] = xz * (1 - c) - y * s;
    m[9] = yz * (1 - c) + x * s;
    m[10]= zz * (1 - c) + c;
    m[11]= 0;
    m[12]= 0;
    m[13]= 0;
    m[14]= 0;
    m[15]= 1;
	
    return m;
}

/* glmDot: compute the dot product of two vectors
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 */
static GLfloat
glmDot(GLfloat* u, GLfloat* v)
{   
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

/* glmCross: compute the cross product of two vectors
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 * n - array of 3 GLfloats (GLfloat n[3]) to return the cross product in
 */
static GLvoid
glmCross(GLfloat* u, GLfloat* v, GLfloat* n)
{
    n[0] = u[1]*v[2] - u[2]*v[1];
    n[1] = u[2]*v[0] - u[0]*v[2];
    n[2] = u[0]*v[1] - u[1]*v[0];
}

/* glmNormalize: normalize a vector
 *
 * v - array of 3 GLfloats (GLfloat v[3]) to be normalized
 */
static GLvoid
glmNormalize(GLfloat* v)
{
    GLfloat l;
    l = (GLfloat)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= l;
    v[1] /= l;
    v[2] /= l;
}


Matrix4 myLookAtMatrix(
	float eyex, float eyey, float eyez, 
	float cx, float cy, float cz, 
	float upx, float upy, float upz)
{
	// http://publib.boulder.ibm.com/infocenter/pseries/v5r3/index.jsp?topic=/com.ibm.aix.opengl/doc/openglrf/gluLookAt.htm
	/*
	Let E be the 3d column vector (eyeX, eyeY, eyeZ).
	Let C be the 3d column vector (centerX, centerY, centerZ).
	Let U be the 3d column vector (upX, upY, upZ).
	Compute L = C - E.
	Normalize L.
	Compute S = L x U.
	Normalize S.
	Compute U' = S x L.
	*/
	float E[3] = {eyex, eyey, eyez};
	float L[3] = {cx-eyex, cy-eyey, cz-eyez};
	glmNormalize(L);
	float U[3] = {upx, upy, upz};
	glmNormalize(U);
	float S[3];
	glmCross(L, U, S);
	glmNormalize(S);
	glmCross(S, L, U);

	Matrix4 m;
	m[0]  = S[0];
	m[1]  = S[1];
	m[2]  = S[2];
	m[3]  = 0;
	m[4]  = U[0];
	m[5]  = U[1];
	m[6]  = U[2];
	m[7]  = 0;
	m[8]  = -L[0];
	m[9]  = -L[1];
	m[10] = -L[2];
	m[11] = 0;
	m[12] = -E[0];
	m[13] = -E[1];
	m[14] = -E[2];
	m[15] = 1;

	return m;
}

Matrix4 myFrustumMatrix(float l, float r, float b, float t, float n, float f)
{
    Matrix4 mat;
    mat[0]  = 2 * n / (r - l);
	mat[1]  = 0;
    mat[2]  = 0;
	mat[3]  = 0;

	mat[4]  = 0;
    mat[5]  = 2 * n / (t - b);
    mat[6]  = 0;
	mat[7]  = 0;
	
	mat[8]  = (r + l) / (r - l);
	mat[9]  = (t + b) / (t - b);
    mat[10] = -(f + n) / (f - n);
    mat[11] = -1;
	
	mat[12] = 0;
	mat[13] = 0;
    mat[14] = -(2 * f * n) / (f - n);
    mat[15] = 0;
    return mat;
}

Matrix4 myPerspectiveProjectionMatrix(float fovY, float aspect, float front, float back)
{
    float tangent = tanf(fovY/2.0f * 180.0f/M_PI); // tangent of half fovY
    float height = front * tangent;         // half height of near plane
    float width = height * aspect;          // half width of near plane

    // params: left, right, bottom, top, near, far
    return myFrustumMatrix(-width, width, -height, height, front, back);
}

Matrix4 myParallelProjectionMatrix(float l, float r, float b, float t, float n, float f)
{
    Matrix4 mat;
    mat[0]  = 2 / (r - l);
	mat[1]  = 0;
	mat[2]  = 0;
	mat[3]  = 0;

	mat[4]  = 0;
    mat[5]  = 2 / (t - b);
	mat[6]  = 0;
	mat[7]  = 0;

	mat[8]  = 0;
	mat[9]  = 0;
    mat[10] = -2 / (f - n);
	mat[11] = 0;

    mat[12] = -(r + l) / (r - l);
    mat[13] = -(t + b) / (t - b);
    mat[14] = -(f + n) / (f - n);
	mat[15] = 1;
    return mat;
}

void showMatrix(Matrix4 m)
{
	printf("===========================\n");
	printf("%lf %lf %lf %lf\n", m[0], m[4], m[8], m[12]);
	printf("%lf %lf %lf %lf\n", m[1], m[5], m[9], m[13]);
	printf("%lf %lf %lf %lf\n", m[2], m[6], m[10], m[14]);
	printf("%lf %lf %lf %lf\n", m[3], m[7], m[11], m[15]);
}

void renderScene(void) 
{
	while(isModelLoading == 1){
		//printf("waiting\n");
	}
	Matrix4 M;
	
	glClearColor(0.3f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// aMVP = identity() * M_normalize(Tn*Sn) * M_transformation(S or R or T) * V(lookat) * P(projection)
			
	M.identity();
			
	// normalize
	M = M * normalization_matrix;

	// previous rotate matrix
	M = M * preM;

	// mouse rotate
	if(isRBtnDown && isMouseMoving){
		float temp = sqrt(rotateX * rotateX + rotateY * rotateY);
		if(temp != 0){
			Matrix4 newM = myRotateMatrix(-temp / 4.0f, rotateY/temp, rotateX/temp, 0.0f);
			// divided by 4 in order to rotate slower
			M = M * newM;
			preM = preM * newM;
		}
	}
	
	// auto rotate
	if (isAutoRotating) {
		Matrix4 newM = myRotateMatrix(3.0f, 0.0f, 1.0f, 0.0f);
		M = M * newM;
		preM = preM * newM;
	}
	
	// scale
	M = M * myScaleMatrix(scale, scale, scale);

	// mouse translate
	M = M * myTranslateMatrix(preTranslateX, preTranslateY, 0);
	if(isMouseMoving && isLBtnDown){
		float ratio = (float)screenWidth / (float)screenHeight;
		float alphaX = (xmax - xmin) / (float)screenWidth;
		float alphaY = (ymax - ymin) / (float)screenHeight;
		if(ratio >= 1){
			alphaX *= ratio;
		}else{
			alphaY /= ratio;
		}
		M = M * myTranslateMatrix(translateX*alphaX, translateY*alphaY, 0);
		preTranslateX += translateX * alphaX;
		preTranslateY += translateY * alphaY;
	}

	Matrix4 modelMatrix;
	// Model matrix ends here
	for(int i=0;i<16;i++){
		modelMatrix[i] = M[i];
	}

	// V (lookat)
	M = M * myLookAtMatrix(eyeVec[0],eyeVec[1],eyeVec[2],  eyeVec[0],eyeVec[1],eyeVec[2]-1.0f,  0,1,0);
	Matrix4 proj;
	// P (project)
	if(projectionMode == PROJECTION_PARA)
	{
		M = M * parallelProjectionMatrix;
		proj = parallelProjectionMatrix;
	}
	else if(projectionMode == PROJECTION_PERS)
	{
		M = M * perspectiveProjectionMatrix;
		proj = perspectiveProjectionMatrix;
	}
	else 
	{
		fprintf(stderr, "ERROR!\n");
		system("pause");
		exit(-1);
	}

	// V (view port)
	M = M * viewPort;

	for(int i=0;i<16;i++) aMVP[i] = M[i];
	Matrix4 view = myLookAtMatrix(eyeVec[0],eyeVec[1],eyeVec[2],  eyeVec[0],eyeVec[1],eyeVec[2]-1.0f,  0,1,0);
	/******
	MAT r(4),t(4),viewtemp(4);				
		Vector3 p1p2,up,p1p2p3,Rz,Rx,Ry;
		for (int i =0;i<4;i++) r[i][i] = t[i][i] = 1.0;
		t[0][3] = (-1.0) * eyeVec[0];		
		t[1][3] = (-1.0) * eyeVec[1];		
		t[2][3] = (-1.0) * eyeVec[2];	
		
		p1p2.x = eyeVec[0];
		p1p2.y = eyeVec[1];
		p1p2.z = eyeVec[2]-1.0;				
		Rz.x = r[2][0] = p1p2.x/p1p2.length();
		Rz.y = r[2][1] = p1p2.y/p1p2.length();
		Rz.z = r[2][2] = p1p2.z/p1p2.length();
		up.x = 0.0;
		up.y = 1.0;
		up.z = 0.0;
		p1p2p3 = up.cross(p1p2);
		Rx.x = r[0][0] = p1p2p3.x/p1p2p3.length();
		Rx.y = r[0][1] = p1p2p3.y/p1p2p3.length();
		Rx.z = r[0][2] = p1p2p3.z/p1p2p3.length();
		Ry = Rz.cross(Rx);
		r[1][0]  = Ry.x;
		r[1][1]	 = Ry.y;
		r[1][2]	 = Ry.z;
		
		viewtemp = r*t;			
		for (int i = 0;i<4;i++){
			for (int j = 0;j<4;j++){
				view[4*i+j] = viewtemp[j][i];	
				//printf("%f ",view[i][j]);
			}
			//printf("\n");
		}



	/******/
		
	glUniform1i(iLocIsTextureMapping, isTextureMapping);
	glUniform1i(iLocVertexLighting, isVertexLighting);
	// Matrices
	glUniformMatrix4fv(iLocViewMat, 1,GL_FALSE, view.get());
	glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, M.get());
	glUniformMatrix4fv(iLocModelMatrix, 1, GL_FALSE, modelMatrix.get());
	glUniformMatrix4fv( iLocRotMatrix, 1, GL_FALSE, preM.get());
	glUniformMatrix4fv( iLocProjMatrix, 1, GL_FALSE, proj.get());
	GLfloat eyePos[4];
	eyePos[0] = eyeVec[0],eyePos[1] = eyeVec[1],eyePos[2] = eyeVec[2],eyePos[3] = 0.0;
	GLMgroup* group = OBJ->groups;
	int gCount = 0;
	while(group){
		// enable attributes array
		glEnableVertexAttribArray(iLocPosition);
		// TODO: texture VertexAttribArray is enabled here
		glEnableVertexAttribArray(iLocTexCoord);
		glEnableVertexAttribArray(iLocNormal);
		// bind attributes array
		glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices[gCount]);
		// TODO: bind texture vertex attribute pointer here
		glVertexAttribPointer(iLocTexCoord, 2, GL_FLOAT, GL_FALSE, 0, vtextures[gCount]);
		glVertexAttribPointer(  iLocNormal, 3, GL_FLOAT, GL_FALSE, 0, vnormals[gCount]);
		// texture mag/min filter
		// TODO: texture mag/min filters are defined here
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_min_filter)  ;
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_mag_filter)	;	
		// texture wrap mode s/t
		// TODO: texture wrap modes are defined here
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_wrap_mode ) ;
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_wrap_mode)  ;
		// bind texture material group by group
		// TODO: bind texture here
		glActiveTexture(GL_TEXTURE0 );
		glBindTexture(GL_TEXTURE_2D, texNum[gCount]);
		
		glUniform4fv(iLocEyePos, 1,  eyePos);
		glUniform4fv( iLocMDiffuse[gCount] ,1, material_diffuse[gCount]);
		glUniform4fv( iLocMAmbient[gCount] ,1, material_ambient[gCount]);
		glUniform4fv(iLocMSpecular[gCount] ,1, material_specular[gCount]);
		glUniform1f(iLocMShininess[gCount] ,  material_shininess[gCount]);
		// draw arrays
		glDrawArrays(GL_TRIANGLES, 0, group->numtriangles*3);

		gCount++;
		group = group->next;
	}

	isMouseMoving = 0;

	Sleep(20);

	glutSwapBuffers();
}

void showHelp()
{
	printf("=============================\n");
	printf("press H / h to show this menu\n");
	printf("\n");
	printf("PROJECTION:        press P / p to switch perspective / orthographic projection\n");
	printf("\n");
	printf("TRANSLATION:       press the left button and drag the mouse to translate\n");
	printf("\n");
	printf("ROTATION:          press the right button and drag the mouse to rotate\n");
	printf("\n");
	printf("SCALE:             scroll mouse wheel up and down to scale\n");
	printf("\n");
	printf("MODEL SWITCHING:   press Z / z to switch previous model.\n");
	printf("                   press X / x to switch next model.\n");
	printf("\n");
	printf("Per-pixel/vertex lighting:   press 'v' to switch.\n");
	printf("\n");
	printf("Directional Light: press '1'\n");
	printf("\n");
	printf("Point Light:       press '2'\n");
	printf("\n");
	printf("Spotlight:         press '3'\n");
	printf("\n");
	printf("Texture Mapping:   press T / t\n");
	printf("\n");
	printf("Switch MAG_FILTER between GL_LINEAR / GL_NEAREST\n");
	printf("\t press 'M'\n");
	printf("\n");
	printf("Switch MIN_FILTER between GL_LINEAR / GL_NEAREST\n");
	printf("\t press 'm'\n");
	printf("\n");
	printf("Switch TEXTURE_WRAPbetween GL_REPEAT / GL_CLAMP\n");
	printf("\t press W / w\n");
	printf("\n");

}

void processSpecialKeys(int key, int x, int y){
	/*
	GLUT_KEY_F1			F1 function key.
	GLUT_KEY_F2			F2 function key.
	GLUT_KEY_F3			F3 function key.
	GLUT_KEY_F4			F4 function key.
	GLUT_KEY_F5			F5 function key.
	GLUT_KEY_F6			F6 function key.
	GLUT_KEY_F7			F7 function key.
	GLUT_KEY_F8			F8 function key.
	GLUT_KEY_F9			F9 function key.
	GLUT_KEY_F10		F10 function key.
	GLUT_KEY_F11		F11 function key.
	GLUT_KEY_F12		F12 function key.
	GLUT_KEY_LEFT		Left directional key.
	GLUT_KEY_UP			Up directional key.
	GLUT_KEY_RIGHT		Right directional key.
	GLUT_KEY_DOWN		Down directional key.
	GLUT_KEY_PAGE_UP	Page up directional key.
	GLUT_KEY_PAGE_DOWN	Page down directional key.
	GLUT_KEY_HOME		Home directional key.
	GLUT_KEY_END		End directional key.
	GLUT_KEY_INSERT		Inset directional key.
	*/
}

void processNormalKeys(unsigned char key, int x, int y) {
	int modifier = glutGetModifiers();
	//printf("modifier = %d\n", modifier);
	//printf("key = %d\n", key);
	switch(modifier){
		case GLUT_ACTIVE_CTRL:{
			switch(key) {
				/* ctrl+q */ case 17: break;
				/* ctrl+a */ case  1: break;
				/* ctrl+w */ case 23: break;
				/* ctrl+s */ case 19: break;
				/* ctrl+e */ case  5: break;
				/* ctrl+d */ case  4: break;
			}
			break;
		}
	}

	// normal keys
	switch(key){
		case 'v':			
			isVertexLighting = (isVertexLighting + 1) % 2;	
			if(isVertexLighting == 0) printf("frag light\n");
			else printf("vertex light\n");
			break;
		case 'm':
			if(texture_min_filter ==  GL_LINEAR)	texture_min_filter =  GL_NEAREST;
			else texture_min_filter =  GL_LINEAR;
			break;
		case 'M':
			if(texture_mag_filter ==  GL_LINEAR)	texture_mag_filter =  GL_NEAREST;
			else texture_mag_filter =  GL_LINEAR;
			break;
		case 'w':case 'W':
			if(texture_wrap_mode == GL_REPEAT) texture_wrap_mode = GL_CLAMP;
			else texture_wrap_mode = GL_REPEAT;
			break;
		case '1':
			if (dirOn) glUniform1i(iLocdirOn , 0),dirOn = false,printf("Directional light off\n");
			else glUniform1i(iLocdirOn , 1),dirOn = true,printf("Directional light On\n");
			break;
		case '3':
			if (spotOn) glUniform1i(iLocspotOn , 0),spotOn = false,printf("Spot light off\n");
			else glUniform1i(iLocspotOn , 1),spotOn = true,printf("Spotlight On\n");
			break;
		case '2':
			if (pointOn) glUniform1i(iLocpointOn , 0),pointOn = false,printf("Point light off\n");
			else glUniform1i(iLocpointOn , 1),pointOn = true,printf("Point light On\n");
			break;
		case 't': case 'T': 
			isTextureMapping = (isTextureMapping + 1) % 2 ;
			break;
		case 'r': case 'R': 
			isAutoRotating = (isAutoRotating + 1 ) % 2;
			break;

		case 'Z': case 'z':
			currentModel = (currentModel + NUM_OF_MODEL - 1) % NUM_OF_MODEL;
			loadModel(currentModel);
			printf("loading %s\n", filename[currentModel]);
			break;

		case 'X': case 'x':
			currentModel = (currentModel + 1) % NUM_OF_MODEL;
			loadModel(currentModel);
			printf("loading %s\n", filename[currentModel]);
			break;

		case 'H': case 'h':
			showHelp();
			break;

		case 'P': case 'p':
			if(projectionMode != PROJECTION_PERS){
				projectionMode = PROJECTION_PERS;
				printf("perspective projection\n");
			}else if(projectionMode != PROJECTION_PARA){
				projectionMode = PROJECTION_PARA;
				printf("parallel projection\n");
			}
			break;

		case 27: // esc key
			exit(0);
	}
	//glutPostRedisplay();
}

void setShaders() {
	char *vs = NULL,*fs = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("shader.vert");
	fs = textFileRead("shader.frag");

	const char * ff = fs;
	const char * vv = vs;

	glShaderSource(v, 1, &vv,NULL);
	glShaderSource(f, 1, &ff,NULL);

	free(vs);free(fs);

	glCompileShader(v);
	glCompileShader(f);

	GLint vCompiled;
	glGetShaderiv(v, GL_COMPILE_STATUS, &vCompiled);
	if (!vCompiled) { 
		GLint length; 
		GLchar* log; 
 
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &length ); 
 
		log = (GLchar*) malloc(length); 
		glGetShaderInfoLog(v, length, &length, log); 
		fprintf(stderr, "[v] compile log = '%s'\n", log); 
		free(log); 
	} 

	GLint fCompiled;
	glGetShaderiv(f, GL_COMPILE_STATUS, &fCompiled);
	if (!fCompiled) { 
		GLint length; 
		GLchar* log; 
 
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &length ); 
 
		log = (GLchar*) malloc(length); 
		glGetShaderInfoLog(f, length, &length, log); 
		fprintf(stderr, "[f] compile log = '%s'\n", log); 
		free(log); 
	} 

	p = glCreateProgram();
	
	glAttachShader(p,v);
	glAttachShader(p,f);
	glLinkProgram(p);
	GLint linked = 0;
	glGetProgramiv(p, GL_LINK_STATUS, &linked );
	if (linked) {
		glUseProgram(p);
	} else {
		GLint length;
		GLchar* log;
		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &length );
		log = (GLchar*) malloc(length);
		glGetProgramInfoLog(p, length, &length, log);
		fprintf(stderr, "link log = '%s'\n", log);
		free(log);
	}

	iLocdirOn = glGetUniformLocation(p, "isDir");
	glUniform1i(iLocdirOn , 1);
	iLocpointOn = glGetUniformLocation(p, "isPoint");
	iLocspotOn = glGetUniformLocation(p, "isSpot");
	iLocEyePos = glGetUniformLocation(p, "eyePos");
	iLocNormal   = glGetAttribLocation (p, "av3normal");	
	iLocPosition = glGetAttribLocation(p, "av4position");	
	iLocTexCoord = glGetAttribLocation(p, "av2texCoord");
	
	iLocTex = glGetUniformLocation(p,"ustexture");
	glUniform1i(iLocTex, 0);
	iLocMVP = glGetUniformLocation(p, "mvp");
	for (int i = 0; i< 40;i++){
		iLocMAmbient[i] =	glGetUniformLocation(p, "Material.ambient");
		iLocMDiffuse[i]  =	glGetUniformLocation(p, "Material.diffuse");
		iLocMSpecular[i] =	glGetUniformLocation(p, "Material.specular");
		iLocMShininess[i] = glGetUniformLocation(p, "Material.shininess");
	}
	//iLocModMat4 = glGetUniformLocation(p, "m");
	iLocLAmbient_0 = glGetUniformLocation(p, "LightSource[0].ambient");	
	iLocLAmbient_1 = glGetUniformLocation(p, "LightSource[1].ambient");
	iLocLAmbient_2 = glGetUniformLocation(p, "LightSource[2].ambient");
	iLocLDiffuse_0 = glGetUniformLocation(p, "LightSource[0].diffuse");
	iLocLDiffuse_1 = glGetUniformLocation(p, "LightSource[1].diffuse");
	iLocLDiffuse_2 = glGetUniformLocation(p, "LightSource[2].diffuse");
	iLocLSpecular_0 = glGetUniformLocation(p, "LightSource[0].specular");
	iLocLSpecular_1 = glGetUniformLocation(p, "LightSource[1].specular");
	iLocLSpecular_2 = glGetUniformLocation(p, "LightSource[2].specular");	
	iLocLPosition_0 = glGetUniformLocation(p, "LightSource[0].position");
	iLocLPosition_1 = glGetUniformLocation(p, "LightSource[1].position");
	iLocLPosition_2 = glGetUniformLocation(p, "LightSource[2].position");	
	iLocLSpotDir_2 = glGetUniformLocation(p, "LightSource[2].spotDirection");
	iLocLSpotCosCutOff_2= glGetUniformLocation(p, "LightSource[2].spotCosCutoff");
	iLocLSpotCutOff_2 = glGetUniformLocation(p, "LightSource[2].spotCutoff");		
	iLocLSpotExp_2 = glGetUniformLocation(p, "LightSource[2].spotExponent");

	iLocIsTextureMapping = glGetUniformLocation(p, "uiisTextureMapping");
	iLocVertexLighting = glGetUniformLocation(p, "uiisVertexLighting");
	iLocModelMatrix  = glGetUniformLocation(p, "um4modelMatrix");
	iLocRotMatrix  = glGetUniformLocation(p, "um4RotMatrix");
	iLocViewMat = glGetUniformLocation(p, "viewMat");
	iLocProjMatrix= glGetUniformLocation(p, "projMat");
	glUseProgram(p);
	
}

void processMouse(int who, int state, int x, int y){
	/*glGetUniformfv(p,iLocEyePos,tModel);	
	for (int i  = 0;i<4;i++)
		printf("%f \n",tModel[i]);		
	printf("-----------\n");*/
	//showMatrix(preM);
	if(who == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && isLBtnDown == 0)
	{
		rotateStartX = (float)x;
		rotateStartY = (float)y;
		isRBtnDown = 1;
	}
	else if(who == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		isRBtnDown = 0;
	}
	else if(who == GLUT_LEFT_BUTTON && state == GLUT_DOWN && isRBtnDown == 0)
	{
		translateX = x - translateStartX;
		translateY = -(y - translateStartY);
		translateStartX = (float)x;
		translateStartY = (float)y;
		isLBtnDown = 1;
	}
	else if(who == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		isLBtnDown = 0;
	}
	else if(who == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		scale = 1.0f;
	}
	else if(who == GLUT_WHEEL_UP && state == GLUT_DOWN)
	{
		if(scale < 2.29) scale *= 1.05f;
	}
	else if(who == GLUT_WHEEL_DOWN && state == GLUT_DOWN)
	{
		if(scale > 0.25) scale /= 1.05f;
	}
}

// callback on mouse drag
void myMotionFunc(int x, int y)
{
	if(isRBtnDown){
		rotateX = x - rotateStartX;
		rotateY = y - rotateStartY;
		rotateStartX = (float)x;
		rotateStartY = (float)y;
	}
	if(isLBtnDown){
		translateX = x - translateStartX;
		translateY = -(y - translateStartY);
		translateStartX = (float)x;
		translateStartY = (float)y;
	}
	isMouseMoving = 1;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(uiWidth, uiHeight);
	glutCreateWindow("CG HW4 TA");
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);
	glutMouseFunc(processMouse);
	glutMotionFunc(myMotionFunc);
	glutSpecialFunc(processSpecialKeys);

	glewInit();
	if(glewIsSupported("GL_VERSION_2_0")){
		printf("Ready for OpenGL 2.0\n");
		showHelp();
	}else{
		printf("OpenGL 2.0 not supported\n");
		system("pause");
		exit(123);
	}

	// init
	preTranslateX = preTranslateY = 0;
	scale = 1.0;
	currentModel = 0;
	setShaders();
	loadModel(currentModel);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glutMainLoop();

	glmDelete(OBJ);
	return 0;
}

