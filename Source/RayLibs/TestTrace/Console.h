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

#ifndef CONSOLE_H
#define CONSOLE_H
#include <windows.h>
#include <io.h> 

#pragma warning(disable : 4311)

//---------------------------------------------------------------------
class Console {
public:
  Console() {
    m_curLine = 0;
    m_output = NULL;
    m_stdOutput = NULL;
    if (::AllocConsole()) {
      m_output = GetStdHandle(STD_ERROR_HANDLE);
      m_stdOutput = _fdopen(_open_osfhandle((long)m_output, 2/*_O_TEXT*/), "w" ); 
      if (m_stdOutput) {
        *stdout = *stderr = *m_stdOutput;
        }
      }
    }
  ~Console() {
      if (m_stdOutput) {
        fclose(m_stdOutput);
        }
      ::FreeConsole();
    }
  void Clear() {
      COORD startPos = {0, 0};
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      if (::GetConsoleScreenBufferInfo(m_output, &csbi)) {
        int count = csbi.dwSize.X * csbi.dwSize.Y;
        ::FillConsoleOutputCharacter(m_output, ' ', count, startPos, NULL);
        }
      SetCursorPos(0, 0);
      m_curLine = 0;
    }
  void Print(const char* format, ...) {
      va_list ap;
      va_start(ap, format);
      vprintf(format, ap);
      va_end(ap);
      SetCursorPos(0, ++m_curLine);
    }

private:
  void SetCursorPos(int x, int y) {
    COORD pos;
    pos.X = x;
    pos.Y = y;
    ::SetConsoleCursorPosition(m_output, pos);
    }

private:
  HANDLE m_output;
  FILE* m_stdOutput;
  int m_curLine;
  };

extern Console* g_console;

#endif // CONSOLE_H