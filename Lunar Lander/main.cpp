/**
* Author: Jeffrey Park
* Assignment: Pong Clone
* Date due: 2024-03-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

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
#include <vector>
#include <string>
#include "Platform.h"
#include "Rocket.h"
#define FIXED_TIMESTEP 0.0166666f
float g_time_accumulator = 0.0f;

using namespace std;

//define log
#define LOG(argument) std::cout << argument << '\n'

//GLOBAL VARIABLES
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

//RGB values for background
float BG_RED = 0.f,
BG_BLUE = 0.f,
BG_GREEN = 0.f,
BG_OPACITY = 0.8f;


//viewport variables
const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

//object speeds
float g_player_speed = 1.0f;

//player position and movements
glm::vec3 g_player_position = glm::vec3(-15.0f, 15.0f, 0.0f);
vector<glm::vec3> platform_positions = {
	glm::vec3(-17.0f, -2.0f, 0.0f),
	glm::vec3(-13.0f, -6.0f, 0.0f),
	glm::vec3(-8.0f, -8.0f, 0.0f),
	glm::vec3(-4.0f, -3.0f, 0.0f),
	glm::vec3(0.0f, -4.0f, 0.0f),
	glm::vec3(5.0f, -9.0f, 0.0f),
	glm::vec3(9.0f, -6.0f, 0.0f),
	glm::vec3(12.0f, -1.0f, 0.0f),
	glm::vec3(18.0f, -3.0f, 0.0f)
};

//scaling for objects
glm::vec3 g_player_scale = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 g_platform_scale = glm::vec3(1.0f, 1.0f, 0.0f);

//define displaywindow and create variable for game loop
SDL_Window* displayWindow;

//booleans to track stage of game
bool gameIsRunning;
bool gameIsPlaying = true;

//shader variables
ShaderProgram g_program;
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

//texture variables
const char PLAYER_SPRITE[] = "rocket.png";
const char PLATFORM_SPRITE[] = "platform.png";
const char FONT_SPRITE[] = "font1.png";

GLuint g_player_texture_id;
GLuint g_platform_texture_id;
GLuint font_texture_id;

string win_msg = "Mission Accomplished";
string lose_msg = "Mission Failed";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

//variables to use delta time
float PREV_TICKS = 0.0f;

const float MS_IN_S = 1000.0f;

float delta_time;

const int FONTBANK_SIZE = 16;

struct GameState {
	Rocket* player;
	Platform* platforms;
};

GameState g_state;
int PLATFORM_COUNT = 9;

bool winner = false;

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


//text drawing function from lecture notes
void draw_text(GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
	float width = 1.0f / FONTBANK_SIZE;
	float height = 1.0f / FONTBANK_SIZE;

	vector<float> vertices;
	vector<float> texture_coordinates;

	for (int i = 0; i < text.size(); i++) {
		int spritesheet_index = (int)text[i];
		float offset = (screen_size + spacing) * i;

		float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
		float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

		vertices.insert(vertices.end(), {
			offset + (-0.5f * screen_size), 0.5f * screen_size,
			offset + (-0.5f * screen_size), -0.5f * screen_size,
			offset + (0.5f * screen_size), 0.5f * screen_size,
			offset + (0.5f * screen_size), -0.5f * screen_size,
			offset + (0.5f * screen_size), 0.5f * screen_size,
			offset + (-0.5f * screen_size), -0.5f * screen_size,
			});

		texture_coordinates.insert(texture_coordinates.end(), {
			u_coordinate, v_coordinate,
			u_coordinate, v_coordinate + height,
			u_coordinate + width, v_coordinate,
			u_coordinate + width, v_coordinate + height,
			u_coordinate + width, v_coordinate,
			u_coordinate, v_coordinate + height,
			});
	}

	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, position);

	g_program.set_model_matrix(model_matrix);
	glUseProgram(g_program.get_program_id());

	glVertexAttribPointer(g_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
	glVertexAttribPointer(g_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());

	glEnableVertexAttribArray(g_program.get_position_attribute());
	glEnableVertexAttribArray(g_program.get_tex_coordinate_attribute());

	glBindTexture(GL_TEXTURE_2D, font_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

	glDisableVertexAttribArray(g_program.get_position_attribute());
	glDisableVertexAttribArray(g_program.get_tex_coordinate_attribute());
}



void initialise() {
	//initalise window
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

	//initialise context
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	gameIsRunning = true;

#ifdef _WINDOWS
	glewInit();
#endif

	//initialise viewport
	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	g_program.load(V_SHADER_PATH, F_SHADER_PATH);

	g_view_matrix = glm::mat4(1.0f);
	g_projection_matrix = glm::ortho(-20.0f, 20.0f, -15.0f, 15.0f, -1.0f, 1.0f);

	g_program.set_view_matrix(g_view_matrix);
	g_program.set_projection_matrix(g_projection_matrix);
	glUseProgram(g_program.get_program_id());

	g_program.set_colour(1, 0, 0, 1);

	//set background
	glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

	//initialise gamestate
	g_player_texture_id = load_texture(PLAYER_SPRITE);
	g_platform_texture_id = load_texture(PLATFORM_SPRITE);
	font_texture_id = load_texture(FONT_SPRITE);
	g_state.player = new Rocket(g_player_position, g_player_scale, g_player_speed, g_player_texture_id);

	g_state.platforms = new Platform[PLATFORM_COUNT];
	for (int i = 0; i < PLATFORM_COUNT; ++i) {
		g_state.platforms[i].set_textID(g_platform_texture_id);
		g_state.platforms[i].set_pos(platform_positions[i]);
		g_state.platforms[i].set_scale(g_platform_scale);
	}

	//enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}

void process_input() {
	SDL_Event event;
	const Uint8* key_state = SDL_GetKeyboardState(NULL);

	if (gameIsPlaying) {
		//move player1 up if w is pressed
		if (key_state[SDL_SCANCODE_UP]) {
			g_state.player->increase_y();
		}
		//move player1 down if s is pressed
		else if (key_state[SDL_SCANCODE_DOWN]) {
			g_state.player->decrease_y();
		}
		//move player1 left if a is pressed
		if (key_state[SDL_SCANCODE_LEFT]) {
			g_state.player->decrease_x();
		}
		//move player1 right if d is pressed
		else if (key_state[SDL_SCANCODE_RIGHT]) {
			g_state.player->increase_x();
		}
	}

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			//close game when window closed to quit
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;
		}
	}
}

void update() {
	//create delta time
	float ticks = (float)SDL_GetTicks() / MS_IN_S;
	delta_time = ticks - PREV_TICKS;
	PREV_TICKS = ticks;
	delta_time += g_time_accumulator;

	for (int i = 0; i < PLATFORM_COUNT; ++i) {
		g_state.platforms[i].update();
		if (g_state.player->detect_collision(&g_state.platforms[i])) {
			gameIsPlaying = false;
			if (g_state.player->get_position().y > g_state.platforms[i].get_pos().y) {
				winner = true;
			}
			else {
				winner = false;
			}
		}
	}

	if (g_state.player->get_position().x > 20.0f || g_state.player->get_position().x < -20.0f || g_state.player->get_position().y < -15.0f) {
		gameIsPlaying = false;
		winner = false;
	}


	if (delta_time < FIXED_TIMESTEP)
	{
		g_time_accumulator = delta_time;
		return;
	}
	while (delta_time >= FIXED_TIMESTEP)
	{
		if (gameIsPlaying) {
			g_state.player->update(FIXED_TIMESTEP);
		}
		delta_time -= FIXED_TIMESTEP;
	}
	g_time_accumulator = delta_time;
}

void render() {

	glClear(GL_COLOR_BUFFER_BIT);

	g_state.player->render(&g_program);
	for (int i = 0; i < PLATFORM_COUNT; ++i) {
		g_state.platforms[i].render(&g_program);
	}

	if (!gameIsPlaying) {
		if (winner) {

			draw_text(font_texture_id, win_msg, 1, 0.1, glm::vec3(-8.0f, 10.0f, 0.0f));
		}
		else {
			draw_text(font_texture_id, lose_msg, 1, 0.1, glm::vec3(-8.0f, 10.0f, 0.0f));
		}
	}

	SDL_GL_SwapWindow(displayWindow);
}

void shutdown() {
	delete g_state.platforms;
	delete g_state.player;
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