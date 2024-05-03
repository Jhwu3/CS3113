#include "Scene.h"

class LevelA : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 10;
    int BULLET_COUNT = 2;
    std::vector<glm::vec2> validSpawns;
    float currentTime = 0.0f;  // Start time at 0
    bool gameWon = false;
    // ————— CONSTRUCTOR ————— //
    ~LevelA();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
