// Authors:
// Betül Demirtaþ 68858
// Doða Ege Ýnhanlý 69033

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
int currentColor = 1;
int currentObject = 0;
int isSpinned = 1;

//Constants for cube
const int NumVerticesCube = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
color4 colors[8];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 points[8] = {
    point4(-0.5, -0.5,  0.5, 1.0),
    point4(-0.5,  0.5,  0.5, 1.0),
    point4(0.5,  0.5,  0.5, 1.0),
    point4(0.5, -0.5,  0.5, 1.0),
    point4(-0.5, -0.5, -0.5, 1.0),
    point4(-0.5,  0.5, -0.5, 1.0),
    point4(0.5,  0.5, -0.5, 1.0),
    point4(0.5, -0.5, -0.5, 1.0)
};

//Chosen color RGBA codes
color4 vertex_colors[2] = {
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};


//Constants for sphere
const int NumTimesToSubdivide = 5;
const int NumTriangles = 4096;
// (4 faces)^(NumTimesToSubdivide + 1)
const int NumVerticesSphere = 3 * NumTriangles;
point4 pointsSphere[NumVerticesSphere];
vec3 normals[NumVerticesSphere];
int Index = 0;

//Constants and values for bunny
// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 90.0, 180.0, -90.0 };
const int NumVerticesBunny = 9840 * 3;
point4 pointsBunny[NumVerticesBunny];

// Global arrays for controlling object specifications
int numVertices[3] = { NumVerticesCube, NumVerticesSphere, NumVerticesBunny*3 };
float scales[3] = { 0.4,0.4,0.04 };
float heightCorrections[3] = { 0, 0.25 * sqrt(2.0), 0 };

// Model-view, projection and color matrices uniform location
GLuint ModelView, Projection, Color;
GLuint vao[3];

// Global variables for controlling window reshaping
float widthStart = -0.1;
float heightStart = 0.1;
float widthEnd = 7.0;
float heightEnd = -7.0;
vec3  Displacement(widthStart + 0.2f + 0.2f * currentObject, heightStart - 0.2f - 0.2f * currentObject, 0.0);

//--------------------------------------------------
// Retrieved from Appendix Example 7 from (E. Angel)
void triangle( const point4& a, const point4& b, const point4& c ) {
    vec3 normal = normalize( cross(b - a, c - b) );
    normals[Index] = normal; pointsSphere[Index] = a; Index++;
    normals[Index] = normal; pointsSphere[Index] = b; Index++;
    normals[Index] = normal; pointsSphere[Index] = c; Index++;
}

//--------------------------------------------------
// Retrieved from Appendix Example 7 from (E. Angel)
point4 unit( const point4& p ) {
    float len = p.x*p.x + p.y*p.y + p.z*p.z;
    point4 t;
    if ( len > DivideByZeroTolerance ) {
        t = p / sqrt(len);
        t.w = 1.0;
    }
    return t;
}

//--------------------------------------------------
// Retrieved from Appendix Example 7 from (E. Angel)
void divide_triangle( const point4& a, const point4& b, const point4& c, int count )
{
    if ( count > 0 ) {
        point4 v1 = unit( a + b );
        point4 v2 = unit( a + c );
        point4 v3 = unit( b + c );
        divide_triangle( a, v1, v2, count - 1 );
        divide_triangle( c, v2, v3, count - 1 );
        divide_triangle( b, v3, v1, count - 1 );
        divide_triangle( v1, v3, v2, count - 1 );
    }
    else {
        triangle( a, b, c );
    }
}

//--------------------------------------------------
// Retrieved from Appendix Example 7 from (E. Angel)
void tetrahedron( int count ) {
    point4 v[4] = {
        vec4( 0.0, 0.0, 1.0, 1.0 ),
        vec4( 0.0, 0.942809, -0.33333, 1.0 ),
        vec4( -0.816497, -0.471405, -0.33333, 1.0 ),
        vec4( 0.816497, -0.471405, -0.33333, 1.0 )
    };
    divide_triangle( v[0], v[1], v[2], count );
    divide_triangle( v[3], v[2], v[1], count );
    divide_triangle( v[0], v[3], v[1], count );
    divide_triangle( v[0], v[2], v[3], count );
}


//----------------------------------------------------------
// Retrieved from spinCube_withReshape example from lectures
void drawCube() {
    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

    glBindVertexArray(vao[0]);

    GLuint cube_indices[] = {
           0, 1, 2,
           2, 3, 0,
           1, 5, 6,
           6, 2, 1,
           7, 6, 5,
           5, 4, 7,
           4, 0, 3,
           3, 7, 4,
           4, 5, 1,
           1, 0, 4,
           3, 2, 6,
           6, 7, 3
    };

    // Create and initialize an index buffer object
    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
}

void drawSphere() {
    // Subdivide a tetrahedron into a sphere
    tetrahedron(NumTimesToSubdivide);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointsSphere) + sizeof(normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointsSphere), pointsSphere);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsSphere), sizeof(normals), normals);

    glBindVertexArray(vao[1]);
}

void drawBunny() {
    GLuint numTriangles = 0, numVertices = 0;
    point4 verticesBunny[4922];
    
    std::ifstream bunnyFile("bunny.off");  // opens bunny.off file to read
    std::string temp;
    std::getline(bunnyFile, temp); //title line
    bunnyFile >> numVertices >> numTriangles >> temp; //info line for number of vertices, triangles and edges (ignored)
    std::getline(bunnyFile, temp); //next line

    // For each line representing vertice coordinates, it assigns the values to the verticesBunny array we created
    for (int i = 0; i < numVertices; i++) {
        bunnyFile >> verticesBunny[i].x >> verticesBunny[i].y >> verticesBunny[i].z;
        verticesBunny[i].w = 1.0;
    }
    // For each line representing triangle coordinates, it assigns the values to the pointsBunny array we created 
    int indices[3];
    for (int i = 0; i < numTriangles; i++) {
        bunnyFile >> temp; // ignored
        for (int j = 0; j < 3; j++) {
            bunnyFile >> indices[j];
            pointsBunny[3*i+j] = verticesBunny[indices[j]];
        }
    }
    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointsBunny) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointsBunny), pointsBunny);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsBunny), sizeof(colors), colors);

    glBindVertexArray(vao[2]);

}

void init() {
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );

    // Set up vertex arrays
    GLuint vPosition = glGetAttribLocation(program, "vPosition");

    // Generate vertex arrays for objects
    glGenVertexArrays(3, vao);

    // Initializes cube
    drawCube();
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    // Initializes sphere
    drawSphere();
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    
    // Initializes bunny
    drawBunny();
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    Color = glGetUniformLocation( program, "Color" );
    
    // Set projection matrix
    mat4  projection;
    projection = Ortho(widthStart, widthEnd, heightEnd, heightStart, -1.0, 1.0); // Ortho(): user-defined function in mat.h
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
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
    Displacement.x += deltaTime; // moves with a constant velocity in the x coordinate
    velocity += acceleration * deltaTime; // velocity changes in time due to gravity (acceleration)
    Displacement.y -= velocity * deltaTime; // moves with a changing velocity in the y coordinate
    // controls the bouncing behaviour of the object by checking the current location in the y coordinates and the direction of its velocity
    if (Displacement.y - heightCorrections[currentObject] <= heightEnd && velocity > 0) {
        velocity *= -energyLoss; // the loss energy when the object hits the ground
    }
    spin();
}

//------------------------------------------------------------
// Retrieved from spinCube_withReshape example from lectures
void display( void ) {
    glBindVertexArray(vao[currentObject]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  Generate the model-view matrix
    mat4  model_view = (Translate( Displacement )  * Scale(scales[currentObject], scales[currentObject], scales[currentObject]) //scales according to the object type specifications 
        * RotateX(Theta[Xaxis]) 
        * RotateY(Theta[Yaxis]) 
        * RotateZ(Theta[Zaxis])); // Scale(), Translate(), RotateX(), RotateY(), RotateZ(): user-defined functions in mat.h

    glUniform4fv( Color, 1, vertex_colors[currentColor] );
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
    glDrawArrays( GL_TRIANGLES, 0, numVertices[currentObject]);
    glDrawElements(GL_TRIANGLES, numVertices[currentObject], GL_UNSIGNED_INT, 0);
    glFlush();
}

//---------------------------------------------------------------------------
// drawingMode is a global variable keeping the current drawing mode selected
// By using glPolygonMode function, it draws the object as lines or filled
void changeDrawingStyle() {
    if (drawingMode == -1) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

//-----------------------------------------------------------
// Takes the mouse cursor positions as input and adjusts 
// the window pixel values according to the projection matrix
void changePosition(double xpos, double ypos) {
    Displacement.x = (xpos*7.1) / 1024 + 0.1;
    Displacement.y = -(ypos*7.1) / 1024 - 0.1;
    velocity = 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) { 
        if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) { // exit
            exit(EXIT_SUCCESS);
        }
        else if (key == GLFW_KEY_C) { // change color
            currentColor += 1;
            currentColor = currentColor % 2;
        }
        else if (key == GLFW_KEY_H) { // get help
            printf("Right-click on mouse -- changes the object type (cube/sphere/bunny)\nLeft-click on mouse -- changes the drawing mode(lines/filled)\nMiddle-click on mouse -- relocates the object on the mouse cursor position\ni -- initializes the pose(top left corner of the window)\nc -- switches between two colors(cyan/red), which is used to draw lines or triangles.\nspace -- makes the object spinning\nup -- enlarges the object\ndown -- shrinks the object\nh -- help; print explanation of our input control\nq -- quit(exit) the program\n");
        }
        else if (key == GLFW_KEY_I) { // relocate the object at the initial position
            Displacement.x = widthStart + 0.2f + 0.2f*currentObject;
            Displacement.y = heightStart - 0.2f - 0.2f*currentObject;
            velocity = 0;
        } 
        else if (key == GLFW_KEY_SPACE) { // spin the object
            isSpinned *= -1;
        }
        else if (key == GLFW_KEY_UP) { // enlarges the object
            scales[currentObject] *= 1.1;
        }
        else if (key == GLFW_KEY_DOWN) { // shrinks the object
            scales[currentObject] /= 1.1;
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_RIGHT:    currentObject += 1; currentObject = currentObject % 3;  break; // changes the current object into the next element in the vao
        case GLFW_MOUSE_BUTTON_MIDDLE: double xpos, ypos; glfwGetCursorPos(window, &xpos, &ypos); changePosition(xpos, ypos); break; // relocates the object according to the mouse cursor position when clicked
        case GLFW_MOUSE_BUTTON_LEFT:   drawingMode *= -1; changeDrawingStyle(); break; // changes the drawing style
        }
    }
}

//----------------------------------------------------------
// Retrieved from spinCube_withReshape example from lectures and
// updated the projection according to the Ortho projection we specified in init function
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); //may not need this since the default is usually the window size
    // Set projection matrix
    mat4  projection;
    if (width <= height) {
        heightStart = 0.1 * (GLfloat)height / (GLfloat)width;
        heightEnd = -7.0 * (GLfloat)height / (GLfloat)width;
        projection = Ortho(-0.1, 7.0, heightEnd, heightStart, -1.0, 1.0);
    }
    else {
        widthStart = -0.1 * (GLfloat)width / (GLfloat)height;
        widthEnd = 7.0 * (GLfloat)width / (GLfloat)height;
        projection = Ortho(widthStart, widthEnd, -7.0, 0.1, -1.0, 1.0);
    }
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

}

//----------------------------------------------------------
// Retrieved from spinCube_withReshape example from lectures
int main( int argc, char **argv ) {

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

