/** \file
* \brief standard posix functions (part)
*
* \author Copyright 2016 Michael Fink <vividos@users.sf.net>
*
* \note
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* \note
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* \note
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301  USA
*/

#include "unistd.h"
#include <windows.h>

/// \see http://stackoverflow.com/questions/5801813/c-usleep-is-obsolete-workarounds-for-windows-mingw
int usleep(useconds_t usec)
{
   HANDLE timer;
   LARGE_INTEGER ft;

   ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

   timer = CreateWaitableTimer(NULL, TRUE, NULL);
   SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
   WaitForSingleObject(timer, INFINITE);
   CloseHandle(timer);

   return 0;
}

unsigned int sleep(unsigned int seconds)
{
   Sleep(seconds * 1000);
   return 0;
}
