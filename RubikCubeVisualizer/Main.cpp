#include "ShaderProgram.h"
#include "LightShaderUniforms.h"
#include "RubikCubeControl.h"

#include <memory>
#include <thread>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace {
	
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const int DELTA_TIME = 1000.f / 30;
	const float MAX_ROTATION_DIST = 30.f;

	int glutWindow;

	std::unique_ptr<ShaderProgram> shader;
	std::shared_ptr<RubikCube> rubikCube;
	std::unique_ptr<RubikCubeControl> rubikCubeControl;
	Camera camera(1.f * WINDOW_WIDTH, 1.f * WINDOW_HEIGHT);

	GLint positionAttribute;
	GLint normalAttribute;

	MatrixShaderUniforms matrixUniforms;
	MaterialShaderUniforms materialUniforms;
	LightShaderUniforms lightUniforms;
	GLint eyePositionUniform;

	bool cameraRotationEnabled = false;
	int prevMouseX = -1;
	int prevMouseY = -1;

	void InitializeUniforms()
	{
		// in attributes
		positionAttribute = glGetAttribLocation(shader->GetProgram(), "position");
		normalAttribute = glGetAttribLocation(shader->GetProgram(), "normal");

		// matrices
		matrixUniforms.pvmMatrixUniform = glGetUniformLocation(shader->GetProgram(), "pvm_matrix");
		matrixUniforms.normalMatrixUniform = glGetUniformLocation(shader->GetProgram(), "normal_matrix");
		matrixUniforms.modelMatrixUniform = glGetUniformLocation(shader->GetProgram(), "model_matrix");

		// materials
		materialUniforms.ambientColorUniform = glGetUniformLocation(shader->GetProgram(), "material_ambient_color");
		materialUniforms.diffuseColorUniform = glGetUniformLocation(shader->GetProgram(), "material_diffuse_color");
		materialUniforms.specularColorUniform = glGetUniformLocation(shader->GetProgram(), "material_specular_color");
		materialUniforms.shininessUniform = glGetUniformLocation(shader->GetProgram(), "material_shininess");

		// light
		lightUniforms.ambientColorUniform = glGetUniformLocation(shader->GetProgram(), "light_ambient_color");
		lightUniforms.diffuseColorUniform = glGetUniformLocation(shader->GetProgram(), "light_diffuse_color");
		lightUniforms.specularColorUniform = glGetUniformLocation(shader->GetProgram(), "light_specular_color");
		lightUniforms.positionUniform = glGetUniformLocation(shader->GetProgram(), "light_position");

		eyePositionUniform = glGetUniformLocation(shader->GetProgram(), "eye_position");
	}

	void Initialize()
	{
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);

		shader = std::make_unique<ShaderProgram>("VertexShader.glsl", "FragmentShader.glsl");
		InitializeUniforms();

		rubikCube = std::make_shared<RubikCube>(positionAttribute, normalAttribute, 3);
		rubikCubeControl = std::make_unique<RubikCubeControl>(rubikCube);
	}

	void Destroy()
	{
		// Must be called before OpenGL destroys it's own content
		rubikCubeControl.reset();
		rubikCube.reset();
		shader.reset();
		glutDestroyWindow(glutWindow);
	}

	void SetupLight()
	{
		glm::vec4 lightPosition(camera.GetEyePosition(), 1.f);
		glUniform4fv(lightUniforms.positionUniform, 1, glm::value_ptr(lightPosition));
		glUniform3f(lightUniforms.ambientColorUniform, .05f, .05f, .05f);
		glUniform3f(lightUniforms.diffuseColorUniform, 1.f, 1.f, 1.f);
		glUniform3f(lightUniforms.specularColorUniform, 1.f, 1.f, 1.f);
	}

	void SetupEyePosition()
	{
		glUniform3fv(eyePositionUniform, 1, glm::value_ptr(camera.GetEyePosition()));
	}

	void Update()
	{
		if (!rubikCubeControl->IsRunning()) {
			Destroy();
			exit(0);
		}

		// FYI delta time is overkill for this simple animation
		rubikCube->Update(1.f / DELTA_TIME);
	}

	void Display()
	{
		// Update part
		Update();

		// Draw part
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->SetActive();
		SetupLight();
		rubikCube->Draw(camera, matrixUniforms, materialUniforms);
		shader->SetInactive();

		glutSwapBuffers();
	}

	void MouseButton(int button, int state, int x, int y)
	{
		if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON) {
			cameraRotationEnabled = (state == GLUT_DOWN);
		}
	}

	void MouseMotion(int x, int y)
	{
		// Initialization
		if (prevMouseX < 0 && prevMouseY < 0) {
			// @goto masterrace
			prevMouseX = x;
			prevMouseY = y;
			return;
		}

		auto deltaX = static_cast<float>(x - prevMouseX);
		auto deltaY = static_cast<float>(y - prevMouseY);

		if (cameraRotationEnabled) {
			if (fabsf(deltaX) < MAX_ROTATION_DIST && fabsf(deltaY) < MAX_ROTATION_DIST) {
				camera.Rotate(deltaX, deltaY);
			}
		}

		prevMouseX = x;
		prevMouseY = y;
	}

	void WheelMotion(int wheel, int direction, int mx, int my)
	{
		if (direction < 0) {
			camera.ZoomIn();
		}
		else {
			camera.ZoomOut();
		}
	}

	void KeyboardDown(unsigned char key, int mx, int my)
	{	
		switch (key) {
		case 27:
			Destroy();
			exit(0);
			break;
		/*case '\t':
			cubeRotationPositive = !cubeRotationPositive;
			break;
		case 'q':
			rubikCube->Rotate(RubikCube::X_AXIS, 0, cubeRotationPositive);
			break;
		case 'w':
			rubikCube->Rotate(RubikCube::X_AXIS, 1, cubeRotationPositive);
			break;
		case 'e':
			rubikCube->Rotate(RubikCube::X_AXIS, 2, cubeRotationPositive);
			break;
		case 'a':
			rubikCube->Rotate(RubikCube::Y_AXIS, 0, cubeRotationPositive);
			break;
		case 's':
			rubikCube->Rotate(RubikCube::Y_AXIS, 1, cubeRotationPositive);
			break;
		case 'd':
			rubikCube->Rotate(RubikCube::Y_AXIS, 2, cubeRotationPositive);
			break;
		case 'z':
			rubikCube->Rotate(RubikCube::Z_AXIS, 0, cubeRotationPositive);
			break;
		case 'x':
			rubikCube->Rotate(RubikCube::Z_AXIS, 1, cubeRotationPositive);
			break;
		case 'c':
			rubikCube->Rotate(RubikCube::Z_AXIS, 2, cubeRotationPositive);
			break;
			*/
		}
	}

	void KeyboardUp(unsigned char key, int mx, int my)
	{
	}

	void Timer(int value)
	{
		glutTimerFunc(DELTA_TIME, Timer, 0);
		glutPostRedisplay();
	}

	void OpenGLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
		const char* message, const void* userParam)
	{
		std::cout << message << std::endl;
	}

	void SetupOpenGLCallback()
	{
		auto debugExtAddr = wglGetProcAddress("glDebugMessageCallbackARB");
		auto debugExtCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKARBPROC>(debugExtAddr);

		if (debugExtCallback) // callback function exists
		{
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			debugExtCallback(reinterpret_cast<GLDEBUGPROCARB>(OpenGLCallback), nullptr);
		}
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutWindow = glutCreateWindow("Rubik's Cube Visualizer");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		return -1;
	}

	Initialize();
	SetupOpenGLCallback();

	glutDisplayFunc(Display);
	glutKeyboardFunc(KeyboardDown);
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutMouseWheelFunc(::WheelMotion);
	glutTimerFunc(DELTA_TIME, Timer, 0);

	glutMainLoop();

	Destroy();

	return 0;
}
