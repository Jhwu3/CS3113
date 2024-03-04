#define GL_GLEXT_PROTOTYPES 1
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

// —— NEW STUFF —— //
#include <ctime>   //
#include "cmath"   //
// ——————————————— //

const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED     = 0.0f,
            BG_BLUE    = 0.5045f,
            BG_GREEN   = 0.0f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X      = 0,
          VIEWPORT_Y      = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char  V_SHADER_PATH[]          = "shaders/vertex_textured.glsl",
            F_SHADER_PATH[]          = "shaders/fragment_textured.glsl",
            BALL_SPRITE_FILEPATH[] = "assets/ball.png",
            PLAYER1_SPRITE_FILEPATH[] = "assets/ronaldo.png",
            PLAYER2_SPRITE_FILEPATH[] = "assets/messi.png",
            GAME_OVER_FILEPATH[] = "assets/gameover.png";
            

const float MILLISECONDS_IN_SECOND     = 1000.0;
const float MINIMUM_COLLISION_DISTANCE = 1.0f;

const int   NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL    = 0;
const GLint TEXTURE_BORDER     = 0;

SDL_Window* g_display_window;
bool  g_game_is_running = true;
float g_previous_ticks  = 0.0f;

ShaderProgram g_shader_program;
glm::mat4     g_view_matrix,
              g_model_matrix,
              g_projection_matrix,
              g_other_model_matrix,
              g_player2_model_matrix,
              g_game_over_model_matrix,
              g_ball2_model_matrix,
              g_ball3_model_matrix;

GLuint g_player_texture_id,
       g_player2_texutre_id,
       g_other_texture_id,
       g_game_over_texture_id,
       g_ball2_texture_id,
       g_ball3_texture_id;

glm::vec3 g_player_position = glm::vec3(-4.0f, 0.0f, 0.0f);
glm::vec3 g_player_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_player2_position = glm::vec3(4.0f, 0.0f, 0.0f);
glm::vec3 g_player2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_other_position  = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_other_movement  = glm::vec3(1.0f, 0.0f, 0.0f);

glm::vec3 g_ball2_position  = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 g_ball2_movement  = glm::vec3(1.0f, 0.0f, 0.0f);

glm::vec3 g_ball3_position  = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 g_ball3_movement  = glm::vec3(-1.0f, 0.0f, 0.0f);

float g_player_speed = 2.0f;
float g_player2_speed = 2.0f;
float g_other_speed = 2.0f;

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    
    return textureID;
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Hello, Collisions!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_model_matrix       = glm::mat4(1.0f);
    g_player2_model_matrix = glm::mat4(1.0f);
    g_other_model_matrix = glm::mat4(1.0f);
    g_game_over_model_matrix = glm::mat4(1.0f);
    g_ball2_model_matrix = glm::mat4(1.0f);
    g_ball3_model_matrix = glm::mat4(1.0f);
    
    g_view_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_player_texture_id = load_texture(PLAYER1_SPRITE_FILEPATH);
    g_other_texture_id  = load_texture(BALL_SPRITE_FILEPATH);
    g_player2_texutre_id = load_texture(PLAYER2_SPRITE_FILEPATH);
    g_game_over_texture_id = load_texture(GAME_OVER_FILEPATH);
    g_ball2_texture_id = load_texture(BALL_SPRITE_FILEPATH);
    g_ball3_texture_id = load_texture(BALL_SPRITE_FILEPATH);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


bool g_playing_vs_pc = false;
bool g_reached_top = false;
bool g_reached_bottom = false;
bool g_reached_top2 = false;
bool g_reached_bottom2 = false;
bool g_game_over = false;
bool g_one_ball = false;
bool g_two_ball = false;
bool g_three_ball = false;

void process_input()
{
    g_player_movement = glm::vec3(0.0f);
    g_player2_movement = glm::vec3(0.0f);
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_game_is_running = false;
                        break;
                    case SDLK_t:
                        g_playing_vs_pc = true;
                        break;
                    case SDLK_1:
                        g_one_ball = true;
                        g_two_ball = false;
                        g_three_ball = false;
                        break;
                    case SDLK_2:
                        g_one_ball = false;
                        g_two_ball = true;
                        g_three_ball = false;
                        break;
                    case SDLK_3:
                        g_one_ball = false;
                        g_two_ball = false;
                        g_three_ball = true;
                        break;
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_W])
    {
        if(g_reached_top || g_game_over){
            g_player_movement.y = 0.0f;
        }else{
            g_player_movement.y = 1.0f;
            g_reached_bottom = false;
        }
    }
    else if (key_state[SDL_SCANCODE_S])
    {
        if(g_reached_bottom || g_game_over ){
            g_player_movement.y = 0.0f;
        }else{
            g_player_movement.y = -1.0f;
            g_reached_top = false;
        }
    }
    
    if (key_state[SDL_SCANCODE_UP])
    {
        if(g_reached_top2 || g_game_over || g_playing_vs_pc){
            g_player2_movement.y = 0.0f;
        }else{
            g_player2_movement.y = 1.0f;
            g_reached_bottom2 = false;
        }
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        if(g_reached_bottom2 || g_game_over || g_playing_vs_pc){
            g_player2_movement.y = 0.0f;
        }else{
            g_player2_movement.y = -1.0f;
            g_reached_top2 = false;
        }
    }
    
    if (glm::length(g_player_movement) > 1.0f)
    {
        g_player_movement = glm::normalize(g_player_movement);
    }
    
    if (glm::length(g_player2_movement) > 1.0f)
    {
        g_player2_movement = glm::normalize(g_player2_movement);
    }
    
    if (glm::length(g_other_movement) > 1.0f)
    {
        g_other_movement = glm::normalize(g_other_movement);
    }
}


// ————————————————————————— NEW STUFF ———————————————————————————— //
bool check_collision(glm::vec3 &position_a, glm::vec3 &position_b)  //
{                                                                   //
//    float x_distance = fabs(position_a[0] - position_b[0]) - ((width_a + width_b) / 2.0f);
//    float y_distance = fabs(position_a[1] - position_b[1]) - ((height_a + height_b) / 2.0f);
//    
//    if(x_distance < 0 && y_distance < 0){
//        return true;
//    }
    
    
    // —————————————————  Distance Formula ———————————————————————— //
    return sqrt(                                                    //
                pow(position_b[0] - position_a[0], 2) +             //
                pow(position_b[1] - position_a[1], 2)               //
            ) < (MINIMUM_COLLISION_DISTANCE/2);
    return false;
}                                                                   //
// ———————————————————————————————————————————————————————————————— //





bool check_screen_boundaries(glm::vec3 &position)
{
    const float SCREEN_LEFT   = -4.9f;
    const float SCREEN_RIGHT  =  4.9f;
    const float SCREEN_TOP    =  3.70f;
    const float SCREEN_BOTTOM = -3.70f;

//    if (position.x <= SCREEN_LEFT) {
//        // Object has reached the left edge of the screen
//        return true;
//    }
//    else if (position.x >= SCREEN_RIGHT) {
//        // Object has reached the right edge of the screen
//        return true;
//    }

    if (position.y >= SCREEN_TOP) {
        // Object has reached the top edge of the screen
        return true;
    }
    else if (position.y <= SCREEN_BOTTOM) {
        // Object has reached the bottom edge of the screen
        return true;
    }
    
    return false;
}


bool check_game_over(glm::vec3 &position)
{
    const float SCREEN_LEFT   = -4.9f;
    const float SCREEN_RIGHT  =  4.9f;
    const float SCREEN_TOP    =  3.70f;
    const float SCREEN_BOTTOM = -3.70f;

    if (position.x <= SCREEN_LEFT) {
        // Object has reached the left edge of the screen
        return true;
    }
    else if (position.x >= SCREEN_RIGHT) {
        // Object has reached the right edge of the screen
        return true;
    }

//    if (position.y >= SCREEN_TOP) {
//        // Object has reached the top edge of the screen
//        return true;
//    }
//    else if (position.y <= SCREEN_BOTTOM) {
//        // Object has reached the bottom edge of the screen
//        return true;
//    }
    
    return false;
}


bool hit_border = false;
bool hit_border2 = false;
bool hit_border3 = false;
bool collided = false;
bool collided2 = false;
bool collided3 = false;
bool collided4 = false;
bool collided5 = false;
bool collided6 = false;
bool going_up = false;
bool going_down = true;

void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    
    // —————————————————————— NEW STUFF ——————————————————————— //
    if (check_collision(g_player_position, g_other_position) && collided == false)   //
    {
        // Calculate the direction from the player to the other object
        g_other_movement.x = - g_other_movement.x;
        g_other_movement.y = g_player_movement.y;
        // Adjust the position of the other object based on the bounce direction and a bounce factor
        
        std::cout <<std::time(nullptr) << ": Collision.\n";
        collided = true;
        collided2 = false;
        hit_border = false;
    }                                                           //
    // —————————————————————————————————————————————————————————//
    
    if (check_collision(g_player2_position, g_other_position) && collided2 == false)   //
    {
        // Calculate the direction from the player to the other object
        g_other_movement.x = - g_other_movement.x;
        g_other_movement.y = g_player2_movement.y;
        // Adjust the position of the other object based on the bounce direction and a bounce factor
           
        std::cout <<std::time(nullptr) << ": Collision.\n";    //
        collided2 = true;
        collided = false;
        hit_border = false;
    }
    
    if (check_collision(g_player_position, g_ball2_position) && collided3 == false)   //
    {
        // Calculate the direction from the player to the other object
        g_ball2_movement.x = - g_ball2_movement.x;
        g_ball2_movement.y = g_player_movement.y;
        // Adjust the position of the other object based on the bounce direction and a bounce factor
        
        std::cout <<std::time(nullptr) << ": Collision.\n";
        collided3 = true;
        collided4 = false;
        hit_border = false;
    }                                                           //
    // —————————————————————————————————————————————————————————//
    
    if (check_collision(g_player2_position, g_ball2_position) && collided3 == false)   //
    {
        // Calculate the direction from the player to the other object
        g_ball2_movement.x = - g_ball2_movement.x;
        g_ball2_movement.y = g_player2_movement.y;
        // Adjust the position of the other object based on the bounce direction and a bounce factor
           
        std::cout <<std::time(nullptr) << ": Collision.\n";    //
        collided3 = true;
        collided4 = false;
        hit_border = false;
    }
    
    if (check_collision(g_player_position, g_ball3_position) && collided5 == false)   //
    {
        // Calculate the direction from the player to the other object
        g_ball3_movement.x = - g_ball3_movement.x;
        g_ball3_movement.y = g_player_movement.y;
        // Adjust the position of the other object based on the bounce direction and a bounce factor
        
        std::cout <<std::time(nullptr) << ": Collision.\n";
        collided5 = true;
        collided6 = false;
        hit_border = false;
    }                                                           //
    // —————————————————————————————————————————————————————————//
    
    if (check_collision(g_player2_position, g_ball3_position) && collided6 == false)   //
    {
        // Calculate the direction from the player to the other object
        g_ball3_movement.x = - g_ball3_movement.x;
        g_ball3_movement.y = g_player2_movement.y;
        // Adjust the position of the other object based on the bounce direction and a bounce factor
           
        std::cout <<std::time(nullptr) << ": Collision.\n";    //
        collided6 = true;
        collided5 = false;
        hit_border = false;
    }
    
                       
    g_model_matrix     = glm::mat4(1.0f);
    if(g_player_position.y >= 3.7f) {
        g_reached_top = true;
    } else if( g_player_position.y <= -3.7f){
        g_reached_bottom = true;
    }
    
    if(g_player2_position.y >= 3.7f) {
        g_reached_top2 = true;
    } else if( g_player2_position.y <= -3.7f){
        g_reached_bottom2 = true;
    }
    
    
    if(g_playing_vs_pc && g_game_over == false){
        if(g_player2_position.y >= 3.7f || going_down) {
            g_player2_movement.y = -1.0f;
            going_up = false;
            going_down = true;
        }
        if( g_player2_position.y <= -3.7f || going_up){
            g_player2_movement.y = 1.0f;
            going_up = true;
            going_down = false;
        }
        
    }
    
    g_player_position += g_player_movement * g_player_speed * delta_time;
    g_model_matrix     = glm::translate(g_model_matrix, g_player_position);
                       
    g_player2_model_matrix = glm::mat4(1.0f);
    g_player2_position += g_player2_movement * g_player2_speed * delta_time;
    g_player2_model_matrix = glm::translate(g_player2_model_matrix, g_player2_position);
                       
    
    // Update the model matrix of the other object
    g_other_model_matrix = glm::mat4(1.0f);
    g_ball2_model_matrix = glm::mat4(1.0f);
    g_ball3_model_matrix = glm::mat4(1.0f);
    //g_other_model_matrix = glm::scale(g_other_model_matrix, glm::vec3(0.5f,0.5f,1.0f));
    
    if (check_screen_boundaries(g_other_position) && hit_border == false){
        std::cout << std::time(nullptr) << ": hit border.\n";
        std::cout <<"before: " << g_other_movement.x << "," << g_other_movement.y << std::endl;
        g_other_movement.y = - g_other_movement.y;
        std::cout << "after: " << g_other_movement.x << "," << g_other_movement.y << std::endl;
        hit_border = true;
    }
    
    if(check_game_over(g_other_position)){
        g_player_movement = glm::vec3(0.0f);
        g_player2_movement = glm::vec3(0.0f);
        g_other_movement = glm::vec3(0.0f);
        g_ball2_movement = glm::vec3(0.0f);
        g_ball3_movement = glm::vec3(0.0f);
        g_game_over = true;
    }
    
    if (check_screen_boundaries(g_ball2_position) && hit_border2 == false){
        std::cout << std::time(nullptr) << ": hit border.\n";
        g_ball2_movement.y = - g_ball2_movement.y;
        hit_border2 = true;
    }
    
    if(check_game_over(g_ball2_position)){
        g_player_movement = glm::vec3(0.0f);
        g_player2_movement = glm::vec3(0.0f);
        g_other_movement = glm::vec3(0.0f);
        g_ball2_movement = glm::vec3(0.0f);
        g_ball3_movement = glm::vec3(0.0f);
        g_game_over = true;
    }
    if (check_screen_boundaries(g_ball3_position) && hit_border3 == false){
        std::cout << std::time(nullptr) << ": hit border.\n";
        g_ball3_movement.y = - g_ball3_movement.y;
        hit_border3 = true;
    }
    
    if(check_game_over(g_ball3_position)){
        g_player_movement = glm::vec3(0.0f);
        g_player2_movement = glm::vec3(0.0f);
        g_other_movement = glm::vec3(0.0f);
        g_ball2_movement = glm::vec3(0.0f);
        g_ball3_movement = glm::vec3(0.0f);
        g_game_over = true;
    }
    
    g_other_position += g_other_movement *  g_other_speed * delta_time;
    
    g_other_model_matrix = glm::translate(g_other_model_matrix, g_other_position);
    
    if(g_two_ball || g_three_ball){
        g_ball2_position += g_ball2_movement *  g_other_speed * delta_time;
        
        g_ball2_model_matrix = glm::translate(g_ball2_model_matrix, g_ball2_position);
    }
    
    if(g_three_ball){
        g_ball3_position += g_ball3_movement *  g_other_speed * delta_time;
        
        g_ball3_model_matrix = glm::translate(g_ball3_model_matrix, g_ball3_position);
    }
  
    


}


void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    draw_object(g_model_matrix, g_player_texture_id);
    draw_object(g_other_model_matrix, g_other_texture_id);
    if(g_one_ball){
        draw_object(g_other_model_matrix, g_other_texture_id);
    }else if(g_two_ball){
        draw_object(g_other_model_matrix, g_other_texture_id);
        draw_object(g_ball2_model_matrix, g_ball2_texture_id);
    } else if(g_three_ball){
        draw_object(g_other_model_matrix, g_other_texture_id);
        draw_object(g_ball2_model_matrix, g_ball2_texture_id);
        draw_object(g_ball3_model_matrix, g_ball3_texture_id);
    }
  
    draw_object(g_player2_model_matrix, g_player2_texutre_id);
    if(g_game_over){
        draw_object(g_game_over_model_matrix, g_game_over_texture_id);
    }
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
