#include "libs.h"


// ------------------------------------------------
//  GLOBALS
// ------------------------------------------------
const char* WINDOW_TITLE = "Graphic Engine";
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int OPENGL_MAJOR_VERSION = 4;
const int OPENGL_MINOR_VERSION = 6;

int framebufferWidth = 0;
int framebufferHeight = 0;

bool firstMouseMove = true;
float lastMouseX = SCREEN_WIDTH * 0.5f;
float lastMouseY = SCREEN_HEIGHT * 0.5f;

Direction_Movement cameraDirFlag = Direction_Movement::NONE;

Camera camera({ -15.0f, 25.0f, 35.0f }, { 0.0f, 1.0f, 0.0f }, -40.0f, -70.0f);

// ------------------------------------------------
//  FUNCTIONS
// ------------------------------------------------
void updateWindowStats(GLFWwindow* window, float deltaTime, size_t vertexCount)
{
	// Static variables to not go out of scope after function ends
	static float timer = 0.0f;
	static int frames = 0;

	timer += deltaTime;
	frames++;

	// Update window stats only once per second
	if (timer >= 1.0f)
	{
		// Calculate FPS
		int fps = glm::round(frames / timer);

		std::stringstream ss;
		ss << WINDOW_TITLE << " | FPS: " << fps << " | Vertices: " << vertexCount;

		// Update stats
		glfwSetWindowTitle(window, ss.str().c_str());

		// Resets timers
		timer = 0.0f;
		frames = 0;
	}
}

void processCameraDirectionMovement(Direction_Movement direction, int action)
{
	if (action == GLFW_PRESS)
		cameraDirFlag |= direction;
	else if (action == GLFW_RELEASE)
		cameraDirFlag &= ~direction;
}


// ------------------------------------------------
//  CALLBACKS
// ------------------------------------------------
void keyboard_input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_TAB:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case GLFW_KEY_W:
		processCameraDirectionMovement(Direction_Movement::FORWARD, action);
		break;
	case GLFW_KEY_S:
		processCameraDirectionMovement(Direction_Movement::BACKWARD, action);
		break;
	case GLFW_KEY_A:
		processCameraDirectionMovement(Direction_Movement::LEFT, action);
		break;
	case GLFW_KEY_D:
		processCameraDirectionMovement(Direction_Movement::RIGHT, action);
		break;
	case GLFW_KEY_SPACE:
		processCameraDirectionMovement(Direction_Movement::UP, action);
		break;
	case GLFW_KEY_LEFT_CONTROL:
		processCameraDirectionMovement(Direction_Movement::DOWN, action);
		break;
	case GLFW_KEY_Q:
		processCameraDirectionMovement(Direction_Movement::ROLL_LEFT, action);
		break;
	case GLFW_KEY_E:
		processCameraDirectionMovement(Direction_Movement::ROLL_RIGHT, action);
		break;
	case GLFW_KEY_LEFT_SHIFT:
		camera.MovementSpeed += 0.5f;
		break;
	}
}

void mouse_movement_callback(GLFWwindow* window, double mousePosX, double mousePosY)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (firstMouseMove)
	{
		lastMouseX = mousePosX;
		lastMouseY = mousePosY;
		firstMouseMove = false;
	}

	float deltaX = static_cast<float>(mousePosX) - lastMouseX;
	float deltaY = lastMouseY - static_cast<float>(mousePosY);

	lastMouseX = mousePosX;
	lastMouseY = mousePosY;

	camera.processMouseMovement(deltaX, deltaY);
}

void mouse_scroll_callback(GLFWwindow* window, double deltaX, double deltaY)
{
	camera.processMouseScroll(deltaY);
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	framebufferWidth = width;
	framebufferHeight = height;
	glViewport(0, 0, width, height);
}


// ------------------------------------------------
//  INITIALIZATION
// ------------------------------------------------
bool initializeOpenGLEngine(GLFWwindow*& window, int& fbW, int& fbH)
{
	std::cout << "INITIALIZING::GLFW::LIBRARY\n";
	if (!glfwInit()) {
		std::cerr << "ERROR::GLFW_INIT\n";
		return false;
	}

	// OpenGL version hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	std::cout << "CREATING::GLFW::WINDOW\n";
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
	if (!window)
	{
		std::cerr << "ERROR::GLFW_CREATE_WINDOW\n";
		glfwTerminate();
		return false;
	}

	// Make OpenGL context current
	glfwMakeContextCurrent(window);
	// Enable v-sync
	glfwSwapInterval(0);

	// Load OpenGL functions
	std::cout << "INITIALIZING::GLAD\n";
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "ERROR::GLAD_LOAD_GL\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	// Set initial viewport
	glfwGetFramebufferSize(window, &fbW, &fbH);
	glViewport(0, 0, fbW, fbH);

	// Set GLFW callbacks and input modes
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, keyboard_input_callback);
	glfwSetCursorPosCallback(window, mouse_movement_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

	// Enable common OpenGL states
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return true;
}


// ------------------------------------------------
//  MAIN
// ------------------------------------------------
int main()
{
	GLFWwindow* window = nullptr;

	if (!initializeOpenGLEngine(window, framebufferWidth, framebufferHeight))
		return EXIT_FAILURE;

	// Load Shaders
	Shader shaderPBRProgram("Shaders/vertex_shader_core.vert", "Shaders/fragment_shader_pbr.frag");
	Shader shaderPhongProgram("Shaders/vertex_shader_core.vert", "Shaders/fragment_shader_core.frag");

	// Load Textures
	Texture albedoTex("Assets/Textures/Metal_color.png", GL_TEXTURE_2D);
	Texture normalTex("Assets/Textures/Metal_normal_gl.png", GL_TEXTURE_2D);
	Texture metallicTex("Assets/Textures/Metal_metalness.png", GL_TEXTURE_2D);
	Texture roughnessTex("Assets/Textures/Metal_roughness.png", GL_TEXTURE_2D);
	Texture ambientOcclusionTex("Assets/Textures/Metal_ambient_occlusion.png", GL_TEXTURE_2D);

	// Default position and color of light
	glm::vec3 lightPosition(0.0f, 0.0f, 5.0f);
	glm::vec3 lightColor(5.0f, 5.0f, 5.0f);

	// Creating PBR material for testing
	PBRMaterial pbrMaterial(glm::vec3(1.0f), 0.0f, 0.5f, 1.0f, &albedoTex, &normalTex, &metallicTex, &roughnessTex, &ambientOcclusionTex);
	PhongMaterial baseMaterial(glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(1.0f), 32.0f);
	PhongMaterial metalMaterial(glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(1.0f), 32.0f, &albedoTex, &metallicTex);

	// Setup primitives
	Plane plane(150.0f, 150.0f);
	Cube cube(6.0f);
	Sphere sphere(1.6f, 64U, 64U);
	Torus torus(18.0f, 1.5f, 64U, 64U);

	Mesh planeGrid(plane);
	Mesh cubeTest(cube);
	Mesh sphereTest(sphere);
	Mesh torusTest(torus);

	planeGrid.setPosition({ 0.0f,-30.0f, 0.0f });
	cubeTest.setPosition({ 0.0f, 0.0f, 0.0f });
	sphereTest.setPosition({ 0.0f, 0.0f, 0.0f });
	torusTest.setPosition({ 0.0f, 0.0f, 0.0f });
	torusTest.setRotation({ 30.0f, 0.0f, 0.0f });

	// Load model
	Model model("Assets/Models/catmark_torus_creases0.obj");

	model.scale({ 10.0f, 10.0f, 10.0f });

	// Camera matrices
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	// Simulation of time (for fixed or delta time update)
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// Render Loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Call any callbacks in the queue
		glfwPollEvents();

		// Camera update
		camera.processMovement(cameraDirFlag, deltaTime);

		// Update view and projection matrices
		viewMatrix = camera.getViewMatrix();
		projectionMatrix = camera.getProjectionMatrix(static_cast<float>(framebufferWidth) / framebufferHeight);

		// Re-color & clear buffers
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Mesh animations
		cubeTest.rotate({
			0.0f,
			10.0f * deltaTime,
			0.0f
			});

		sphereTest.setPosition({
			glm::cos(currentFrame * 0.5f) * 10,
			glm::sin(currentFrame * 0.5f) * 10,
			0.0f
			});
		sphereTest.rotate({
			0.0f,
			deltaTime * 100,
			0.0f
			});

		torusTest.rotate({
			20.0f * deltaTime,
			0.0f,
			10.0f * deltaTime
			});

		// Light movement
		lightPosition.x += glm::cos(currentFrame) * 10 * deltaTime;
		lightPosition.y += glm::cos(currentFrame) * 5 * deltaTime;

		// Using shader program and apply scene changes to Vertex and Fragment shader
		//shaderPBRProgram.use();
		//shaderPBRProgram.set("view_matrix", viewMatrix);
		//shaderPBRProgram.set("projection_matrix", projectionMatrix);
		//shaderPBRProgram.set("light_position", lightPosition);
		//shaderPBRProgram.set("light_color", lightColor);
		//shaderPBRProgram.set("camera_position", camera.Position);

		//pbrMaterial.apply(shaderPBRProgram);
		//sphereTest.render(shaderPBRProgram);
		//torusTest.render(shaderPBRProgram);
		//model.render(shaderPBRProgram);

		shaderPhongProgram.use();
		shaderPhongProgram.set("view_matrix", viewMatrix);
		shaderPhongProgram.set("projection_matrix", projectionMatrix);
		shaderPhongProgram.set("light_position", lightPosition);
		shaderPhongProgram.set("light_color", lightColor);
		shaderPhongProgram.set("camera_position", camera.Position);

		baseMaterial.apply(shaderPhongProgram);
		planeGrid.render(shaderPhongProgram);
		torusTest.render(shaderPhongProgram);

		metalMaterial.apply(shaderPhongProgram);
		cubeTest.render(shaderPhongProgram);
		sphereTest.render(shaderPhongProgram);
		model.render(shaderPhongProgram);
		for (size_t i = 0; i < 1; i++)
		{
			//sphereTest.render(shaderPhongProgram);
			//sphereTest.move({ 5.0f, 0.0f, 0.0f });
		}

		size_t totalVertexCount = (
			planeGrid.getVertexCount() +
			cubeTest.getVertexCount() +
			sphereTest.getVertexCount() +
			torusTest.getVertexCount() +
			model.getTotalVertexCount() +
			0
			);

		// Update stats (in window title) like fps and count of vertices in the scene
		updateWindowStats(window, deltaTime, totalVertexCount);

		// Enable swaping buffers (double buffered scene)
		glfwSwapBuffers(window);
	}

	// Cleanup and call destructors
	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}