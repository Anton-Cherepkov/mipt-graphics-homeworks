// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


#include <common/shader.hpp>

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Anton Cherepkov HW2", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // --------------------------------------------------------------
    // Create and compile our GLSL program from the shaders
    GLuint ProgramID_1 = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader_1.fragmentshader" );
    GLuint ProgramID_2 = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader_2.fragmentshader" );

    GLuint MatrixID_1 = glGetUniformLocation(ProgramID_1, "MVP");
    GLuint MatrixID_2 = glGetUniformLocation(ProgramID_2, "MVP");

    const GLuint ProgramIDs[] = {ProgramID_1, ProgramID_2};
    const GLuint MatrixIDs[] =  {MatrixID_1, MatrixID_2};
    // --------------------------------------------------------------

    // --------------------------------------------------------------
    static const GLfloat g_vertex_buffer_data[] = {
            -0.9f, -0.9f, 0.0f,
            0.9f, -0.9f, 0.0f,
            0.0f,  0.6f, 0.0f,

            -0.9f, 0.9f, 0.0f,
            0.9f, 0.9f, 0.0f,
            0.0f, -0.6f, 0.0f,
    };
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    // --------------------------------------------------------------

    // --------------------------------------------------------------
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // --------------------------------------------------------------

    size_t current_step = 0;
    const size_t MAX_STEPS = 128;

    const glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    const glm::mat4 Model = glm::mat4(1.0f);
    float angle, dist;
    glm::mat4 View;
    glm::mat4 MVP;

    do{
        current_step = (current_step + 1) % MAX_STEPS;

        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );

        // Calculate MVP
        angle = acos(-1) * current_step / MAX_STEPS;
        dist = 1.5f * cos(2 * acos(-1) * current_step / MAX_STEPS) + 5.0f;
        View = glm::lookAt(
                glm::vec3(
                        dist * cos(angle),
                        0.0f,
                        dist * sin(angle)
                ),
                glm::vec3(0,0,0),     // and looks at the origin
                glm::vec3(0,1,0)        // Head is up (set to 0,-1,0 to look upside-down)
        );
        MVP = Projection * View * Model;

        for (size_t k = 0; k < 2; ++k) {
            glUseProgram(ProgramIDs[k]);
            glUniformMatrix4fv(MatrixIDs[k], 1, GL_FALSE, &MVP[0][0]);

            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glVertexAttribPointer(
                    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
            );
            glDrawArrays(GL_TRIANGLES, 3 * k, 3); // 3 indices starting at 0 -> 1 triangle
            glDisableVertexAttribArray(0);
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(ProgramID_1);
    glDeleteProgram(ProgramID_2);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
