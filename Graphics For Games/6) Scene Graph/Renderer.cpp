#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
    CubeRobot::CreateCube(); // Important !
    camera = new Camera();
		
	currentShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");

    if (!currentShader -> LinkProgram()) {
		 return;
	}

	 projMatrix = Matrix4::Perspective(1.0f, 10000.0f,(float)width / (float)height, 45.0f);

	 camera -> SetPosition(Vector3(0, 30, 175));
	
	 root = new SceneNode();
	 for (int i = 0; i < 9; i++) {
		 for (int j = 0; j < 9; j++) {
			 for (int k = 0; k < 9; k++){
			 CubeRobot * zaku = new CubeRobot();
			 zaku->SetTransform(Matrix4::Translation(Vector3((i-4) * 100, (j-4) * 100, (k-4) * 100)) * Matrix4::Scale(Vector3(2, 2, 2)));
			 root->AddChild(zaku);
		 }
	 }
 }
	
	 glEnable(GL_DEPTH_TEST);
	 init = true;

}
 Renderer ::~Renderer(void) {
	 delete root;
	 CubeRobot::DeleteCube(); // Also important !
}

 void Renderer::UpdateScene(float msec,float f) {
	 camera->UpdateCamera(msec);
	 viewMatrix = camera -> BuildViewMatrix();
	 root->Update(msec);
	 root->SetTransform(Matrix4::Rotation(f, Vector3(1, 3, 5)));
 }

 void Renderer::RenderScene() {
	  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	 
	  glUseProgram(currentShader -> GetProgram());
	  UpdateShaderMatrices();
	 
	  glUniform1i(glGetUniformLocation(currentShader -> GetProgram(), "diffuseTex"), 1);

	  DrawNode(root);
	
	  glUseProgram(0);

	  SwapBuffers(); }void Renderer::DrawNode(SceneNode * n) {
	 if (n -> GetMesh()) {
		 Matrix4 transform = n -> GetWorldTransform() * Matrix4::Scale(n -> GetModelScale());

		 glUniformMatrix4fv(
		 glGetUniformLocation(currentShader -> GetProgram(),"modelMatrix"), 1, false, (float *)& transform);
		
		 glUniform4fv(glGetUniformLocation(currentShader -> GetProgram(),"nodeColour"), 1, (float *)& n -> GetColour());

		 glUniform1i(glGetUniformLocation(currentShader -> GetProgram(), "useTexture"), (int)n -> GetMesh() -> GetTexture());

		 n -> Draw(*this);
	 }
	 
		  for (vector < SceneNode * >::const_iterator
			  i = n -> GetChildIteratorStart();
			  i != n -> GetChildIteratorEnd(); ++i) {
		  DrawNode(*i);
	 } }