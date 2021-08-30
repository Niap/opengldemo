#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define ASSET(x) if(!x) __debugbreak();
#define GLCALL(x) GlClearError(); \
    x\
    ASSET(GlGetError(__LINE__))

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
        -0.5f,-0.5f,//0
        0.5f,-0.5f,//1
        0.5f,0.5f,//2
        -0.5f,0.5f,//3
    };

    unsigned int indices[] = {
        0,1,2,2,3,0
    };

    unsigned int buffer;
    glGenBuffers(1,&buffer);
    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    glBufferData(GL_ARRAY_BUFFER,8*sizeof(float),positions,GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),0);

    unsigned int ibo; //inde buffer object
    glGenBuffers(1,&ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,6*sizeof(unsigned int),indices,GL_STATIC_DRAW);

    std::string vertexShader = 
        "#version 330 core\n"
        "layout(location=0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "\n"
        "{\n"
        " gl_Position = position;\n"
        "}\n";

    std::string fragmentShader = 
        "#version 330 core\n"
        "layout(location=0) out vec4 color;\n"
        "uniform vec4 u_Color;\n"
        "void main()\n"
        "{\n"
        " color = u_Color;\n"
        "}\n";


    unsigned int shader = CreateShader(vertexShader,fragmentShader);
    glUseProgram(shader);

    GLint uid = glGetUniformLocation(shader,"u_Color");
    glUniform4f(uid,0.0f,1.0f,0.0f,1.0f);

    printf("Version: %s\n", glGetString(GL_VERSION));

    float increament = 0.05;
    float r = 0.0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        //glDrawArrays(GL_TRIANGLES,0,6);
        glUniform4f(uid,r,1.0f,0.0f,1.0f);
        GLCALL(glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);)
        if( r>1.0f ){
            increament = -0.05;
        }else if(r < 0.0f){
            increament = 0.05;
        }
        r += increament;
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}