#ifndef PBRT_SHAPE_FACTORY
#define PBRT_SHAPE_FACTORY

#include <Common/Common.h>
#include "../PbrtParamSet.h"
#include <Raytracer/Core/TriangleMesh.h>
#include <Shapes/Cylinder.h>
#include <Shapes/Disk.h>
#include <Shapes/Sphere.h>
#include "../PbrtUtils.h"

namespace PbrtImport
  {

  class ShapeFactory
    {
    public:
      ShapeFactory(intrusive_ptr<Log> ip_log): mp_log(ip_log) {}

      intrusive_ptr<TriangleMesh> CreateShape(const std::string &i_name, const Transform &i_obj_to_world, const GraphicsState &i_graphics_state, const ParamSet &i_params) const
        {
        if (i_name == "trianglemesh")
          return _CreateTriangleMeshShape(i_obj_to_world, i_graphics_state.reverseOrientation,  i_params, i_graphics_state.floatTextures);
        else if (i_name == "sphere")
          return _CreateSphereShape(i_obj_to_world, i_graphics_state.reverseOrientation, i_params);
        else if (i_name == "cylinder")
          return _CreateCylinderShape(i_obj_to_world, i_graphics_state.reverseOrientation, i_params);
        else if (i_name == "disk")
          return _CreateDiskShape(i_obj_to_world, i_graphics_state.reverseOrientation, i_params);
        else if (i_name == "cone")
          return _CreateConeShape(i_obj_to_world, i_graphics_state.reverseOrientation, i_params);
        else if (i_name == "paraboloid")
          return _CreateParaboloidShape(i_obj_to_world, i_graphics_state.reverseOrientation, i_params);
        else if (i_name == "hyperboloid")
          return _CreateHyperboloidShape(i_obj_to_world, i_graphics_state.reverseOrientation, i_params);
        else if (i_name == "heightfield")
          return _CreateHeightfieldShape(i_obj_to_world, i_graphics_state.reverseOrientation, i_params);
        else if (i_name == "loopsubdiv")
          return _CreateLoopSubdivShape(i_obj_to_world, i_graphics_state.reverseOrientation, i_params);
        else if (i_name == "nurbs")
          return _CreateNURBSShape(i_obj_to_world, i_graphics_state.reverseOrientation, i_params);
        else
          PbrtImport::Utils::LogError(mp_log, std::string("Shape \"") + i_name + std::string("\" unknown."));

        return NULL;
        }

    private:
      intrusive_ptr<TriangleMesh> _CreateSphereShape(const Transform &i_obj_to_world, bool i_reverse_orientation, const ParamSet &i_params) const
        {
        float radius = i_params.FindOneFloat("radius", 1.f);
        float zmin = i_params.FindOneFloat("zmin", -radius);
        float zmax = i_params.FindOneFloat("zmax", radius);
        float phimax = i_params.FindOneFloat("phimax", 360.f);

        if (zmin != -radius) PbrtImport::Utils::LogError(mp_log, std::string("Parameter zmin is not support for Sphere shape."));
        if (zmax != radius) PbrtImport::Utils::LogError(mp_log, std::string("Parameter zmax is not support for Sphere shape."));
        if (phimax != 360.f) PbrtImport::Utils::LogError(mp_log, std::string("Parameter phimax is not support for Sphere shape."));

        Sphere sphere;
        sphere.SetTransformation(i_obj_to_world * MakeScale(radius));
        sphere.SetInvertOrientation(i_reverse_orientation);
        sphere.SetSubdivisions(5);
        return sphere.BuildMesh();
        }

      intrusive_ptr<TriangleMesh> _CreateCylinderShape(const Transform &i_obj_to_world, bool i_reverse_orientation, const ParamSet &i_params) const
        {
        float radius = i_params.FindOneFloat("radius", 1);
        float zmin = i_params.FindOneFloat("zmin", -1);
        float zmax = i_params.FindOneFloat("zmax", 1);
        float phimax = i_params.FindOneFloat("phimax", 360);

        Cylinder cylinder;
        cylinder.SetTransformation(i_obj_to_world * MakeTranslation(Vector3D_d(0, 0, zmin)) * MakeScale(radius, radius, zmax-zmin));
        cylinder.SetMaxPhi(phimax*INV_PI);
        cylinder.SetInvertOrientation(i_reverse_orientation);
        return cylinder.BuildMesh();
        }

      intrusive_ptr<TriangleMesh> _CreateDiskShape(const Transform &i_obj_to_world, bool i_reverse_orientation, const ParamSet &i_params) const
        {
        float height = i_params.FindOneFloat("height", 0.);
        float radius = i_params.FindOneFloat("radius", 1);
        float inner_radius = i_params.FindOneFloat("innerradius", 0);
        float phimax = i_params.FindOneFloat("phimax", 360);

        Disk disk;
        disk.SetTransformation(i_obj_to_world * MakeTranslation(Vector3D_d(0, 0, height)) * MakeScale(radius, radius, 1));
        disk.SetMaxPhi(phimax*INV_PI);
        disk.SetInnerRadius(inner_radius/radius);
        disk.SetInvertOrientation(i_reverse_orientation);
        return disk.BuildMesh();
        }

      intrusive_ptr<TriangleMesh> _CreateTriangleMeshShape(const Transform &i_obj_to_world, bool i_reverse_orientation, const ParamSet &i_params,
        const std::map<std::string, intrusive_ptr<const Texture<double>>> &i_float_textures) const
        {
        int nvi, npi, nuvi, nsi, nni;
        const int *vi = i_params.FindInt("indices", &nvi);
        const Point3D_d *P = i_params.FindPoint("P", &npi);
        const float *uvs = i_params.FindFloat("uv", &nuvi);
        if (!uvs) uvs = i_params.FindFloat("st", &nuvi);
        bool discardDegnerateUVs = i_params.FindOneBool("discarddegenerateUVs", false);

        // XXX should complain if uvs aren't an array of 2...
        if (uvs)
          {
          if (nuvi < 2 * npi)
            {
            std::string message = std::string("Not enough of \"uv\"s for triangle mesh.  Expected ");
            message += 2*npi; message += ", found "; message += nuvi; message += ".  Discarding.";
            PbrtImport::Utils::LogError(mp_log, message);
            uvs = NULL;
            }
          else if (nuvi > 2 * npi)
            {
            std::string message = std::string("More \"uv\"s provided than will be used for triangle mesh. (");
            message += 2*npi; message += " expected, "; message += nuvi; message += " found)";
            PbrtImport::Utils::LogWarning(mp_log, message);
            }
          }
        if (!vi || !P) return NULL;

        const Vector3D_d *S = i_params.FindVector("S", &nsi);
        if (S && nsi != npi)
          {
          PbrtImport::Utils::LogError(mp_log, "Number of \"S\"s for triangle mesh must match \"P\"s");
          S = NULL;
          }

        const Vector3D_d *N = i_params.FindNormal("N", &nni);
        if (N && nni != npi)
          {
          PbrtImport::Utils::LogError(mp_log, "Number of \"N\"s for triangle mesh must match \"P\"s");
          N = NULL;
          }

        if (discardDegnerateUVs && uvs && N)
          {
          // if there are normals, check for bad uv's that
          // give degenerate mappings; discard them if so
          const int *vp = vi;
          for (int i = 0; i < nvi; i += 3, vp += 3)
            {
            double area = 0.5 * (Vector3D_d(P[vp[0]]-P[vp[1]])^Vector3D_d(P[vp[2]]-P[vp[1]])).Length();
            if (area < 1e-7) continue; // ignore degenerate tris.
            if ((uvs[2*vp[0]] == uvs[2*vp[1]] &&
              uvs[2*vp[0]+1] == uvs[2*vp[1]+1]) ||
              (uvs[2*vp[1]] == uvs[2*vp[2]] &&
              uvs[2*vp[1]+1] == uvs[2*vp[2]+1]) ||
              (uvs[2*vp[2]] == uvs[2*vp[0]] &&
              uvs[2*vp[2]+1] == uvs[2*vp[0]+1]))
                {
                PbrtImport::Utils::LogWarning(mp_log, "Degenerate uv coordinates in triangle mesh.  Discarding all uvs.");
                uvs = NULL;
                break;
                }
            }
          }

        for (int i = 0; i < nvi; ++i)
          if (vi[i] >= npi)
            {
            std::string message = std::string("TriangleMesh has out of-bounds vertex index ");
            message += vi[i]; message += " ("; message += npi; message += " \"P\" values were given)";
            PbrtImport::Utils::LogError(mp_log, message);
            return NULL;
            }

        intrusive_ptr<const Texture<double>> alphaTex = NULL;
        std::string alphaTexName = i_params.FindTexture("alpha");
        if (alphaTexName != "")
          {
          PbrtImport::Utils::LogError(mp_log, "Alpha textures are not supported. Skipping.");

          if (i_float_textures.find(alphaTexName) != i_float_textures.end())
            alphaTex = i_float_textures.find(alphaTexName)->second;
          else
            PbrtImport::Utils::LogError(mp_log, std::string("Couldn't find float texture \"") + alphaTexName.c_str() + std::string("\" for \"alpha\" parameter"));
          }
        else if (i_params.FindOneFloat("alpha", 1.f) == 0.f)
          {
          PbrtImport::Utils::LogError(mp_log, "Alpha textures are not supported. Skipping.");
          alphaTex.reset( new ConstantTexture<double>(0.0) );
          }

        // Convert all the data to our format and apply the transformation.
        std::vector<Point3D_f> vertices;
        std::vector<Vector3D_f> normals, tangents;
        for(int i=0;i<npi;++i) vertices.push_back( Convert<float>(i_obj_to_world(P[i])) );
        if (N) for(int i=0;i<nni;++i) normals.push_back( Convert<float>(i_obj_to_world.TransformNormal(N[i].Normalized())) );
        if (S) for(int i=0;i<nsi;++i) tangents.push_back( Convert<float>(i_obj_to_world(S[i])) );

        std::vector<MeshTriangle> triangles;
        for(int i=0;i<nvi;i+=3)
          {
          MeshTriangle triangle;
          triangle.m_vertices[0] = vi[i+0];
          triangle.m_vertices[1] = vi[i+1];
          triangle.m_vertices[2] = vi[i+2];

          if (uvs)
            {
            triangle.m_uvs[0] = Point2D_f(uvs[2*vi[i+0]], uvs[2*vi[i+0]+1]);
            triangle.m_uvs[1] = Point2D_f(uvs[2*vi[i+1]], uvs[2*vi[i+1]+1]);
            triangle.m_uvs[2] = Point2D_f(uvs[2*vi[i+2]], uvs[2*vi[i+2]+1]);
            }
          else
            {
            // this is what pbrt does when UVs are not provided
            triangle.m_uvs[0] = Point2D_f(0.f, 0.f);
            triangle.m_uvs[1] = Point2D_f(1.f, 0.f);
            triangle.m_uvs[2] = Point2D_f(1.f, 1.f);
            }
          triangles.push_back(triangle);
          }

        return new TriangleMesh(vertices, triangles, normals, tangents, (N!=NULL), i_reverse_orientation);
        }

      intrusive_ptr<TriangleMesh> _CreateConeShape(const Transform &i_obj_to_world, bool i_reverse_orientation, const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Cone shape is not supported");
        return NULL;
        }

      intrusive_ptr<TriangleMesh> _CreateParaboloidShape(const Transform &i_obj_to_world, bool i_reverse_orientation, const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Paraboloid shape is not supported");
        return NULL;
        }

      intrusive_ptr<TriangleMesh> _CreateHyperboloidShape(const Transform &i_obj_to_world, bool i_reverse_orientation, const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Hyperboloid shape is not supported");
        return NULL;
        }

      intrusive_ptr<TriangleMesh> _CreateHeightfieldShape(const Transform &i_obj_to_world, bool i_reverse_orientation, const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Heightfield shape is not supported");
        return NULL;
        }

      intrusive_ptr<TriangleMesh> _CreateLoopSubdivShape(const Transform &i_obj_to_world, bool i_reverse_orientation, const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "LoopSubdiv shape is not supported");
        return NULL;
        }

      intrusive_ptr<TriangleMesh> _CreateNURBSShape(const Transform &i_obj_to_world, bool i_reverse_orientation, const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "NURBS shape is not supported");
        return NULL;
        }

    private:
      intrusive_ptr<Log> mp_log;
    };

  };

#endif // PBRT_SHAPE_FACTORY