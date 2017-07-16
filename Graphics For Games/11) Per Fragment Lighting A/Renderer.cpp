#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera(0.0f, 0.0f, Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z));
	
	heightMap = new HeightMap(TEXTUREDIR"fuji.raw");

	//quad = Mesh::GenerateQuad();
	//currentShader = new Shader(SHADERDIR"PerPixelVertex.glsl",SHADERDIR"PerPixelFragment.glsl");
	currentShader = new Shader(SHADERDIR"BumpVertex.glsl", SHADERDIR"BumpFragment.glsl");
	
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"BarrenReds1.jpg", SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//heightMap -> SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"BarrenRedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	/*reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl",SHADERDIR"ReflectFragment.glsl");

	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl",SHADERDIR"SkyboxFragment.glsl");

	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl",SHADERDIR"PerPixelFragment.glsl");
*/
    light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 1.4f)),
	Vector4(1, 1, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);

	light1= new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 5.0f)),
	Vector4(0, 1, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) / 1.0f);

	if (!currentShader -> LinkProgram() || !heightMap -> GetTexture()) {
		return;
	}
	/*if (!reflectShader -> LinkProgram() || !lightShader -> LinkProgram() ||!skyboxShader -> LinkProgram()) {
		return;
	}
*/
	SetTextureRepeating(heightMap -> GetTexture(), true);
	SetTextureRepeating(heightMap -> GetBumpMap(), true);
	

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,(float)width / (float)height, 45.0f);
	
	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete light;
	delete light1;
}

void Renderer::UpdateScene(float msec) {
	camera -> UpdateCamera(msec);
	viewMatrix = camera -> BuildViewMatrix();}void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader -> GetProgram());

	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),"bumpTex"), 1);
	glUniform3fv(glGetUniformLocation(currentShader -> GetProgram(),"cameraPos"), 1, (float *)& camera -> GetPosition());
	
	UpdateShaderMatrices();

	SetShaderLight(*light);

	SetShaderLight1(*light1);

	heightMap -> Draw();
	
	glUseProgram(0);
	
	SwapBuffers();
}

