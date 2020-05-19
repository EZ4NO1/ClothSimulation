#include "include.h"
#include "Cloth.h"
#include "Shader.h"
#include "sphere.h"
#include "Plane.h"
#include "text.h"
#include "AirPillow.h"
#include "OBBTreeNode.h"
#include<iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
bool line_mode = true;
using namespace std;

int mode = 0;
void key_callback_wind(GLFWwindow* window, int key, int scancode, int action, int mods);
void key_callback_main(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
int bind_texture(const char* file)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //    设置纹理过滤参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    int width, height, nrChannels;

    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}
vec3 wind(vec3 x);
vec3 wind_value = vec3(0);
text text_render;
int win_width = 640 * 2;
int win_height = 480 * 2;
bool firstin = true;
int theta = 45;
int radius = sqrt(3 * 3 + 3 * 3);
vec3 center = vec3(0, -1, 0);
vec3 up = vec3(0, 1, 0);
GLuint uniform_view;
unsigned int shaderID;
void printwind() {
    std::string wind = "wind:";
    char cache[50];
    sprintf_s(cache, 50, "(%.1f,%.1f,%.1f)", wind_value.x, wind_value.y, wind_value.z);
    wind += cache;
    text_render.display(wind, vec2(20, win_height - 20), 0.5, vec3(0.5, 0.8f, 0.2f));
    text_render.display("Tips:", vec2(20, win_height - 80), 0.5, vec3(0.5, 1.0f, 0.2f));
    text_render.display("Press A/D,W/S,Q/E to control wind on x,y,z axis", vec2(20, win_height - 110), 0.5, vec3(0.5, 1.0f, 0.2f));
    text_render.display("Scroll mouse to cahnge view", vec2(20+20, win_height - 140), 0.5, vec3(0.5, 1.0f, 0.2f));
    text_render.display("Press F2 to change LineMode/TextureMode", vec2(20+20, win_height - 170), 0.5, vec3(0.5, 1.0f, 0.2f));
    text_render.display("Press Esc to back to main menu", vec2(20+20, win_height - 200), 0.5, vec3(0.5, 1.0f, 0.2f));
}
void demo1(unsigned int ShadeID, GLFWwindow* window) {
    static Cloth cloth;
    static Sphere sph1;
    static Plane plane1;
    if (firstin) {
        wind_value = vec3(0);
        glfwSetKeyCallback(window, key_callback_wind);
       cloth = Cloth(10, 10, ShadeID, 0.1, 0.1, true, vec3(0), vec2(90, 0),50,50,50);
       cloth.wind = wind;
        sph1 = Sphere(ShadeID, vec3(0.0f, -1.3f, 0.0f), 0.5f,20);
        plane1 = Plane(ShadeID, vec3(0, -2.0f, 0), vec2(2.0f, 2.0f));
        cloth.collisionSet.push_back(&sph1);
        cloth.collisionSet.push_back(&plane1);
        firstin = false;
    }
    sph1.display();
    plane1.display();
    cloth.display();
    printwind();
}
void demo3(unsigned int ShadeID, GLFWwindow* window) {

    static AirPillow pillow;
    static Plane plane1;
    if (firstin) {
        wind_value = vec3(0);
        glfwSetKeyCallback(window, key_callback_wind);
        pillow = AirPillow(20, 10, ShadeID, 0.1, 0.1, true, vec3(0), vec2(90, 0),100.0f,100.0f,100.0f);
        pillow.setwind(wind);
        plane1 = Plane(ShadeID, vec3(0, -2.0f, 0), vec2(2.0f, 2.0f));
        pillow.setcollision(&plane1);
        firstin = false;
        pillow.pressure = 0;
    }
    float step = 0.2f;
    if (glfwGetKey(window, 'Z')==GLFW_RELEASE) {
        pillow.pressure -= step;
    }
    if (glfwGetKey(window, 'X')== GLFW_RELEASE) {
        pillow.pressure += step;
    }
  
    std::string Pre = "pressure:";
    char cache[50];
    sprintf_s(cache, 50, "%.1f", pillow.pressure);
    Pre += cache;
    text_render.display(Pre, vec2(20, win_height - 40), 0.5, vec3(0.5, 0.8f, 0.2f));
    plane1.display();
    pillow.display();
    printwind();
    text_render.display("Press Z/X to control the press in AirPillow", vec2(20 + 20, win_height - 230), 0.5, vec3(0.5, 1.0f, 0.2f));
}
vec3 wind(vec3 x) {
    return wind_value;
}
void key_callback_wind(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float step = 0.4;
    if (key == 'A' && action == GLFW_PRESS) {
       // std::cout << "t" << std::endl;
        wind_value -= vec3(step, 0, 0);
    }
    if (key == 'D' && action == GLFW_PRESS)
        wind_value+= vec3(step, 0, 0);
    if (key == 'W' && action == GLFW_PRESS)
        wind_value += vec3(0,step , 0);
    if (key == 'S' && action == GLFW_PRESS)
        wind_value -= vec3(0,step , 0);
    if (key == 'Q' && action == GLFW_PRESS)
        wind_value -= vec3(0,0 , step);
    if (key == 'E' && action == GLFW_PRESS)
        wind_value += vec3(0,0 , step);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        firstin = true;
        mode = 0;
        glfwSetKeyCallback(window, key_callback_main);
    }
    if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
        Cloth::line_mode = !Cloth::line_mode;
        Plane::line_mode = !Plane::line_mode;
        Sphere::line_mode = !Sphere::line_mode;
    }  
    //std::cout << key << std::endl;
}
void key_callback_main(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == '1' && action == GLFW_PRESS) {
        mode = 1;
    }
    if (key == '2' && action == GLFW_PRESS)
        mode = 2;
    if (key == '3' && action == GLFW_PRESS)
        mode = 3;
    if (key == '4' && action == GLFW_PRESS)
        mode = 4;
    if (key == '5' && action == GLFW_PRESS)
        mode = 5;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    //std::cout << key << std::endl;
}
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        int step = 15;
        theta = (theta +((int)yoffset)* step) % 360;
        vec3 eye = center + vec3(radius * sin(glm::radians((float)theta)), 0, radius * cos(glm::radians((float)theta)));
        //std::cout << glm::to_string(eye) << endl;
        glUseProgram(shaderID);
        glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(glm::lookAt(eye, center, up)));
}
void  demo2(unsigned int ShadeID,GLFWwindow* window) {
    static Cloth cloth;
    if (firstin) {
        wind_value = vec3(0);
        glfwSetKeyCallback(window, key_callback_wind);
        cloth = Cloth(15, 15, ShadeID, 0.1, 0.05, true, vec3(0), vec2(0, 0),100,100,100);
        cloth.wind = wind;
        cloth.node[cloth.height - 1][0].fixed = true;
        cloth.node[cloth.height - 1][cloth.width - 1].fixed = true;
        firstin = false;
    }
    cloth.display();
    printwind();

}
void  demo4(unsigned int ShadeID, GLFWwindow* window) {
    static Cloth cloth1;
    static Cloth cloth2;
    static OBBTreeNode OBB1;
    static OBBTreeNode OBB2;
    
    if (firstin) {
        wind_value = vec3(0);
        glfwSetKeyCallback(window, key_callback_wind);
        cloth1 = Cloth(5, 5, ShadeID, 0.2, 0.2, true, vec3(0,-0.4,-0.2), vec2(90, 0), 100, 100, 100);
        cloth2 = Cloth(5, 5, ShadeID, 0.22, 0.22, true, vec3(0,-0.8,0.4), vec2(0, 0), 100, 100, 100);
        cloth1.wind = wind;
        cloth2.wind = wind;
        cloth1.node[(cloth1.height - 1)][0].fixed = true;
        cloth1.node[(cloth1.height - 1) ][cloth1.width - 1].fixed = true; 
        cloth2.node[(cloth2.height - 1)][0].fixed = true;
        cloth2.node[(cloth2.height - 1) ][cloth2.width - 1].fixed = true;
        OBB1 = OBBTreeNode(&cloth1);
        OBB2 = OBBTreeNode(&cloth2);
        firstin = false;
    }
   //double timebeforeOBB = glfwGetTime();

    OBB1.update();
    OBB2.update();
    OBBTreeNode::collition(&OBB2, &OBB1);

   // glfwSetTime(timebeforeOBB);
    cloth1.display();
    cloth2.display();
    
    printwind();

}
void  demo5(unsigned int ShadeID, GLFWwindow* window) {
    static Cloth cloth;
    static OBBTreeNode OBB;
    if (firstin) {
        wind_value = vec3(0);
        glfwSetKeyCallback(window, key_callback_wind);
        cloth = Cloth(10, 10, ShadeID, 0.12, 0.12, true, vec3(0,-0.4,0), vec2(90, 0), 50, 50, 50);
        cloth.wind = wind;
        cloth.node[cloth.height /2][0].fixed = true;
        cloth.node[cloth.height /2][cloth.width - 1].fixed = true;
        OBB = OBBTreeNode(&cloth);
        firstin = false;
    }
    cloth.display();
    double timebeforeOBB = glfwGetTime();
    OBB.update();
    OBB.self_collision();
    glfwSetTime(timebeforeOBB);
    printwind();

}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    win_height = height;
    win_width = width;
    glUniformMatrix4fv(glGetUniformLocation(text_render.ShaderID, "projection"), 1, GL_FALSE, glm::value_ptr(glm::ortho(0.0f, (float)width, 0.0f, (float)height)));
    glViewport(0, 0, width, height);
}

int main(void)
{

   
    GLFWwindow* window;
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_REFRESH_RATE, 60);
    window = glfwCreateWindow(win_width, win_height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window) ;
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);



    Shader ourShader("shader.vsh", "shader.fsh");

    uniform_view = glGetUniformLocation(ourShader.ID, "view");
    GLuint uniform_projection = glGetUniformLocation(ourShader.ID, "projection");

    float fovy = 120, aspect = (float)640 / 480;
    ourShader.use();
    shaderID = ourShader.ID;
    vec3 eye = center + vec3(radius * sin(glm::radians((float)theta)), 0, radius * cos(glm::radians((float)theta)));
    glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(glm::lookAt(eye, center, up)));
    glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(glm::perspective(fovy, aspect, 0.1f, 20.0f)));
    float inittime = glfwGetTime();
    int count = 0;
    text_render = text(vec2(win_width, win_height), "segoepr.ttf");
    std::string fpshint = "FPS:";
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback_main);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    Cloth::textureID = bind_texture("cloth.jpg");
    Plane::textureID = bind_texture("floor.jpg");
    Sphere::textureID = bind_texture("pink.jpg");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Cloth::textureID);
    Cloth::textureID = 1;

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Plane::textureID);
    Plane::textureID = 2;


    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, Sphere::textureID);
    Sphere::textureID = 3;


    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (count == 100) {
            fpshint = "FPS:" + std::to_string((int)(100 / (glfwGetTime() - inittime)));
            inittime = glfwGetTime();
            count = 0;
        }
        count++;
        //demowind(ourShader.ID,window);
        //demo1(ourShader.ID,window);
        text_render.display(fpshint, vec2(win_width - 120, win_height - 20), 0.5, vec3(0.5, 0.8f, 0.2f));
        if (mode == 0) {
            text_render.display("Please Press:", vec2(win_width / 2 - 140, win_height / 2 + 140), 0.5, vec3(0.8f, 0.8f, 0.2f));
            text_render.display("1:     Cloth collision demo", vec2(win_width / 2 - 140, win_height / 2 + 110), 0.5, vec3(0.8f, 0.8f, 0.2f));
            text_render.display("2:     Wind demo", vec2(win_width / 2 - 140, win_height / 2 + 80), 0.5, vec3(0.8f, 0.8f, 0.2f));
            text_render.display("3:     Air-pillow demo", vec2(win_width / 2 - 140, win_height / 2 + 50), 0.5, vec3(0.8f, 0.8f, 0.2f));
            text_render.display("4:     Collition between clothes demo", vec2(win_width / 2 - 140, win_height / 2 + 20), 0.5, vec3(0.8f, 0.8f, 0.2f));
            text_render.display("5:     Self-collision of cloth demo", vec2(win_width / 2 - 140, win_height / 2 - 10), 0.5, vec3(0.8f, 0.8f, 0.2f));
        }
        if (mode == 1) {
            demo1(ourShader.ID, window);
        }
        if (mode == 2) {
            demo2(ourShader.ID, window);
        }
        if (mode == 3) {
            demo3(ourShader.ID, window);
        }
        if (mode == 4) {
            demo4(ourShader.ID, window);
        }
        if (mode == 5) {
            demo5(ourShader.ID, window);
        }

            glfwSwapBuffers(window);
            double timebeforepull = glfwGetTime();
            glfwPollEvents();
            glfwSetTime(timebeforepull);
            // }
        }

        glfwTerminate();
        return 0;
 }