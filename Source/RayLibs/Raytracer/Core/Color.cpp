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

#include "Color.h"

const ColorSystem global_sRGB_D65_ColorSystem(Point2D_d(0.64, 0.33), Point2D_d(0.3, 0.6), Point2D_d(0.15, 0.06), Point2D_d(0.3127, 0.3290), 2.2);
const ColorSystem global_sRGB_E_ColorSystem(Point2D_d(0.64, 0.33), Point2D_d(0.3, 0.6), Point2D_d(0.15, 0.06), Point2D_d(1.0, 1.0)/3.0, 2.2);