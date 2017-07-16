#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Light.h"
#include "../../nclgl/TextMesh.h"
#include "../../nclgl/ParticleEmitter.h"

#define LIGHTNUM 8 
#define SHADOWSIZE 2048 
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void CalculateFrameRate();
	
protected:
	void DrawShadowScene();
	void FillBuffers(); 
	void DrawPointLights(); 
	void CombineBuffers(); 
	void DrawWater();
	void DrawSkybox();
	void DrawMyText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);
	void GenerateScreenTexture(GLuint & into, bool depth = false);
	void GenPartiExplo();// Generate explosion effect
	void GenPartiSnow();// Generate snow effect
	void GenPartiVolcano();// Generate volcano effect
	void SetShaderParticleSize(float f);	
	void GenPartiExplo1();

	Shader* sceneShader; 

	Shader* pointlightShader; 
	Shader* combineShader; 
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* shadowShader;
	Shader* textShader;
	Shader* particleShader;
	Shader* processShader;
	
	Light* light1;
	Light* pointLights; 
	Mesh* heightMap; 
	OBJMesh* sphere; 
	Mesh* quad; 
	Camera* camera; 

	Font*	basicFont;

	float rotation; 
	ParticleEmitter*	emitterExplo;
	ParticleEmitter*	emitterExplo1;
	ParticleEmitter*	emitterSnow;
	ParticleEmitter*	emitterVolcano;

	GLuint bufferFBO; 
	GLuint bufferColourTex; 
	GLuint bufferNormalTex; 
	GLuint bufferDepthTex; 
	GLuint cubeMap;
	GLuint shadowTex;
	GLuint shadowFBO;
	GLuint pointLightFBO; 
	GLuint lightEmissiveTex; 
	GLuint lightSpecularTex; 	
	GLuint processFBO;
	GLuint bufferColourTex1[2];

	GLint current_memory;
	GLint previous_memory;

	float waterRotate;
	Vector3 particlePos;

	float expNum;
	int explosion;
	int explosiontimer;

};