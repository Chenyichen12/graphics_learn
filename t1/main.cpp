#include "glm/ext/matrix_transform.hpp"
#include <QApplication>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const int Y_SEGMENTS = 50;
const int X_SEGMENTS = 50;
const float PI = 3.14159265359;

class MyOpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core {
public:
  MyOpenGLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {}

protected:
  unsigned int vao;
  unsigned int vbo;
  unsigned int ebo;
  QOpenGLShaderProgram *shaderProgram;
  void initializeGL() override {
    initializeOpenGLFunctions();
    auto sphereVertices = QList<float>();
    for (int y = 0; y <= Y_SEGMENTS; y++) {
      for (int x = 0; x <= X_SEGMENTS; x++) {
        float xSegment = (float)x / (float)X_SEGMENTS;
        float ySegment = (float)y / (float)Y_SEGMENTS;
        float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        float yPos = std::cos(ySegment * PI);
        float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        sphereVertices.push_back(xPos);
        sphereVertices.push_back(yPos);
        sphereVertices.push_back(zPos);
      }
    }

    auto sphereIndices = QList<unsigned int>();
    for (int i = 0; i < Y_SEGMENTS; i++) {
      for (int j = 0; j < X_SEGMENTS; j++) {
        sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
        sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
        sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
        sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
        sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
        sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
      }
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    // 生成并绑定球体的VAO和VBO
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float),
                 &sphereVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shaderProgram = new QOpenGLShaderProgram(this);

    const char *vs = "#version 330 core\n"
                     "layout (location = 0) in vec3 aPos;\n"
                     "uniform mat4 view;\n"
                     "void main()\n"
                     "{\n"
                     "    gl_Position =vec4(aPos, 1.0)* view ;\n"
                     "}";

    const char *fs = "#version 330 core\n"
                     "out vec4 FragColor;\n"
                     "void main()\n"
                     "{\n"
                     "    FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
                     "}";

    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vs);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fs);
    shaderProgram->link();
  }
  void resizeGL(int w, int h) override { glViewport(0, 0, w, h); }
  void paintGL() override {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgram->bind();
    auto rotateMartix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f),
                                    glm::vec3(1.0f, 0.0f, 0.0f));
    unsigned int loc = glGetUniformLocation(shaderProgram->programId(), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(rotateMartix));

    glBindVertexArray(vao);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT,
                   0);
    glBindVertexArray(0);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  auto w = new MyOpenGLWidget();
  w->resize(500, 500);
  w->show();
  QApplication::exec();
  return 0;
}