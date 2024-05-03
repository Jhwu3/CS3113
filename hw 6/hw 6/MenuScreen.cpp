#include "MenuScreen.h"
#include "Utility.h"


MenuScreen::~MenuScreen()
{
    delete m_state.words;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}



void MenuScreen::initialise()
{
    m_state.next_scene_id = -1;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    m_state.words = new Entity();
    m_state.words->m_texture_id = Utility::load_texture("assets/font1.png");
    
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(2.0f, -2.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->deactivate();
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_state.bgm = Mix_LoadMUS("assets/audio/Galaxy.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(1.0f);
    m_state.jump_sfx = Mix_LoadWAV("assets/audio/bubble.wav");
}
    




void MenuScreen::render(ShaderProgram* program) {
    // Solid color screen
    glClear(GL_COLOR_BUFFER_BIT);

    glm::vec3 text_position(1.85f, -4.0f, 0.0f);
    Utility::draw_text(program, m_state.words->m_texture_id, "Press Enter to Start.", 0.45f, 0.0001f, text_position);
}
