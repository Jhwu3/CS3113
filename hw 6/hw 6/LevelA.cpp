#include "LevelA.h"
#include "Utility.h"
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()


#define LEVEL_WIDTH 42
#define LEVEL_HEIGHT 10

unsigned int LEVEL_DATA[] =
{
    1, 1, 1, 66, 1, 1, 67, 1, 1, 1, 1, 1, 1, 1, 1, 1, 67, 1, 1, 1, 33, 1, 1, 66, 1, 1, 1, 1, 1, 1, 67, 1, 1, 1, 1, 1, 1, 33, 1, 1, 1, 3,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19,
    18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19,
    18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19,
    18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19,
    18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19,
    32, 1, 1, 1, 1, 1, 1, 33, 34, 1, 1, 1, 33, 1, 1, 1, 1, 1, 1, 1, 1, 34, 1, 1, 1, 1, 66, 1, 1, 1, 1, 1, 1, 1, 1, 33, 1, 1, 1, 1, 1, 35,
};

LevelA::~LevelA()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    delete[] m_state.bullets;
    delete m_state.words;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}




void LevelA::initialise()
{
    float activationInterval = 8.0f;
    srand(static_cast<unsigned>(time(NULL)));
    GLuint map_texture_id = Utility::load_texture("assets/images/c_tilemap.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 16, 10);
    
    float safeDistance = 1.5f;

    m_state.words = new Entity();
    m_state.words->m_texture_id = Utility::load_texture("assets/font1.png");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    m_state.bgm = Mix_LoadMUS("assets/audio/Galaxy.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(1.0f);
    
    m_state.jump_sfx = Mix_LoadWAV("assets/audio/bubble.wav");


    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(2.0f, -2.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.8f);
//    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/images/characters.png");
    
    // Walking
    m_state.player->m_walking[m_state.player->LEFT]  = new int[2] { 2, 3 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[2] { 2, 3 };
    m_state.player->m_walking[m_state.player->UP]    = new int[2] { 2, 3 };
    m_state.player->m_walking[m_state.player->DOWN]  = new int[2] { 2, 3  };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
    m_state.player->m_animation_frames = 2;
    m_state.player->m_animation_index  = 0;
    m_state.player->m_animation_time   = 0.0f;
    m_state.player->m_animation_cols   = 9;
    m_state.player->m_animation_rows   = 3;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);
    
    
    // Jumping
    m_state.player->m_jumping_power = 5.0f;
    
    
    // Determine valid spawn points

    for (int y = 0; y < LEVEL_HEIGHT; y++) {
        for (int x = 0; x < LEVEL_WIDTH; x++) {
            int tileIndex = y * LEVEL_WIDTH + x;
            if (LEVEL_DATA[tileIndex] == 0) {  // Check if the tile is walkable
                glm::vec2 spawnPoint(x, y);
                float distance = glm::distance(spawnPoint, glm::vec2(m_state.player->get_position().x, m_state.player->get_position().y));
                if (distance >= safeDistance) {
                    validSpawns.push_back(spawnPoint);
                }
            }
        }
    }
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("assets/images/enemy.png");
    
    m_state.enemies = new Entity[ENEMY_COUNT];
    float baseSpeed = 1.0f; // Base speed for enemies
    float speedVariance = 0.5f; // Maximum variance in speed

    for (int i = 0; i < ENEMY_COUNT; ++i) {
        int spawnIndex = rand() % validSpawns.size(); // Get a random index for spawn points
        glm::vec2 spawnPoint = validSpawns[spawnIndex]; // Get the spawn position

        m_state.enemies[i].set_entity_type(ENEMY);
        m_state.enemies[i].set_ai_type(GUARD);
        m_state.enemies[i].set_ai_state(IDLE);
        m_state.enemies[i].m_texture_id = enemy_texture_id;
        m_state.enemies[i].set_position(glm::vec3(spawnPoint.x, -spawnPoint.y, 0.0f));
        m_state.enemies[i].set_movement(glm::vec3(0.0f));
        m_state.enemies[i].set_speed(baseSpeed + (rand() % 100 / 100.0f) * speedVariance);
        m_state.enemies[i].activationTime = i * activationInterval;
        m_state.enemies[i].jump_sfx = m_state.jump_sfx;
        m_state.enemies[i].initiallyActive = false; // Start inactive
        m_state.enemies[i].deactivate();
    }
    m_state.enemies[0].activate();
    m_state.enemies[0].initiallyActive = true; // Start inactive
    m_state.enemies[0].hasBeenActivated = true;
    m_state.enemies[1].activate();
    m_state.enemies[1].initiallyActive = true; // Start inactive
    m_state.enemies[1].hasBeenActivated = true;
    m_state.enemies[2].activate();
    m_state.enemies[2].initiallyActive = true; // Start inactive
    m_state.enemies[2].hasBeenActivated = true;
    
//    Bullets stuff
    GLuint bullet_texture_id = Utility::load_texture("assets/images/orb.png");
    m_state.bullets = new Entity[BULLET_COUNT];
    
    m_state.bullets[0].set_entity_type(BULLET);
    m_state.bullets[0].m_texture_id = bullet_texture_id;
    m_state.bullets[0].set_position(glm::vec3(m_state.player->get_position().x + 1.0f, m_state.player->get_position().y + 1.0f, 0.0f));
    m_state.bullets[0].set_speed(2.5f);
    m_state.bullets[0].set_movement(glm::vec3(0.0f));
    m_state.bullets[0].set_height(0.2f);
    m_state.bullets[0].set_width(0.2f);
    m_state.bullets[0].jump_sfx = m_state.jump_sfx;
    
    m_state.bullets[1].set_entity_type(BULLET);
    m_state.bullets[1].m_texture_id = bullet_texture_id;
    m_state.bullets[1].set_position(glm::vec3(m_state.player->get_position().x + 2.0f, m_state.player->get_position().y - 2.0f, 0.0f));
    m_state.bullets[1].set_speed(2.5f);
    m_state.bullets[1].set_movement(glm::vec3(0.0f));
    m_state.bullets[1].set_height(0.2f);
    m_state.bullets[1].set_width(0.2f);
    m_state.bullets[0].m_orbitRadius = 1.0f; // Radius of 1 unit around the player
    m_state.bullets[1].m_orbitRadius = 2.0f; // Radius of 2 units around the player
    m_state.bullets[0].m_orbitAngle = 0.0f;
    m_state.bullets[1].m_orbitAngle = 180.0f;
    m_state.bullets[1].jump_sfx = m_state.jump_sfx;
    m_state.bullets[1].deactivate();
    
    
    
    /**
     BGM and SFX
     */
 
}

void LevelA::update(float delta_time)
{
    if (currentTime >= 120.0f) {
            gameWon = true;  // Assume gameWon is a bool member of LevelA
            return;  // Stop updating game logic if the game is won
        }
    
    currentTime += delta_time;  // Accumulate time passed
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map, currentTime);
    
    for (int i = 0; i < BULLET_COUNT; i++)
    {
        m_state.bullets[i].update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map, currentTime);
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if (!m_state.enemies[i].is_active()) {
                
                m_state.enemies[i].reactivate(validSpawns, currentTime);
        }
        m_state.enemies[i].update(delta_time, m_state.player, NULL, NULL, m_state.map, currentTime);
    }
}


void LevelA::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    
    // Save the original projection matrix
    glm::mat4 originalProjection = glm::ortho(-6.5f, 6.5f, -5.625f, 5.625f, -1.0f, 1.0f);

    glm::mat4 uiProjection = glm::ortho(0.0f, static_cast<float>(640), 0.0f, static_cast<float>(480), -1.0f, 1.0f);
    program->set_projection_matrix(uiProjection);

    // Text position to bottom right corner
    glm::vec3 text_position(640 - 450, 40, 0);
    std::string timeText = "Time Elapsed: " + std::to_string(static_cast<int>(currentTime)) + "s";
    Utility::draw_text(program, m_state.words->m_texture_id, timeText, 25.5f, 0.00001f, text_position);
    
    program->set_projection_matrix(originalProjection);
    if(!m_state.player->is_active()){
        glm::mat4 uiProjection = glm::ortho(0.0f, static_cast<float>(640), 0.0f, static_cast<float>(480), -1.0f, 1.0f);
        program->set_projection_matrix(uiProjection);
        
        glm::vec3 center_position(240, 240, 0);
        Utility::draw_text(program, m_state.words->m_texture_id, "You Lose!", 25.0f, 0.0001f, center_position);
        
        
        glm::mat4 originalProjection = glm::ortho(-6.5f, 6.5f, -5.625f, 5.625f, -1.0f, 1.0f);
        program->set_projection_matrix(originalProjection);
    }
    
    if (!gameWon && m_state.player->is_active()) {
        for (int i = 0; i < BULLET_COUNT; i++) {
            m_state.bullets[i].render(program);
        }
        
        for (int i = 0; i < ENEMY_COUNT; i++) {
            m_state.enemies[i].render(program);
        }
    } else if(m_state.player->is_active()){
        glm::mat4 uiProjection = glm::ortho(0.0f, static_cast<float>(640), 0.0f, static_cast<float>(480), -1.0f, 1.0f);
        program->set_projection_matrix(uiProjection);
        
        glm::vec3 center_position(240, 240, 0);
        Utility::draw_text(program, m_state.words->m_texture_id, "You Win!", 25.0f, 0.0001f, center_position);
        
        
        glm::mat4 originalProjection = glm::ortho(-6.5f, 6.5f, -5.625f, 5.625f, -1.0f, 1.0f);
        program->set_projection_matrix(originalProjection);
    }
}
