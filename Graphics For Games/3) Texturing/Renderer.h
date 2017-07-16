#pragma once
#include "../../nclgl/OGLRenderer.h"
class Renderer : public OGLRenderer {
	public:
		Renderer(Window & parent);
		virtual ~Renderer(void);
		
		virtual void RenderScene();
		
		void UpdateTextureMatrix(float rotation);
		/*virtual void UpdateScene ( float msec );*/
		void ToggleRepeating();
		void ToggleFiltering();
		
protected:
		Mesh* triangle;
        bool filtering;
        bool repeating;};