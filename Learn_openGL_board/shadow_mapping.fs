#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	//원근 분할을 한다
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//[0,1] 범위로 변환한다
	projCoords = projCoords * 0.5 + 0.5;
	//빛의 원근법으로부터 깊이 값을 비슷하게 한다 ([0,1] 범위 fragPosLight를 좌표처럼 사용한다)
	float closetDepth = texture(shadowMap, projCoords.xy).r;
	//빛의 원근법으로부터 현재 조각의 깊이를 얻는다
	float currentDepth = projCoords.z;
	//기울기를 계산한다 (깊이 맵의 해상도와 기울기를 기반으로)
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	//현재 조각의 위치가 그림자 안에 있는지 확인한다
	//PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x){
		for(int y =-1; y <=1; ++y){
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	//빛의 절두체의 far_plane 영역 밖에서 그림자를 0.0으로 유지한다
	if(projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

void main(){

	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightColor = vec3(0.3);
	
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}