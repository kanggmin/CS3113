#pragma once
#include "Scene.h"

class LoseMenu : public Scene {
public:
    GLuint font_texture_id;
    // ����� CONSTRUCTOR ����� //
    ~LoseMenu();

    // ����� METHODS ����� //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};