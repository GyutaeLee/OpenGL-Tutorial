#ifndef __CHAN_RENDER_OBJECT_H__
#define __CHAN_RENDER_OBJECT_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "chanAssetManager.h"
#include "chanCamera.h"

class chanRenderObject
{
public:
	chanRenderObject();
	chanRenderObject(chanAssetManager* _AssetManager,
		CHAN_MODEL_ENUM _modelEnum, CHAN_SHADER_ENUM _shaderEnum);
	
	~chanRenderObject();

	void setScale(const glm::vec3& _scale);
	void setPosition(const glm::vec3& _position);

	/*
	# to reach at the render function
	# You should set Dir, Point, SpotLight for the lighting system.
	# then update with view and projection matrix
	*/
	void setDirLight(std::vector<DirectionalLight>& _dirLight);
	void setPointLight(std::vector<chanPointLight>& _pointLight, const glm::mat4& view);
	void setSpotLight(std::vector<chanSpotLight>& _spotLight, 
		std::vector<chanSpotLight>& _viewerspotLight, 
		const glm::mat4& view);
	void update(const glm::mat4& view, const glm::mat4& projection);

	/*
	# With the light set, you will
	# Model, View, Projection Setup
	# then Draw
	*/
	void render();
private:
	glm::vec3 m_position;
	glm::vec3 m_scale;
	
	Model* m_model;
	Shader* m_shader;
};

#endif