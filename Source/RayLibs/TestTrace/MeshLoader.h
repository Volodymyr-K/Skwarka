/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#pragma warning(disable : 4003)
#include <Common/Common.h>

#include <Math/Geometry.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Shapes/Sphere.h>
#include <Math/Transform.h>
#include <string>
#include <cstdio>
#include <vector>
#include <sstream>
#include <map>

intrusive_ptr<TriangleMesh> LoadMeshFromStl(std::string i_filename, bool i_smooth)
  {
  std::vector<Point3D_f> vertices;
  std::vector<MeshTriangle> triangles;
  std::vector<float> uv_parameterization;

  std::map< std::pair<float,std::pair<float,float>>, size_t > vertices_tmp;
#pragma warning(disable : 4996)
  FILE *fp=fopen(i_filename.c_str(),"r");

  Vector3D_f normal;
  Point3D_f v1,v2,v3;
  size_t ind=0,num_ver=0;
  while(true)
    {
    char buf[1024];
    if ( !fgets(buf,1024,fp) ) break;

    std::string s=buf;
    if (s.find("normal")!=-1)
      {
      std::stringstream sstream;
      sstream << s;

      std::string dummy;
      float x,y,z;
      sstream >> dummy >> dummy >> x >> y >> z;
      normal=Vector3D_f(x,y,z);
      }
    if (s.find("vertex")!=-1)
      {
      std::stringstream sstream;
      sstream << s;

      std::string dummy;
      float x,y,z;
      sstream >> dummy >> x >> y >> z;
      if (num_ver==0) v1=Point3D_f(x,y,z);
      if (num_ver==1) v2=Point3D_f(x,y,z);
      if (num_ver==2) v3=Point3D_f(x,y,z);
      ++num_ver;
      }

    size_t ind1,ind2,ind3;
    if (num_ver==3)
      {
      num_ver=0;
      std::pair<float,std::pair<float,float>> vertex1=std::make_pair(v1[0],std::make_pair(v1[1],v1[2]));
      std::pair<float,std::pair<float,float>> vertex2=std::make_pair(v2[0],std::make_pair(v2[1],v2[2]));
      std::pair<float,std::pair<float,float>> vertex3=std::make_pair(v3[0],std::make_pair(v3[1],v3[2]));
      if (vertices_tmp.find(vertex1)==vertices_tmp.end()) {vertices.push_back(v1);vertices_tmp[vertex1]=ind1=ind++;} else ind1=vertices_tmp[vertex1];
      if (vertices_tmp.find(vertex2)==vertices_tmp.end()) {vertices.push_back(v2);vertices_tmp[vertex2]=ind2=ind++;} else ind2=vertices_tmp[vertex2];
      if (vertices_tmp.find(vertex3)==vertices_tmp.end()) {vertices.push_back(v3);vertices_tmp[vertex3]=ind3=ind++;} else ind3=vertices_tmp[vertex3];

      if ( (Vector3D_f(v2-v1)^Vector3D_f(v3-v1))*normal < 0.0) std::swap(ind1,ind2);
      MeshTriangle tr;
      tr.m_vertices[0]=ind1;tr.m_vertices[1]=ind2;tr.m_vertices[2]=ind3;
      if (ind1!=ind2 && ind1!=ind3 && ind2!=ind3) triangles.push_back(tr);
      }

    }
  fclose(fp);

  return intrusive_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, i_smooth) );
  }

intrusive_ptr<TriangleMesh> LoadMeshFromPLY(std::string i_filename, Transform i_trans, bool i_smooth)
  {
  std::vector<Point3D_f> vertices;
  std::vector<MeshTriangle> triangles;
  std::vector<float> uv_parameterization;

#pragma warning(disable : 4996)
  FILE *fp=fopen(i_filename.c_str(),"r");

  bool header_done=false;
  size_t vertices_num, triangles_num;
  size_t i=0;
  while(true)
    {
    char buf[1024];
    if ( !fgets(buf,1024,fp) ) break;

    std::string s=buf;
    if (s.find("element vertex")!=-1)
      {
      std::stringstream sstream;
      sstream << s;

      std::string dummy;
      sstream >> dummy >> dummy >> vertices_num;
      }
    if (s.find("element face")!=-1)
      {
      std::stringstream sstream;
      sstream << s;

      std::string dummy;
      sstream >> dummy >> dummy >> triangles_num;
      }
    if (s.find("end_header")!=-1)
      {
      header_done=true;
      continue;
      }
    if (header_done==false) continue;

    if (i<vertices_num)
      {
      std::stringstream sstream;
      sstream << s;

      float x,y,z;
      sstream >> x >> y >> z;
      //vertices.push_back(Point3D_f(x,y,z));
      vertices.push_back(i_trans(Point3D_f(-x,-z,y)));
      }
    else
      {
      std::stringstream sstream;
      sstream << s;

      std::string dummy;
      size_t n,v1,v2,v3;
      sstream >> n;
      if (n==3)
        {
        sstream >> v1 >> v2 >> v3;
        MeshTriangle tr;
        tr.m_vertices[0]=v1;tr.m_vertices[1]=v2;tr.m_vertices[2]=v3;
        if (v1!=v2 && v1!=v3 && v2!=v3) triangles.push_back(tr);
        }
      }

    ++i;
    }
  fclose(fp);

  return intrusive_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, i_smooth) );
  }


intrusive_ptr<TriangleMesh> LoadMeshFromPbrt(std::string i_vertices_filename, std::string i_triangles_filename)
  {
  std::vector<Point3D_f> vertices;
  std::vector<MeshTriangle> triangles;
  std::vector<float> uv_parameterization;

  std::map<std::pair<std::pair<float,float>,float>,size_t> uniq;
  std::vector<size_t> repl;
  #pragma warning(disable : 4996)
  FILE *fp=fopen(i_vertices_filename.c_str(),"r");
  //FILE *fp2=fopen("vertices3.txt","w");
  int ver=0;
  while(true)
  {
  // if(++ver>100000) break;
  float x,y,z;
  int read = fscanf(fp,"%f %f %f",&x,&y,&z);
  //fprintf(fp2,"%f %f %f\n",-1100+5000*z,5000*x-500,-200+5000*y);

  if (read<=0) break;
  vertices.push_back(Point3D_f(x,y,z));

  if (uniq.find(std::make_pair(std::make_pair(x,y),z))==uniq.end())
  {
  uniq[std::make_pair(std::make_pair(x,y),z)]=vertices.size()-1;
  repl.push_back( (size_t)((int)vertices.size()-1) );
  }
  else
  {
  repl.push_back(uniq[std::make_pair(std::make_pair(x,y),z)]);
  }
  }
  fclose(fp);
  //fclose(fp2);

  int num_tr=0;
  fp=fopen(i_triangles_filename.c_str(),"r");
  int tr=0;
  while(true)
  {
  size_t v1,v2,v3;
  // if(++tr>10000) break;
  int read = fscanf(fp,"%d %d %d",&v1,&v2,&v3);
  if (read<=0) break;

  v1=repl[v1];
  v2=repl[v2];
  v3=repl[v3];
  if (v1==v2 || v1==v3 || v2==v3)
  continue;

  MeshTriangle tr(v1,v2,v3);
  triangles.push_back(tr);
  }
  fclose(fp);

  return intrusive_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, true) );
  }

void LoadDensities(std::vector<std::vector<std::vector<double>>> &o_densities, std::string i_filename)
  {
  FILE *fp=fopen(i_filename.c_str(),"r");

  size_t x,y,z;
  fscanf(fp, "%d %d %d ", &x, &y, &z);
  o_densities.assign(x, std::vector<std::vector<double>> (y, std::vector<double>(z,0.0)));
  for(size_t i=0;i<z;++i)
    for(size_t j=0;j<y;++j)
      for(size_t k=0;k<x;++k)
        {
        double d;
        fscanf(fp,"%lf ", &d);
        o_densities[k][j][i]=d;
        }

  fclose(fp);
  }

#endif // MESH_LOADER_H