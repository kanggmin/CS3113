#include "Level1.h"
#include "Utility.h"

#define LEVEL_WIDTH 30
#define LEVEL_HEIGHT 10
#define ENEMY_COUNT 2

const char PLAYER_SPRITE[] = "assets/characters/kenny.png";
const char TILEMAP_SPRITE[] = "assets/map/tilemap.png";
const char WALKER_SPRITE[] = "assets/characters/walker.png";
const char JUMPER_SPRITE[] = "assets/characters/jumper.png";
const char SHOOTER_SPRITE[] = "assets/characters/shooter.png";
const char DOOR_SPRITE[] = "assets/door.png";

unsigned int LEVEL_DATA[] =
{
    1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 3, 0, 0, 0, 0, 1, 3,
    5, 6, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 10, 10, 6, 6, 6, 3, 0, 0, 0, 5, 7,
    5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 9, 6, 6, 6, 2, 2, 2, 6, 7,
    5, 6, 6, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 5, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 7, 0, 0, 0, 0, 0, 1, 2, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 6, 2, 2, 2, 2, 2, 6, 6, 6, 6, 2, 3, 0, 0, 0, 0, 1, 2, 6, 6, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7,
    5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7,
};

bool g_game_is_over = false;
int enemies_alive = 2;

void Level1::initialise() {
    //player setup
    g_state.player = new Entity(PLAYER, glm::vec3(3.0f, -3.0f, 0.0f), 2.5f, glm::vec3(0.0f, -20.0f, 0.0f), Utility::load_texture(PLAYER_SPRITE),
        new int[3] { 0, 4, 8 }, new int[3] { 3, 7, 11 }, 3, 0, 0.1f, 4, 3, 0.8f, 0.8f);
    g_state.player->set_direction("right");
    g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->RIGHT];

    g_state.enemies = new Entity[ENEMY_COUNT];
    //ai setup
    //walker
    g_state.enemies[0] = Entity(ENEMY, glm::vec3(5.0f, -4.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), Utility::load_texture(WALKER_SPRITE),
        new int[2] { 0, 1 }, new int[2] { 0, 1 }, 2, 0, 0.1f, 2, 1, 0.8f, 0.8f);
    g_state.enemies[0].set_direction("right");
    g_state.enemies[0].set_ai_type(WALKER);

    //jumper
    g_state.enemies[1] = Entity(ENEMY, glm::vec3(16.0f, -4.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), Utility::load_texture(JUMPER_SPRITE),
        new int[2] { 0, 1 }, new int[2] { 0, 1 }, 2, 0, 0.1f, 2, 1, 0.8f, 0.8f);
    g_state.enemies[1].set_direction("right");
    g_state.enemies[1].set_ai_type(WALKER);
    g_state.enemies[1].set_jumping_power(6.0f);

    // Jumping
    g_state.player->m_jumping_power = 10.0f;

    //portal
    g_state.portal = new Entity(PORTAL, glm::vec3(26.0f, -2.0f, 0.0f), 0.0f, glm::vec3(0.0f, -20.0f, 0.0f), Utility::load_texture(DOOR_SPRITE),
        new int[1] {0}, new int[1] { 0 }, 1, 0, 0.1f, 1, 1, 0.8f, 0.8f);

    

    //map setup
    GLuint map_texture_id = Utility::load_texture(TILEMAP_SPRITE);
    g_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 4, 3);
}

void Level1::update(float delta_time) {
    if (!g_game_is_over) {
        g_state.player->update(delta_time, g_state.player, g_state.enemies, enemies_alive, g_state.map);
        g_state.portal->update(delta_time, g_state.player, g_state.player, enemies_alive, g_state.map);
        for (int i = 0; i < ENEMY_COUNT; i++) {
            g_state.enemies[i].update(delta_time, g_state.player, NULL, 0, g_state.map);
        }
    }
}

void Level1::render(ShaderProgram* program)
{
    g_state.map->render(program);
    g_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        g_state.enemies[i].render(program);
    }
    g_state.portal->render(program);
}

Level1::~Level1()
{
    //delete[]  g_state.enemies;
    delete    g_state.player;
    delete    g_state.map;
}