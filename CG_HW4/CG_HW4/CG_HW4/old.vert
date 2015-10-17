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
uniform int uiisVertexLighting;
uniform int isDir,isPoint,isSpot;
uniform MaterialParameters Material;
uniform LightSourceParameters LightSource[3];
uniform vec4 eyePos;

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
	
	
	vv4position = av4position;;
	v2texCoord = av2texCoord;
	vv4vertice = um4modelMatrix * av4position;	
	mat3 model = mat3(um4modelMatrix);//3x3 model matrix
	vec4 color[3] , specular[3] ;
	vec3 normal, lightDir[3];
	vec4 diffuse[3], ambient[3];
	float NdotL,NdotHV,att,attenuation;
	float dist;
	for(int i = 0;i < 3; i++)
		color[i] = vec4(0,0,0,0);

	vv4diffuse[2] = Material.diffuse * LightSource[2].diffuse;
	vv4ambient[2] = Material.ambient * LightSource[2].ambient;
	vv4diffuse[1] = Material.diffuse * LightSource[1].diffuse;
	vv4ambient[1] = Material.ambient * LightSource[1].ambient;
	vv4diffuse[0] = Material.diffuse * LightSource[0].diffuse;
	vv4ambient[0] = Material.ambient * LightSource[0].ambient;

	vv3halfVector[0] = normalize( vec3(LightSource[0].position) + vec3(eyePos -um4modelMatrix*av4position) ) ;
	vv3halfVector[1] = normalize( vec3(LightSource[1].position) + vec3(eyePos) - vec3(um4modelMatrix * av4position)) ;
	vv3halfVector[2] = normalize( vec3(LightSource[2].position) + vec3(eyePos) - vec3( um4modelMatrix* av4position)) ;		
	
	normal = normalize( transpose(inverse(model)) * av3normal);
	vv3normal = normal;
	if (uiisVertexLighting == 1){
		
		if(isDir == 1 ) {		
			lightDir[0] = normalize(vec3(LightSource[0].position) );		
			NdotL = max(dot(normal, lightDir[0]), 0.0);		
	
			if (NdotL > 0.0) {						
				NdotHV = max(dot(normal, vv3halfVector[0]),0.0);
				specular[0] = Material.specular * LightSource[0].specular * pow(NdotHV,Material.shininess);			
			}	
			color[0] =  NdotL * vv4diffuse[0]  +   vv4ambient[0] + specular[0];		
		}
		if(isSpot == 1 ) {		
			lightDir[2] = normalize(vec3(LightSource[2].position) - vec3(um4modelMatrix * av4position) );
			dist = length(vec3(LightSource[2].position) - vec3(um4modelMatrix * av4position) );
			NdotL = max(dot(normal, lightDir[2]), 0.0);		
			if (NdotL > 0.0) {
				float spotEffect = dot( normalize(LightSource[2].spotDirection), normalize(-lightDir[2]) );
				if (spotEffect > LightSource[2].spotCosCutoff) {
					spotEffect = pow(spotEffect,LightSource[2].spotExponent );						
					NdotHV = max(dot(normal, vv3halfVector[2]),0.0);
					specular[2] = Material.specular * LightSource[2].specular * pow(NdotHV,Material.shininess);
					att = spotEffect / (constantAttenuation + linearAttenuation * dist + quadraticAttenuation * dist * dist);
					color[2] =  att * ( NdotL * vv4diffuse[2]  + vv4ambient[2] + specular[2]);
				}
			}	
		}

		if(isPoint ==1 ){
			lightDir[1] = normalize(vec3(LightSource[1].position) - vec3(um4modelMatrix * av4position) );
			dist = length(vec3(LightSource[1].position) - vec3(um4modelMatrix * av4position) );
			NdotL = max(dot(normal, lightDir[1]), 0.0);			
			if (NdotL > 0.0) {						
				NdotHV = max(dot(normal, vv3halfVector[1]),0.0);
				specular[1] = Material.specular * LightSource[1].specular * pow(NdotHV,Material.shininess);
				att = 1.0 / (constantAttenuation + linearAttenuation * dist + quadraticAttenuation * dist * dist);
				color[1] =  att * ( NdotL * vv4diffuse[1]  + vv4ambient[1] + specular[1]);	
			}		
		}
		vv4color = color[0] + color[1] + color[2];
	}
	
	gl_Position = mvp * av4position;
}
