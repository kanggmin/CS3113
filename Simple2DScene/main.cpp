/**
* Author: Jeffrey Park
* Assignment: Simple 2D Scene
* Date due: 2024-02-17, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/*
-One of your objects must be moving in relation to the other object.
EC
-Have your background change color in a pattern (e.g. every 2 seconds).
*/

#ifdef _WINDOWS
#include <GL/glew.h>
#endif


//define necessary files
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

//define log
#define LOG(argument) std::cout << argument << '\n'

//GLOBAL VARIABLES
const int WINDOW_WIDTH = 640,
	WINDOW_HEIGHT = 480;

//RGB values for background
float BG_RED = 0.3f,
	BG_BLUE = 0.4f,
	BG_GREEN = 0.5f,
	BG_OPACITY = 1.0f;


//viewport variables
const int VIEWPORT_X = 0,
	VIEWPORT_Y = 0,
	VIEWPORT_WIDTH = WINDOW_WIDTH,
	VIEWPORT_HEIGHT = WINDOW_HEIGHT;

//EARTH AND MOON TRANSFORMATION VARIABLES
float EARTH_ROT;
float MOON_ROT;

float EARTH_X_TRAN = 0.0f;
float EARTH_Y_TRAN = 0.0f;

float MOON_X_TRAN = 0.0f;
float MOON_Y_TRAN = 0.0f;

const float EARTH_RAD = 1.0f;
const float MOON_RAD = 2.0f;

const int E_DEGREES_PER_SEC = 10.0f;
const int M_DEGREES_PER_SEC = 90.0f;

//earth and moon model matrices
glm::mat4 earth_model_matrix;
glm::mat4 moon_model_matrix;

//define displaywindow and create variable for game loop
SDL_Window* displayWindow;
bool gameIsRunning = true;

//shader variables
ShaderProgram g_program;
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";


//texture variables
const char EARTH_SPRITE[] = "earth.png";
const char MOON_SPRITE[] = "moon.png";

GLuint g_earth_texture_id;
GLuint g_moon_texture_id;

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

//variables to use deltaTime
float PREV_TICKS = 0.0f;

const float MS_IN_S = 1000.0f;

float deltaTime;
float BG_COUNTER = 0.0f;

//load textures into gl
GLuint load_texture(const char* filepath) {
	int width, height, number_of_components;
	unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

	if (image == NULL) {
		LOG("Unable to load image. Make sure path is correct.");
		assert(false);
	}

	GLuint textureID;
	glGenTextures(NUMBER_OF_TEXTURES, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);

	return textureID;
}


//draw given object id into scene
void draw_object(glm::mat4 object_model_matrix, GLuint object_texture_id) {
	g_program.set_model_matrix(object_model_matrix);
	glBindTexture(GL_TEXTURE_2D, object_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void initialise() {
	//initalise window
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Simple 2D Scene", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

	//initialise context
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	

#ifdef _WINDOWS
	glewInit();
#endif

	//initialise viewport
	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	g_program.load(V_SHADER_PATH, F_SHADER_PATH);

	g_view_matrix = glm::mat4(1.0f);
	g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	g_program.set_view_matrix(g_view_matrix);
	g_program.set_projection_matrix(g_projection_matrix);
	glUseProgram(g_program.get_program_id());

	//initialise matrices
	earth_model_matrix = glm::mat4(1.0f);
	moon_model_matrix = glm::mat4(1.0f);

	//initialise textures
	g_earth_texture_id = load_texture(EARTH_SPRITE);
	g_moon_texture_id = load_texture(MOON_SPRITE);

	//set background
	glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

	
	//enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input() {
	SDL_Event event;
	//close game if window closed
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}
}

void update() {
	//create deltatime
	float ticks = (float)SDL_GetTicks() / MS_IN_S;
	deltaTime = ticks - PREV_TICKS;
	PREV_TICKS = ticks;

	//define earth and moon transformations
	EARTH_X_TRAN += cos(ticks) * EARTH_RAD * deltaTime;
	EARTH_Y_TRAN += sin(ticks) * EARTH_RAD * deltaTime;
	EARTH_ROT = E_DEGREES_PER_SEC * deltaTime;

	MOON_X_TRAN += cos(ticks) * MOON_RAD * deltaTime;
	MOON_Y_TRAN += sin(ticks) * MOON_RAD * deltaTime;
	MOON_ROT += M_DEGREES_PER_SEC * deltaTime;

	float X_SCALE = cos(ticks);
	float Y_SCALE = cos(ticks);
	
	//earth translation
	earth_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(EARTH_X_TRAN, EARTH_Y_TRAN, 0.f));
	
	//moon transformation and rotation
	moon_model_matrix = glm::translate(earth_model_matrix, glm::vec3(MOON_X_TRAN, MOON_Y_TRAN, 0.f));
	moon_model_matrix = glm::rotate(moon_model_matrix, glm::radians(MOON_ROT), glm::vec3(0.0f, 0.0f, 1.0f));

	//earth scaling
	earth_model_matrix = glm::scale(earth_model_matrix, glm::vec3(X_SCALE, Y_SCALE, 1.0f));
}

void render() {

	glClear(GL_COLOR_BUFFER_BIT);
	
	// Vertices
	float vertices[] = {
		-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
	};

	// Textures
	float texture_coordinates[] = {
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
	};
	
	//count time
	BG_COUNTER += deltaTime;

	//if 2 seconds pass add one to red and set background
	if (BG_COUNTER >= 2) {
		BG_RED += 0.1f;
		BG_COUNTER = 0;
		glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
	}

	glVertexAttribPointer(g_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(g_program.get_position_attribute());
	
	glVertexAttribPointer(g_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
	glEnableVertexAttribArray(g_program.get_tex_coordinate_attribute());

	draw_object(earth_model_matrix, g_earth_texture_id);
	draw_object(moon_model_matrix, g_moon_texture_id);

	glDisableVertexAttribArray(g_program.get_position_attribute());
	glDisableVertexAttribArray(g_program.get_tex_coordinate_attribute());

	SDL_GL_SwapWindow(displayWindow);
}

void shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	initialise();

	while (gameIsRunning) {
		process_input();

		update();

		render();
	}

	shutdown();
	return 0;
}