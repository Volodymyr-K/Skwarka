#include <windows.h>
#include "gl/gl.h"
#include "gl/glut.h"

#pragma warning(disable : 4003)
#include <Common/Common.h>

#include <iostream>
#include <Math/Geometry.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Shapes/Sphere.h>
#include <vector>
#include <cstdio>
#include <TestTrace/MeshLoader.h>

#pragma warning(disable : 4996)

const char window_caption[] = "Raytracer";
intrusive_ptr<TriangleMesh> p_mesh;

void LoadMesh()
  {
  std::vector<Point3D_d> vertices;
  std::vector<MeshTriangle> triangles;
  std::vector<float> uv_parameterization;

  RayDifferential rd;
/*
  Sphere s;
  s.SetParameter("Center","0 0 0");
  s.SetParameter("Radius","0.4");
  s.SetParameter("Subdivisions","5");
  p_mesh=s.BuildMesh();
*/

  p_mesh = intrusive_ptr<TriangleMesh>( LoadMeshFromStl("kitchen.stl", false) );

  TopologyInfo ti = p_mesh->GetTopologyInfo();

  //p_mesh->ComputeDifferentialGeometry(10,r,dg); 
/*
  FILE *fp=fopen("vertices.txt","r");
  while(true)
    {
    float x,y,z;
    int read = fscanf(fp,"%f %f %f",&x,&y,&z);
    if (read<=0) break;
    vertices.push_back(Point3D_f(x,y,z));
    }
    fclose(fp);

  fp=fopen("triangles.txt","r");
  while(true)
    {
    int v1,v2,v3;
    int read = fscanf(fp,"%d %d %d",&v1,&v2,&v3);
    if (read<=0) break;

    MeshTriangle tr(v1,v2,v3);
    triangles.push_back(tr);
    }
  fclose(fp);

  intrusive_ptr<TriangleMesh> meshlocal = intrusive_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, uv_parameterization) );

  p_mesh=p_meshlocal;
  */
  }

//---------------------------
// InitAll
//---------------------------
void InitAll() {
    glClearColor(0.50, 0.50, 0.50, 1.0);
    glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

  float lightPos[4] = {0, 0, 1, 0};
  float lightDiffuse[4] = {2, 2, 2, 1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, lightDiffuse);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

float an=0,r=50000;
float cx=0, cy=0, cz=0.0;

//------------------ glut handlers -----------------------
void glut_display() {
  glLoadIdentity();
  gluLookAt(cx, cy, cz,   0, 0, 1000,  0, 0, 1);
  //glScalef(10, 10, 10);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBegin(GL_TRIANGLES);
  for (size_t i=0;i<p_mesh->GetNumberOfTriangles();++i)
    {
    MeshTriangle triangle = p_mesh->GetTriangle(i);

    Point3D_d v1 = Convert<double>(p_mesh->GetVertex(triangle.m_vertices[0]));
    Point3D_d v2 = Convert<double>(p_mesh->GetVertex(triangle.m_vertices[1]));
    Point3D_d v3 = Convert<double>(p_mesh->GetVertex(triangle.m_vertices[2]));

    Vector3D_d n = Convert<double>((Vector3D_d(v2-v1)^Vector3D_d(v3-v1)).Normalized());

    glNormal3f(n[0],n[1],n[2]);
    glVertex3f(v1[0], v1[1], v1[2]);

    glNormal3f(n[0],n[1],n[2]);
    glVertex3f(v2[0], v2[1], v2[2]);

    glNormal3f(n[0],n[1],n[2]);
    glVertex3f(v3[0], v3[1], v3[2]);
   }
  glEnd();
    
    glutSwapBuffers();
}

void glut_reshape(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (float)w / (float)h, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

void glut_idle() {
    glutPostRedisplay();
}

void glut_keyboard(unsigned char key, int x, int y) {
  if (key==97)
    {
    an+=0.1;
    }
  if (key==100)
    {
    an-=0.1;
    }

  if (key==113)
    {
    r+=10;
    }
  if (key==101)
    {
    r-=10;
    }

  cx=r*sin(an);
  cy=r*cos(an);

    if (key == 27) {
        exit(0);
    }
}

int main (int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(500, 500);
    glutCreateWindow(window_caption);

    glutDisplayFunc(glut_display);
    glutReshapeFunc(glut_reshape);
    glutIdleFunc(glut_idle);
    glutKeyboardFunc(glut_keyboard);

    InitAll();

  LoadMesh();

    glutMainLoop();
}
