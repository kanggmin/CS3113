#include "WinMenu.h"

const char FONT_SPRITE[] = "assets/font/font.png";


void WinMenu::initialise() {
	font_texture_id = Utility::load_texture(FONT_SPRITE);
}

void WinMenu::update(float delta_time) {}

void WinMenu::render(ShaderProgram* program) {
	Utility::draw_text(program, font_texture_id, "You Win!", 1, 0.01, glm::vec3(-4.0, 1.0, 0.0));
	Utility::draw_text(program, font_texture_id, "Press enter to", 1, 0.01, glm::vec3(-6.0, -1.0, 0.0));
	Utility::draw_text(program, font_texture_id, "play again", 1, 0.01, glm::vec3(-4.0, -2.0, 0.0));
}

WinMenu::~WinMenu()
{
}