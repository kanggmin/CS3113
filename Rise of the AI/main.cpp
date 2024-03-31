/**
* Author: Jeffrey Park
* Assignment: Rise of the AI
* Date due: 2024-03-30, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include <Windows.h>

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

//define necessary files
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3
#define LEVEL1_WIDTH 30
#define LEVEL1_HEIGHT 10
#define LOG(argument) std::cout << argument << '\n'
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"

enum ProjectileType { SHURIKEN, FIRE, DISC };

using namespace std;

//window constants
const char GAME_WINDOW_NAME[] = "Rise of the AI";
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

//RGB values for background
float BG_RED = 0.15f,
BG_BLUE = 0.85f,
BG_GREEN = 0.45f,
BG_OPACITY = 1.0f;

//viewport constants
const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

//texture constants
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
const char PLAYER_SPRITE[] = "assets/characters/kenny.png";
const char TILEMAP_SPRITE[] = "assets/map/tilemap.png";
const char SHURIKEN_SPRITE[] = "assets/effects/shuriken.png";
const char FIRE_SPRITE[] = "assets/effects/fire.png";
const char DISC_SPRITE[] = "assets/effects/disc.png";
const char WALKER_SPRITE[] = "assets/characters/walker.png";
const char JUMPER_SPRITE[] = "assets/characters/jumper.png";
const char SHOOTER_SPRITE[] = "assets/characters/shooter.png";
const char FONT_SPRITE[] = "assets/font/font.png";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;
const int FONTBANK_SIZE = 16.0f;

unsigned int LEVEL_1_DATA[] =
{
    1, 3, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 12, 0, 0, 0, 0, 0, 1, 2, 2, 2, 3, 0, 0, 0, 0, 1, 3,
    5, 6, 3, 0, 0, 0, 0, 0, 0, 9, 2, 2, 2, 11, 0, 0, 0, 0, 0, 9, 10, 6, 6, 6, 3, 0, 0, 0, 5, 7,
    5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 6, 6, 6, 2, 2, 2, 6, 7,
    5, 6, 6, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 5, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 7, 0, 0, 0, 0, 0, 1, 2, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 6, 2, 2, 2, 2, 2, 6, 6, 6, 6, 2, 3, 0, 0, 0, 0, 1, 2, 6, 6, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7,
};

//variables to use delta time
float prev_ticks = 0.0f;
const float MS_IN_S = 1000.0f;
float delta_time;
float g_accumulator = 0.0f;

//object speeds
float g_player_speed = 1.0f;

//player position
glm::vec3 g_player_position = glm::vec3(0.0f, 0.0f, 0.0f);

//scaling for objects
glm::vec3 g_player_scale = glm::vec3(1.0f, 1.0f, 0.0f);

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_game_is_over = false;
bool winner;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

GLuint font_texture_id;

struct GameState {
	Entity* player;
	Entity* enemies;

    vector<Entity*> attacks;

	Map* map;
};

GameState g_state;

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

    g_shader_program.SetModelMatrix(model_matrix);
    glUseProgram(g_shader_program.programID);

    glVertexAttribPointer(g_shader_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glVertexAttribPointer(g_shader_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates.data());

    glEnableVertexAttribArray(g_shader_program.positionAttribute);
    glEnableVertexAttribArray(g_shader_program.texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(g_shader_program.positionAttribute);
    glDisableVertexAttribArray(g_shader_program.texCoordAttribute);
}

GLuint load_texture(const char* filepath)
{
	int width, height, number_of_components;
	unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

	if (image == NULL)
	{
		LOG("Unable to load image. Make sure the path is correct.");
		assert(false);
	}

	GLuint texture_id;
	glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(image);

	return texture_id;
}

void throw_projectile(Entity* user, ProjectileType proj) {
    //define projectile base on what type
    if (proj == SHURIKEN) {
        Entity* attack = new Entity(ATTACK, user->get_position(), 7.0f, glm::vec3(0.0f, 0.0f, 0.0f), load_texture(SHURIKEN_SPRITE),
            new int[4] { 0, 3, 2, 1 }, new int[4] { 0, 1, 2, 3 }, 4, 0, 0.01f, 4, 1, 0.5f, 0.5f);

        if (user->get_direction() == "left") {
            attack->move_left();
            attack->m_animation_indices = attack->m_walking[attack->LEFT];
        }
        else {
            attack->move_right();
            attack->m_animation_indices = attack->m_walking[attack->RIGHT];
        }
        g_state.attacks.push_back(attack);
    }
    else if (proj == FIRE) {
        Entity* attack = new Entity(ATTACK, user->get_position(), 10.0f, glm::vec3(0.0f, -20.0f, 0.0f), load_texture(FIRE_SPRITE),
            new int[4] { 0, 1, 2, 3 }, new int[4] { 0, 1, 2, 3 }, 4, 0, 0.01f, 4, 1, 0.5f, 0.5f);

        if (user->get_direction() == "left") {
            attack->move_left();
            attack->m_animation_indices = attack->m_walking[attack->LEFT];
        }
        else {
            attack->move_right();
            attack->m_animation_indices = attack->m_walking[attack->RIGHT];
        }

        g_state.attacks.push_back(attack);
    }
    else if (proj == DISC) {
        Entity* attack = new Entity(ENEMY_ATTACK, user->get_position(), 10.0f, glm::vec3(0.0f, 0.0f, 0.0f), load_texture(DISC_SPRITE),
            new int[4] { 0, 1, 2, 3 }, new int[4] { 0, 1, 2, 3 }, 4, 0, 0.01f, 4, 1, 0.5f, 0.5f);

        if (user->get_direction() == "left") {
            attack->m_animation_indices = attack->m_walking[attack->LEFT];
        }
        else {
            attack->m_animation_indices = attack->m_walking[attack->RIGHT];
        }
        attack->set_movement(normalize(g_state.player->get_position() - user->get_position()));
        g_state.attacks.push_back(attack);
    }
}

void initialise() {
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    //video setup
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.Load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-10.0f, 10.0f, -7.0f, 7.0f, -1.0f, 1.0f);

    g_shader_program.SetProjectionMatrix(g_projection_matrix);
    g_shader_program.SetViewMatrix(g_view_matrix);

    glUseProgram(g_shader_program.programID);

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    
    font_texture_id = load_texture(FONT_SPRITE);

    //player setup
    g_state.player = new Entity(PLAYER, glm::vec3(3.0f, -3.0f, 0.0f), 2.5f, glm::vec3(0.0f, -20.0f, 0.0f), load_texture(PLAYER_SPRITE),
        new int[3] { 0, 4, 8 }, new int[3] { 3, 7, 11 }, 3, 0, 0.1f, 4, 3, 0.8f, 0.8f);
    g_state.player->set_direction("right");
    g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->RIGHT];

    g_state.enemies = new Entity[ENEMY_COUNT];
    //ai setup
    //walker
    g_state.enemies[0] = Entity(ENEMY, glm::vec3(11.0f, 0.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), load_texture(WALKER_SPRITE),
        new int[2] { 0, 1 }, new int[2] { 0, 1 }, 2, 0, 0.1f, 2, 1, 0.8f, 0.8f);
    g_state.enemies[0].set_direction("right");
    g_state.enemies[0].set_ai_type(WALKER);
    
    //jumper
    g_state.enemies[1] = Entity(ENEMY, glm::vec3(16.0f, -4.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), load_texture(JUMPER_SPRITE),
        new int[2] { 0, 1 }, new int[2] { 0, 1 }, 2, 0, 0.1f, 2, 1, 0.8f, 0.8f);
    g_state.enemies[1].set_direction("right");
    g_state.enemies[1].set_ai_type(JUMPER);
    g_state.enemies[1].set_jumping_power(6.0f);

    g_state.enemies[2] = Entity(ENEMY, glm::vec3(27.0f, -1.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), load_texture(SHOOTER_SPRITE),
        new int[3] { 0, 1, 2 }, new int[3] { 0, 1, 2 }, 3, 0, 0.1f, 3, 1, 0.8f, 0.8f);
    g_state.enemies[2].set_direction("left");
    g_state.enemies[2].set_ai_type(SHOOTER);
    g_state.enemies[2].set_jumping_power(10.0f);
    g_state.enemies[2].set_ai_state(IDLE);
    

    // Jumping
    g_state.player->m_jumping_power = 8.0f;

    //map setup
    GLuint map_texture_id = load_texture(TILEMAP_SPRITE);
    g_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 4, 3);
    
    //blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                //quit game
                g_game_is_running = false;
                break;

            case SDLK_SPACE:
                //jump
                if (g_state.player->m_jumps >= 1)
                {
                    g_state.player->m_is_jumping = true;
                }
                break;

            case SDLK_x:
                throw_projectile(g_state.player, SHURIKEN);
                break;

            case SDLK_z:
                throw_projectile(g_state.player, FIRE);
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_state.player->move_left();
        g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_state.player->move_right();
        g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->RIGHT];
    }
    // This makes sure that the player can't move faster diagonally
    if (glm::length(g_state.player->get_movement()) > 1.0f)
    {
        g_state.player->set_movement(glm::normalize(g_state.player->get_movement()));
    }
}

void update()
{
    //check how many enemies alive
    int alive = ENEMY_COUNT;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (!g_state.enemies[i].get_active()) {
            alive -= 1;
        }
    }
    //end game is all enemies dead or player dead
    if (alive == 0) {
        g_game_is_over = true;
        winner = true;
    }

    if (!g_state.player->get_active()) {
       g_game_is_over = true;
       winner = false;
    }

    float ticks = (float)SDL_GetTicks() / MS_IN_S;
    float delta_time = ticks - prev_ticks;
    prev_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    //update player, projectiles, and enemies
    while (delta_time >= FIXED_TIMESTEP)
    {
        if (!g_game_is_over) {
            g_state.player->update(FIXED_TIMESTEP, g_state.player, g_state.enemies, ENEMY_COUNT, g_state.map);
            for (int i = 0; i < ENEMY_COUNT; i++) {
                g_state.enemies[i].update(FIXED_TIMESTEP, g_state.player, NULL, 0, g_state.map);
                if (g_state.enemies[i].get_ai_type() == SHOOTER && g_state.enemies[i].m_can_shoot) {
                    throw_projectile(&g_state.enemies[i], DISC);
                    g_state.enemies[i].m_can_shoot = false;
                }
            }
            if (g_state.attacks.size() > 0) {
                for (Entity* proj : g_state.attacks) {
                    if (proj->get_entity_type() == ATTACK) {
                        proj->update(FIXED_TIMESTEP, proj, g_state.enemies, ENEMY_COUNT, g_state.map);
                    }
                    else if (proj->get_entity_type() == ENEMY_ATTACK) {
                        proj->update(FIXED_TIMESTEP, proj, g_state.player, ENEMY_COUNT, g_state.map);
                    }
                }
            }
        }
        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_state.player->get_position().x, -g_state.player->get_position().y, 0.0f));
    
}

void render()
{
    g_shader_program.SetViewMatrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);
    
    //render player, enemies, and projectiles
    g_state.player->render(&g_shader_program);
    g_state.map->render(&g_shader_program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        g_state.enemies[i].render(&g_shader_program);
    }

    if (g_state.attacks.size() > 0) {
        for (Entity* proj : g_state.attacks) {
            proj->render(&g_shader_program);
        }
    }
    //render end game message
    if (g_game_is_over && winner) {
        draw_text(font_texture_id, "YOU WIN!", 1, 0.1, g_state.player->get_position() + glm::vec3(-4.0f, 2.0f, 0.0f));
    }
    else if (g_game_is_over && !winner) {
        draw_text(font_texture_id, "YOU LOSE!", 1, 0.1, g_state.player->get_position() + glm::vec3(-4.0f, 2.0f, 0.0f));
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    for (Entity* proj : g_state.attacks) {
        delete proj;
    }
    delete g_state.player;
    delete g_state.map;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}