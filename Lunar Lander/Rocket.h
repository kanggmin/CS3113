class Rocket
{
private:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    glm::vec3 m_scale;
    float m_speed;
    glm::mat4 m_model_matrix;
    GLuint m_texture_id;

public:
    // ————— METHODS ————— //
    Rocket(glm::vec3 pos, glm::vec3 sca, float speed, GLuint textid);

    void update(float delta_time);
    void render(ShaderProgram* program);

    void increase_y();
    void decrease_y();
    void increase_x();
    void decrease_x();

    bool detect_collision(Platform *plat);

    // ————— GETTERS ————— // 
    glm::vec3 const get_position() const { return m_position; };
    glm::vec3 const get_scale() const { return m_position;  }
    float const get_speed() const { return m_speed; }
    glm::mat4 const get_model_matrix() const { return m_model_matrix; };
    GLuint const get_texture_id() const { return m_texture_id; };
    glm::vec3 const get_velocity()     const { return m_velocity; };
    glm::vec3 const get_acceleration() const { return m_acceleration; };

    // ————— SETTERS ————— //
    void const set_position(glm::vec3 new_position) { m_position = new_position; };
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; };
    void const set_acceleration(glm::vec3 new_position) { m_acceleration = new_position; };
};