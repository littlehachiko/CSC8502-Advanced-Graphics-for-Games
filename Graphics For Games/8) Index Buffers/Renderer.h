#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/CubeRobot.h"
#include "../nclgl/HeightMap.h"
#include < algorithm >

 class Renderer : public OGLRenderer {
	public:
		Renderer(Window & parent);
		virtual ~Renderer(void);
	
		virtual void RenderScene();
		virtual void UpdateScene(float msec);

	protected:
	 void DrawNode(SceneNode * n);
	 float f;
	 SceneNode * root;
	 HeightMap * heightMap;
	 Camera * camera;
};
