//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>

//GML libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include "logger.h"

#define WIDTH     800
#define HEIGHT    600
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)
#define INDICE_TO_PTR(x) ((void*)(x))

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    //SDL2 / OpenGL Context initialization : 
    ////////////////////////////////////////
    
    //Initialize SDL2
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
        return 0;
    }

    //Create a Window
    SDL_Window* window = SDL_CreateWindow("VR Camera",                           //Titre
                                          SDL_WINDOWPOS_UNDEFINED,               //X Position
                                          SDL_WINDOWPOS_UNDEFINED,               //Y Position
                                          WIDTH, HEIGHT,                         //Resolution
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); //Flags (OpenGL + Show)

    //Initialize OpenGL Version (version 3.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    //Initialize the OpenGL Context (where OpenGL resources (Graphics card resources) lives)
    SDL_GLContext context = SDL_GL_CreateContext(window);

    //Tells GLEW to initialize the OpenGL function with this version
    glewExperimental = GL_TRUE;
    glewInit();


    //Start using OpenGL to draw something on screen
    glViewport(0, 0, WIDTH, HEIGHT); //Draw on ALL the screen

    //The OpenGL background color (RGBA, each component between 0.0f and 1.0f)
    glClearColor(0.0, 0.0, 0.0, 1.0); //Full Black

    glEnable(GL_DEPTH_TEST); //Active the depth test
    glDepthFunc(GL_LESS);

    //TODO : TD1
    //From here you can load your OpenGL objects, like VBO, Shaders, etc.
    // Vertices table
    static const GLfloat positions[] = { // Codage XYZ -> middle : X=0.0,Y=0.0 ; left bottom : X=-1.0,Y=-1.0 ;
                                                      //  right top : X=1.0,Y=1.0
        -1.0f, -1.0f, 0.0f, //first triangle
        1.0f,  1.0f, 0.0f,  
        1.0f, -1.0f, 0.0f,  //end of first triangle
        -1.0f, -1.0f, 0.0f, //second triangle
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
    };

   

    // Color table
    static const GLfloat colors[] = { // Codage RGB -> 1.0 : normal color ; <0.0 : black 
        1.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    

    // This will identify our vertex buffer
    GLuint vboID;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vboID);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
      ////1 Triangle :
      //  // Generate size of buffer
      //  glBufferData(GL_ARRAY_BUFFER, 3*(3+3)*sizeof(float), nullptr, GL_DYNAMIC_DRAW);
      //  // Put datas in buffer
      //  glBufferSubData(GL_ARRAY_BUFFER, 0, 3*3*sizeof(float), positions);
      //  glBufferSubData(GL_ARRAY_BUFFER, 3*3*sizeof(float), 3*3*sizeof(float), colors);
      //Multiples Triangles :
        // Generate size of buffer
        glBufferData(GL_ARRAY_BUFFER, 2 * 3 * (3 + 3) * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        // Put datas in buffer
        glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * 3 * sizeof(float), positions);
        glBufferSubData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), 6 * 3 * sizeof(float), colors);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    const char* vertexPath = "Shaders/color.vert";
    const char* fragPath = "Shaders/color.frag";

    FILE* vertexFile = fopen(vertexPath, "r");
    FILE* fragFile = fopen(fragPath, "r");
    //TODO test if the files are correct (testing NULL)
    Shader* shader = Shader::loadFromFiles(vertexFile, fragFile);
    fclose(vertexFile);
    fclose(fragFile);

    if (shader == NULL)
    {
        //Print an error message (an error occured while compiling). The message is also
        return EXIT_FAILURE;
    }


    bool isOpened = true;

    //Main application loop
    while(isOpened)
    {
        //Time in ms telling us when this frame started. Useful for keeping a fix framerate
        uint32_t timeBegin = SDL_GetTicks();

        //Fetch the SDL events
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_WINDOWEVENT:
                    switch(event.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE:
                            isOpened = false;
                            break;
                        default:
                            break;
                    }
                    break;
                //We can add more event, like listening for the keyboard or the mouse. See SDL_Event documentation for more details
            }
        }



        

        //Clear the screen : the depth buffer and the color buffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glm::mat4 projection(1.0f);
        glm::mat4 camera(1.0f);
        glm::mat4 model(1.0f);

        // Transformation : scale then rotate then translate
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        //model = glm::rotate(model, glm::degrees(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
        

        camera = glm::lookAt(
            glm::vec3(0, 0, 1), // Camera is at (4,3,3), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

        glm::mat4 mvp = projection * camera * model;

        //TODO do something with your shader
        glUseProgram(shader->getProgramID());
        { //The brackets are useless but help at the clarity of the code
            
            glBindBuffer(GL_ARRAY_BUFFER, vboID);
            //Work with vPosition
            GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");
            glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); //It is here that you
            glEnableVertexAttribArray(vPosition); //Enable "vPosition"
            //Work with vColor
            GLint vColor = glGetAttribLocation(shader->getProgramID(), "vColor");
            //Simple transformation : size divide by two
            //GLint uScale = glGetUniformLocation(shader->getProgramID(), "uScale");
            //glUniform1f(uScale, 0.5f);
            GLint uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP");
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
            //Colors start at 9*sizeof(float) (3*nbVertices*sizeof(float)) for the second
            glVertexAttribPointer(vColor, 3, GL_FLOAT, 0, 0, INDICE_TO_PTR(18 * sizeof(float)));
            //glVertexAttribPointer(vColor, 3, GL_FLOAT, 0, 0, 0);

            //Convert an indice to void* : (void*)(x)
            glEnableVertexAttribArray(vColor); //Enable"vColor"
            glDrawArrays(GL_TRIANGLES, 0, 3*3); //Draw the triangle (three points which
            glBindBuffer(GL_ARRAY_BUFFER, 0); //Close the VBO (not mandatory but recommended
        }

        

        glUseProgram(0); //Close the program. This is heavy for the GPU. In reality we do this


        //delete shader; //Delete the shader (usually at the end of the program)


        



        //Display on screen (swap the buffer on screen and the buffer you are drawing on)
        SDL_GL_SwapWindow(window);

        //Time in ms telling us when this frame ended. Useful for keeping a fix framerate
        uint32_t timeEnd = SDL_GetTicks();

        //We want FRAMERATE FPS
        if(timeEnd - timeBegin < TIME_PER_FRAME_MS)
            SDL_Delay((uint32_t)(TIME_PER_FRAME_MS) - (timeEnd - timeBegin));
    }

    delete shader;
    glDeleteBuffers(1, &vboID);
    
    //Free everything
    if(context != NULL)
        SDL_GL_DeleteContext(context);
    if(window != NULL)
        SDL_DestroyWindow(window);

    return 0;
}
