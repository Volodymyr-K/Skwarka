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

#ifndef KDTREE_TEST_H
#define KDTREE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/KDTree.h>

class CustomPointFilter
  {
  public:
    CustomPointFilter()
      {
      }

    bool operator()(const Point3D_d &i_point) const
      {
      int sum = (int)(i_point[0]+i_point[1]+i_point[2]);
      return (sum%2)==1;
      }
  };

class KDTreeTestSuite : public CxxTest::TestSuite
  {
  public:

    void setUp()
      {
      size_t N=10000;

      m_points.clear();
      for (size_t i=0;i<N;++i)
        {
        Point3D_d point(RandomDouble(1000)-500, RandomDouble(1000)-500, RandomDouble(1000)-500);
        m_points.push_back(point);
        }

      // Add few duplicate points for testing corner cases.
      for (size_t i=0;i<100;++i)
        m_points.push_back(m_points[i]);

      mp_kdtree.reset(new KDTree<Point3D_d>(m_points));

      m_number_of_points = N + 100;
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_KDTree_GetNumberOfPoints()
      {
      TS_ASSERT_EQUALS(mp_kdtree->GetNumberOfPoints(), m_number_of_points);
      }

    void test_KDTree_GetAllPoints()
      {
      const std::vector<Point3D_d> &points = mp_kdtree->GetAllPoints();
      TS_ASSERT_EQUALS(points.size(), m_number_of_points);
      }

    void test_KDTree_NearestPoint()
      {
      size_t T=1000;
      for(size_t t=0;t<T;++t)
        {
        Point3D_d point(RandomDouble(1000)-500, RandomDouble(1000)-500, RandomDouble(1000)-500);
        double max_dist = RandomDouble(1000);

        const Point3D_d *p_nearest = mp_kdtree->GetNearestPoint(point, max_dist);

        Point3D_d nearest2;
        double min_dist_sqr=DBL_INF;
        for(size_t i=0;i<m_points.size();++i)
          {
          double dist_sqr=Vector3D_d(m_points[i]-point).LengthSqr();
          if (dist_sqr<=max_dist*max_dist && dist_sqr<min_dist_sqr)
            {
            min_dist_sqr=dist_sqr;
            nearest2=m_points[i];
            }
          }

        if (p_nearest == NULL)
          {
          if (min_dist_sqr != DBL_INF)
            {
            TS_FAIL("Nearest point is wrong.");
            return;
            }
          }
        else
          if (*p_nearest != nearest2 && Vector3D_d(*p_nearest-point).LengthSqr()!=min_dist_sqr)
            {
            TS_FAIL("Nearest point is wrong.");
            return;
            }
        }
      }

    void test_KDTree_NearestPointWithFilter()
      {
      CustomPointFilter filter;

      size_t T=1000;
      for(size_t t=0;t<T;++t)
        {
        Point3D_d point(RandomDouble(1000)-500, RandomDouble(1000)-500, RandomDouble(1000)-500);
        double max_dist = RandomDouble(1000);

        const Point3D_d *p_nearest = mp_kdtree->GetNearestPoint(point, filter, max_dist);

        Point3D_d nearest2;
        double min_dist_sqr=DBL_INF;
        for(size_t i=0;i<m_points.size();++i)
          if (filter(m_points[i]))
            {
            double dist_sqr=Vector3D_d(m_points[i]-point).LengthSqr();
            if (dist_sqr<=max_dist*max_dist && dist_sqr<min_dist_sqr)
              {
              min_dist_sqr=dist_sqr;
              nearest2=m_points[i];
              }
            }

        if (p_nearest == NULL)
          {
          if (min_dist_sqr != DBL_INF)
            {
            TS_FAIL("Nearest point is wrong.");
            return;
            }
          }
        else
          if (*p_nearest != nearest2 && Vector3D_d(*p_nearest-point).LengthSqr()!=min_dist_sqr)
            {
            TS_FAIL("Nearest point is wrong.");
            return;
            }
        }
      }

    void test_KDTree_NearestPoints()
      {
      size_t T=1000;
      for(size_t t=0;t<T;++t)
        {
        Point3D_d point(RandomDouble(1000)-500, RandomDouble(1000)-500, RandomDouble(1000)-500);
        double max_dist = RandomDouble(1000);

        size_t num = RandomInt(500)+1;
        std::vector<KDTree<Point3D_d>::NearestPoint> nearest_points(num);
        size_t found = mp_kdtree->GetNearestPoints(point, num, &(nearest_points[0]), max_dist);

        TS_ASSERT(found <= num);

        // Test that returned distances are correct.
        std::vector<double> distances;
        for(size_t i=0;i<found;++i)
          {
          distances.push_back(nearest_points[i].m_distance_sqr);
          if (Vector3D_d(*nearest_points[i].mp_point - point).LengthSqr() != nearest_points[i].m_distance_sqr)
            {
            TS_FAIL("Nearest distances are wrong.");
            return;
            }
          }

        // Test that found points are indeed the nearest ones.
        std::vector<double> distances2;
        for(size_t i=0;i<m_points.size();++i)
          {
          double dist_sqr=Vector3D_d(m_points[i]-point).LengthSqr();
          if (dist_sqr<=max_dist*max_dist)
            distances2.push_back(dist_sqr);
          }

        std::sort(distances.begin(), distances.end());
        std::sort(distances2.begin(), distances2.end());
        if (distances2.size()>num) distances2.resize(num);

        if (distances != distances2)
          {
          TS_FAIL("Nearest distances are wrong.");
          return;
          }
        }
      }

    void test_KDTree_NearestPointsWithFilter()
      {
      CustomPointFilter filter;

      size_t T=1000;
      for(size_t t=0;t<T;++t)
        {
        Point3D_d point(RandomDouble(1000)-500, RandomDouble(1000)-500, RandomDouble(1000)-500);
        double max_dist = RandomDouble(1000);

        size_t num = RandomInt(500)+1;
        std::vector<KDTree<Point3D_d>::NearestPoint> nearest_points(num);
        size_t found = mp_kdtree->GetNearestPoints(point, num, &(nearest_points[0]), filter, max_dist);

        TS_ASSERT(found <= num);

        // Test that returned distances are correct.
        std::vector<double> distances;
        for(size_t i=0;i<found;++i)
          {
          distances.push_back(nearest_points[i].m_distance_sqr);
          if (Vector3D_d(*nearest_points[i].mp_point - point).LengthSqr() != nearest_points[i].m_distance_sqr)
            {
            TS_FAIL("Nearest distances are wrong.");
            return;
            }
          }

        // Test that found points are indeed the nearest ones.
        std::vector<double> distances2;
        for(size_t i=0;i<m_points.size();++i)
          if (filter(m_points[i]))
            {
            double dist_sqr=Vector3D_d(m_points[i]-point).LengthSqr();
            if (dist_sqr<=max_dist*max_dist)
              distances2.push_back(dist_sqr);
            }

        std::sort(distances.begin(), distances.end());
        std::sort(distances2.begin(), distances2.end());
        if (distances2.size()>num) distances2.resize(num);

        if (distances != distances2)
          {
          TS_FAIL("Nearest distances are wrong.");
          return;
          }
        }
      }


    void test_KDTree_EmptyTree()
      {
      std::vector<Point3D_d> points;

      shared_ptr<KDTree<Point3D_d>> p_kdtree (new KDTree<Point3D_d>(points));
      TS_ASSERT_EQUALS(p_kdtree->GetNumberOfPoints(), 0);

      const Point3D_d *p_nearest = p_kdtree->GetNearestPoint(Point3D_d());
      TS_ASSERT(p_nearest == NULL);

      std::vector<KDTree<Point3D_d>::NearestPoint> nearest_points(10);
      size_t found = p_kdtree->GetNearestPoints(Point3D_d(), 10, &(nearest_points[0]));
      TS_ASSERT_EQUALS(found, 0);
      }

    // This test just tests that KDTree can be instantiated with any type that has operator[] returning double.
    void test_KDTree_VectorType()
      {
      size_t N=100;

      std::vector<std::vector<double>> vectors;
      for (size_t i=0;i<N;++i)
        {
        std::vector<double> vct;
        vct.push_back(RandomDouble(1000)-500);
        vct.push_back(RandomDouble(1000)-500);
        vct.push_back(RandomDouble(1000)-500);

        vectors.push_back(vct);
        }

      KDTree<std::vector<double>> kdtree(vectors);

      const std::vector<double> *p_vector = kdtree.GetNearestPoint(Point3D_d(0,0,0));
      TS_ASSERT(p_vector);
      }

  private:
    shared_ptr<KDTree<Point3D_d>> mp_kdtree;
    std::vector<Point3D_d> m_points;

    size_t m_number_of_points;
  };

#endif // KDTREE_TEST_H