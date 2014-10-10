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

#ifndef SCENE_IMPORTER_H
#define SCENE_IMPORTER_H

#include <Common/Common.h>
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/Camera.h>
#include <vector>

class SceneImporter
  {
  public:
    virtual intrusive_ptr<const Scene> GetScene() const = 0;
    virtual std::vector<intrusive_ptr<const Camera>> GetCameras() const = 0;
  };

#endif // SCENE_IMPORTER_H
