#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"

class Renderer : public OGLRenderer {
  public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	
	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	
  protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	  
	Shader * lightShader;
	Shader * reflectShader;
	Shader * skyboxShader;
	  
	//HeightMap * heightMap;
    Mesh * quad;
  
	GLuint cubeMap;
	  
	float waterRotate;

	Mesh * heightMap;
	Camera * camera;
	Light * light;		Light * light1;};
