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

#ifndef MAIN_TEST_H
#define MAIN_TEST_H

#include <cxxtest/TestSuite.h>

// This is a dummy test suite needed only to generate main() function 
// to run the other test suites.
class DummySuite : public CxxTest::TestSuite
  {
  public:
    void testDummy( void )
      {
      TS_ASSERT(true);
      }
  };

#endif // MAIN_TEST_H