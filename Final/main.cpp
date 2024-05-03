/**
* Author: Jeffrey Park
* Assignment: Platformer
* Date due: 2024-04-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/*
TODO:

*/

#include <Windows.h>
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

//define necessary files
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f

#include "Utility.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "LoseMenu.h"
#include "WinMenu.h"
#include <SDL_mixer.h>

const int CD_QUAL_FREQ = 44100,  // compact disk (CD) quality frequency
AUDIO_CHAN_AMT = 2,
AUDIO_BUFF_SIZE = 4096;

const char BGM_FILEPATH[] = "assets/sfx/music.mp3";
const int LOOP_FOREVER = -1;

const int PLAY_ONCE = 0,
NEXT_CHNL = -1,
MUTE_VOL = 0,
MILS_IN_SEC = 1000,
ALL_SFX_CHN = -1;

Mix_Music* g_music;
Mix_Chunk* g_pew_sfx;

using namespace std;

//window constants
const char GAME_WINDOW_NAME[] = "Platformer";
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
const char FONT_SPRITE[] = "assets/font/font.png";

//variables to use delta time
float prev_ticks = 0.0f;
const float MS_IN_S = 1000.0f;
float delta_time;
float g_accumulator = 0.0f;

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_game_is_playing = false;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

Utility g_util;
GLuint font_texture_id;

Scene* g_current_scene;
MainMenu* g_menu;
Level1* g_level_1;
Level2* g_level_2;
Level3* g_level_3;
LoseMenu* g_lose_menu;
WinMenu* g_win_menu;

int* mouseX;
int* mouseY;

int g_player_lives = 3;
float dmg_timer = 0;

void pew_sfx() {
    Mix_PlayChannel(NEXT_CHNL, g_pew_sfx, PLAY_ONCE);
    Mix_FadeInChannel(NEXT_CHNL, g_pew_sfx, PLAY_ONCE, MILS_IN_SEC);
    Mix_Volume(ALL_SFX_CHN, MIX_MAX_VOLUME / 4);
    Mix_VolumeChunk(g_pew_sfx, MIX_MAX_VOLUME / 8);
}

void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
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

    mouseX = new int;
    mouseY = new int;

    //music setup
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(CD_QUAL_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHAN_AMT, AUDIO_BUFF_SIZE);
    g_music = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_music, LOOP_FOREVER);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 8);
    g_pew_sfx = Mix_LoadWAV("assets/sfx/pew.wav");

    //levels setup
    g_menu = new MainMenu();
    g_level_1 = new Level1();
    g_level_2 = new Level2();
    g_level_3 = new Level3();
    g_lose_menu = new LoseMenu();
    g_win_menu = new WinMenu();
    switch_to_scene(g_menu);
    font_texture_id = Utility::load_texture(FONT_SPRITE);

    //blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    if (g_game_is_playing) {
        g_current_scene->g_state.player->set_movement(glm::vec3(0.0f));
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(mouseX, mouseY);
            *mouseX -= 320;
            *mouseY = 240 - *mouseY;         
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (g_game_is_playing) {
                if (g_current_scene->g_state.player->can_shoot) {
                    pew_sfx();
                }
                g_current_scene->g_state.player->shoot(mouseX, mouseY);
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_m:
                if (g_game_is_playing) {
                    
                    break;
                }
            case SDLK_q:
                //quit game
                g_game_is_running = false;
                break;
            case SDLK_RETURN:
                if (!g_game_is_playing && g_current_scene == g_menu) {
                    switch_to_scene(g_level_1);
                    g_player_lives = 3;
                    g_game_is_playing = true;
                }
            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (g_game_is_playing) {
        if (key_state[SDL_SCANCODE_A])
        {
            g_current_scene->g_state.player->move_left();
            g_current_scene->g_state.player->m_animation_indices = g_current_scene->g_state.player->m_walking[g_current_scene->g_state.player->LEFT];
        }
        if (key_state[SDL_SCANCODE_D])
        {
            g_current_scene->g_state.player->move_right();
            g_current_scene->g_state.player->m_animation_indices = g_current_scene->g_state.player->m_walking[g_current_scene->g_state.player->RIGHT];
        }
        if (key_state[SDL_SCANCODE_W]) {
            g_current_scene->g_state.player->move_up();
            g_current_scene->g_state.player->m_animation_indices = g_current_scene->g_state.player->m_walking[g_current_scene->g_state.player->UP];
        }
        if (key_state[SDL_SCANCODE_S]) {
            g_current_scene->g_state.player->move_down();
            g_current_scene->g_state.player->m_animation_indices = g_current_scene->g_state.player->m_walking[g_current_scene->g_state.player->DOWN];
        }
        // This makes sure that the player can't move faster diagonally
        if (glm::length(g_current_scene->g_state.player->get_movement()) > 1.0f)
        {
            g_current_scene->g_state.player->set_movement(glm::normalize(g_current_scene->g_state.player->get_movement()));
        }
    }
}

void update()
{
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
        g_current_scene->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }
    if (g_game_is_playing) {
        if (g_current_scene == g_level_1 && g_level_1->m_enemies_alive == 0) {
            switch_to_scene(g_level_2);
        }
        else if (g_current_scene == g_level_2 && g_level_2->m_enemies_alive == 0) {
            switch_to_scene(g_level_3);
        }
        else if (g_current_scene == g_level_3 && g_current_scene->g_state.boss->get_active() == false && !g_current_scene->g_state.noBoss ) {
            switch_to_scene(g_win_menu);
            g_game_is_playing = false;
            return;
        }
        if (g_current_scene->g_state.player->hit == true && dmg_timer >= 0.5) {
            g_current_scene->g_state.player->hit = false;
            g_player_lives -= 1;
            dmg_timer = 0.0f;
        }
        if (dmg_timer >= 0.5) {
            g_current_scene->g_state.player->can_hit = true;
        }

        dmg_timer += delta_time;
        g_accumulator = delta_time;

        g_view_matrix = glm::mat4(1.0f);
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->g_state.player->get_position().x, 
            -g_current_scene->g_state.player->get_position().y, 0.0f));

        if (g_player_lives <= 0) {
            g_game_is_playing = false;
            switch_to_scene(g_lose_menu);
        }
    }
    else {
        g_view_matrix = glm::mat4(1.0f);
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void render()
{
    g_shader_program.SetViewMatrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    g_current_scene->render(&g_shader_program);
    if (g_game_is_playing) {
        Utility::draw_text(&g_shader_program, font_texture_id, to_string(g_player_lives), 1, 0.1,
            g_current_scene->g_state.player->get_position() + glm::vec3(0.0, 1.0, 0.0));
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    Mix_HaltMusic();
    delete g_level_1;
    delete g_level_2;
    delete g_level_3;
    delete g_menu;
    delete g_lose_menu;
    delete g_win_menu;
    SDL_Quit();
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