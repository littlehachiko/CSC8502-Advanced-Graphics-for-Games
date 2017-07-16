#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	CubeRobot::CreateCube();
	camera = new Camera();
	camera->SetPosition(Vector3(0.0f, 750.0f, 750.0f));
	sceneShader=new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	currentShader = new Shader(SHADERDIR"LandVertex.glsl", SHADERDIR"LandFragment.glsl");

	if (!currentShader->LinkProgram()) {
		return;
	}
	if (!sceneShader->LinkProgram()) {
		return;
	}
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"BarrenReds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	if (!heightMap->GetTexture()) {
		return;
	}
	SetTextureRepeating(heightMap->GetTexture(), true);    projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);	root = new SceneNode();
	/*for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 9; k++) {
				CubeRobot * zaku = new CubeRobot();
				zaku->SetTransform(Matrix4::Translation(Vector3((i - 4) * 100, (j - 4) * 100, (k - 4) * 100)) * Matrix4::Scale(Vector3(2, 2, 2)));*/
				root->AddChild(/*zaku*/new CubeRobot());
	/*		}
		}
	}	*/
				root->SetTransform(root->GetTransform() * Matrix4::Translation(Vector3(15.0f, 200.0f, 0.0f)));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	init = true;}Renderer ::~Renderer(void) {
	 delete heightMap;
	 delete camera;
	 delete root;
	 CubeRobot::DeleteCube();
}

void Renderer::UpdateScene(float msec) {
	camera -> UpdateCamera(msec);
	viewMatrix = camera -> BuildViewMatrix();	root->Update(msec);
	/*root->SetTransform(Matrix4::Rotation(f, Vector3(1, 3, 5)));*/}void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(currentShader -> GetProgram());
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),"diffuseTex"), 0);
	heightMap -> Draw();

	glUseProgram(sceneShader->GetProgram());
	UpdateShaderMatrices1();
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 1);
	DrawNode(root);
	
	glUseProgram(0);
	SwapBuffers();}void Renderer::DrawNode(SceneNode * n) {
	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());


	glUniformMatrix4fv(
	glGetUniformLocation(sceneShader->GetProgram(), "modelMatrix"), 1, false, (float *)& transform);

	glUniform4fv(glGetUniformLocation(sceneShader->GetProgram(), "nodeColour"), 1, (float *)& n->GetColour());

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());

	n->Draw(*this);
	}

	for (vector < SceneNode * >::const_iterator
		i = n->GetChildIteratorStart();
		i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}}