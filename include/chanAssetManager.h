/*
18-07-23
AssetManager manages all of resources from in this application
the structure of the AssetManager is loading a resoure if the resource is not loaded.
if the resource is aready loaded, you can just get a pointer to it.
*/

#ifndef __CHAN_ASSET_MANAGER_H
#define __CHAN_ASSET_MANAGER_H

#include "chanAssetUtils.h"
#include "Model.h"
#include "Shader.h"
#include "chanLightUtils.h"
#include "DirectionalLight.h"
#include "chanPointLight.h"
#include "chanSpotLight.h"

class chanAssetManager
{
public:
	Model* getModel(CHAN_MODEL_ENUM _modelEnum);
	// add a char parameter to distinguish pointer-return type
	Model getModel(CHAN_MODEL_ENUM _modelEnum, char); 

	Shader* getShader(CHAN_SHADER_ENUM _shaderEnum);
	Shader getShader(CHAN_SHADER_ENUM _shaderEnum, char);

	DirectionalLight* getDirLight(CHAN_DIR_LIGHT_ENUM _dirLightEnum);
	DirectionalLight getDirLight(CHAN_DIR_LIGHT_ENUM _dirLightEnum, char);

	chanPointLight* getPointLight(CHAN_POINT_LIGHT_ENUM _pointLightEnum);
	chanPointLight getPointLight(CHAN_POINT_LIGHT_ENUM _pointLightEnum, char);

	chanSpotLight* getSpotLight(CHAN_SPOT_LIGHT_ENUM _spotLightEnum);
	chanSpotLight getSpotLight(CHAN_SPOT_LIGHT_ENUM _spotLightEnum, char);

	//  !!! MANUAL INITIALIZATION  !!!
	chanAssetManager()
	{
		m_model[MODEL_NANOSUIT] = Model("Model/nanosuit/nanosuit.obj");
		m_model[MODEL_ROOSTER] = Model("Model/royal-rooster/rooster.dae");

		m_shader[SHADER_BASIC_LIGHT] = Shader("ShaderFolder/lighting_practice.vs", "ShaderFolder/lighting_practice.fs");
		m_shader[SHADER_LIGHT_SOURCE] = Shader("ShaderFolder/lighting_source.vs", "ShaderFolder/lighting_source.fs");

		m_dirLight[LIGHT_BASIC_DIR] = DirectionalLight("dirLight", glm::vec3(-0.2f, -1.0f, -0.3f),
			glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.5, 0.5, 0.5), glm::vec3(1.0, 1.0, 1.0));

		m_pointLight[LIGHT_GREEN_POINT] = chanPointLight("pointLights",
			glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.0f),
			1.f, 0.7f, 1.8f,
			glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.49f, 0.98f, 0.09f), glm::vec3(0.49f, 0.98f, 0.09f));
		m_pointLight[LIGHT_RED_POINT] = chanPointLight("pointLights",
			glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.0f),
			1.f, 0.7f, 1.8f,
			glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		m_spotLight[LIGHT_VIEWER_SPOT] = chanSpotLight("spotLight",
			glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(1.0f),
			1.0, 0.09, 0.032,
			glm::cos(glm::radians(3.5f)), glm::cos(glm::radians(10.0f)),
			glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 1.f, 1.f));
	} // chanAssetManager()
private:
	Model m_model[NUM_CHAN_MODEL_ENUM];
	Shader m_shader[NUM_CHAN_SHADER_ENUM];

	DirectionalLight m_dirLight[NUM_CHAN_DIR_LIGHT_ENUM];
	chanPointLight m_pointLight[NUM_CHAN_POINT_LIGHT_ENUM];
	chanSpotLight m_spotLight[NUM_CHAN_SPOT_LIGHT_ENUM];
};

#endif