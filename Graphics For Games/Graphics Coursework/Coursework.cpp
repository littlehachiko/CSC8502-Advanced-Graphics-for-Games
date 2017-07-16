#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/Window.h"
#include "Renderer.h"

int main() {
	Window w("Coursework", 1280,720,false);
	if(!w.HasInitialised()) {
		return -1;
	}

	srand((unsigned int)w.GetTimer()->GetMS() * 1000.0f);
	
	Renderer renderer(w); 
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
			
		}
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}