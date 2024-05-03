#include "Level2.h"
#include "Utility.h"

#define LEVEL_WIDTH 30
#define LEVEL_HEIGHT 30
#define ENEMY_COUNT 13

const char PLAYER_SPRITE[] = "assets/characters/player.png";
const char TILEMAP_SPRITE[] = "assets/map/tilemap.png";
const char SLEEPER_SPRITE[] = "assets/characters/sleeper.png";
const char FONT_SPRITE[] = "assets/font/font.png";

unsigned int LEVEL2_DATA[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, 38, 33, 33, 33, 33, 33, 33, 33, 33, 33, 39, -1, -1, -1, -1, -1, -1, 38, 33, 33, 33, 33, 39, -1, -1, -1,
    -1, -1, -1, -1, 28,  0,  1,  1,  1,  1,  1,  1,  1,  2, 26, -1, -1, -1, -1, -1, -1, 28,  0,  1,  1,  2, 26, -1, -1, -1,
    -1, -1, -1, -1, 28,  6,  7,  7,  7,  7,  7,  7,  7,  8, 32, 39, -1, -1, -1, -1, -1, 28,  6,  7,  7,  8, 32, 39, -1, -1,
    -1, -1, -1, 38, 34,  6,  7,  7,  7,  7,  7,  7,  7, 24,  2, 26, -1, -1, -1, -1, -1, 28,  6,  7,  7, 24,  2, 26, -1, -1,
    -1, -1, -1, 28,  0, 25,  7,  7,  7,  7,  7,  7,  7,  7,  8, 26, -1, -1, -1, -1, -1, 28,  6,  7,  7,  7,  8, 32, 39, -1,
    -1, -1, -1, 28,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8, 26, -1, -1, -1, -1, -1, 28,  6,  7,  7,  7, 24,  2, 26, -1,
    -1, -1, 38, 34,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8, 26, -1, -1, -1, -1, -1, 28,  6,  7,  7,  7,  7,  8, 26, -1,
    -1, -1, 28,  0, 25,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8, 32, 33, 33, 39, -1, -1, 28,  6,  7,  7,  7,  7,  8, 26, -1,
    -1, -1, 28,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 24,  1,  1,  2, 26, -1, -1, 28,  6,  7,  7,  7,  7,  8, 26, -1,
    -1, 38, 34,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8, 32, 33, 33, 34,  6,  7,  7,  7,  7,  8, 26, -1,
    -1, 28,  0, 25,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 24,  1,  1,  1,  1, 25,  7,  7,  7,  7,  8, 26, -1,
    -1, 28,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8, 26, -1,
    -1, 28,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 18, 14, 26, -1,
    -1, 28,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8, 20, 45, -1,
    -1, 28,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8, 26, -1, -1,
    -1, 28,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 18, 14, 26, -1, -1,
    -1, 28,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 18, 13, 13, 13, 13, 13, 13, 19,  7,  8, 20, 45, -1, -1,
    -1, 28,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8, 20, 21, 21, 21, 21, 22, 12, 13, 14, 26, -1, -1, -1,
    -1, 28, 12, 13, 19,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8, 26, -1, -1, -1, -1, 44, 21, 21, 21, 45, -1, -1, -1,
    -1, 44, 22, 41,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 18, 14, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 28, 41, 12, 13, 19,  7,  7,  7, 18, 13, 13, 13, 13, 14, 20, 45, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 44, 21, 22, 41, 12, 13, 13, 13, 14, 20, 21, 21, 21, 21, 45, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, 44, 21, 21, 21, 21, 21, 21, 45, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

void Level2::initialise() {

    

    font_texture = Utility::load_texture(FONT_SPRITE);

    //player setup
    g_state.player = new Entity(PLAYER, glm::vec3(10.0f, -8.0f, 0.0f), 3.5f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(PLAYER_SPRITE),
        new int[4] { 12, 13, 14, 15 }, new int[4] { 4, 5, 6, 7 }, 4, 0, 0.05f, 4, 4, 1.3f, 1.0f, 3);
    g_state.player->set_up_and_down_anims(new int[4] {8, 9, 10, 11}, new int[4] {0, 1, 2, 3});
    g_state.player->set_direction("right");
    g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->RIGHT];


    //ai setup
    g_state.enemies = new Entity[ENEMY_COUNT];
    g_state.enemies[0] = Entity(ENEMY, glm::vec3(18.0f, -18.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[0].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[0].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[0].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[0].m_animation_indices = g_state.enemies[0].m_sleeping;
    g_state.enemies[0].set_ai_type(SLEEPER);
    g_state.enemies[0].set_ai_state(SLEEPING);

    g_state.enemies[1] = Entity(ENEMY, glm::vec3(5.0f, -25.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[1].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[1].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[1].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[1].m_animation_indices = g_state.enemies[0].m_sleeping;
    g_state.enemies[1].set_ai_type(SLEEPER);
    g_state.enemies[1].set_ai_state(SLEEPING);

    g_state.enemies[2] = Entity(ENEMY, glm::vec3(4.0f, -16.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[2].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[2].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[2].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[2].m_animation_indices = g_state.enemies[0].m_sleeping;
    g_state.enemies[2].set_ai_type(SLEEPER);
    g_state.enemies[2].set_ai_state(SLEEPING);

    g_state.enemies[3] = Entity(ENEMY, glm::vec3(7.0f, -22.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[3].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[3].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[3].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[3].m_animation_indices = g_state.enemies[3].m_sleeping;
    g_state.enemies[3].set_ai_type(SLEEPER);
    g_state.enemies[3].set_ai_state(SLEEPING);

    g_state.enemies[4] = Entity(ENEMY, glm::vec3(24.0f, -10.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[4].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[4].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[4].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[4].m_animation_indices = g_state.enemies[4].m_sleeping;
    g_state.enemies[4].set_ai_type(SLEEPER);
    g_state.enemies[4].set_ai_state(SLEEPING);

    g_state.enemies[5] = Entity(ENEMY, glm::vec3(25.0f, -18.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[5].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[5].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[5].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[5].m_animation_indices = g_state.enemies[0].m_sleeping;
    g_state.enemies[5].set_ai_type(SLEEPER);
    g_state.enemies[5].set_ai_state(SLEEPING);

    g_state.enemies[6] = Entity(ENEMY, glm::vec3(13.0f, -22.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[6].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[6].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[6].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[6].m_animation_indices = g_state.enemies[0].m_sleeping;
    g_state.enemies[6].set_ai_type(SLEEPER);
    g_state.enemies[6].set_ai_state(SLEEPING);

    g_state.enemies[7] = Entity(ENEMY, glm::vec3(8.0f, -15.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[7].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[7].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[7].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[7].m_animation_indices = g_state.enemies[0].m_sleeping;
    g_state.enemies[7].set_ai_type(SLEEPER);
    g_state.enemies[7].set_ai_state(SLEEPING);

    g_state.enemies[8] = Entity(ENEMY, glm::vec3(15.0f, -19.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[8].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[8].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[8].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[8].m_animation_indices = g_state.enemies[3].m_sleeping;
    g_state.enemies[8].set_ai_type(SLEEPER);
    g_state.enemies[8].set_ai_state(SLEEPING);

    g_state.enemies[9] = Entity(ENEMY, glm::vec3(18.0f, -17.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[9].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[9].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[9].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[9].m_animation_indices = g_state.enemies[4].m_sleeping;
    g_state.enemies[9].set_ai_type(SLEEPER);
    g_state.enemies[9].set_ai_state(SLEEPING);

    g_state.enemies[10] = Entity(ENEMY, glm::vec3(26.0f, -17.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[10].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[10].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[10].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[10].m_animation_indices = g_state.enemies[0].m_sleeping;
    g_state.enemies[10].set_ai_type(SLEEPER);
    g_state.enemies[10].set_ai_state(SLEEPING);

    g_state.enemies[11] = Entity(ENEMY, glm::vec3(25.0f, -8.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[11].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[11].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[11].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[11].m_animation_indices = g_state.enemies[3].m_sleeping;
    g_state.enemies[11].set_ai_type(SLEEPER);
    g_state.enemies[11].set_ai_state(SLEEPING);

    g_state.enemies[12] = Entity(ENEMY, glm::vec3(24.0f, -19.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(SLEEPER_SPRITE),
        new int[4] { 18, 19, 20, 21 }, new int[4] { 12, 13, 14, 15 }, 4, 0, 0.5f, 6, 4, 1.0f, 1.0f, 3);
    g_state.enemies[12].set_up_and_down_anims(new int[4] {6, 7, 8, 9}, new int[4] {0, 1, 2, 3});
    g_state.enemies[12].set_m_sleeping(new int[4] { 4, 10, 16, 22});
    g_state.enemies[12].set_m_waking(new int[4] { 5, 11, 17});
    g_state.enemies[12].m_animation_indices = g_state.enemies[4].m_sleeping;
    g_state.enemies[12].set_ai_type(SLEEPER);
    g_state.enemies[12].set_ai_state(SLEEPING);
    //map setup
    GLuint map_texture_id = Utility::load_texture(TILEMAP_SPRITE);
    g_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL2_DATA, map_texture_id, 1.0f, 6, 8);
}

void Level2::update(float delta_time) {
    if (!g_game_is_over) {
        m_enemies_alive = 0;
        g_state.player->update(delta_time, g_state.player, g_state.enemies, ENEMY_COUNT, g_state.map);
        for (int i = 0; i < ENEMY_COUNT; i++) {
            g_state.enemies[i].update(delta_time, g_state.player, g_state.enemies, ENEMY_COUNT, g_state.map);
            if (g_state.enemies[i].get_active()) {
                m_enemies_alive += 1;
            }
        }
    }
}

void Level2::render(ShaderProgram* program)
{
    //Control text
    Utility::draw_text(program, font_texture, "W,A,S,D to move", 1, 0, glm::vec3(8.0, -28.0, 0.0));
    Utility::draw_text(program, font_texture, "Mouse to aim", 1, 0, glm::vec3(9.0, -29.0, 0.0));
    Utility::draw_text(program, font_texture, "Left click to shoot", 1, 0, glm::vec3(6.0, -30.0, 0.0));
    g_state.map->render(program);
    g_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        g_state.enemies[i].render(program);
    }
}

Level2::~Level2()
{
    //delete[] g_state.enemies;
    delete    g_state.player;
    delete    g_state.map;
}