attribute vec2 av2texCoord;
attribute vec4 av4position;
attribute vec3 av3normal;

struct LightSourceParameters {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
	vec4 halfVector;
	vec3 spotDirection;
	float spotExponent;
	float spotCutoff; // (range: [0.0,90.0], 180.0)
	float spotCosCutoff; // (range: [1.0,0.0],-1.0)
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

struct MaterialParameters {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform mat4 mvp;
uniform mat4 um4modelMatrix;
uniform mat4 um4RotMatrix;
uniform int uiisVertexLighting;
uniform int isDir,isPoint,isSpot;
uniform MaterialParameters Material;
uniform LightSourceParameters LightSource[3];
uniform vec4 eyePos;
uniform mat4 viewMat,projMat;

varying vec2 v2texCoord;
varying vec4 vv4vertice;
varying vec4 vv4color;
varying vec4 vv4position;
varying vec4 vv4ambient[3], vv4diffuse[3];
varying vec3 vv3normal, vv3halfVector[3];

const float constantAttenuation = 1;
const float linearAttenuation = 1;
const float quadraticAttenuation = 1;

void main() {	
	
	
	v2texCoord = av2texCoord;

	vv4vertice =  um4modelMatrix * av4position;	
	mat3 model = mat3(um4modelMatrix);//3x3 model matrix --> rot matrix
	vec4 color[3] , specular[3] ;
	vec3 normal, lightDir[3];
	float shininess = 5.0;
	float NdotL,NdotHV,att,attenuation;
	float dist;
	for(int i = 0;i < 3; i++){
		color[i] = vec4(0.0);
		specular[i] = vec4(0.0);	
	}

	vv4diffuse[2] = Material.diffuse * LightSource[2].diffuse;
	vv4ambient[2] = Material.ambient * LightSource[2].ambient;
	vv4diffuse[1] = Material.diffuse * LightSource[1].diffuse;
	vv4ambient[1] = Material.ambient * LightSource[1].ambient;
	vv4diffuse[0] = Material.diffuse * LightSource[0].diffuse;
	vv4ambient[0] = Material.ambient * LightSource[0].ambient;
	vec3 eyeDir = (eyePos - vv4vertice).xyz;
	vv3halfVector[0] = normalize(  LightSource[0].position.xyz) + normalize(eyeDir) ;
	vv3halfVector[1] = normalize( (LightSource[1].position - vv4vertice).xyz) + normalize(eyeDir) ;
	vv3halfVector[2] = normalize( (LightSource[2].position - vv4vertice).xyz) + normalize(eyeDir) ;		
	
	
	//normal = normalize( (inverse(um4RotMatrix) * av3normal.xyzz).xyz);
	
	normal = normalize(transpose(inverse(mat3( um4modelMatrix))) * av3normal);
	vv3normal = normal;
	if (uiisVertexLighting == 1){
		
		if(isDir == 1 ) {	
			
			lightDir[0] = normalize( LightSource[0].position.xyz );		
			NdotL = max(dot(normal, lightDir[0]), 0.0);		
			

			if (NdotL > 0.0) {			
				color[0] += vv4ambient[0];			
				color[0] +=  NdotL * vv4diffuse[0];
				NdotHV = max(dot(normal, vv3halfVector[0]),0.0);
				specular[0] = Material.specular * LightSource[0].specular * pow(NdotHV,shininess);	
				color[0] += specular[0];
			}		
			color[0] = vec4(dot(normal, vv3halfVector[0]));
		}

		if(isPoint == 1 ){
			lightDir[1] = normalize( LightSource[1].position.xyz - vv4vertice.xyz );
			dist = length( LightSource[1].position.xyz - vv4vertice.xyz );
			att = 1.0 / (constantAttenuation + linearAttenuation * dist + quadraticAttenuation * dist * dist);
			NdotL = max(dot(normal, lightDir[1]), 0.0) ;	
			
			if (NdotL > 0.0) {				
				color[1] +=	att * (NdotL * vv4diffuse[1] + vv4ambient[1]);
				NdotHV = max(dot(normal, vv3halfVector[1]),0.0);
				specular[1] = Material.specular * LightSource[1].specular * pow(NdotHV,shininess);				
				color[1] +=  att * specular[1];	
			}	
			
		}
		if(isSpot == 1 ) {		
			lightDir[2] = normalize(LightSource[2].position.xyz - vv4vertice.xyz );
			dist = length( LightSource[2].position.xyz - vv4vertice.xyz );
			NdotL = max(dot(normal, lightDir[2]), 0.0);					
			if (NdotL > 0.0) {
				float spotEffect = dot( normalize(LightSource[2].spotDirection), normalize(-lightDir[2]) );

				if (spotEffect > LightSource[2].spotCosCutoff) {					
					spotEffect = pow(spotEffect,LightSource[2].spotExponent );	
					att = spotEffect / (constantAttenuation + linearAttenuation * dist + quadraticAttenuation * dist * dist);
					color[2] += att * ( NdotL * vv4diffuse[2]  + vv4ambient[2] );								
					NdotHV = max(dot(normal, vv3halfVector[2]),0.0);					
					specular[2] = Material.specular * LightSource[2].specular * pow(NdotHV,shininess);					
					color[2] += att * specular[2];
				}
			}	
		}
		vv4color = color[0]  + color[1] + color[2];
	}
	
	gl_Position =  mvp* av4position;
}
