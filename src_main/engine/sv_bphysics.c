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
// Includes
///////////////////////////////////////////////////////////////////////////////

/** Server includes */
#include "server.h"

///////////////////////////////////////////////////////////////////////////////
// External functions
///////////////////////////////////////////////////////////////////////////////
extern void PF_dprintf (char *fmt, ...);
extern void PF_Configstring (int index, char *val);
extern void PF_error (char *fmt, ...);

///////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////

/** Engine functions required by physics. */
physics_import_t* pe;

///////////////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Initializes imports for physics system.
 */
void SV_InitPhysProgs(void)
{
    // unload anything we have now
    physics_import_t import;
    if (pe) {
        SV_ShutdownPhysProgs();
    }

    // load a new game dll
    import.bprintf       = SV_BroadcastPrintf;
    import.dprintf       = PF_dprintf;

    import.configstring  = PF_Configstring;
    import.error         = PF_error;

    import.TagMalloc     = Z_TagMalloc;
    import.TagFree       = Z_Free;
    import.FreeTags      = Z_FreeTags;

    import.cvar          = Cvar_Get;
    import.cvar_set      = Cvar_Set;
    import.cvar_forceset = Cvar_ForceSet;

    import.argc          = Cmd_Argc;
    import.argv          = Cmd_Argv;
    import.args          = Cmd_Args;
}