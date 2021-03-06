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

#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include <Common/Common.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Mapping.h>
#include <Raytracer/Core/MIPMap.h>
#include <Raytracer/Core/Spectrum.h>
#include <vector>

/**
* Texture implementation that looks up texture values in a 2D map (i.e. image).
* The texture uses pluggable Mapping2D implementation to map DifferentialGeometry to a 2D image point.
* The texture uses MIPMap to store the image and retrieve the values. The texture values are filtered with an anisotropic filter.
*
* The MemoryType template parameter corresponds to the type of the actual values stored by the texture.
* The ReturnType template parameter corresponds to the type of the values the texture returns.
* The Converter template parameter is a class used to convert from MemoryType to ReturnType. The default implementation is used if not specified otherwise.
*/
template<typename MemoryType, typename ReturnType, typename Converter = DefaultConverter<MemoryType,ReturnType>>
class ImageTexture: public Texture<ReturnType>
  {
  public:
    /**
    * Crates ImageTexture for the given image and 2D mapping.
    * @param i_image 2D array of the image values. All inner vectors should have the same size. Should have at least one row and at least one column.
    * @param ip_mapping 2D mapping used to map DifferentialGeometry to an image point. Should not be NULL.
    * @param i_repeat Sets whether to wrap the texture on its edges. If false, the value is considered zero (black) beyond the image bounds.
    * @param i_max_anisotropy Maximum anisotropy allowed (ratio of the major ellipse axis to its minor axis). Should be greater or equal than 1.0.
    */
    ImageTexture(const std::vector<std::vector<MemoryType>> &i_image, intrusive_ptr<const Mapping2D> ip_mapping, bool i_repeat = true, double i_max_anisotropy = 8.0);

    /**
    * Crates ImageTexture from the specified image source and 2D mapping.
    * @param ip_image_source ImageSource implementation that defines image for the ImageTexture. The image defined by the ImageSource should not be empty.
    * @param ip_mapping 2D mapping used to map DifferentialGeometry to an image point. Should not be NULL.
    * @param i_repeat Sets whether to wrap the texture on its edges. If false, the value is considered zero (black) beyond the image bounds.
    * @param i_max_anisotropy Maximum anisotropy allowed (ratio of the major ellipse axis to its minor axis). Should be greater or equal than 1.0.
    */
    ImageTexture(intrusive_ptr<const ImageSource<MemoryType>> ip_image_source, intrusive_ptr<const Mapping2D> ip_mapping, bool i_repeat = true, double i_max_anisotropy = 8.0);

    /**
    * Crates ImageTexture from the specified MIPMap and 2D mapping.
    * @param ip_mip_map MIPMap object that defines image for the ImageTexture.
    * @param ip_mapping 2D mapping used to map DifferentialGeometry to an image point. Should not be NULL.
    */
    ImageTexture(intrusive_ptr<const MIPMap<MemoryType>> ip_mip_map, intrusive_ptr<const Mapping2D> ip_mapping);

    /**
    * Returns the value corresponding to the specified DifferentialGeometry and triangle index.
    */
    ReturnType Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const;

  private:
    intrusive_ptr<const MIPMap<MemoryType>> mp_mip_map;
    intrusive_ptr<const Mapping2D> mp_mapping;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename MemoryType, typename ReturnType, typename Converter>
ImageTexture<MemoryType,ReturnType,Converter>::ImageTexture(const std::vector<std::vector<MemoryType>> &i_image,
                                                  intrusive_ptr<const Mapping2D> ip_mapping, bool i_repeat, double i_max_anisotropy): mp_mapping(ip_mapping)
  {
  ASSERT(ip_mapping);

  mp_mip_map.reset(new MIPMap<MemoryType>(i_image, i_repeat, i_max_anisotropy) );
  }

template<typename MemoryType, typename ReturnType, typename Converter>
ImageTexture<MemoryType,ReturnType,Converter>::ImageTexture(intrusive_ptr<const ImageSource<MemoryType>> ip_image_source,
                                                            intrusive_ptr<const Mapping2D> ip_mapping, bool i_repeat, double i_max_anisotropy): mp_mapping(ip_mapping)
  {
  ASSERT(ip_image_source);
  ASSERT(ip_image_source->GetHeight()>0 && ip_image_source->GetWidth()>0);
  ASSERT(ip_mapping);

  mp_mip_map.reset(new MIPMap<MemoryType>(ip_image_source, i_repeat, i_max_anisotropy) );
  }

template<typename MemoryType, typename ReturnType, typename Converter>
ImageTexture<MemoryType, ReturnType, Converter>::ImageTexture(intrusive_ptr<const MIPMap<MemoryType>> ip_mip_map, intrusive_ptr<const Mapping2D> ip_mapping) :
mp_mapping(ip_mapping), mp_mip_map(ip_mip_map)
  {
  ASSERT(ip_mapping);
  ASSERT(ip_mip_map);
  }

template<typename MemoryType, typename ReturnType, typename Converter>
ReturnType ImageTexture<MemoryType,ReturnType,Converter>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  Point2D_d point;
  Vector2D_d dp_dx, dp_dy;
  mp_mapping->Map(i_dg, i_triangle_index, point, dp_dx, dp_dy);

  return Converter::Convert( mp_mip_map->Evaluate(point, dp_dx, dp_dy) );
  }

////////////////////////////////////////// DefaultConverter ///////////////////////////////////////////////

// The generic Converter implementation.
template<typename InputType, typename OutputType>
class DefaultConverter
  {
  public:
    static OutputType Convert(const InputType &i_input)
      {
      return static_cast<OutputType>(i_input);
      }
  };

// Explicit specialization for Spectrum type. Uses native Spectrum conversion routine.
template<typename InputSpectrumType, typename OutputSpectrumType>
class DefaultConverter<Spectrum<InputSpectrumType>, Spectrum<OutputSpectrumType>>
  {
  public:
    static Spectrum<OutputSpectrumType> Convert(const Spectrum<InputSpectrumType> &i_input)
      {
      return ::Convert<OutputSpectrumType>(i_input);
      }
  };

// Explicit specialization for SpectrumCoef type. Uses native SpectrumCoef conversion routine.
template<typename InputSpectrumType, typename OutputSpectrumType>
class DefaultConverter<SpectrumCoef<InputSpectrumType>, SpectrumCoef<OutputSpectrumType>>
  {
  public:
    static SpectrumCoef<OutputSpectrumType> Convert(const SpectrumCoef<InputSpectrumType> &i_input)
      {
      return ::Convert<OutputSpectrumType>(i_input);
      }
  };

#endif // IMAGE_TEXTURE_H
