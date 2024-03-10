class Platform {
private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::mat4 m_model_matrix;
    GLuint m_texture_id;
public:
    Platform(glm::vec3 pos, glm::vec3 sca, GLuint textID);
    Platform();


    void update();
    void render(ShaderProgram* program);

    //setters
    void set_pos(glm::vec3 new_pos) { m_position = new_pos; };
    void set_scale(glm::vec3 new_scale) { m_scale = new_scale; };
    void set_textID(GLuint textID) { m_texture_id = textID; };

    //getters
    glm::vec3 get_pos() { return m_position; };
    glm::vec3 get_scale() { return m_scale; };
};