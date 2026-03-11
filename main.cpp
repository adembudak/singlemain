#include <GL/glew.h>
#include <GL/gl.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cassert>
#include <iostream>

void GLAPIENTRY GLErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
  std::cout << message << '\n';
}

void glfw_error_callback(int error, const char* description) {
  std::cout << description << '\n';
}

struct vec2 {
  float x, y;
  static constexpr std::size_t count = 2;
};

static_assert(sizeof(vec2) == 2 * sizeof(float));

int main(int argc, const char* argv[]) {
  if(int ret = glfwInit(); ret != GLFW_TRUE)
    return -1;

  glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "pot", NULL, NULL);
  if(window == nullptr) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if(GLenum err = glewInit(); err != GLEW_OK)
    return -1;

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(GLErrorMessageCallback, nullptr);

  glfwSetErrorCallback(glfw_error_callback);

  /*
    glfwSetWindowSizeCallback(window, glfw_onResize);
    glfwSetKeyCallback(window, glfw_onKey);
    glfwSetMouseButtonCallback(window, glfw_onMouseButton);
    glfwSetCursorPosCallback(window, glfw_onMouseMove);
    glfwSetScrollCallback(window, glfw_onMouseWheel);
  */

  const char* vertex_shader = R"(
#version 460 core

in vec2 in_vertexPosition;
in vec2 in_textureCoordinate;


out vec2 textureCoordinate;

void main() {
  textureCoordinate = in_textureCoordinate;
  gl_Position = vec4(in_vertexPosition, 0.0, 1.0);
}

)";

  const char* fragment_shader = R"(
#version 460 core

uniform sampler2D sampler;

in vec2 textureCoordinate;
out vec4 fragmentColor;

void main() {
  fragmentColor = texture(sampler, textureCoordinate);
}

)";

  GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderID, 1, &vertex_shader, nullptr);
  glCompileShader(vertexShaderID);

  GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderID, 1, &fragment_shader, nullptr);
  glCompileShader(fragmentShaderID);

  GLuint programID = glCreateProgram();

  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);

  glLinkProgram(programID);

  glDetachShader(programID, vertexShaderID);
  glDetachShader(programID, fragmentShaderID);

  glUseProgram(programID);

  GLuint vertexAttributeArrayID;
  glCreateVertexArrays(1, &vertexAttributeArrayID);
  glBindVertexArray(vertexAttributeArrayID);

  GLuint vertexPositionArrayID;
  glCreateBuffers(1, &vertexPositionArrayID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexPositionArrayID);

  // clang-format off
  const vec2 positionData[] = { {-1.0f, -1.0f}, {0.0f,  1.0f }, {1.0f,  -1.0f} };
  // clang-format on

  GLint vertexAttributePositionLocation = glGetAttribLocation(programID, "in_vertexPosition");
  glNamedBufferStorage(vertexPositionArrayID, std::size(positionData) * sizeof(decltype(positionData[0])), positionData, GL_MAP_READ_BIT);
  glVertexArrayVertexBuffer(vertexAttributeArrayID, vertexAttributePositionLocation, vertexPositionArrayID, 0, sizeof(vec2));
  glVertexArrayAttribFormat(vertexAttributeArrayID, vertexAttributePositionLocation, vec2::count, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(vertexAttributeArrayID, vertexAttributePositionLocation, vertexAttributePositionLocation);
  glEnableVertexArrayAttrib(vertexAttributeArrayID, vertexAttributePositionLocation);

  GLuint textureID;
  glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  int width, height, nComponents;
  stbi_uc* image_data = stbi_load("resources/textures/wood.jpg", &width, &height, &nComponents, 0);
  assert(image_data != nullptr);

  glTextureStorage2D(textureID, 1, GL_SRGB8, width, height);
  glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image_data);
  glBindTextureUnit(glGetUniformLocation(programID, "sampler"), textureID);

  GLuint textureCoordinatesID;
  glCreateBuffers(1, &textureCoordinatesID);
  glBindBuffer(GL_ARRAY_BUFFER, textureCoordinatesID);
  // clang-format off
  const vec2 textureUVData[3]{ vec2{0.0, 0.0}, vec2{0.5, 1.0}, vec2{1.0, 0.0} };
  // clang-format on

  GLint textureCoordinateLocation = glGetAttribLocation(programID, "in_textureCoordinate");
  glNamedBufferStorage(textureCoordinatesID, std::size(textureUVData) * sizeof(decltype(textureUVData[0])), std::data(textureUVData), GL_MAP_READ_BIT);
  glVertexArrayVertexBuffer(vertexAttributeArrayID, textureCoordinateLocation, textureCoordinatesID, 0, sizeof(vec2));
  glVertexArrayAttribFormat(vertexAttributeArrayID, textureCoordinateLocation, vec2::count, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(vertexAttributeArrayID, textureCoordinateLocation, textureCoordinateLocation);

  glEnableVertexArrayAttrib(vertexAttributeArrayID, textureCoordinateLocation);

  assert(glGetError() == GL_NO_ERROR);

  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glDeleteTextures(1, &textureID);
  glDeleteBuffers(1, &textureCoordinatesID);
  glDeleteBuffers(1, &vertexPositionArrayID);
  glDeleteVertexArrays(1, &vertexAttributeArrayID);
  glDeleteShader(fragmentShaderID);
  glDeleteShader(vertexShaderID);
  glDeleteProgram(programID);

  glfwTerminate();
  return 0;
}
