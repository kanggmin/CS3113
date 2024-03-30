/**
* Author: Jeffrey Park
* Assignment: Rise of the AI
* Date due: 2024-03-30, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

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
}

Entity::Entity(EntityType type, glm::vec3 pos, float speed, glm::vec3 accel, GLuint textID,
    int* left_anim, int* right_anim, int frames, int index, float time, int cols, int rows, float height, float width) {
    m_entity_type = type;
    m_position = pos;
    m_speed = speed;
    m_velocity = glm::vec3(0.0f);
    m_acceleration = accel;
    m_texture_id = textID;
    m_movement = glm::vec3(0.0f);

    m_animation_indices = right_anim;
    m_walking[LEFT] = left_anim;
    m_walking[RIGHT] = right_anim;
    m_animation_frames = frames;
    m_animation_index = index;
    m_animation_time = time;
    m_animation_cols = cols;
    m_animation_rows = rows;
    m_height = height;
    m_width = width;
    m_model_matrix = glm::mat4(1.0f);

}

Entity::~Entity()
{
    delete[] m_animation_left;
    delete[] m_animation_right;
    delete[] m_walking;
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

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
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
    case WALKER:
        ai_walk();
        break;

    case JUMPER:
        ai_jump();
        break;
    case SHOOTER:
        ai_shoot(player, delta_time);
        break;
    default:
        break;
    }
}

void Entity::ai_walk()
{
    //keep walking and change direction if walk into a wall
    if (m_facing_direction == "right") {
        move_right();
    }
    else {
        move_left();
    }
}

void Entity::ai_jump() {
    //jump around until hits a wall then turn around
    if (m_collided_top || m_collided_bottom) {
        if (m_facing_direction == "right") {
            move_right();
        }
        else {
            move_left();
        }
        m_is_jumping = true;
    }
}

void Entity::ai_shoot(Entity* player, float delta_time) {
    //jump in place until player approaches
    //shoot projectiles every second when attacking
    switch (m_ai_state) {
    case IDLE:
        if (m_collided_top || m_collided_bottom) {
            m_is_jumping = true;
        }
        if (glm::distance(m_position, player->get_position()) < 5.0f) m_ai_state = ATTACKING;
        break;
    case ATTACKING:
        if (m_position.x > player->get_position().x) {
            m_facing_direction = "left";
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        }
        else {
            m_facing_direction = "right";
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        if (m_collided_top || m_collided_bottom) {
            m_is_jumping = true;
        }
        if (!m_can_shoot) {
            m_prev_shot += delta_time;
            if (m_prev_shot > (1 / SHOTS_PER_SECOND)) {
                m_can_shoot = true;
                m_prev_shot = 0;
            }
        }
        break;
    default:
        break;
    }
    
}

void Entity::update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map)
{
    if (!m_is_active) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0 || m_ai_type == SHOOTER)
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

    // We make two calls to our check_collision methods, one for the collidable objects and one for
    // the map.
    m_position.x += m_velocity.x * delta_time;
    m_position.y += m_velocity.y * delta_time;
    check_collision_x(map);
    check_collision_y(map);
    check_collision_x(objects, object_count);
    check_collision_y(objects, object_count);

    
    

    if (m_entity_type == ENEMY) { ai_activate(player, delta_time); }

    //update jump
    if (m_is_jumping)
    {
        m_jumps -= 1;
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    if (m_collided_bottom) {
        m_jumps = 1;
    }

    ///remove attack when collides with wall
    if (m_entity_type == ATTACK) {
        if (m_collided_bottom || m_collided_top || m_collided_left || m_collided_right) {
            m_is_active = false;
        }
    }
    //change enemy direction
    else if (m_entity_type == ENEMY) {
        if (m_collided_right) {
            m_facing_direction = "left";
        }
        else if (m_collided_left) {
            m_facing_direction = "right";
        }
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
            //kill entities in certain interactions
            if (m_entity_type == PLAYER && collidable_entity->m_entity_type == ENEMY) {
                m_is_active = false;
            }
            else if (m_entity_type == ATTACK && collidable_entity->m_entity_type == ENEMY) {
                collidable_entity->kill();
                kill();
            }
            else if (m_entity_type == ENEMY_ATTACK && collidable_entity->m_entity_type == PLAYER) {
                collidable_entity->kill();
                kill();
            }
            float y_distance = fabs(m_position.y - collidable_entity->get_position().y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->get_height() / 2.0f));
            if (m_velocity.y > 0) {
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
            //kill entities in certain interactions
            if (m_entity_type == PLAYER && collidable_entity->m_entity_type == ENEMY) {
                m_is_active = false;
            }
            else if (m_entity_type == ATTACK && collidable_entity->m_entity_type == ENEMY) {
                collidable_entity->kill();
                kill();
            }
            else if (m_entity_type == ENEMY_ATTACK && collidable_entity->m_entity_type == PLAYER) {
                collidable_entity->kill();
                kill();
            }
            float x_distance = fabs(m_position.x - collidable_entity->get_position().x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->get_width() / 2.0f));
            if (m_velocity.x > 0) {
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
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
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
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;

    }
    //kill entities in certain interactions
    if (m_collided_bottom == true || m_collided_top == true) {
        if (m_entity_type == ENEMY_ATTACK) {
            kill();
        }
    }
}

void const Entity::check_collision_x(Map* map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = -m_velocity.x;

        m_collided_right = true;
    }
    //kill entities in certain interactions
    if (m_collided_left == true || m_collided_right == true) {
        if (m_entity_type == ENEMY_ATTACK) {
            kill();
        }
    }
}



void Entity::render(ShaderProgram* program)
{
    if (!m_is_active) { return; }

    program->SetModelMatrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


bool const Entity::check_collision(Entity* other) const
{
    if (other == this) return false;
    // If either entity is inactive, there shouldn't be any collision
    if (!m_is_active || !other->m_is_active) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}