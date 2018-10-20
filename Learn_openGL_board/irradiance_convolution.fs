#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
	// 세계 벡터는 원점의 접선 표면의 법선으로 작용해 WorldPos에 정렬된다
	// 이 법선을 감안할 때, 환경의 모든 들어오는 빛을 계산해라
	// 이 광도의 결과는 우리가 PBR 쉐이더에서 방사량을 샘플링하는데 사용하는
	// Normal direction에서 오는 빛의 광도이다
	vec3 N = normalize(WorldPos);

	vec3 irradiance = vec3(0.0);

	// tangent space calculation from origin point
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, N);
	up = cross(N, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// 구형에서 직각 좌표 (탄젠트 공간에서)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// 세계를 향한 접선 공간에서
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	FragColor = vec4(irradiance, 1.0);
}