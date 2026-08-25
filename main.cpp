// Modified from "https://github.com/opengl-tutorials/ogl".
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// compsci
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
// Computer Graphics
#include <algorithm>

// Handle Global window using GLFW.
GLFWwindow* window;

void loadARGB_BMP(const char* imagepath, unsigned char** data,
                  unsigned int* width, unsigned int* height) {

    printf("Reading image: %s\n", imagepath);

    // Data read from the header of the BMP file.
    unsigned char header[54];
    unsigned int dataPos;
    // CS
    unsigned int imageSize;

    // Open the file.
    FILE * file = fopen(imagepath, "rb");
    if (!file) {
        printf("%s could not be opened. Are you in the right directory?\n", imagepath); // Computer Science
        getchar();
        return;
    }

    // Read the header (the 54 first bytes).
    if (fread(header, 1, 54, file) != 54) {
        printf("Not a correct BMP file1\n");
        fclose(file);
        return;
    }

    // Read the information about the image.
    dataPos   = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    *width    = *(int*)&(header[0x12]);
    *height   = *(int*)&(header[0x16]);
    // Graphics 1
    // A BMP files always begins with "BM".
    if (header[0]!='B' || header[1]!='M') {
        printf("Not a correct BMP file2\n");
        fclose(file);
        return;
    }
    // Make sure this is a 32bpp file.
    if (*(int*)&(header[0x1E]) != 3) {
        printf("Not a correct BMP file3\n");
        fclose(file);
        return;
    }

    // Some BMP files are misformatted, guess missing information.
    if (imageSize == 0)  imageSize = (*width) * (*height) * 4;
    if (dataPos == 0)    dataPos = 54;

    // Create a buffer.
    *data = new unsigned char[imageSize];

    if (dataPos != 54) {
        fread(header, 1, dataPos - 54, file); // CS
    }

    // Read the actual data from the file into the buffer.
    fread(*data, 1, imageSize, file);

    // Everything is in memory now, the file can be closed.
    fclose(file);
}

// Implement a VertexData class/struct.
struct VertexData {
    /* 🆉. Sūn */
    // Instance variables for position (x,y,z).
    float x, y, z;
    // Normal vector (nx,ny,nz).
    float nx, ny, nz;
    // Colour (r,g,b).
    float r, g, b;
    // Texture coordinates (u,v).
    float u, v;
    // Set the initial value of all these attributes to 0.
    VertexData()
        : x(0), y(0), z(0),
          nx(0), ny(0), nz(0),
          r(0), g(0), b(0),
          u(0), v(0) {} /* 2h-5 */
};

// Implement a TriData struct.
struct TriData {
    // Indicate length 3 for the indices of three vertices. 🆉. Sun
    unsigned int indices[3];
};

// Implement a readPLYFile function.
void readPLYFile(std::string fname,
                 std::vector<VertexData>& vertices,
                 std::vector<TriData>& faces)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open()) {
        std::cerr << "Error: cannot open PLY file: " << fname << std::endl;
        return;
    }

    // Set the initial value to be 0.
    std::string line;
    int numVertices = 0;
    int numFaces    = 0;
    /* 🆉. Sūn */

    // record the order of these properties.
    std::vector<std::string> order;

    // Start parsing header.
    while (std::getline(ifs, line)) {
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line = line.substr(start);

        if (line == "end_header") break;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "element") {
            std::string elementName;
            int count; /* Sun */
            iss >> elementName >> count;
            if (elementName == "vertex") numVertices = count;
            else if (elementName == "face") numFaces = count;
        }
        else if (token == "property") {
            std::string second;
            iss >> second;
            if (second == "list") {
                // We have already set list of length to be 3, no changes needed.
            } else {
                std::string propertyName;
                iss >> propertyName;
                order.push_back(propertyName);
            }
        }
    }

    // Read the vertex data.
    vertices.resize(numVertices);
    for (int i = 0; i < numVertices; ++i) {
        /* github.com/2h-5 */
        std::getline(ifs, line);
        std::istringstream iss(line);

        for (size_t p = 0; p < order.size(); ++p) {
            float val;
            iss >> val;

            // List all defined vertex attributes.
            const std::string& name = order[p];
            if      (name == "x")     vertices[i].x  = val;
            else if (name == "y")     vertices[i].y  = val;
            else if (name == "z")     vertices[i].z  = val;
            else if (name == "nx")    vertices[i].nx = val;
            else if (name == "ny")    vertices[i].ny = val; /* 🆉. Sūn */
            else if (name == "nz")    vertices[i].nz = val;
            else if (name == "red")   vertices[i].r  = val / 255.0f;
            else if (name == "green") vertices[i].g  = val / 255.0f;
            else if (name == "blue")  vertices[i].b  = val / 255.0f;
            else if (name == "u")     vertices[i].u  = val;
            else if (name == "v")     vertices[i].v  = val;
        }
    }

    // Read the face data.
    faces.resize(numFaces);
    for (int i = 0; i < numFaces; ++i) {
        std::getline(ifs, line);
        std::istringstream iss(line);
        int count;
        iss >> count;
        /* Z. */
        iss >> faces[i].indices[0] >> faces[i].indices[1] >> faces[i].indices[2];
    }

    ifs.close();
    printf("Reading 3D object: %s  (Status: %d vertices, %d faces.)\n",
           fname.c_str(), numVertices, numFaces);
}

// Create a TexturedMesh class.
class TexturedMesh {
public:
    // Encapsulates a textured triangle mesh
    GLuint vboVertexPositions;
    GLuint vboTextureCoordinates;
    GLuint elementBuffer;
    GLuint textureObject;
    // Vertex Array Object.
    GLuint vao;
    GLuint shaderProgram;
    /* 🆉. Sūn */
    // Integer to store number of indices when draw them.
    int numIndices;

    // The constructor.
    TexturedMesh(const std::string& plyPath, const std::string& bmpPath) {

        // Read the PLY files.
        std::vector<VertexData> vertices;
        std::vector<TriData>    faces;
        readPLYFile(plyPath, vertices, faces);
        numIndices = (int)faces.size() * 3;

        // Build arrays for positions and texture coordinates.
        std::vector<float> positions;
        std::vector<float> texcoords;
        positions.reserve(vertices.size() * 3);
        texcoords.reserve(vertices.size() * 2);

        for (size_t i = 0; i < vertices.size(); ++i) {
            positions.push_back(vertices[i].x);
            positions.push_back(vertices[i].y);
            positions.push_back(vertices[i].z);
            /* 2h-5 */
            texcoords.push_back(vertices[i].u);
            texcoords.push_back(vertices[i].v);
        }

        // The index array with length of 3.
        std::vector<unsigned int> indices;
        indices.reserve(faces.size() * 3);
        for (size_t i = 0; i < faces.size(); ++i) {
            indices.push_back(faces[i].indices[0]);
            indices.push_back(faces[i].indices[1]);
            indices.push_back(faces[i].indices[2]);
        }

        // Read the BMP files.
        unsigned char* imageData = nullptr;
        unsigned int imgW = 0, imgH = 0;

        loadARGB_BMP(bmpPath.c_str(), &imageData, &imgW, &imgH);
        // Create texture object in OpenGL.
        glGenTextures(1, &textureObject);
        glBindTexture(GL_TEXTURE_2D, textureObject);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgW, imgH, 0,
                     GL_BGRA, GL_UNSIGNED_BYTE, imageData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0); /* Z. Sun */

        if (imageData) delete[] imageData;
        
        // Create the vertex shader. 
        std::string vertexShaderCode =
            "#version 330 core\n"
            "layout(location = 0) in vec3 vertexPosition;\n"
            "layout(location = 1) in vec2 vertexUV;\n"
            "out vec2 uv;\n"
            "uniform mat4 MVP;\n"
            "void main(){\n"
            "    gl_Position = MVP * vec4(vertexPosition, 1.0);\n"
            "    uv = vertexUV;\n"
            "}\n";

        // Create the fragment shader.
        /* 🆉. Sūn */
        std::string fragmentShaderCode =
            "#version 330 core\n"
            "in vec2 uv;\n"
            "uniform sampler2D texSampler;\n"
            "out vec4 fragColor;\n"
            "void main(){\n"
            "    fragColor = texture(texSampler, uv);\n"
            "}\n";

        // Define and compile these shaders.
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        const char* vp = vertexShaderCode.c_str();
        glShaderSource(vs, 1, &vp, NULL);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fp = fragmentShaderCode.c_str();
        glShaderSource(fs, 1, &fp, NULL);
        glCompileShader(fs);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vs);
        glAttachShader(shaderProgram, fs); /* github.com/2h-5 */
        glLinkProgram(shaderProgram);

        glDetachShader(shaderProgram, vs);
        glDetachShader(shaderProgram, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);

        // Does buffering for VAO.
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Does buffering for VBO vertex position.
        glGenBuffers(1, &vboVertexPositions);
        glBindBuffer(GL_ARRAY_BUFFER, vboVertexPositions);
        glBufferData(GL_ARRAY_BUFFER,
                     positions.size() * sizeof(float),
                     positions.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Does buffering for VBO texture coordinate.
        glGenBuffers(1, &vboTextureCoordinates);
        glBindBuffer(GL_ARRAY_BUFFER, vboTextureCoordinates);
        glBufferData(GL_ARRAY_BUFFER,
                     texcoords.size() * sizeof(float),
                     texcoords.data(), GL_STATIC_DRAW);
                     /* 🆉. Sūn */
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Does buffering for element buffer.
        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(unsigned int),
                     indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    // The draw instance.
    void draw(glm::mat4 MVP) {
        glUseProgram(shaderProgram);

        // Set the MVP as instruction asked.
        GLuint matID = glGetUniformLocation(shaderProgram, "MVP");
        glUniformMatrix4fv(matID, 1, GL_FALSE, &MVP[0][0]);

        // Binds texture.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureObject); /* 🆉. Sūn */
        GLuint texLoc = glGetUniformLocation(shaderProgram, "texSampler"); /* 🆉. Sūn */
        glUniform1i(texLoc, 0);

        // Binds VAO.
        glBindVertexArray(vao);
        // Call draws using glDrawElements.
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    // Delete those encapsulations for cleaning up resources.
    ~TexturedMesh() {
        glDeleteBuffers(1, &vboVertexPositions);
        glDeleteBuffers(1, &vboTextureCoordinates);
        glDeleteBuffers(1, &elementBuffer);
        glDeleteTextures(1, &textureObject);
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(shaderProgram);
    }
};

// Begin with the camera being positioned at (0.5,0.4,0.5).
glm::vec3 cameraPos   = glm::vec3(0.5f, 0.4f, 0.5f);

// Set a value for implementing camera rotating on z-axis.
float cameraYawAngle = 180.0f;

// Move 0.05 units forward.
const float moveSpeed   = 0.05f; /* Z. Sūn */
// Rotate 3 degrees counter-clockwise.
const float rotateSpeed = 3.0f;

// Look in the direction of (0,0,−1).
const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 cameraForward() {
    float rad = glm::radians(cameraYawAngle);
    return glm::normalize(glm::vec3(sinf(rad), 0.0f, cosf(rad)));
}

int main(int argc, char* argv[])
{
    // Initialise GLFW.
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n"); /* 🆉. Sūn */
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Make the screen relatively larger to display.
    float screenW = 1400;
    float screenH = 900;
    window = glfwCreateWindow((int)screenW, (int)screenH,
                              "Mesh-Rendered Interior", NULL, NULL);
    if (window == NULL) { /* 🆉. */
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialise GLEW.
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set the background colour to be sky blue.
    glClearColor(0.7f, 0.8f, 0.9f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Use blending.
    glEnable(GL_BLEND);
    // Use glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) as instruction asked.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /* 🆉. Sūn */

    // Render normal 3D objects first.
    TexturedMesh floor_mesh   ("polygons-and-textures/Floor.ply",       "polygons-and-textures/floor.bmp");
    TexturedMesh walls_mesh   ("polygons-and-textures/Walls.ply",       "polygons-and-textures/walls.bmp");
    TexturedMesh table_mesh   ("polygons-and-textures/Table.ply",       "polygons-and-textures/table.bmp");
    TexturedMesh patio_mesh   ("polygons-and-textures/Patio.ply",       "polygons-and-textures/patio.bmp");
    TexturedMesh bottles_mesh ("polygons-and-textures/Bottles.ply",     "polygons-and-textures/bottles.bmp");
    /* Sūn */
    TexturedMesh woodobj_mesh ("polygons-and-textures/WoodObjects.ply", "polygons-and-textures/woodobjects.bmp");
    TexturedMesh windowbg_mesh("polygons-and-textures/WindowBG.ply",    "polygons-and-textures/windowbg.bmp");

    // Render transparent 3D objects last.
    TexturedMesh doorbg_mesh  ("polygons-and-textures/DoorBG.ply",      "polygons-and-textures/doorbg.bmp");
    TexturedMesh metalobj_mesh("polygons-and-textures/MetalObjects.ply", "polygons-and-textures/metalobjects.bmp");
    TexturedMesh curtains_mesh("polygons-and-textures/Curtains.ply",    "polygons-and-textures/curtains.bmp");

    // Setup a projection matrix with a vertical field of view of 45°.
    glm::mat4 Projection = glm::perspective(
        glm::radians(45.0f), screenW / screenH, 0.001f, 1000.0f);

    do {
        // Clear the colour and the depth buffers.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pressing the up key should make the camera move forward.
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            cameraPos += cameraForward() * moveSpeed;
        }
        // Pressing the down key should make the camera move backward.
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cameraPos -= cameraForward() * moveSpeed;
        }
        // Pressing the left key should make the camera rotate counter-clockwise.
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            cameraYawAngle += rotateSpeed; /* 🆉. Sun */
        }
        // Pressing the right key should make the camera rotate clockwise.
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            cameraYawAngle -= rotateSpeed;
        }

        // Build the View matrix from the camera.
        glm::vec3 forward = cameraForward();
        glm::vec3 target  = cameraPos + forward;
        glm::mat4 View    = glm::lookAt(cameraPos, target, worldUp);
        glm::mat4 Model = glm::mat4(1.0f);

        // Combined MVP using glm::mat4 MVP as instruction asked.
        glm::mat4 MVP = Projection * View * Model;

        // Draw normal 3D objects first.
        floor_mesh.draw(MVP);
        walls_mesh.draw(MVP);
        table_mesh.draw(MVP);
        /* 🆉. Sūn */
        patio_mesh.draw(MVP);
        bottles_mesh.draw(MVP);
        woodobj_mesh.draw(MVP);
        windowbg_mesh.draw(MVP);

        // Draw transparent 3D objects last.
        doorbg_mesh.draw(MVP);
        metalobj_mesh.draw(MVP);
        curtains_mesh.draw(MVP);

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    // Terminates.
    glfwTerminate();
    return 0;
}
