#include "glm/ext/vector_float3.hpp"
#include <QApplication>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <gl/gl.h>
#include <glm/glm.hpp>
#include <qopenglshaderprogram.h>
const int RECUR = 3;
class MyWidget : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core {
  QList<glm::vec3> points;
  QList<glm::vec3> colors;
  unsigned int vao;
  unsigned int vbo;
  unsigned int color_vbo;
  QOpenGLShaderProgram *shaderProgram;
  glm::vec3 base_color_[4] = {
      glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f)};

protected:
  void triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, int color) {
    points.append(a);
    colors.append(base_color_[color]);
    points.append(b);
    colors.append(base_color_[color]);
    points.append(c);
    colors.append(base_color_[color]);
  }
  void tetra(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d) {
    triangle(a, c, b, 0);
    triangle(a, c, d, 1);
    triangle(a, b, d, 2);
    triangle(b, c, d, 3);
  }
  void divideTetra(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
                   int count) {
    if (count == 0) {
      tetra(a, b, c, d);
    } else {
      auto ab = (a + b) / 2.0f;
      auto ac = (a + c) / 2.0f;
      auto ad = (a + d) / 2.0f;
      auto bc = (b + c) / 2.0f;
      auto bd = (b + d) / 2.0f;
      auto cd = (c + d) / 2.0f;

      count--;
      divideTetra(a, ab, ac, ad, count);
      divideTetra(ab, b, bc, bd, count);
      divideTetra(ac, bc, c, cd, count);
      divideTetra(ad, bd, cd, d, count);
    }
  }

  void initializeGL() override {
    initializeOpenGLFunctions();
    glm::vec3 v[4] = {
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, -0.5f, 0.5f)};
    divideTetra(v[0], v[1], v[2], v[3], RECUR);
    // for (auto p : points) {
    //   qDebug() << p.x << p.y << p.z;
    // }
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3),
                 points.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &color_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                 colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);

    shaderProgram = new QOpenGLShaderProgram(this);

    const char *vs = "#version 330 core\n"
                     "layout(location = 0) in vec3 aPos;\n"
                     "layout(location = 1) in vec3 aColor;\n"
                     "out vec3 ourColor;\n"
                     "void main() {\n"
                     "  gl_Position = vec4(aPos, 1.0);\n"
                     "  ourColor = aColor;\n"
                     "}\n";
    const char *fs = "#version 330 core\n"
                     "out vec4 FragColor;\n"
                     "in vec3 ourColor;\n"
                     "void main() {\n"
                     "  FragColor = vec4(ourColor, 1.0);\n"
                     "}\n";

    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vs);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fs);
    shaderProgram->link();
  }
  void paintGL() override {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgram->bind();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, points.size());
  }
  void resizeGL(int w, int h) override { glViewport(0, 0, w, h); }
};

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  MyWidget w;
  w.show();
  QApplication::exec();
}