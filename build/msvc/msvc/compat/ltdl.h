/** \file
* \brief libtool dynamic loading library implementation (part)
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
#pragma once

/* Note that this is only a basic implementation of functions needed by gPhoto */

typedef struct lt_dlhandle_struct* lt_dlhandle;

#define lt_ptr void*

int lt_dlinit();
int lt_dlexit();

int lt_dladdsearchdir(const char* search_dir);

int lt_dlforeachfile(
   const char* search_path,
   int(*func) (const char *filename, lt_ptr data),
   lt_ptr data);

lt_dlhandle lt_dlopenext(const char* filename);
lt_ptr lt_dlsym(lt_dlhandle handle, const char* name);

const char* lt_dlerror();
int lt_dlclose(lt_dlhandle handle);
