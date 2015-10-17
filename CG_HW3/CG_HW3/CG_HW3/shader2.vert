attribute vec4 v_coord;
attribute vec3 v_normal;

struct LightSourceParameters
{
  vec4 position;
  vec4 diffuse;
  vec4 specular;
  vec4 ambient;
  float constantAttenuation, linearAttenuation, quadraticAttenuation;
  float spotCutoff, spotExponent;
  vec3 spotDirection;
};

struct MaterialParameters
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
uniform mat4 m, v, p,mvp;
varying vec4 color;
uniform int isDir,isPoint,isSpot;
uniform LightSourceParameters LightSource[3];
//uniform mat3 m_3x3_inv_transp ;
//uniform mat4 v_inv;
uniform MaterialParameters Material;
const float constantAttenuation = 0.4;
const float linearAttenuation = 0.4;
const float quadraticAttenuation = 0.4;

void main(void)
{
	
	mat3 m_3x3_inv_transp = transpose(inverse(mat3(m)));
	vec3 normalDirection = normalize(m_3x3_inv_transp * v_normal);
	vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - m * v_coord));
	vec3 lightDirection;
	float attenuation;
 
	if (LightSource[0].position.w == 0.0) // directional light
    {
		attenuation = 1.0; // no attenuation
		lightDirection = normalize(vec3(LightSource[0].position));
    }
	
	else // point or spot light (or other kind of light)
    {
		vec3 vertexToLightSource = vec3(LightSource[0].position - m * v_coord);
		float distance = length(vertexToLightSource);
		lightDirection = normalize(vertexToLightSource);
		attenuation = 1.0 / (constantAttenuation + linearAttenuation * distance + quadraticAttenuation * distance * distance);
 
		if (LightSource[0].spotCutoff <= 90.0) // spotlight
		{
			float clampedCosine = max(0.0, dot(-lightDirection, normalize(LightSource[0].spotDirection)));
			if (clampedCosine < cos(radians(LightSource[0].spotCutoff))) // outside of spotlight cone
			{
				attenuation = 0.0;
			}
			else
			{
				attenuation = attenuation * pow(clampedCosine, LightSource[0].spotExponent);
			}
		}
    }
 
	vec3 ambientLighting = vec3(LightSource[0].ambient) * vec3(Material.ambient);
 
	vec3 diffuseReflection = attenuation * vec3(LightSource[0].diffuse) * vec3(Material.diffuse) * max(0.0, dot(normalDirection, lightDirection));
 
	vec3 specularReflection;
	if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
    {
		specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
    }
	else // light source on the right side
    {
		specularReflection = attenuation * vec3(LightSource[0].specular) * vec3(Material.specular) * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), Material.shininess);
    }
 
	color = vec4(ambientLighting  + diffuseReflection + specularReflection, 1.0);
	gl_Position = mvp * v_coord;
}
