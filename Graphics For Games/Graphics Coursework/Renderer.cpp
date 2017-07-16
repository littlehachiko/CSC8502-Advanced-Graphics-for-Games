#include "Renderer.h"

//#define POST_PASSES 10

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {

	glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,&previous_memory);

	expNum = 0.0f;
	rotation = 0.0f;
	camera = new Camera(0.0f, 0.0f,Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 800, RAW_WIDTH * HEIGHTMAP_X));
	//fire.png http://pngimg.com/download/5992
	emitterExplo = new ParticleEmitter(TEXTUREDIR"fire.jpg");
	emitterExplo1 = new ParticleEmitter(TEXTUREDIR"particle.tga");

	emitterSnow = new ParticleEmitter(TEXTUREDIR"snowflake.jpg");

	emitterVolcano = new ParticleEmitter(TEXTUREDIR"particle.tga");
	light1 = new	Light(Vector3((RAW_HEIGHT*HEIGHTMAP_X)/1.0f, 900.0f, (RAW_HEIGHT*HEIGHTMAP_Z)/1.0f),
		                  Vector4(1.0f, 1.0f, 1.0f, 1), 2500.f);

	quad = Mesh::GenerateQuad();

	pointLights = new Light[LIGHTNUM * LIGHTNUM]; 

	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light & l = pointLights[(x * LIGHTNUM) + z];
			
			float xPos = (RAW_WIDTH * HEIGHTMAP_X / (LIGHTNUM - 1)) * x;
			float zPos = (RAW_HEIGHT * HEIGHTMAP_Z / (LIGHTNUM - 1)) * z;
			l.SetPosition(Vector3(xPos, 100.0f, zPos));
			
			float r = 0.8f + (float)(rand() % 129) / 128.0f;
			float g = 0.8f + (float)(rand() % 129) / 128.0f;
			float b = 0.8f + (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 1.0f));

			float radius = (RAW_WIDTH * HEIGHTMAP_X / LIGHTNUM)/1.5f;
			l.SetRadius(radius);
		}
	}

	heightMap = new HeightMap(TEXTUREDIR"liceland1.raw");

	// ArcticSwimmingHole.jpghttp://spiralgraphics.biz/packs/snow_ice/index.htm?0#anchor
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	// ArcticSwimmingHoleBump.png use http://cpetry.github.io/NormalMap-Online/ to generate
	quad->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"water.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	//DeepFreeze.jpg http://spiralgraphics.biz/packs/snow_ice/?3#anchor
	heightMap -> SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"RockPools.jpg", SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	// DeepFreeze.png use http://cpetry.github.io/NormalMap-Online/ to generate
	heightMap -> SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"RockPools.png", SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"redsunset_ft.tga", TEXTUREDIR"redsunset_bk.tga",
		                            TEXTUREDIR"redsunset_up.tga", TEXTUREDIR"redsunset_dn.tga",
		                            TEXTUREDIR"redsunset_rt.tga", TEXTUREDIR"redsunset_lf.tga",
		                            SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	SetTextureRepeating(quad -> GetTexture(), true);
	SetTextureRepeating(quad -> GetBumpMap(), true);
	SetTextureRepeating(heightMap -> GetTexture(), true);
	SetTextureRepeating(heightMap -> GetBumpMap(), true);
	
	sphere = new OBJMesh();

	if (!sphere -> LoadOBJMesh(MESHDIR"ico.obj")) {
		return;
	}

	shadowShader = new Shader(SHADERDIR"lightshaver.glsl", SHADERDIR"lightshafrag.glsl");//shadow

	if (!shadowShader->LinkProgram()) {
		return;
	}

	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"ReflectFragment.glsl");
	if (!reflectShader->LinkProgram()) {
		return;
	}

	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");
	if (!skyboxShader->LinkProgram()) {
		return;
	}

	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"PerPixelFragment.glsl");
	if (!lightShader->LinkProgram()) {
		return;
	}

	sceneShader = new Shader(SHADERDIR"BumpVertex.glsl",SHADERDIR"bufferFragment.glsl");
	if (!sceneShader -> LinkProgram()) {
		return;
	}

	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!textShader->LinkProgram()) {
		return;
	}

	combineShader = new Shader(SHADERDIR"combinevert.glsl", SHADERDIR"combinefrag.glsl");
	if (!combineShader -> LinkProgram()) {
		return;
	}

	pointlightShader = new Shader(SHADERDIR"pointlightvert.glsl",SHADERDIR"pointlightfrag.glsl");
	if (!pointlightShader -> LinkProgram()) {
		 return;
	}

	particleShader = new Shader(SHADERDIR"vertex.glsl", SHADERDIR"fragment.glsl",SHADERDIR"geometry.glsl");
	if (!particleShader->LinkProgram()) {
		return;
	}

	glGenTextures(1, &shadowTex);

	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &bufferFBO);
	
	glGenFramebuffers(1, &pointLightFBO);
	
	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;
	
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
	GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);
	
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
	GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	waterRotate = 0.0f;
	projMatrix = Matrix4::Perspective(1.0f, 80000.0f, (float)width / (float)height, 45.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}

Renderer ::~Renderer(void) {
    delete sceneShader;
    delete combineShader;
    delete pointlightShader;
	delete shadowShader;
	delete reflectShader;
	delete textShader;
	delete skyboxShader;
	delete lightShader;
    delete heightMap;
    delete camera;
    delete sphere;
	delete basicFont;
    delete quad;
    delete[] pointLights;
	delete light1;
	delete emitterExplo;
	delete emitterExplo1;
	delete emitterSnow;
	delete emitterVolcano;

	

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	
	glDeleteTextures(2, bufferColourTex1);
	
	glDeleteFramebuffers(1, &processFBO);

}

void Renderer::GenerateScreenTexture(GLuint & into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0,depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8,
		         width, height, 0,depth ? GL_DEPTH_COMPONENT : GL_RGBA,GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateScene(float msec) {
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Z)) {
		expNum = 600.0;
	}
	else
	{
		expNum = 0.0;
	}
	emitterExplo->UpdateExplo(msec);
	emitterExplo1->UpdateExplo(msec);
	emitterSnow->UpdateSnow(msec);
	emitterVolcano->UpdateVolcano(msec);

	camera -> UpdateCamera(msec);
	viewMatrix = camera -> BuildViewMatrix();

	rotation = msec * 0.01f;
	waterRotate += msec * 0.001f;
	explosiontimer--;
	if (explosiontimer == 0) {
		explosion = false;
	}
}

void Renderer::RenderScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, 1.0f, 45.0f);
	


	DrawShadowScene();

	viewMatrix = camera->BuildViewMatrix();

	FillBuffers();

	
	DrawPointLights();
	
	DrawSkybox();
	CombineBuffers();
	CalculateFrameRate();
	DrawMyText("Memory: "+ to_string((previous_memory-current_memory)/1024)+ "MB", Vector3(1,40, 1), 35.0f);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	viewMatrix = camera->BuildViewMatrix();
	


	//GenPartiExplo();
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_Z)) {
		explosiontimer = 200;
		explosion = 0;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_M)) {
		explosiontimer = 200;
		explosion = 1;
	}
	if (explosion == 0) {
		
	     GenPartiExplo();
	}
	else if (explosion == 1) {
		GenPartiExplo1();
	}

	GenPartiSnow();
	GenPartiVolcano();

	glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &current_memory);

	SwapBuffers();
}

void Renderer::DrawSkybox() {
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}

void Renderer::DrawShadowScene() {   

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, 1.0f, 90.0f);
	viewMatrix = Matrix4::BuildViewMatrix(light1->GetPosition(), 
		Vector3(0, 0, 0));
	shadowMatrix = biasMatrix *(projMatrix * viewMatrix);
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	
	UpdateShaderMatrices();

	

	heightMap->Draw();
	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light1);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 256 * HEIGHTMAP_Y / 4.8f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(6000, 1, 6000)) *
		Matrix4::Rotation(66, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);
}

void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glDisable(GL_BLEND);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	Matrix4 tempMatrix = shadowMatrix*modelMatrix;
	//DrawSkybox();
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),"bumpTex"), 1);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1,false, *&modelMatrix.values);//shadow

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false ,*&tempMatrix.values);//shadow

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 6);

	glActiveTexture(GL_TEXTURE6); 
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	//1123
	heightMap -> Draw();
	DrawWater();
	//GenPartiExplo();
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights() {
	SetCurrentShader(pointlightShader);
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBlendFunc(GL_ONE, GL_ONE);
	
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),"depthTex"), 3);

	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),"normTex"), 4);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);
	
	glUniform3fv(glGetUniformLocation(currentShader -> GetProgram(),"cameraPos"), 1, (float *)& camera -> GetPosition());
	
	glUniform2f(glGetUniformLocation(currentShader -> GetProgram(),"pixelSize"), 1.0f / width, 1.0f / height);

	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500,(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));

	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);
	
	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light & l = pointLights[(x * LIGHTNUM) + z];
			float radius = l.GetRadius();
			
			modelMatrix =pushMatrix *
				Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *popMatrix *
				Matrix4::Translation(l.GetPosition()) *
				Matrix4::Scale(Vector3(radius, radius, radius));
			
			l.SetPosition(modelMatrix.GetPositionVector());
			
				SetShaderLight(l);
			
				UpdateShaderMatrices();
			
				float dist = (l.GetPosition() -camera -> GetPosition()).Length();
			if (dist < radius) {// camera is inside the light volume !
				glCullFace(GL_FRONT);
			}
			else {
				glCullFace(GL_BACK);
			}
			sphere -> Draw();	
		}
	}
	Light & l = *light1;
	float radius = l.GetRadius();

	modelMatrix = Matrix4::Translation(l.GetPosition()) *
		          Matrix4::Scale(Vector3(radius, radius, radius));

	l.SetPosition(modelMatrix.GetPositionVector());

	SetShaderLight(l);

	UpdateShaderMatrices();

	float dist = (l.GetPosition() - camera->GetPosition()).Length();
	if (dist < radius) {
		glCullFace(GL_FRONT);
	}
	else {
		glCullFace(GL_BACK);
	}
	sphere->Draw();
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.1f, 0.1f, 0.1f, 1);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::CombineBuffers() {
	SetCurrentShader(combineShader);
	
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),"diffuseTex"), 2);

	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),"emissiveTex"), 3);

	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),"specularTex"), 4);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);
	
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	
	quad -> Draw();
	
	glUseProgram(0);
}

void Renderer::DrawMyText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	TextMesh* mesh = new TextMesh(text, *basicFont);

	if (perspective) {
		textureMatrix.ToIdentity();
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		textureMatrix.ToIdentity();
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}

	SetCurrentShader(textShader);
								
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
}

void Renderer::CalculateFrameRate() {
	char sFPS[20];
	static float framesPerSecond = 0.0f;     
	static float lastTime = 0.0f;
	static float avgfps = 0.0f;

	float currentTime = GetTickCount() * 0.001f;

	++framesPerSecond;

	sprintf(sFPS,"FPS: %d", int(avgfps));
	
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		avgfps = framesPerSecond;
		framesPerSecond = 0;
	}
	DrawMyText(sFPS, Vector3(1, 1, 1), 35.0f);
}

void Renderer::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}

void Renderer::GenPartiExplo() {

	glUseProgram(currentShader->GetProgram());
	SetCurrentShader(particleShader);

	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	
	SetShaderParticleSize(emitterExplo->GetParticleSize());
	emitterExplo->SetParticleSize(100.0f);
	emitterExplo->SetParticleVariance(1.0f);
	emitterExplo->SetLaunchParticles(expNum);
	
	emitterExplo->SetParticleLifetime(2000.0f);
	emitterExplo->SetParticleSpeed(2.0f);

	//modelMatrix = Matrix4::Translation(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 800, RAW_WIDTH * HEIGHTMAP_X));
	
		particlePos = Vector3(camera->GetPosition());
	
	modelMatrix = Matrix4::Translation(particlePos);
	
	UpdateShaderMatrices();
	

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	emitterExplo->Draw();

	glDepthMask(GL_TRUE);

	glUseProgram(0);

}

void Renderer::GenPartiSnow() {

	glUseProgram(currentShader->GetProgram());
	SetCurrentShader(particleShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	SetShaderParticleSize(emitterSnow->GetParticleSize());
	emitterSnow->SetParticleSize(36.0f);
	emitterSnow->SetParticleVariance(1.0f);
	emitterSnow->SetLaunchParticles(20.0f);
	emitterSnow->SetParticleLifetime(3000.0f);
	emitterSnow->SetParticleSpeed(0.02f);

	modelMatrix = Matrix4::Translation(
		Vector3 (0.0f, 2000.0f, 0.0f)
	);
	UpdateShaderMatrices();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	emitterSnow->Draw();
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(0);
}

void Renderer::GenPartiVolcano() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, bufferFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glUseProgram(currentShader->GetProgram());
	SetCurrentShader(particleShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	SetShaderParticleSize(emitterVolcano->GetParticleSize());
	emitterVolcano->SetParticleSize(30.0f);
	emitterVolcano->SetParticleVariance(1.0f);
	emitterVolcano->SetLaunchParticles(15.0f);
	emitterVolcano->SetParticleLifetime(2000.0f);
	emitterVolcano->SetParticleSpeed(0.3f);

	modelMatrix = Matrix4::Translation(
		Vector3(3700.0f, 1098.f, 4300.f)
		);
	UpdateShaderMatrices();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	emitterVolcano->Draw();
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(0);
}

void Renderer::GenPartiExplo1() {
	glUseProgram(currentShader->GetProgram());
	SetCurrentShader(particleShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_X)) {
		SetShaderParticleSize(emitterExplo1->GetParticleSize());
		emitterExplo1->SetParticleSize(50.0f);
		emitterExplo1->SetParticleVariance(1.0f);
		emitterExplo1->SetLaunchParticles(500.0f);
		emitterExplo1->SetParticleLifetime(2000.0f);
		emitterExplo1->SetParticleSpeed(1.1f);

		//modelMatrix = Matrix4::Translation(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 800, RAW_WIDTH * HEIGHTMAP_X));

		particlePos = Vector3(camera->GetPosition());

		modelMatrix = Matrix4::Translation(particlePos);
		UpdateShaderMatrices();
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	emitterExplo1->Draw();

	glDepthMask(GL_TRUE);

	glUseProgram(0);
	
}
