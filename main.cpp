#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ASSET(x) if(!x) __debugbreak();
#define GLCALL(x) GlClearError(); \
    x\
    GlGetError(__LINE__);

static void GlClearError(){
    while(GL_NO_ERROR != glGetError());
}

static bool GlGetError(int line){
    while(GLenum error = glGetError()){
        std::cout << "[OpenGl error] (" << error << " ) on line:"<< line << std::endl;
        return false;
    }
    return true;
}



static unsigned int CompileShader(const unsigned int type,const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id,1,&src,nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id,GL_COMPILE_STATUS,&result);
    if(result == GL_FALSE){
        int length;
        glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);
        char* messge = (char * )alloca(length * sizeof(char));
        glGetShaderInfoLog(id,length,&length,messge);
        std::cout << "fild to compile" <<  (type == GL_VERTEX_SHADER?"vertex":"fragment") << std::endl;
        std::cout << messge << std::endl;
    }

    return id;
}

static int CreateShader(const std::string& vertexShader,const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER,fragmentShader);

    glAttachShader(program,vs);
    glAttachShader(program,fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        printf("Error: %s\n", glewGetErrorString(err));
    }

    float positions[] = {
        -0.5f,-0.5f,0.0f,0.0f,//0
        0.5f,-0.5f,1.0f,0.0f,//1
        0.5f,0.5f,1.0f,1.0f,//2
        -0.5f,0.5f,0.0f,1.0f,//3
    };

    unsigned int indices[] = {
        0,1,2,2,3,0
    };

   /* glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int buffer;
    glGenBuffers(1,&buffer);
    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    glBufferData(GL_ARRAY_BUFFER,4*4*sizeof(float),positions,GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int ibo; //inde buffer object
    glGenBuffers(1,&ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,6*sizeof(unsigned int),indices,GL_STATIC_DRAW);

    std::string vertexShader = 
        "#version 330 core\n"
        "layout(location=0) in vec4 position;\n"
        "layout(location=1) in vec2 texCood;\n"
        "out vec2 v_TexCood;\n"
        "\n"
        "void main()\n"
        "\n"
        "{\n"
        " gl_Position = position;\n"
        " v_TexCood = texCood;\n"
        "}\n";

    std::string fragmentShader = 
        "#version 330 core\n"
        "out vec4 color;\n"
        "in vec2 v_TexCood;\n"
        "uniform sampler2D u_Texture;\n"
        "void main()\n"
        "{\n"
        " vec4 texColor = texture(u_Texture,v_TexCood);\n"
        " color = texColor;\n"
        "}\n";


    unsigned int shader = CreateShader(vertexShader,fragmentShader);
    glUseProgram(shader);

    printf("Version: %s\n", glGetString(GL_VERSION));

    int width, height, bbp;
    
    stbi_set_flip_vertically_on_load(true); 
    unsigned char* image_buffer = stbi_load("face.png", &width, &height, &bbp, 4);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_buffer);
    glActiveTexture(GL_TEXTURE0);
    GLCALL(glBindTexture(GL_TEXTURE_2D, texture);)

    GLint textureId = glGetUniformLocation(shader, "u_Texure");
    GLCALL(glUniform1i(textureId,0);)

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        //glDrawArrays(GL_TRIANGLES,0,6);
        GLCALL(glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);)
    
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}