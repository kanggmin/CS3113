#include "LoseMenu.h"

const char FONT_SPRITE[] = "assets/font/font.png";


void LoseMenu::initialise() {
	font_texture_id = Utility::load_texture(FONT_SPRITE);
}

void LoseMenu::update(float delta_time) {}

void LoseMenu::render(ShaderProgram* program) {
	Utility::draw_text(program, font_texture_id, "You Lose!", 1, 0.01, glm::vec3(-4.0, 1.0, 0.0));
	Utility::draw_text(program, font_texture_id, "Press enter to", 1, 0.01, glm::vec3(-6.0, -1.0, 0.0));
	Utility::draw_text(program, font_texture_id, "play again", 1, 0.01, glm::vec3(-4.0, -2.0, 0.0));
}

LoseMenu::~LoseMenu()
{
}