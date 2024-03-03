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

using namespace std;

//define log
#define LOG(argument) std::cout << argument << '\n'

//GLOBAL VARIABLES
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

//RGB values for background
float BG_RED = 0.2f,
BG_BLUE = 0.6f,
BG_GREEN = 0.3f,
BG_OPACITY = 0.1f;


//viewport variables
const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

//object matrices
glm::mat4 p_rectangle1_matrix;
glm::mat4 p_rectangle2_matrix;
glm::mat4 g_ball1_matrix;
glm::mat4 g_ball2_matrix;
glm::mat4 g_ball3_matrix;

glm::mat4 g_ball_matrices[] = {
	g_ball1_matrix,
	g_ball2_matrix,
	g_ball3_matrix
};

//object speeds
float g_player_speed = 3.0f;
float g_ball_speed = 1.5f;

//player and ball, positions and movements
glm::vec3 g_player1_position = glm::vec3(0, 0, 0);
glm::vec3 g_player1_movement = glm::vec3(0, 0, 0);

glm::vec3 g_player2_position = glm::vec3(0, 0, 0);
glm::vec3 g_player2_movement = glm::vec3(0, 0, 0);

bool player2isPlaying = true;
bool goingUp = true;

glm::vec3 g_ball1_position = glm::vec3(0, 0, 0);
glm::vec3 g_ball1_movement = glm::vec3(1, 0, 0);

glm::vec3 g_ball2_position = glm::vec3(0, 2, 0);
glm::vec3 g_ball2_movement = glm::vec3(-0.5, 0, 0);

glm::vec3 g_ball3_position = glm::vec3(0, -2, 0);
glm::vec3 g_ball3_movement = glm::vec3(1, 0, 0);

glm::vec3 g_ball_positions[] = { g_ball1_position, g_ball2_position, g_ball3_position };
glm::vec3 g_ball_movements[] = { g_ball1_movement, g_ball2_movement, g_ball3_movement };


//scaling for objects
glm::vec3 paddle_scale = glm::vec3(1, 1, 0);
glm::vec3 ball_scale = glm::vec3(0.5, 0.5, 0);

//define displaywindow and create variable for game loop
SDL_Window* displayWindow;

//booleans to track stage of game
bool gameIsRunning;
bool gameIsPlaying;
bool canStartGame;

//shader variables
ShaderProgram g_program;
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";


//texture variables
const char PLAYER1_SPRITE[] = "player1.jpg";
const char PLAYER2_SPRITE[] = "player2.jpg";
const char BALL_SPRITE[] = "baseball.png";
const char FONT_SPRITE[] = "font1.png";

GLuint g_player1_texture_id;
GLuint g_player2_texture_id;
GLuint g_ball_texture_id;
GLuint g_font_texture_id;

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

//variables to use delta time
float PREV_TICKS = 0.0f;

const float MS_IN_S = 1000.0f;

float delta_time;
float BG_COUNTER = 0.0f;

const int FONTBANK_SIZE = 16;

//messages for ui
const string INTRO_MSG = "CHOOSE # OF BALLS (1,2,3)";
const string INTRO_MSG2 = "PRESS Y TO START";
const string WIN_MSG1 = "PLAYER 1 WINS";
const string WIN_MSG2 = "PLAYER 2 WINS";

//number of balls being played
int ball_num = 1;

// 1 is player 1 won, 2 is player 2 won
int winner;

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
void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
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

	program->set_model_matrix(model_matrix);
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

//calculate positions of objects to search for collision
bool detect_collision(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 scale1, glm::vec3 scale2) {
	float x_dist = fabs(pos1.x - pos2.x) - ((scale1.x + scale2.x) / 2.0f);
	float y_dist = fabs(pos1.y - pos2.y) - ((scale1.y + scale2.y) / 2.0f);

	if (x_dist <= 0.0f && y_dist <= 0.0f) {
		return true;
	}
	return false;
}

//ensure that paddles dont go out of borders
glm::vec3 enforce_bounds(glm::vec3 p_position) {
	if (p_position.y >= 3.0f) {
		p_position.y = 3.0f;
	}
	else if (p_position.y <= -3.0f) {
		p_position.y = -3.0f;
	}
	return p_position;
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

	gameIsRunning = true;
	gameIsPlaying = false;
	canStartGame = true;
	winner = 0;

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

	//set background
	glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

	//initialise matrices
	p_rectangle1_matrix = glm::mat4(1.0f);
	p_rectangle2_matrix = glm::mat4(1.0f);
	g_ball1_matrix = glm::mat4(1.0f);
	g_ball2_matrix = glm::mat4(1.0f);
	g_ball3_matrix = glm::mat4(1.0f);

	//initialise textures
	g_player1_texture_id = load_texture(PLAYER1_SPRITE);
	g_player2_texture_id = load_texture(PLAYER2_SPRITE);
	g_ball_texture_id = load_texture(BALL_SPRITE);
	g_font_texture_id = load_texture(FONT_SPRITE);

	//enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input() {
	SDL_Event event;
	const Uint8* key_state = SDL_GetKeyboardState(NULL);

	if (gameIsPlaying) {
		//move player1 up if w is pressed
		if (key_state[SDL_SCANCODE_W]) {
			g_player1_movement.y = 1.0f;
		}
		//move player1 down if s is pressed
		else if (key_state[SDL_SCANCODE_S]) {
			g_player1_movement.y = -1.0f;
		}
		//player 1 movement set to 0 otherwise
		else {
			g_player1_movement.y = 0.0f;
		}
		//check if player 2 is playing and allow controls if true
		if (player2isPlaying) {
			//up arrow to move player 2 up
			if (key_state[SDL_SCANCODE_UP]) {
				g_player2_movement.y = 1.0f;
			}
			//down arrow to move player 2 down
			else if (key_state[SDL_SCANCODE_DOWN]) {
				g_player2_movement.y = -1.0f;
			}
			//player 2 does not move if up or down not pressed
			else {
				g_player2_movement.y = 0.0f;
			}
		}
		else {
			//move player 2 paddles up and down borders of screen if player 2 not playing
			if (g_player2_position.y > 3.0f || g_player2_position.y < -3.0f) {
				goingUp = !goingUp;
			}
			if (goingUp) {
				g_player2_movement.y = 1.0f;
			}
			else {
				g_player2_movement.y = -1.0f;
			}
		}
		for (int i = 0; i < ball_num; ++i) {
			//check if ball is out of bounds on left or right
			//player 1 wins if ball x coord >= 5
			if (g_ball_positions[i].x >= 5) {
				gameIsPlaying = false;
				winner = 1;
				g_ball_movements[i].x, g_ball_movements[i].y = 0, 0;
			}
			//player 2 wins if ball x coord <= 5
			else if (g_ball_positions[i].x <= -5) {
				gameIsPlaying = false;
				winner = 2;
				g_ball_movements[i].x, g_ball_movements[i].y = 0, 0;
			}
			//check if ball collides with any paddle and change direction accordingly
			if (detect_collision(g_ball_positions[i], g_player1_position, ball_scale, paddle_scale)) {
				g_ball_movements[i].x = -g_ball_movements[i].x;
				g_ball_movements[i].y = g_ball_positions[i].y - g_player1_position.y;
			}
			else if (detect_collision(g_ball_positions[i], g_player2_position, ball_scale, paddle_scale)) {
				g_ball_movements[i].x = -g_ball_movements[i].x;
				g_ball_movements[i].y = g_ball_positions[i].y - g_player2_position.y;
			}
			//if ball hits top or bottom of screen change y direction
			if (g_ball_positions[i].y > 3.6f) {
				g_ball_movements[i].y = -g_ball_movements[i].y;
			}
			else if (g_ball_positions[i].y < -3.6f) {
				g_ball_movements[i].y = -g_ball_movements[i].y;
			}
		}
		//make sure paddles dont go out of bounds
		g_player1_position = enforce_bounds(g_player1_position);
		g_player2_position = enforce_bounds(g_player2_position);
	}
	
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		//close game when window closed to quit
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				//press t to toggle player 2 playing
				case SDLK_t:
					player2isPlaying = !player2isPlaying;
					break;
				//press y to start game
				case SDLK_y:
					gameIsPlaying = true;
					canStartGame = false;
					break;
				//can only choose balls when game not playing
				//press 1 to choose 1 ball
				case SDLK_1:
					if (canStartGame == true) {
						ball_num = 1;
						break;
					}
				//press 2 to choose 2 balls
				case SDLK_2:
					if (canStartGame == true) {
						ball_num = 2;
						break;
					}
				//press 3 to choose 3 balls
				case SDLK_3:
					if (canStartGame == true) {
						ball_num = 3;
						break;
					}
			}
		}
		
	}
}

void update() {
	//create delta time
	float ticks = (float)SDL_GetTicks() / MS_IN_S;
	delta_time = ticks - PREV_TICKS;
	PREV_TICKS = ticks;

	//update player and ball positions if game is playing
	if (gameIsPlaying) {
		g_player1_position += g_player1_movement * g_player_speed * delta_time;
		g_player2_position += g_player2_movement * g_player_speed * delta_time;
		for (int i = 0; i < ball_num; ++i) {
			g_ball_positions[i] += g_ball_movements[i] * g_ball_speed * delta_time;
		}
	}
	
	//set paddles to starting positions
	g_player1_position.x = -4;
	g_player2_position.x = 4;
	
	//translate paddle1 according to input
	p_rectangle1_matrix = glm::mat4(1.0f);
	p_rectangle1_matrix = glm::translate(p_rectangle1_matrix, g_player1_position);
	p_rectangle1_matrix = glm::scale(p_rectangle1_matrix, paddle_scale);
	
	//translate paddle2 according to input
	p_rectangle2_matrix = glm::mat4(1.0f);
	p_rectangle2_matrix = glm::translate(p_rectangle2_matrix, g_player2_position);
	p_rectangle2_matrix = glm::scale(p_rectangle2_matrix, paddle_scale);

	//update each ball in play
	for (int i = 0; i < ball_num; ++i) {
		g_ball_matrices[i] = glm::mat4(1.0f);
		g_ball_matrices[i] = glm::translate(g_ball_matrices[i], g_ball_positions[i]);
		g_ball_matrices[i] = glm::scale(g_ball_matrices[i], ball_scale);
		if (glm::length(g_ball_movements[i]) > 1.0f) {
			g_ball_movements[i] = glm::normalize(g_ball_movements[i]);
		}
	}

	//normalize movements
	if (glm::length(g_player1_movement) > 1.0f) {
		g_player1_movement = glm::normalize(g_player1_movement);
	}
	if (glm::length(g_player2_movement) > 1.0f) {
		g_player2_movement = glm::normalize(g_player2_movement);
	}
	
}

void render() {

	glClear(GL_COLOR_BUFFER_BIT);

	// Vertices
	float vertices[] = {
		-0.5f, -0.5, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
	};
	
	// Textures
	float texture_coordinates[] = {
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
	};

	glVertexAttribPointer(g_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
	glVertexAttribPointer(g_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);

	glEnableVertexAttribArray(g_program.get_position_attribute());
	glEnableVertexAttribArray(g_program.get_tex_coordinate_attribute());

	//draw balls and paddles
	draw_object(p_rectangle1_matrix, g_player1_texture_id);
	draw_object(p_rectangle2_matrix, g_player2_texture_id);
	for (int i = 0; i < ball_num; ++i) {
		draw_object(g_ball_matrices[i], g_ball_texture_id);
	}

	glDisableVertexAttribArray(g_program.get_position_attribute());
	glDisableVertexAttribArray(g_program.get_tex_coordinate_attribute());

	//display start message
	if (!gameIsPlaying && canStartGame) {
		draw_text(&g_program, g_font_texture_id, INTRO_MSG, 0.25, 0.1, glm::vec3(-4, 3, 0));
		draw_text(&g_program, g_font_texture_id, INTRO_MSG2, 0.25, 0.1, glm::vec3(-3, 2.5, 0));
	}
	//display winner 1 msg
	else if (winner == 1 && !gameIsPlaying && !canStartGame) {
		draw_text(&g_program, g_font_texture_id, WIN_MSG1, 0.25, 0.1, glm::vec3(-2, 3, 0));
	}
	//display winner 2 msg
	else if (winner == 2 && !gameIsPlaying && !canStartGame) {
		draw_text(&g_program, g_font_texture_id, WIN_MSG2, 0.25, 0.1, glm::vec3(-2, 3, 0));
	}

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