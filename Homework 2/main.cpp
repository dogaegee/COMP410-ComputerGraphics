// Betül Demirtaş 68858 - Doğa Ege İnhanlı 69033
// Homework 2


#include "Angel.h"
#include <vector>
using namespace std;

typedef vec4 point4;
typedef vec4 color4;

//global variables
const int numCube = 27;
const int verticeNumCube = 36;
const int numVertices = verticeNumCube * numCube;
const int loopNum = 3;
const int faceNumToRotate = 9;
const int verticesOnCube = 8;
point4 points[numVertices];
color4 colors[numVertices];
GLuint vColor;
double aspectX = 45.0;
double aspectZ = -45.0;
int rotatableSection = -1;
vector<int> elementsToRotate;
point4 vertices[numCube][8];
int rotationDirection = -1;
int Index = 0;

color4 faceColors[6] = {
    color4(1.0, 0.4, 0.4, 1.0), // bright pink
    color4(1.0, 1.0, 0.4, 1.0), // bright yellow
    color4(0.4, 1.0, 0.4, 1.0), // bright green
    color4(0.4, 0.4, 1.0, 1.0), // bright blue
    color4(1.0, 0.65, 0.0, 1.0), // orange
    color4(1.0, 1.0, 1.0, 1.0), //white 
};


int indexList[numCube][loopNum] = {
    {0, 0, 0}, 
    {1, 0, 0}, 
    {2, 0, 0}, 
    {0, 1, 0}, 
    {1, 1, 0}, 
    {2, 1, 0}, 
    {0, 2, 0}, 
    {1, 2, 0}, 
    {2, 2, 0}, 
    {0, 0, 1}, 
    {1, 0, 1}, 
    {2, 0, 1}, 
    {0, 1, 1}, 
    {1, 1, 1}, 
    {2, 1, 1}, 
    {0, 2, 1}, 
    {1, 2, 1}, 
    {2, 2, 1}, 
    {0, 0, 2},
    {1, 0, 2}, 
    {2, 0, 2}, 
    {0, 1, 2}, 
    {1, 1, 2}, 
    {2, 1, 2}, 
    {0, 2, 2}, 
    {1, 2, 2}, 
    {2, 2, 2}  
};
 

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;


//----------------------------------------------------------------------------
// 
mat4 getRotationMatrix(float theta, int x, int y, int z) {
    const float alpha = DegreesToRadians * theta;

    mat4 rotationMatrix;

    if (x == 1) {
        rotationMatrix = {
            1, 0, 0, 0,
            0, cos(alpha), -sin(alpha), 0,
            0, sin(alpha), cos(alpha), 0,
            0, 0, 0, 1
        };
    }
    else if (y == 1) {
        rotationMatrix = {
            cos(alpha), 0, sin(alpha), 0,
            0, 1, 0, 0,
            -sin(alpha), 0, cos(alpha), 0,
            0, 0, 0, 1
        };
    }
    else if (z == 1) {
        rotationMatrix = {
            cos(alpha), -sin(alpha), 0, 0,
            sin(alpha), cos(alpha), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }
    else {
        rotationMatrix = mat4(1.0);
    }

    return rotationMatrix;
}

//----------------------------------------------------------------------------
// quad generates two triangles for each face and assigns colors to the vertices

void
quad(int a, int b, int c, int d, int currentCube, int currentColor)
{
    // Initialize colors

    colors[Index] = faceColors[currentColor]; points[Index] = vertices[currentCube][a]; Index++;
    colors[Index] = faceColors[currentColor]; points[Index] = vertices[currentCube][b]; Index++;
    colors[Index] = faceColors[currentColor]; points[Index] = vertices[currentCube][c]; Index++;
    colors[Index] = faceColors[currentColor]; points[Index] = vertices[currentCube][a]; Index++;
    colors[Index] = faceColors[currentColor]; points[Index] = vertices[currentCube][c]; Index++;
    colors[Index] = faceColors[currentColor]; points[Index] = vertices[currentCube][d]; Index++;
}


//----------------------------------------------------------------------------
// generate 12 triangles: 36 vertices and 36 colors
void
generateCubes(int i)
{
    quad(1, 0, 3, 2, i, 0);
    quad(2, 3, 7, 6, i, 1);
    quad(3, 0, 4, 7, i, 2);
    quad(6, 5, 1, 2, i, 3);
    quad(4, 5, 6, 7, i, 4);
    quad(5, 4, 0, 1, i, 5);

}


void assignRotatedVertices() {
   
    mat4 m;
    if (rotationDirection == 1) {
        m = getRotationMatrix(-10.0, 1.0, 0.0, 0.0);
    }
    else if (rotationDirection == -1) {
        m = getRotationMatrix(10.0, 1.0, 0.0, 0.0);
    }
    else if (rotationDirection == 2) {
        m = getRotationMatrix(10.0, 0.0, 1.0, 0.0);
    }
    else if (rotationDirection == -2) {
        m = getRotationMatrix(-10.0, 0.0, 1.0, 0.0);
    }
    else if (rotationDirection == 3) {
        m = getRotationMatrix(10.0, 0.0, 0.0, 1.0);
    }
    else if (rotationDirection == -3) {
        m = getRotationMatrix(-10.0, 0.0, 0.0, 1.0);
    }
    
    
    for (int i = 0; i < faceNumToRotate; i++) {
        for (int j = 0; j < verticesOnCube; j++) {
            vertices[elementsToRotate[i]][j] = m * vertices[elementsToRotate[i]][j];
        }
    }
}


void rotateVertices(int d, int s) {
    rotationDirection = d;
    for (int i = 0; i < numCube; i++) {
        if (indexList[i][s] == indexList[rotatableSection][s]) {
            elementsToRotate.push_back(i);
        }
    }
    
    assignRotatedVertices();
    Index = 0;
    for (int i = 0; i < numCube; i++) {
        generateCubes(i);
    }
    
}

void rotatedIndices(int a, int b) {
    for (int i = 0; i < faceNumToRotate; i++) {
        int e = elementsToRotate[i];
        int stateA = indexList[e][a];
        int stateB = indexList[e][b];

        switch (stateA * 3 + stateB) {
            case 0: indexList[e][b] = 2; break;
            case 1: indexList[e][a] = 1; indexList[e][b] = 2; break;
            case 2: indexList[e][a] = 2; break;
            case 3: indexList[e][a] = 0; indexList[e][b] = 1; break;
            case 4: indexList[e][a] = 1; break;
            case 5: indexList[e][a] = 2; indexList[e][b] = 1; break;
            case 6: indexList[e][a] = 0; break;
            case 7: indexList[e][a] = 1; indexList[e][b] = 0; break;
            case 8: indexList[e][b] = 0; break;
            default: break;
        }
    }
}

void rotate(int a, int b, int c, int d) {
    for (int i = 0; i < faceNumToRotate; i++) {
        rotateVertices(a, b);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    }
    rotatedIndices(c, d);
    elementsToRotate.clear();
}

void randomizeCube() {
    int randCube[10];
    int randAxis[10];
    srand(time(0)); // seed the random number generator with the current time
    // generate 10 random integer values between 0-27 and put them in the array
    for (int i = 0; i < 10; i++) {
        randCube[i] = rand() % 27; // generate a random number between 0-27
        randAxis[i] = rand() % 3;
    }

    for (int i = 0; i < 10; i++) {
        rotatableSection = randCube[i];
        if (randAxis[i] == 0) {
            rotate(3, 2, 1, 0);
        }
        else if ((randAxis[i] == 1)) {
            rotate(2, 1, 0, 2);
        }
        else if ((randAxis[i] == 2)) {
            rotate(1, 0, 1, 2);
        }
        elementsToRotate.clear();
        rotatableSection = -1;
    }
}

void generateVertices() {
    for (int i = 0; i < loopNum; i++) {
        for (int j = 0; j < loopNum; j++) {
            for (int k = 0; k < loopNum; k++) {
                int index = i + loopNum * j + loopNum * loopNum * k;
                float x = i % loopNum;
                float y = j % loopNum;
                float z = k % loopNum;
                vertices[index][0] = point4(-1.5 + x, -1.5 + y, -0.5 + z, 1.0);
                vertices[index][1] = point4(-1.5 + x, -0.5 + y, -0.5 + z, 1.0);
                vertices[index][2] = point4(-0.5 + x, -0.5 + y, -0.5 + z, 1.0);
                vertices[index][3] = point4(-0.5 + x, -1.5 + y, -0.5 + z, 1.0);
                vertices[index][4] = point4(-1.5 + x, -1.5 + y, -1.5 + z, 1.0);
                vertices[index][5] = point4(-1.5 + x, -0.5 + y, -1.5 + z, 1.0);
                vertices[index][6] = point4(-0.5 + x, -0.5 + y, -1.5 + z, 1.0);
                vertices[index][7] = point4(-0.5 + x, -1.5 + y, -1.5 + z, 1.0);

            }
        }
    }
}
//----------------------------------------------------------------------------
// init
void
init()
{
    generateVertices();
    for (int i = 0; i < numCube; i++) {
        generateCubes(i);
    }
    randomizeCube();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); 
    
    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // set up color array
    vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));


    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation(program, "ModelView");
    Projection = glGetUniformLocation(program, "Projection");

    // Set projection matrix
    mat4  projection;
    projection = Ortho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0); // Ortho(): user-defined function in mat.h
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}



//---------------------------------------------------------------------
//
// display
//
void
display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  Generate tha model-view matrix
    const vec3 displacement(0.0, 0.0, 0.0);
    mat4 model_view = (Translate(displacement) * Scale(1.0, 1.0, 1.0) *
                        RotateX(Theta[Xaxis] + aspectX) *
                        RotateY(Theta[Yaxis]) *
                        RotateZ(Theta[Zaxis] + aspectZ) * Translate(displacement));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glFlush();
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{      
    int a = -1;
    int b = -1;
    int c = -1;
    int d = -1;
    int rotated = false;
     if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) { 
            exit(EXIT_SUCCESS);
        }
        else if (key == GLFW_KEY_R) { 
            randomizeCube();
        }
        else if (key == GLFW_KEY_Y) {
            printf("\nQ -- quit(exit) the program\nR -- randomizes the cube by randomly turning different sections\narrow-up -- turns the whole cube towards north\narrow-down -- turns the whole cube towards south\narrow-right -- turns the whole cube towards east\narrow-left -- turns the whole cube towards west\nA -- turns most-back cubes counter-clockwise in z-axis\nS -- turns middle cubes counter-clockwise in z-axis\nD -- turns most-front cubes counter-clockwise in z-axis\nF -- turns most-back cubes counter-clockwise in x-axis\nG -- turns middle cubes counter-clockwise in x-axis\nH -- turns most-front cubes counter-clockwise in x-axis\nJ -- turns most-back cubes counter-clockwise in y-axis\nK -- turns middle cubes counter-clockwise in y-axis\nL -- turns most-front cubes counter-clockwise in y-axis\nY -- help;print explanation of our input control\n");
        }
        else if (key == GLFW_KEY_A) {
            rotatableSection = 0;
            a = 3;
            b = 2;
            c = 1; 
            d = 0;
            rotated = true;
        }
        else if (key == GLFW_KEY_S) {
            rotatableSection = 9;
            a = 3;
            b = 2;
            c = 1;
            d = 0;
            rotated = true;
        }
        else if (key == GLFW_KEY_D) {
            rotatableSection = 18;
            a = 3;
            b = 2;
            c = 1;
            d = 0;
            rotated = true;
        }
        else if (key == GLFW_KEY_F) {
            rotatableSection = 0;
            a = 2;
            b = 1;
            c = 0;
            d = 2;
            rotated = true;
        }
        else if (key == GLFW_KEY_G) {
            rotatableSection = 4;
            a = 2;
            b = 1;
            c = 0;
            d = 2;
            rotated = true;
        }
        else if (key == GLFW_KEY_H) {
            rotatableSection = 8;
            a = 2;
            b = 1;
            c = 0;
            d = 2;
            rotated = true;
        }
        else if (key == GLFW_KEY_J) {
            rotatableSection = 0;
            a = 1;
            b = 0;
            c = 1;
            d = 2;
            rotated = true;
        }
        else if (key == GLFW_KEY_K) {
            rotatableSection = 1;
            a = 1;
            b = 0;
            c = 1;
            d = 2;
            rotated = true;
        }
        else if (key == GLFW_KEY_L) {
            rotatableSection = 2;
            a = 1;
            b = 0;
            c = 1;
            d = 2;
            rotated = true;
            
        }
        if (rotated) {
            rotate(a, b, c, d);
            rotated = false;
        }
     }
     if (key == GLFW_KEY_LEFT) { 
         aspectZ += 5;
     }
     else if (key == GLFW_KEY_RIGHT) {
         aspectZ -= 5;
     }
     else if (key == GLFW_KEY_UP) {
         aspectX += 5;
     }
     else if (key == GLFW_KEY_DOWN) {
         aspectX -= 5;
     }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    // Set the projection matrix
    mat4 projection;

    if (width <= height)
        projection = Ortho(-5.0, 5.0, -5.0 * ((GLfloat)height / width), 5.0 * ((GLfloat)height / width), -5.0, 5.0);
    else
        projection = Ortho(-5.0 * ((GLfloat)width / height), 5.0 * ((GLfloat)width / height), -5.0, 5.0, -5.0, 5.0);

    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

}


int
main(int argc, char** argv)
{
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    init();

    double frameRate = 120, currentTime, previousTime = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        if (currentTime - previousTime >= 1 / frameRate) {
            previousTime = currentTime;
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