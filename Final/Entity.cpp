/**
* Author: Jeffrey Park
* Assignment: Rise of the AI
* Date due: 2024-03-30, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include "Utility.h"

const char BULLET_SPRITE[] = "assets/effects/rock.png";
const char FONT_SPRITE[] = "assets/font/font.png";

Entity::Entity()
{
    //physics
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);

    //translation
    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
    hit = false;
    can_hit = true;
}

Entity::Entity(EntityType type, glm::vec3 pos, float speed, glm::vec3 accel, GLuint textID,
    int* left_anim, int* right_anim, int frames, int index, float time, int cols, int rows, float height, float width, int health) {
    m_entity_type = type;
    m_position = pos;
    m_speed = speed;
    m_velocity = glm::vec3(0.0f);
    m_acceleration = accel;
    m_texture_id = textID;
    m_movement = glm::vec3(0.0f);
    m_walking[LEFT] = left_anim;
    m_walking[RIGHT] = right_anim;
    m_animation_indices = left_anim;
    m_animation_frames = frames;
    m_animation_index = index;
    m_animation_time = time;
    m_animation_cols = cols;
    m_animation_rows = rows;
    m_height = height;
    m_width = width;
    m_model_matrix = glm::mat4(1.0f);
    can_shoot = true;
    shot_rate = 0.5f;
    time_since_shot = 0.0f;
    bullet_life = 0.0f;
    bullet_lifetime = 1.0f;
    m_hp = health;
    can_hit = true;
    if (m_entity_type == PLAYER) {
        shot_index = 0;
        for (int i = 0; i < 5; i++) {
            Entity* shot = new Entity(BULLET, m_position, 5.0f, glm::vec3(0), Utility::load_texture(BULLET_SPRITE),
                new int[1] { 0 }, new int[1] { 0 }, 1, 0, 0.1f, 1, 1, 0.5f, 0.5f, 1);
            shot->m_is_active = false;
            shots.push_back(shot);
        }
    }
}

Entity::Entity(EntityType type, glm::vec3 pos, float speed, glm::vec3 accel, GLuint textID,
    int* left_anim, int* right_anim, int frames, int index, float time, int cols, int rows, float height, float width, int health, Entity* Boss) {
    m_entity_type = type;
    m_position = pos;
    m_speed = speed;
    m_velocity = glm::vec3(0.0f);
    m_acceleration = accel;
    m_texture_id = textID;
    m_movement = glm::vec3(0.0f);
    m_walking[LEFT] = left_anim;
    m_walking[RIGHT] = right_anim;
    m_animation_indices = left_anim;
    m_animation_frames = frames;
    m_animation_index = index;
    m_animation_time = time;
    m_animation_cols = cols;
    m_animation_rows = rows;
    m_height = height;
    m_width = width;
    m_model_matrix = glm::mat4(1.0f);
    can_shoot = true;
    shot_rate = 0.5f;
    time_since_shot = 0.0f;
    bullet_life = 0.0f;
    bullet_lifetime = 1.0f;
    m_hp = health;
    can_hit = true;
    boss = Boss;
    if (m_entity_type == PLAYER) {
        shot_index = 0;
        for (int i = 0; i < 5; i++) {
            Entity* shot = new Entity(BULLET, m_position, 5.0f, glm::vec3(0), Utility::load_texture(BULLET_SPRITE),
                new int[1] { 0 }, new int[1] { 0 }, 1, 0, 0.1f, 1, 1, 0.5f, 0.5f, 1);
            shot->m_is_active = false;
            shots.push_back(shot);
            shot->boss = Boss;
        }
    }
}


Entity::~Entity()
{
    for (Entity* e : shots) {
        delete e;
    }
    delete[] m_animation_left;
    delete[] m_animation_right;
    delete[] m_animation_up;
    delete[] m_animation_down;
    delete[] m_walking;
    if (m_waking != NULL) {
        delete[] m_waking;
    }
    if (m_sleeping != NULL) {
        delete[] m_sleeping;
    }
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };
    float playerVertices[] = { -0.5, -1.0, 0.5, -1.0, 0.5, 1.0, -0.5, -1.0, 0.5, 1.0, -0.5, 1.0 };

    float sleeperVertices[] = { -0.8, -0.8, 0.8, -0.8, 0.8, 0.8, -0.8, -0.8, 0.8, 0.8, -0.8, 0.8 };

    float bossVertices[] = { -5, -5, 5, -5, 5, 5, -5, -5, 5, 5, -5, 5 };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    if (m_entity_type == PLAYER) {
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, playerVertices);
    }
    else if (m_entity_type == ENEMY && m_ai_type == SLEEPER) {
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, sleeperVertices);
    }
    else if (m_ai_type == BOSS) {
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, bossVertices);
    }
    else {
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    }
    
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::ai_activate(Entity* player, float delta_time)
{
    switch (m_ai_type)
    {
    case SLEEPER:
        if (m_ai_state == SLEEPING) {
            ai_sleep(player);
            break;
        }
        else if (m_ai_state == WAKING) {
            ai_waking();
            break;
        }
        else if (m_ai_state == ATTACKING) {
            ai_attacking(player);
            break;
        }
    case BOSS:
        if (m_ai_state == WAITING) {
            ai_wait(player, delta_time);
            break;
        }
        else if (m_ai_state == CHARGING) {
            ai_charge(player, delta_time);
            break;
        }
    default:
        break;
    }
}

void Entity::ai_wait(Entity* player, float delta_time) {
    boss_time += delta_time;
    m_movement = glm::vec3(0.0f);
    if (boss_time >= 2.5f) {
        boss_time = 0;
        m_ai_state = CHARGING;
        bossX = player->get_position().x - m_position.x;
        bossY = player->get_position().y - m_position.y;
        m_animation_indices = m_walking[2];
    }
}

void Entity::ai_charge(Entity* player, float delta_time) {
    glm::vec3 direction = glm::normalize(glm::vec3(bossX, bossY, 0));
    m_movement = direction;
    boss_time += delta_time;
    if (boss_time >= 2.0f) {
        boss_time = 0;
        m_ai_state = WAITING;
        m_animation_indices = m_walking[0];
    }
}

void Entity::ai_sleep(Entity* player) {
    can_hit = false;
    if (glm::distance(m_position, player->get_position()) < 6.0f) {
        m_ai_state = WAKING;
        m_animation_index = 0;
        m_animation_indices = m_waking;
    }

}

void Entity::ai_waking() {
    if (m_animation_index == 3) {
        m_ai_state = ATTACKING;
        can_hit = true;
        m_animation_time = 0.1f;
    }
}

void Entity::ai_attacking(Entity* player) {
    if (m_position.x > player->get_position().x) {
        move_left();
    }
    else if (m_position.x < player->get_position().x) {
        move_right();
    }
    if (m_position.y > player->get_position().y) {
        move_down();
    }
    else if (m_position.y < player->get_position().y) {
        move_up();
    }
}

void Entity::shoot(int* mouseX, int* mouseY) {
    if (can_shoot) {
        glm::vec3 direction = glm::normalize(glm::vec3(*mouseX, *mouseY, 0));
        Entity* shot;
        if (shot_index > 4) {
            shot_index = 0;
        }
        shot = shots[shot_index];
        shot->bullet_life = 0;
        shot->set_position(m_position);
        shot->set_movement(glm::normalize(direction));
        shot->m_is_active = true;
        shot_index += 1;
        can_shoot = false;
    }
}

void Entity::knockback(glm::vec3 attack_vel) {
    attack_vel *= 8;

    m_velocity.x += attack_vel.x;
    m_velocity.y += attack_vel.y;
    m_position.x += m_velocity.x * delta_time;
    m_position.y += m_velocity.y * delta_time;
}

void Entity::update(float deltatime, Entity* player, Entity* objects, int object_count, Map* map)
{
    if (!m_is_active) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    delta_time = deltatime;

    if (!can_shoot) {
        time_since_shot += delta_time;
        if (time_since_shot >= shot_rate) {
            can_shoot = true;
            time_since_shot = 0;
        }
    }

    if (m_entity_type == PLAYER && boss != NULL && can_hit) {
        if (check_collision(boss)) {
            hit = true;
            can_hit = false;
        }
    }

    if (m_entity_type == BULLET) {
        bullet_life += delta_time;
        if (bullet_life >= bullet_lifetime) {
            m_is_active = false;
        }
        if (boss != NULL) {
            if (check_collision(boss) && boss->can_hit) {
                m_is_active = false;
                boss->m_hp -= 1;
            }
        }
    }

    if (m_entity_type == ENEMY) {
        ai_activate(player, delta_time);
    }

    if (m_hp <= 0) {
        m_is_active = false;
    }

    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0 || m_ai_state == SLEEPING || m_ai_state == WAKING || m_ai_type == BOSS)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float)1 / FRAMES_PER_SECOND;

            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;

                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
        else {
            m_animation_index = 0;
        }
    }

    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;

    m_velocity.y = m_movement.y * m_speed;
    m_velocity += m_acceleration * delta_time;

    // We make two calls to our check_collision methods, one for the collidable objects and one for
    // the map.
    m_position.x += m_velocity.x * delta_time;
    m_position.y += m_velocity.y * delta_time;

    check_collision_y(map);
    check_collision_x(map);

    check_collision_x(objects, object_count);
    check_collision_y(objects, object_count);

    for (Entity* shot : shots) {
        shot->update(delta_time, player, objects, object_count, map);
    }

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            if (m_entity_type == BULLET && collidable_entity->m_entity_type == ENEMY && can_hit) {
                collidable_entity->m_hp -= 1;
                m_is_active = false;
            }
            else if (m_entity_type == BULLET && collidable_entity->m_entity_type == BOSS) {
                collidable_entity->m_hp -= 1;
                m_is_active = false;
            }
            float y_distance = fabs(m_position.y - collidable_entity->get_position().y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->get_height() / 2.0f));
            if (m_velocity.y >= 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            if (m_entity_type == PLAYER && collidable_entity->m_entity_type == ENEMY && can_hit) {
                hit = true;

                can_hit = false;
                if (collidable_entity->m_facing_direction == "right") {
                    m_velocity.y += 10.0f;
                    m_velocity.x -= 5000.0f;
                    
                }
                else {
                    m_velocity.y += 10.0f;
                    m_velocity.x += 5000.0f;
                }            
            }
            if (m_entity_type == BULLET && collidable_entity->m_entity_type == ENEMY) {
                collidable_entity->knockback(m_velocity);
                collidable_entity->m_hp -= 1;
                m_is_active = false;
            }
            else if (m_entity_type == BULLET && collidable_entity->m_entity_type == BOSS) {
                collidable_entity->m_hp -= 1;
                m_is_active = false;
            }
            float x_distance = fabs(m_position.x - collidable_entity->get_position().x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->get_width() / 2.0f));
            if (m_velocity.x >= 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }
        }
    }
}


void const Entity::check_collision_y(Map* map)
{
    if (m_entity_type == BULLET) { return; }
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
}

void const Entity::check_collision_x(Map* map)
{
    if (m_entity_type == BULLET) { return; }
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2) + 0.05, m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2) + 0.05, m_position.z);

    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
    if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    } 
    if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
         m_position.x -= penetration_x;
         m_velocity.x = 0;
         m_collided_right = true;
    }
}



void Entity::render(ShaderProgram* program)
{
    if (!m_is_active) { return; }
    
    float y = 1.0f;
    float x = 0.0f;
    if (m_entity_type == ENEMY) {
        if (m_ai_type == BOSS) { y = 3.0f; x = -1.0f; }
        Utility::draw_text(program, Utility::load_texture(FONT_SPRITE), to_string(m_hp), 1, 0.1,
            m_position + glm::vec3(x, y, 0.0));
    }
    for (Entity* shot : shots) {
        shot->render(program);
    }

    program->SetModelMatrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float playerVertices[] = { -0.5, -1.0, 0.5, -1.0, 0.5, 1.0, -0.5, -1.0, 0.5, 1.0, -0.5, 1.0 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    if (m_entity_type == PLAYER) {
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, playerVertices);
    }
    else {
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    }
    
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


bool const Entity::check_collision(Entity* other) const
{
    if (other == this || !can_hit) return false;
    // If either entity is inactive, there shouldn't be any collision
    if (!m_is_active || !other->m_is_active) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}