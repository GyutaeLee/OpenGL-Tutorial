#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

const float PI = 3.14159265359;
// PBR 코드를 단순화하기 위해 세계 공간에 대한 접선-법선을 얻는 쉬운 트릭
// 무슨 일이 일어나지 않아도 걱정하지 말아라.
// 일반적으로 퍼포먼스를 늘리는 일반적인 방법으로 normal 맵핑을 하고 싶다.
// 나는 계획을 세우면 보통 맵핑 튜토리얼 어딘가에서 이 기술을 기록할 것이다
vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(WorldPos);
	vec3 Q2 = dFdy(WorldPos);
	vec2 st1 = dFdx(TexCoords);
	vec2 st2 = dFdy(TexCoords);

	vec3 N = normalize(Normal);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = -normalize(cross(N,T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
	float metallic = texture(metallicMap, TexCoords).r;
	float roughness = texture(roughnessMap, TexCoords).r;
	float ao = texture(aoMap, TexCoords).r;

	vec3 N = getNormalFromMap();
	vec3 V = normalize(camPos - WorldPos);

	// 수직 입사각에서 반사율을 계산
	// dia-electric(플라스틱과 같은)이 0.04의 F0를 사용하고
	// 금속인 경우 F0(metallic workflow)로 albedo 색상을 사용한다
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// 반사율 방정식
	vec3 Lo = vec3(0.0);
	for(int i=0; i<4; ++i)
	{
		// 빛 당 빛의 밝기를 계산한다
		vec3 L = normalize(lightPositions[i] - WorldPos);
		vec3 H = normalize(V + L);
		float distance = length(lightPositions[i] - WorldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColors[i] * attenuation;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(clamp(dot(H,V),0.0,1.0),F0);

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N,V), 0.0) * max(dot(N,L), 0.0);
		vec3 specular = nominator / max(denominator, 0.001); // 0으로 나누는 것을 방지하기 위해 0.001

		// kS는 Fresnel과 동일하다
		vec3 kS = F;

		// 또는 에너지 보존의 경우 확산 및 반사 조명은 1.0을 초과 할 수 없다. (표면이 빛을 내는 경우 제외)
		// 이 관계를 유지하기 위해 확산 성분(kD)은 1.0 - kS와 같아야한다.
		vec3 kD = vec3(1.0) - kS;

		// 비금속만이 확산 조명을 갖도록 역 금속도로 kD를 곱하거나
		// 부분 금속(순수 금속에는 확산 광이 없는 경우)을 선형 혼합으로 곱해라.
		kD *= 1.0 - metallic;

		// NdotL에 의한 빛의 크기 조절
		float NdotL = max(dot(N,L),0.0);

		// 나가는 발광 Lo에 추가한다
		// 우리가 이미 BRD에 Fresnel(kS)을 곱해서 kS를 다시 곱하지 않을 것임을 주목해라
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	// ambient lighting (주변 조명)
	// 다음 IBL 튜토리얼은 이 ambient lighting을 environment lighting으로 대체함
	vec3 ambient = vec3(0.03) * albedo * ao;

	vec3 color = ambient + Lo;

	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);
}