#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "./common.h"
#include "./bb.h"
#include "./mesh.h"
#include "./io.h"
#include "./texture.h"

#define ARCBALL_RADIUS 1

using namespace std;


Mesh mesh;

GLuint* texture_ids;

// window parameters
int window_width = 800, window_height = 600;
float window_aspect = window_width / static_cast<float>(window_height);

const Vec3f O = {{0, 0, 0}};
float theta = 0;
Vec3f start = {{0, 0, 0}}, end = {{0, 0, 0}}, axis = {{0, 0, 0}};
bool isRotate;
float scale = 1.0f, scaleDelta = 0.0f;

float rotationM[] = {
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f
};

bool scene_lighting;

Vec3f eye = {2.0f, 2.0f, 5.0f};
Vec3f center = {0.0f, 0.0f, 0.0f};
Vec3f up = {0.0f, 1.0f, 0.0f};

GLfloat whiteSpecularLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat whiteDiffuseLight[] = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat blackAmbientLight[] = {0.0f, 0.0f, 0.0f, 1.0f};

void Light() {
  GLfloat position[] = {30.0f, 30.0f, -100.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);

  glLightfv(GL_LIGHT1, GL_POSITION, eye.x);

  glLightfv(GL_LIGHT1, GL_AMBIENT, blackAmbientLight);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteDiffuseLight);
  glLightfv(GL_LIGHT1, GL_SPECULAR, whiteSpecularLight);
}

void DrawVertices() {
  glLineWidth(1);
  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < mesh.vertices.size(); ++i) {
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(mesh.vertices[i][0],
               mesh.vertices[i][1],
               mesh.vertices[i][2]);
  }
  glEnd();
}

void SetMaterial(int id) {
  Material m = mesh.material(id);
  glMaterialfv(GL_FRONT, GL_AMBIENT, m.ambient().x);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, m.diffuse().x);
  glMaterialfv(GL_FRONT, GL_SPECULAR, m.specular().x);
  glBindTexture(GL_TEXTURE_2D, m.texture_id());
}

void DrawPolygons() {
  glEnable(GL_TEXTURE_2D);
  for (int i = 0; i < mesh.polyVerts.size(); ++i) {
    int mat_idx = mesh.polygon2material(i);
    bool material = mat_idx != -1;
    if (material)
      SetMaterial(mat_idx);

    glBegin(GL_POLYGON);
    for (int j = 0; j < mesh.polyVerts[i].size(); ++j) {
      if (material) {
        glNormal3fv(mesh.normals[mesh.polyVerts[i][j]].unit().x);
        glTexCoord2f(mesh.textures[mesh.polyTexts[i][j]][0],
                   mesh.textures[mesh.polyTexts[i][j]][1]);
      }
      glVertex3f(mesh.vertices[mesh.polyVerts[i][j]][0],
                 mesh.vertices[mesh.polyVerts[i][j]][1],
                 mesh.vertices[mesh.polyVerts[i][j]][2]);
    }
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
}

void DrawNormals() {
  glBegin(GL_LINES);
  for (int i = 0; i < mesh.normals.size(); ++i) {
    glVertex3f(mesh.vertices[i][0],
               mesh.vertices[i][1],
               mesh.vertices[i][2]);
    glVertex3f(mesh.vertices[i][0] + mesh.normals[i][0],
               mesh.vertices[i][1] + mesh.normals[i][1],
               mesh.vertices[i][2] + mesh.normals[i][2]);
  }
  glEnd();
}

Vec3f ambient = {1, 1 , 1};
Vec3f specular = {0, 0, 0};
Vec3f diffuse = {1, 1, 0};


void Display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  // glEnable(GL_LIGHT1);
  if (scene_lighting)
    Light();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, window_aspect, 1, 1500);

  // TODO call gluLookAt such that mesh fits nicely in viewport.
  // mesh.bb() may be useful.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_RESCALE_NORMAL);
  gluLookAt(eye[0], eye[1], eye[2],
            center[0], center[1], center[2],
            up[0], up[1], up[2]);
  if (!scene_lighting)
    Light();


  //  Arc ball and zoom
  glRotatef(theta, axis[0], axis[1], axis[2]);
  glMultMatrixf(rotationM);
  glTranslatef(
             -mesh.bb().center()[0],
             -mesh.bb().center()[1],
             -mesh.bb().center()[2]);
  // TODO set up lighting, material properties and render mesh.
  // Be sure to call glEnable(GL_RESCALE_NORMAL) so your normals
  // remain normalized throughout transformations.

  // You can leave the axis in if you like.
  glDisable(GL_RESCALE_NORMAL);
  glLineWidth(4);
  glDisable(GL_LIGHTING);
  DrawAxis();
  glEnable(GL_LIGHTING);
  // glColor3f(1.0f, 0.0f, 0.0f);
  // DrawNormals();
  // glColor3f(0.0f, 0.0f, 0.0f);
  DrawPolygons();
  glDisable(GL_LIGHT0);
  // glDisable(GL_LIGHT1);
  glDisable(GL_LIGHTING);

  glFlush();
  glutSwapBuffers();
}

void PrintMatrix(GLfloat* m) {
  cout.precision(2);
  int w = 6;
  for (int i = 0; i < 4; ++i) {
    cout << setprecision(2) << setw(w) << m[i] << " "
         << setprecision(2) << setw(w) << m[i+4] << " "
         << setprecision(2) << setw(w) << m[i+8] << " "
         << setprecision(2) << setw(w) << m[i+12] << " "
         << endl;
  }
  cout << endl;
}

void PrintMatrix(GLint matrix) {
  GLfloat m[16];
  glGetFloatv(matrix, m);
  PrintMatrix(m);
}

void PrintMatrix() {
  PrintMatrix(GL_MODELVIEW_MATRIX);
}

void LoadMatrix(GLfloat* m) {
  // transpose to column-major
  for (int i = 0; i < 4; ++i) {
    for (int j = i; j < 4; ++j) {
      swap(m[i*4+j], m[j*4+i]);
    }
  }
  glLoadMatrixf(m);
}

void MultMatrix(GLfloat* m) {
  // transpose to column-major
  for (int i = 0; i < 4; ++i) {
    for (int j = i; j < 4; ++j) {
      swap(m[i*4+j], m[j*4+i]);
    }
  }
  glMultMatrixf(m);
}

void Init() {
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  // resize the window
  window_aspect = window_width/static_cast<float>(window_height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, window_aspect, 1, 1500);
}

void DrawAxis() {
  const Vec3f c = {0, 0, 0};
  const float L = 1;
  const Vec3f X = {L, 0, 0}, Y = {0, L, 0}, Z = {0, 0, L};

  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3fv(c.x);
  glVertex3fv((c+X).x);
  glColor3f(0, 1, 0);
  glVertex3fv(c.x);
  glVertex3fv((c+Y).x);
  glColor3f(0, 0, 1);
  glVertex3fv(c.x);
  glVertex3fv((c+Z).x);
  glEnd();
}

Vec3f arcBall(int x, int y) {
  float xNorm = 2.0f * x / window_width - 1;
  float yNorm = 2.0f * y / window_height - 1;
  float zNorm;

  float distance = xNorm * xNorm + yNorm * yNorm;  // distance squared
  if (distance > 1) {  // Outside unit circle, need to move it
    distance = 1;
  }
  zNorm = sqrt(1 - distance);

  return Vec3f::makeVec(xNorm, yNorm, zNorm).unit();
}

float magnitude3v(Vec3f v) {
  return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

void arcDrag(int x, int y) {
  end = arcBall(x, y);
  axis = start^end;
  float magnitude = magnitude3v(start) * magnitude3v(end);
  if (magnitude > 0) {
    theta = ((O - end) * (O - start)) / magnitude;
    theta = acos(theta) * 180/M_PI;
  } else {
    theta = 0;
  }
}

void saveRotation() {
  glPushMatrix();
  glLoadIdentity();
  glRotatef(theta, axis[0], axis[1], axis[2]);
  glMultMatrixf(rotationM);
  glGetFloatv(GL_MODELVIEW_MATRIX, rotationM);
  axis.zero();
  theta = 0;
  glPopMatrix();
}

// Dragging bottom to top = 100% increase
void zoom(int y) {
  float difference = y - start[1];
  if (difference > 0) {
    eye[0] = eye[0]*0.99;
    eye[1] = eye[1]*0.99;
    eye[2] = eye[2]*0.99;
  } else if (difference < 0) {
    eye[0] = eye[0]*1.01;
    eye[1] = eye[1]*1.01;
    eye[2] = eye[2]*1.01;
  }
}

void MouseButton(int button, int state, int x, int y) {
  y = window_height - y;
  if (button == 0) {
    isRotate = true;
    if (state == 0) {  // Mouse pressed
      start = arcBall(x, y);
    } else {  // Mouse released
      saveRotation();
    }
  } else if (button == 2) {
    isRotate = false;
    if (state == 0) {
      start = Vec3f::makeVec(x, y, 0);
    }
  }
  glutPostRedisplay();
}

void MouseMotion(int x, int y) {
  y = window_height - y;
  if (isRotate) {
    arcDrag(x, y);
  } else {
    zoom(y);
  }
  glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 's':
    eye[2] = eye[2] - 1;
    center[2] = center[2] - 1;
    break;
  case 'w':
    eye[2] = eye[2] + 1;
    center[2] = center[2] + 1;
    break;
  case 'd':
    eye[0] = eye[0] - 1;
    center[0] = center[0] - 1;
    break;
  case 'a':
    eye[0] = eye[0] + 1;
    center[0] = center[0] + 1;
    break;
  case 'q':
  case 27:  // esc
    exit(0);
    break;
  }
  glutPostRedisplay();
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << endl;
    cout << "Usage: ./viewer (filename.obj | -s) [-l]" << endl;
    cout << endl;
    cout << "To load data/test.obj: ./viewer data/test.obj" << endl;
    cout << "To load a custom scene: ./viewer -s" << endl;
    cout << endl;
    return 0;
  }

  // Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(window_width, window_height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Object viewer");
  glutMouseFunc(MouseButton);
  glutMotionFunc(MouseMotion);
  glutKeyboardFunc(Keyboard);
  glutDisplayFunc(Display);

  Init();

  if (string(argv[1]) == "-s") {
    cout << "Create scene" << endl;
  } else {
    string filename(argv[1]);
    cout << filename << endl;

    // Detect whether to fix the light source(s) to the camera or the world
    scene_lighting = false;
    if (argc > 2 && string(argv[2]) == "-l") {
      scene_lighting = true;
    }

    // Parse the obj file, compute the normals, read the textures

    ParseObj(filename, mesh);
    mesh.compute_normals();

    texture_ids = new GLuint[mesh.num_materials()];
    glGenTextures(mesh.num_materials(), texture_ids);

    for (int i = 0; i < mesh.num_materials(); ++i) {
      Material& material = mesh.material(i);
      material.LoadTexture(texture_ids[i]);
    }
  }

  glutMainLoop();

  return 0;
}
