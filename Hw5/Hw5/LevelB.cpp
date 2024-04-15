#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 8

unsigned int LEVELB_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 120, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0,
    0, 140, 0, 0, 140, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 140, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

LevelB::~LevelB()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    delete    m_state.words;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelB::initialise()
{
    m_state.next_scene_id = -1;
    GLuint map_texture_id = Utility::load_texture("assets/images/tilemap.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 20, 9);
    
    m_state.words = new Entity();
    m_state.words->m_texture_id = Utility::load_texture("assets/font1.png");
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/images/characters.png");
    m_state.player->m_number_lives = m_number_of_lives;
    
    // Walking
//    m_state.player->m_walking[m_state.player->LEFT]  = new int[4] { 1, 5, 9,  13 };
//    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 3, 7, 11, 15 };
//    m_state.player->m_walking[m_state.player->UP]    = new int[4] { 2, 6, 10, 14 };
//    m_state.player->m_walking[m_state.player->DOWN]  = new int[4] { 0, 4, 8,  12 };
    m_state.player->m_walking[m_state.player->LEFT]  = new int[4] { 0, 1 };
    m_state.player->m_walking[m_state.player->RIGHT]  = new int[4] { 0, 1 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
    m_state.player->m_animation_frames = 2;
    m_state.player->m_animation_index  = 0;
    m_state.player->m_animation_time   = 0.0f;
    m_state.player->m_animation_cols   = 9;
    m_state.player->m_animation_rows   = 3;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);
    
    // Jumping
    m_state.player->m_jumping_power = 7.0f;
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("assets/images/enemy.png");
    
    m_state.enemies = new Entity[ENEMY_COUNT];
    for (int i = 0; i < ENEMY_COUNT; i++){

        
        m_state.enemies[i].set_entity_type(ENEMY);
        m_state.enemies[i].set_ai_type(GUARD);
        m_state.enemies[i].set_ai_state(Patrol);
        m_state.enemies[1].set_jumping_power(2.0f);
        m_state.enemies[1].set_ai_state(Patrol);
        m_state.enemies[2].set_ai_state(RunPatrol);
        m_state.enemies[i].m_texture_id = enemy_texture_id;
        if(i == 0){
            m_state.enemies[i].set_position(glm::vec3(4.5f, 0.0f, 0.0f));
        } else if(i == 1) {
            m_state.enemies[i].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
        }else{
            m_state.enemies[i].set_position(glm::vec3(11.0f, 0.0f, 0.0f));
        }
        m_state.enemies[i].set_movement(glm::vec3(0.0f));
        m_state.enemies[i].set_speed(0.5f);
        m_state.enemies[i].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
        m_state.enemies[i].set_width(0.8f);
//        g_game_state.enemies[i].set_height(0.97f);
    }
   
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("assets/audio/Galaxy.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(1.0f);
    
    m_state.jump_sfx = Mix_LoadWAV("assets/audio/Bubble.wav");
}

void LevelB::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    m_number_of_lives = m_state.player->m_number_lives;
    for(int i = 0; i < ENEMY_COUNT;i++){
        m_state.enemies[i].update(delta_time, m_state.player, NULL, 0, m_state.map);
    }
    
    if (m_state.player->get_position().y < -10.0f) m_state.next_scene_id = 3;
}

void LevelB::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    for(int i = 0; i < ENEMY_COUNT;i++){
        m_state.enemies[i].render(program);
    }
    
    if(m_state.player->get_is_active()){
        std::string lives = std::to_string (m_state.player->m_number_lives);
        Utility::draw_text(program, m_state.words->m_texture_id, "Lives:" + lives , 0.25f, 0.0001f, (glm::vec3(m_state.player->get_position().x - 0.6f,m_state.player->get_position().y + 0.7f , 0.0f)));
    }
    
    if(!m_state.player->get_is_active()){
//        std::cout <<
        Utility::draw_text(program, m_state.words->m_texture_id, "You Lose", 1.0f, 0.0f, (glm::vec3(m_state.player->get_position().x -3.5f, -3.0f, 0.0f)));
        
    }
    
}
