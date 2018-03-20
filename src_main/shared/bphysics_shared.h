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
#pragma once

// bphysics_shared.h -- physics dll information visible to server.

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

/** Version numbers. */
#define PHYSICS_API_VERSION     1

/** Allocation tags. */
#define PHYSICS_ALLOC_TAG_BASE  400
#define PHYSICS_ALLOC_TAG_SETUP (PHYSICS_ALLOC_TAG_BASE + 1)

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

/** Shared includes. */
#include "q_shared.h"

///////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////

/** Physics API export. */
#define PHYSICS_API_PROC "GetPhysicsAPI"
typedef void* (__cdecl *GetPhysicsAPIFn)(void *);

///////////////////////////////////////////////////////////////////////////////
// Structs
///////////////////////////////////////////////////////////////////////////////

/** Engine -> Physics system interface. */
typedef struct
{
    // special messages
    void (*bprintf)             (int printlevel, char *fmt, ...);
    void (*dprintf)             (char *fmt, ...);

    // config strings hold all the index strings, the lightstyles,
    // and misc data like the sky definition and cdtrack.
    // All of the current configstrings are sent to clients when
    // they connect, and changes are sent to all connected clients.
    void (*configstring)        (int num, char *string);
    void (*error)               (char *fmt, ...);

    // managed memory allocation
    void* (*TagMalloc)          (int size, int tag);
    void  (*TagFree)            (void *block);
    void  (*FreeTags)           (int tag);

    // console variable interaction
    cvar_t* (*cvar)             (char *var_name, char *value, int flags);
    cvar_t* (*cvar_set)         (char *var_name, char *value);
    cvar_t* (*cvar_forceset)    (char *var_name, char *value);

    // ClientCommand and ServerCommand parameter access
    int   (*argc)               (void);
    char* (*argv)               (int n);
    char* (*args)               (void);	// concatenation of all argv >= 1
} physics_import_t;

/** Physics -> Engine API. */
typedef struct
{
    /** Version of the physics api. */
    int apiversion;

    // the init function will only be called when a game starts,
    // not each time a level is loaded. Persistant data for clients
    // and the server can be allocated in init
    void (__cdecl *Init)     (void);
    void (__cdecl *Shutdown) (void);
} physics_export_t;