// Authors:
// Betül Demirtaş 68858
// Doğa Ege İnhanlı 69033

#include "Angel.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <cmath>

typedef vec4 color4;
typedef vec4 point4;

//Values for movement
double acceleration = 9.8f;
double velocity = 0;
double deltaTime = 0.01f;
double energyLoss = 0.95;

//Values for options
int drawingMode = 1;
int isSpinned = 1;
int shader = 1;
int componentState = 0;
int materialState = 0;
bool lightSourceMove = true;
float widthStart = -2.0;
float heightStart = 2.0;
int displayMode = 1; // 0 for wireframe, 1 for shading, 2 for texture
bool isMoving = true;

//Constants for sphere
const int NumTimesToSubdivide = 5;
const int NumTriangles = 4096; // (4 faces)^(NumTimesToSubdivide + 1)
const int NumVerticesSphere = 3 * NumTriangles;
point4 pointsSphere[NumVerticesSphere];
vec3 normals[NumVerticesSphere];
int Index = 0;

//Constants and variables for textures
GLuint textures[2];
int textureSelection = 0;
GLubyte image[2048][1024][3]; //earth.ppm
GLubyte image2[512][256][3]; //basketball.ppm
vec2    tex_coords[NumVerticesSphere];
bool textureFlag = false; 
GLuint  TextureFlag;

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 90.0, 180.0, -90.0 };

// Global arrays for controlling object specifications
float scale = 0.2f;
float heightCorrections = 0.25 * sqrt(2.0);

// Model-view, projection and color matrices uniform location
GLuint program;
GLuint ModelView, Projection;
GLuint vao;
vec3 Displacement(0.0, 0.0, -4.5);

//--------------------------------------------------
// Retrieved from Appendix Example 7 from (E. Angel)
void triangle(const point4& a, const point4& b, const point4& c) {
    double u, v; //texture coordinates
    //populates points and colors for 1 triangle on the sphere
    vec3  normal = normalize(cross(b - a, c - b));
    normals[Index] = normal;
    pointsSphere[Index] = a;
    //calculating texture coordinates
    v = (0.5 - asin(a.y) / M_PI);
    u = (0.5 * (atan2(a.z, a.x) / M_PI + 1));
    tex_coords[Index] = vec2(u, v);
    Index++;

    normals[Index] = normal;
    pointsSphere[Index] = b;
    v = (0.5 - asin(b.y) / M_PI);
    u = (0.5 * (atan2(b.z, b.x) / M_PI + 1));
    tex_coords[Index] = vec2(u, v);
    Index++;

    normals[Index] = normal;
    pointsSphere[Index] = c;
    v = (0.5 - asin(c.y) / M_PI);
    u = (0.5 * (atan2(c.z, c.x) / M_PI + 1));
    tex_coords[Index] = vec2(u, v);
    Index++;
}

//--------------------------------------------------
// Retrieved from Appendix Example 7 from (E. Angel)
point4 unit(const point4& p) {
    float len = p.x * p.x + p.y * p.y + p.z * p.z;
    point4 t;
    if (len > DivideByZeroTolerance) {
        t = p / sqrt(len);
        t.w = 1.0;
    }
    return t;
}

//--------------------------------------------------
// Retrieved from Appendix Example 7 from (E. Angel)
void divide_triangle(const point4& a, const point4& b, const point4& c, int count)
{
    if (count > 0) {
        point4 v1 = unit(a + b);
        point4 v2 = unit(a + c);
        point4 v3 = unit(b + c);
        divide_triangle(a, v1, v2, count - 1);
        divide_triangle(c, v2, v3, count - 1);
        divide_triangle(b, v3, v1, count - 1);
        divide_triangle(v1, v3, v2, count - 1);
    }
    else {
        triangle(a, b, c);
    }
}

//--------------------------------------------------
// Retrieved from Appendix Example 7 from (E. Angel)
void tetrahedron(int count) {
    point4 v[4] = {
        vec4(0.0, 0.0, 1.0, 100.0),
        vec4(0.0, 0.942809, -0.33333, 100.0),
        vec4(-0.816497, -0.471405, -0.33333, 100.0),
        vec4(0.816497, -0.471405, -0.33333, 100.0)
    };
    divide_triangle(v[0], v[1], v[2], count);
    divide_triangle(v[3], v[2], v[1], count);
    divide_triangle(v[0], v[3], v[1], count);
    divide_triangle(v[0], v[2], v[3], count);
}

void drawSphere() {
    // Subdivide a tetrahedron into a sphere
    tetrahedron(NumTimesToSubdivide);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointsSphere) + sizeof(normals) + sizeof(tex_coords), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointsSphere), pointsSphere);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsSphere), sizeof(normals), normals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsSphere) + sizeof(normals), sizeof(tex_coords), tex_coords);

}

void setProperties() {
    // Initialize texture objects
    glGenTextures(2, textures);

    //settings for earth.ppm
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //settings for basketball.ppm
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textures[textureSelection]); //set current texture

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vPosition);
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sizeof(pointsSphere)));
    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sizeof(pointsSphere) + sizeof(normals)));
    TextureFlag = glGetUniformLocation(program, "TextureFlag");
    glUniform1i(TextureFlag, textureFlag);

    // Initialize shader lighting parameters
    point4 light_position(1.0, 1.0, 1.0, 0.0); //directional light source
    color4 light_ambient(0.2, 0.2, 0.2, 1.0); // L_a
    color4 light_diffuse(1.0, 1.0, 1.0, 1.0); // L_d
    color4 light_specular(1.0, 1.0, 1.0, 1.0); // L_s

    color4 material_ambient(1.0, 0.0, 1.0, 1.0); // k_a
    color4 material_diffuse(1.0, 0.8, 0.0, 1.0); // k_d
    color4 material_specular(1.0, 0.8, 0.0, 1.0); // k_s
    float  material_shininess = 100.0;

    color4 ambient_product = light_ambient * material_ambient; // k_a * L_a
    color4 diffuse_product = light_diffuse * material_diffuse; // k_d * L_d
    color4 specular_product = light_specular * material_specular; // k_s * L_s

    glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
    glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
    glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
    glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
    glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation(program, "ModelView");
    Projection = glGetUniformLocation(program, "Projection");

    mat4  projection;
    projection = Perspective(45.0, 1.0, 0.5, 6.0);

    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); //to discard invisible faces from rendering
}

//-----------------------------------------------------------------
// By considering the boolean named shader, it sets the current program
void changeShader() {
    if (shader == 1) {
        program = InitShader("vshader_1.glsl", "fshader_1.glsl");
    }
    else {
        program = InitShader("vshader_2.glsl", "fshader_2.glsl");
    }
    setProperties();
    glUseProgram(program);
    glBindVertexArray(vao);
}

void init() {
    drawSphere();
    program = InitShader("vshader_2.glsl", "fshader_2.glsl");
    setProperties();
    glUseProgram(program);
    glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}

//-----------------------------------------------------------------
// Updates angles of the object for spin movement
void spin() {
    if (isSpinned == -1) {
        Theta[Xaxis] += 4;
        Theta[Yaxis] += 4;
        Theta[Zaxis] += 4;
    }
}

//-----------------------------------------------------------------
// Updates the current location of the object for bouncing movement
void update(void)
{
    if (isMoving) {
        Displacement.x += deltaTime; // moves with a constant velocity in the x coordinate
        velocity += acceleration * deltaTime; // velocity changes in time due to gravity (acceleration)
        Displacement.y -= velocity * deltaTime; // moves with a changing velocity in the y coordinate
        // controls the bouncing behavior of the object by checking the current location in the y coordinates and the direction of its velocity
        if (Displacement.y <= -2.0 + heightCorrections && velocity > 0) {
            Displacement.y = -2.0 + heightCorrections; // reset y position to ground level
            velocity *= -energyLoss; // energy loss when the object hits the ground
        }
    }
    
    spin();
}

//----------------------------------------------------------------------------------------------
// By checking the current component state, it sets the ambient, diffuse and specular properties
void updateComponents() {
    color4 ambient_product;
    color4 diffuse_product;
    color4 specular_product;
    point4 light_position(1.0, 1.0, 1.0, 0.0); //directional light source

    color4 light_ambient(0.2, 0.2, 0.2, 1.0); // L_a
    color4 light_diffuse(1.0, 1.0, 1.0, 1.0); // L_d
    color4 light_specular(1.0, 1.0, 1.0, 1.0); // L_s

    color4 material_ambient(1.0, 0.0, 1.0, 1.0); // k_a
    color4 material_diffuse(1.0, 0.8, 0.0, 1.0); // k_d
    color4 material_specular(1.0, 0.8, 0.0, 1.0); // k_s

    if (componentState == 0) {
        ambient_product = light_ambient * material_ambient; // k_a * L_a
        diffuse_product = light_diffuse * material_diffuse; // k_d * L_d
        specular_product = light_specular * material_specular; // k_s * L_s
    }
    else if (componentState == 1) {
        ambient_product = light_ambient * material_ambient; // k_a * L_a
        diffuse_product = light_diffuse * material_diffuse; // k_d * L_d
        color4 specular_product(0.0, 0.0, 0.0, 0.0); // k_s * L_s
    }
    else if (componentState == 2) {
        ambient_product = light_ambient * material_ambient; // k_a * L_a
        color4 diffuse_product(0.0, 0.0, 0.0, 0.0); // k_d * L_d
        color4 specular_product(0.0, 0.0, 0.0, 0.0); // k_s * L_s
    }
    else if (componentState == 3) {
        color4 ambient_product(0.0, 0.0, 0.0, 0.0); // k_a * L_a
        color4 diffuse_product(0.0, 0.0, 0.0, 0.0); // k_d * L_d
        color4 specular_product(0.0, 0.0, 0.0, 0.0); // k_s * L_s
    }

    glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
    glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
    glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
    glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);

}

//--------------------------------------------------------------------------------------------------------------
// By checking the current material state, it sets the shininess of the object to get a metallic or plastic view
void updateShininess() {
    float  material_shininess = 100.0;
    color4 ambient_product;
    color4 diffuse_product;
    color4 specular_product;
    color4 light_ambient(0.2, 0.2, 0.2, 1.0); // L_a
    color4 light_diffuse(1.0, 1.0, 1.0, 1.0); // L_d
    color4 light_specular(1.0, 1.0, 1.0, 1.0); // L_s
    if (materialState == 1) {
        // Plastic material properties
        color4 material_ambient(0.0, 0.0, 0.0, 1.0); // k_a
        color4 material_diffuse(0.6, 0.6, 0.6, 1.0); // k_d
        color4 material_specular(0.2, 0.2, 0.2, 1.0); // k_s
        material_shininess = 5.0;

        ambient_product = light_ambient * material_ambient; // k_a * L_a
        diffuse_product = light_diffuse * material_diffuse; // k_d * L_d
        specular_product = light_specular * material_specular; // k_s * L_s
        glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
        glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
        glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
    }
    else if (materialState == 2) {
        // Metallic material properties
        color4 material_ambient(0.1, 0.1, 0.1, 1.0); // k_a
        color4 material_diffuse(0.8, 0.8, 0.8, 1.0); // k_d
        color4 material_specular(0.9, 0.9, 0.9, 1.0); // k_s
        material_shininess = 300.0;
        ambient_product = light_ambient * material_ambient; // k_a * L_a
        diffuse_product = light_diffuse * material_diffuse; // k_d * L_d
        specular_product = light_specular * material_specular; // k_s * L_s
        glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
        glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
        glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
    }
    glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}

//---------------------------------------------------------------------------------------------
// If the light source move mode is on, the light position is set as the position of the object
void moveLightSource() {
    if (lightSourceMove) {
        point4 light_position(Displacement.x, Displacement.y, 1.0, 0.0); //directional light source
        glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
    }
}

//---------------------------------------------------------------
// It updates the display mode according to the current selection
void setDrawingMode() {
    if (displayMode == 0) {
        textureFlag = false;
        glUniform1i(TextureFlag, textureFlag);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
    }
    else if (displayMode == 1) {
        textureFlag = false;
        glUniform1i(TextureFlag, textureFlag);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // shading
    }
    else if (displayMode == 2) {
        textureFlag = true;
        glUniform1i(TextureFlag, textureFlag);
        glBindTexture(GL_TEXTURE_2D, textures[textureSelection]); // texture
    }
}

//------------------------------------------------------------
// Retrieved from spinCube_withReshape example from lectures
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const vec3 displacement(0.0, 0.0, -4.5);
    mat4 model_view = (Translate(Displacement) * Scale(scale, scale, scale) //scales according to the object type specifications 
        * RotateX(Theta[Xaxis])
        * RotateY(Theta[Yaxis])
        * RotateZ(Theta[Zaxis])); // Scale(), Translate(), RotateX(), RotateY(), RotateZ(): user-defined functions in mat.h

    updateComponents();
    moveLightSource();
    setDrawingMode();
    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);
    glFlush();
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) { // exit
            exit(EXIT_SUCCESS);
        }
        else if (key == GLFW_KEY_H) { // get help
            printf("Control Options:\n"
                "----------------\n"
                "Middle-click on mouse: stops a moving object or sets a stationary object in motion\n"
                "r: initializes the pose (top left corner of the window)\n"
                "space: makes the object spinning\n"
                "z: enlarges the object\n"
                "w: shrinks the object\n"
                "t: changes the display mode (wireframe/shading/texture)\n"
                "s: changes the shader (1/2)\n"
                "o: changes the component state, it closes components one by one respectively, if every one of them is already closed it opens all of them (specular/diffuse/ambient)\n"
                "m: changes the material state (metallic/plastic)\n"
                "l: changes the moving property of the light source (moving with the object/not moving)\n"
                "i: changes the current texture if the texture mode is on (earth/basketball)\n"
                "h: help; prints explanation of input controls\n"
                "q: quit (exit) the program\n\n"
                "Created by:\n"
                "-----------\n"
                "Betul Demirtas\n"
                "Doga Ege Inhanli\n\n");
        }
        else if (key == GLFW_KEY_R) { // relocate the object at the initial position
            Displacement.x = widthStart + heightCorrections;
            Displacement.y = heightStart - heightCorrections;
            velocity = 0;
        }
        else if (key == GLFW_KEY_SPACE) { // spin the object
            isSpinned *= -1;
        }
        else if (key == GLFW_KEY_Z) { // enlarges the object
            scale *= 1.1;
        }
        else if (key == GLFW_KEY_W) { // shrinks the object
            scale /= 1.1;
        }
        else if (key == GLFW_KEY_T) { // changes the display mode (wireframe, shading, texture)
            displayMode = (displayMode + 1) % 3;
        }
        else if (key == GLFW_KEY_S) { // changes the shader
            shader *= -1;
            changeShader();
        }
        else if (key == GLFW_KEY_O) { // changes the component state (specular, diffuse, ambient)
            componentState = (componentState + 1) % 4;
        }
        else if (key == GLFW_KEY_M) { // changes the material state
            materialState = (materialState + 1) % 3;
            updateShininess();
        }
        else if (key == GLFW_KEY_L) { // changes the moving property of the light source
            lightSourceMove = !lightSourceMove;
        }
        else if (key == GLFW_KEY_I) { // changes the current texture if texture mod is on
            textureSelection = (textureSelection + 1) % 2;
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) { 
        switch (button) {
        case GLFW_MOUSE_BUTTON_MIDDLE: isMoving = !isMoving; // when middle mouse button clicked, it stops the object
        }
    }
}

//----------------------------------------------------------
// Retrieved from spinCube_withReshape example from lectures and
// updated the projection according to the Ortho projection we specified in init function
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    float widthStartInitial = -2.0;
    float heightStartInitial = 2.0;
    float aspectInitial = 1.0;

    glViewport(0, 0, width, height); //may not need this since the default is usually the window size
    GLfloat aspect = GLfloat(width) / GLfloat(height);

    if (width <= height) {
        heightStart = heightStartInitial / aspect;
    }
    else {
        widthStart = widthStartInitial * aspect;
    }

    mat4 projection = Perspective(45.0, aspect, 0.5, 6.0);
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

//------------------------------------------------------------
// Retreived from the code provided in the homework zip folder
bool readPPMFile(char const* filename) {

    int nameFlag = strcmp("earth.ppm", filename);

    std::string workingDirectory;
    size_t size = 300;
    std::string fileName(filename);
    int n, m, k, nm;
    char c;
    char b[100];
    int red, green, blue;
    FILE* fd = fopen(fileName.c_str(), "r");

    if (fd == NULL) {
        std::cout << "File cannot be opened!" << std::endl;
        return false;
    }

    fscanf(fd, "%[^\n] ", b);
    fscanf(fd, "%c", &c);
    while (c == '#') {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n", b);
        fscanf(fd, "%c", &c);
    }
    ungetc(c, fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);
    nm = n * m;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            fscanf(fd, "%d %d %d", &red, &green, &blue);
            if (nameFlag == 0) {
                image[n - i - 1][j][0] = (GLubyte)red;
                image[n - i - 1][j][1] = (GLubyte)green;
                image[n - i - 1][j][2] = (GLubyte)blue;

            }
            else {
                image2[n - i - 1][j][0] = (GLubyte)red;
                image2[n - i - 1][j][1] = (GLubyte)green;
                image2[n - i - 1][j][2] = (GLubyte)blue;
            }

        }
    }
    fclose(fd);
    return true;
}
//----------------------------------------------------------
// Retrieved from spinCube_withReshape example from lectures
int main(int argc, char** argv) {

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1024, 1024, "Homework 1", NULL, NULL);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }


    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    char const* earth = "earth.ppm";
    char const* basketball = "basketball.ppm";
    readPPMFile(earth);
    readPPMFile(basketball);
    init();

    double frameRate = 120, currentTime, previousTime = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        if (currentTime - previousTime >= 1 / frameRate) {
            previousTime = currentTime;
            update();
        }
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);

    return 0;
}