/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/
// vid.c -- Main windowed and fullscreen graphics interface module.
#include <assert.h>
#include <float.h>

#include "client.h"
#include "vid_common.h"

#include "SDL.h"

// Console variables that we need to access from this module
cvar_t		*vid_gamma;
cvar_t		*vid_fullscreen;

// Global variables used internally by this module
viddef_t	viddef;				// global video state; used by other modules
qboolean	reflib_active = 0;


vidmode_t vid_modes[] = {
	{ "Mode  0:  320x240", 320, 240, 0 },
	{ "Mode  1:  400x300", 400, 300, 1 },
	{ "Mode  2:  512x384", 512, 384, 2 },
	{ "Mode  3:  640x400", 640, 400, 3 },
	{ "Mode  4:  640x480", 640, 480, 4 },
	{ "Mode  5:  800x500", 800, 500, 5 },
	{ "Mode  6:  800x600", 800, 600, 6 },
	{ "Mode  7:  960x720", 960, 720, 7 },
	{ "Mode  8: 1024x480", 1024, 480, 8 },
	{ "Mode  9: 1024x640", 1024, 640, 9 },
	{ "Mode 10: 1024x768", 1024, 768, 10 },
	{ "Mode 11: 1152x768", 1152, 768, 11 },
	{ "Mode 12: 1152x864", 1152, 864, 12 },
	{ "Mode 13: 1280x800", 1280, 800, 13 },
	{ "Mode 14: 1280x720", 1280, 720, 14 },
	{ "Mode 15: 1280x960", 1280, 960, 15 },
	{ "Mode 16: 1280x1024", 1280, 1024, 16 },
	{ "Mode 17: 1366x768", 1366, 768, 17 },
	{ "Mode 18: 1440x900", 1440, 900, 18 },
	{ "Mode 19: 1600x1200", 1600, 1200, 19 },
	{ "Mode 20: 1680x1050", 1680, 1050, 20 },
	{ "Mode 21: 1920x1080", 1920, 1080, 21 },
	{ "Mode 22: 1920x1200", 1920, 1200, 22 },
	{ "Mode 23: 2048x1536", 2048, 1536, 23 },
};

#define VID_NUM_MODES (sizeof(vid_modes) / sizeof(vid_modes[0]))


/*
==========================================================================

DLL GLUE

==========================================================================
*/

#define	MAXPRINTMSG	4096

void VID_Printf (int print_level, char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];
	static qboolean	inupdate;

	va_start (argptr, fmt);
	vsprintf (msg, fmt, argptr);
	va_end (argptr);

	if (print_level == PRINT_ALL)
	{
		Com_Printf ("%s", msg);
	}
	else if (print_level == PRINT_DEVELOPER)
	{
		Com_DPrintf ("%s", msg);
	}
	else if (print_level == PRINT_ALERT)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Alert", msg, NULL);
	}
}

void VID_Error (int err_level, char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];
	static qboolean	inupdate;

	va_start (argptr, fmt);
	vsprintf (msg, fmt, argptr);
	va_end (argptr);

	Com_Error (err_level, "%s", msg);
}

//==========================================================================

/*
============
VID_Restart_f

Console command to re-start the video mode and refresh DLL.
============
*/
void VID_Restart_f (void)
{
	extern qboolean ref_modified;
	ref_modified = true;
}

/*
============
VID_ListModes_f

Console command to list out video modes
============
*/
void VID_ListModes_f(void)
{
	int i;
	Com_Printf("Supported video modes (gl_mode):\n");
	for (i = 0; i<VID_NUM_MODES; ++i)
	{
		Com_Printf("  %s\n", vid_modes[i].description);
	}
	Com_Printf("  Mode -1: gl_customwidth x gl_customheight\n");
}

/*
==============
VID_GetModeInfo
==============
*/
qboolean VID_GetModeInfo (int *width, int *height, int mode)
{
	if ((mode < 0) || (mode >= VID_NUM_MODES))
	{
		return false;
	}

	*width = vid_modes[mode].width;
	*height = vid_modes[mode].height;

	return true;
}

/*
==============
VID_NewWindow
==============
*/
void VID_NewWindow (int width, int height)
{
	viddef.width = width;
	viddef.height = height;

	cl.force_refdef = true;		// can't use a paused refdef
}

void VID_FreeReflib (void)
{
	reflib_active = false;
}

/*
==============
VID_LoadRefresh
==============
*/
qboolean VID_LoadRefresh (void)
{
	if (reflib_active)
	{
		R_Shutdown ();
		VID_FreeReflib ();
	}

	Swap_Init ();

	if (R_Init() == -1)
	{
		R_Shutdown ();
		VID_FreeReflib ();
		return false;
	}

	// Ensure that all key states are cleared
	Key_ClearStates();

	Com_Printf ("------------------------------------\n");
	reflib_active = true;

	return true;
}

/*
============
VID_CheckChanges

This function gets called once just before drawing each frame, and it's sole purpose in life
is to check to see if any of the video mode parameters have changed, and if they have to
update the rendering DLL and/or video mode to match.
============
*/

// this must be initialized to true so that the first frame will init the ref properly
qboolean ref_modified = true;

void VID_CheckChanges (void)
{
	if (ref_modified)
	{
		cl.force_refdef = true;		// can't use a paused refdef
		S_StopAllSounds ();
		BGM_Stop ();
	}

	while (ref_modified)
	{
		// refresh has changed
		ref_modified = false;
		vid_fullscreen->modified = true;
		cl.refresh_prepped = false;
		cl.cinematicpalette_active = false;
		cls.disable_screen = true;

		if (!VID_LoadRefresh ())
		{
			Com_Error (ERR_FATAL, "Couldn't load OpenGL refresh!");
		}

		cls.disable_screen = false;
	}
}


/*
============
VID_Init
============
*/
void VID_Init (void)
{
	// Create the video variables so we know how to start the graphics drivers
	vid_fullscreen = Cvar_Get ("vid_fullscreen", "0", CVAR_ARCHIVE);
	vid_gamma = Cvar_Get ("vid_gamma", "1.2", CVAR_ARCHIVE);

	// Add some console commands that we want to handle
	Cmd_AddCommand ("vid_restart", VID_Restart_f);
	Cmd_AddCommand ("vid_listmodes", VID_ListModes_f);

	// Start the graphics mode and load refresh
	VID_CheckChanges ();
}

/*
============
VID_Shutdown
============
*/
void VID_Shutdown (void)
{
	if (reflib_active)
	{
		R_Shutdown ();
		VID_FreeReflib ();
	}
}
