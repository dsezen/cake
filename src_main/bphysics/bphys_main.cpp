/******************************************************************************
 * Copyright (C) 2018 Deniz Sezen
 ******************************************************************************
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ******************************************************************************
 */

///////////////////////////////////////////////////////////////////////////////
// Main physics module function.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

/** Standard library includes. */
#include <cstdarg>

extern "C"
{
    /** Shared code includes. */
    #include "q_shared.h"
    #include "bphysics_shared.h"
}

///////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////

/** List of exported functions and data from this module. */
physics_export_t globals;

/** List of imported functions from the engine. */
physics_import_t pi;

///////////////////////////////////////////////////////////////////////////////
// Forward declarations.
///////////////////////////////////////////////////////////////////////////////

/** Subsystem management. */
void InitPhysics(void);
void ShutdownPhysics(void);

///////////////////////////////////////////////////////////////////////////////
// Exported
///////////////////////////////////////////////////////////////////////////////

/**
 * GetPhysicsAPI
 *
 * Returns a pointer to the structure with all entry points and global
 * variables for this module.
 */
extern "C" Q_DLL_EXPORT physics_export_t* GetPhysicsAPI(physics_import_t* import)
{
    pi = *import;

    globals.apiversion = PHYSICS_API_VERSION;
    globals.Init = InitPhysics;
    globals.Shutdown = ShutdownPhysics;

    return &globals;
}

///////////////////////////////////////////////////////////////////////////////
// Module functions
///////////////////////////////////////////////////////////////////////////////

/**
 * InitPhysics
 * 
 * Initializes the physics system.
 */
void InitPhysics(void)
{

}

/**
 * ShutdownPhysics
 *
 * Cleans up and shuts down the physics system.
 */
void ShutdownPhysics(void)
{

}

///////////////////////////////////////////////////////////////////////////////
// Shared functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Com_Printf
 *
 * Prints data to the given string.
 */
extern "C" void Com_Printf(char *msg, ...)
{
    va_list		argptr;
    char		text[1024];

    va_start(argptr, msg);
    vsprintf(text, msg, argptr);
    va_end(argptr);

    pi.dprintf("%s", text);
}