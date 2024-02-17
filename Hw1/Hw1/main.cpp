/**
* Author: [Your name here]
* Assignment: Simple 2D Scene
* Date due: 2024-02-17, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"               // We'll talk about these later in the course

#define LOG(argument) std::cout << argument << '\n'

// Our window dimensions
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

int g_frame_counter = 0;

// Background color components
const float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

// Our viewport—or our "camera"'s—position and dimensions
const int VIEWPORT_X      = 0,
          VIEWPORT_Y      = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

// Our shader filepaths; these are necessary for a number of things
// Not least, to actually draw our shapes
// We'll have a whole lecture on these later
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";


// Our object's fill colour
const float TRIANGLE_RED     = 1.0,
            TRIANGLE_BLUE    = 0.4,
            TRIANGLE_GREEN   = 0.4,
            TRIANGLE_OPACITY = 1.0;

bool g_game_is_running = true;
SDL_Window* g_display_window;

ShaderProgram g_shader_program;

glm::mat4 g_view_matrix,        // Defines the position (location and orientation) of the camera
          g_model_matrix,       // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week
          g_projection_matrix;  // Defines the characteristics of your camera, such as clip panes, field of view, projection method, etc.
glm::mat4 g_orb_model_matrix;

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"


const char PLAYER_SPRITE[] = "assets/dragon.png";
const char ORB_SPRITE[] = "assets/energy.png";

GLuint g_player_texture_id;
GLuint g_orb_texture_id;
const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0; // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0; // this value MUST be zero


GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        LOG(filepath);
        assert(false);
    }
    
    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);
    
    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Hw 1",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // Initialise our camera
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    // Load the shaders for handling textures
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);


    
    // Initialise our view, model, and projection matrices
    g_view_matrix       = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    g_model_matrix      = glm::mat4(1.0f);  // Defines every translation, rotations, or scaling applied to an object
    g_orb_model_matrix = glm:: mat4(1.0f);
    g_orb_model_matrix = glm::translate(g_orb_model_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
    g_orb_model_matrix = glm::scale(g_orb_model_matrix, glm::vec3(2.0f,2.0f,0.0f));
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.
    
//    g_model_matrix = glm::translate(g_model_matrix, glm::vec3(5.0f, 0.0f, 0.0f));
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    // Notice we haven't set our model matrix yet!
    
    g_shader_program.set_colour(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);
    
    // Each object has its own unique ID
    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    
    // Load our player image
    g_player_texture_id = load_texture(PLAYER_SPRITE);
    g_orb_texture_id = load_texture(ORB_SPRITE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_game_is_running = false;
        }
    }
}

const float GROWTH_FACTOR = 1.20f;
const float SHRINK_FACTOR = 0.80f;
const int MAX_FRAME = 40;           // this value is, of course, up to you

bool g_is_growing = true;

const float MILLISECONDS_IN_SECOND = 1000.0;

float g_triangle_x = 0.0f;
float g_triangle_rotate = 0.0f;
float g_previous_ticks = 0.0f;
const float DEGREES_PER_SECOND = 90.0f;
const float radius = 1.5f;
const float speed = 1.0f;

float factor = 1.01f;


void update()
{
    
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;  // get the current number of ticks
    float delta_time = ticks - g_previous_ticks;     // the delta time is the difference from the last frame
    g_previous_ticks = ticks;
    
    // STEP 1
    glm::vec3 scale_vector;
    g_frame_counter += 1;
    
    if((int)ticks %  4 == 0){
        glClearColor(0.0f, 0.0f, 0.0f, BG_OPACITY);
    }else{
        glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    }

    
    // STEP 2
    if (g_frame_counter >= MAX_FRAME)
    {
        g_is_growing = !g_is_growing;
        g_frame_counter = 0;
    }else{
        if(g_is_growing){
            factor += 0.7f * delta_time ;
        }else{
            factor -= 0.7f * delta_time;
        }
    }
    
    // STEP 4
    scale_vector = glm::vec3(g_is_growing ? factor : factor,
                             g_is_growing ? factor : factor,
                             1.0f);
    
    float rotation_angle = speed * ticks;
    float rotation_angle2 = speed * 1.5 * ticks;
    float tri_x = radius * cos(rotation_angle);
    float tri_y = radius * sin(rotation_angle);
    
    float tri_x2 = radius * cos(rotation_angle2);
    float tri_y2 = radius * sin(rotation_angle2);
    
    g_triangle_x += 1.0f * delta_time;
    g_triangle_rotate += DEGREES_PER_SECOND * delta_time;
    g_model_matrix = glm::mat4(1.0f);
    
    
    g_model_matrix = glm::scale(g_model_matrix, scale_vector);
    g_model_matrix = glm::translate(g_model_matrix, glm::vec3(tri_x, tri_y, 0.0f));
    g_orb_model_matrix = glm::translate(g_model_matrix, glm::vec3(tri_x2,tri_y2,0.0f));
    //g_orb_model_matrix = glm::rotate(g_model_matrix, glm::radians(g_triangle_rotate), glm::vec3(0.0f, 0.0f ,1.0f));
    g_orb_model_matrix = glm::rotate(g_orb_model_matrix, glm::radians(g_triangle_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
        };
    
    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
        };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    draw_object(g_model_matrix, g_player_texture_id);
    draw_object(g_orb_model_matrix, g_orb_texture_id);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    
    
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }

/**
 Start here—we can see the general structure of a game loop without worrying too much about the details yet.
 */
int main(int argc, char* argv[])
{
    // Initialise our program—whatever that means
    initialise();
    
    while (g_game_is_running)
    {
        process_input();  // If the player did anything—press a button, move the joystick—process it
        update();         // Using the game's previous state, and whatever new input we have, update the game's state
        render();         // Once updated, render those changes onto the screen
    }
    
    shutdown();  // The game is over, so let's perform any shutdown protocols
    return 0;
}
