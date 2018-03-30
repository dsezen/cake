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
#include "q_shared.h"

vec3_t vec3_origin = {0, 0, 0};

//============================================================================

#ifdef _WIN32
#pragma optimize( "", off )
#endif

void RotatePointAroundVector (vec3_t dst, const vec3_t dir, const vec3_t point, float degrees)
{
	float	m[3][3];
	float	im[3][3];
	float	zrot[3][3];
	float	tmpmat[3][3];
	float	rot[3][3];
	int	i;
	vec3_t vr, vup, vf;
	float	radians = DEG2RAD (degrees);

	vf[0] = dir[0];
	vf[1] = dir[1];
	vf[2] = dir[2];

	PerpendicularVector (vr, dir);
	CrossProduct (vr, vf, vup);

	m[0][0] = vr[0];
	m[1][0] = vr[1];
	m[2][0] = vr[2];

	m[0][1] = vup[0];
	m[1][1] = vup[1];
	m[2][1] = vup[2];

	m[0][2] = vf[0];
	m[1][2] = vf[1];
	m[2][2] = vf[2];

	memcpy (im, m, sizeof (im));

	im[0][1] = m[1][0];
	im[0][2] = m[2][0];
	im[1][0] = m[0][1];
	im[1][2] = m[2][1];
	im[2][0] = m[0][2];
	im[2][1] = m[1][2];

	memset (zrot, 0, sizeof (zrot));
	zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;

	Q_sincos (radians, &zrot[0][1], &zrot[0][0]);
	Q_sincos (radians, &zrot[1][0], &zrot[1][1]);

	zrot[1][0] = -zrot[1][0];

	R_ConcatRotations (m, zrot, tmpmat);
	R_ConcatRotations (tmpmat, im, rot);

	for (i = 0; i < 3; i++)
	{
		dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
	}
}

#ifdef _WIN32
#pragma optimize( "", on )
#endif



void AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
{
	float		angle;
	static float		sr, sp, sy, cr, cp, cy;
	// static to help MS compiler fp bugs

	angle = angles[YAW] * (M_PI * 2 / 360);
	Q_sincos (angle, &sy, &cy);

	angle = angles[PITCH] * (M_PI * 2 / 360);
	Q_sincos (angle, &sp, &cp);

	angle = angles[ROLL] * (M_PI * 2 / 360);
	Q_sincos (angle, &sr, &cr);

	if (forward)
	{
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}

	if (right)
	{
		right[0] = (-1 * sr * sp * cy + -1 * cr * -sy);
		right[1] = (-1 * sr * sp * sy + -1 * cr * cy);
		right[2] = -1 * sr * cp;
	}

	if (up)
	{
		up[0] = (cr * sp * cy + -sr * -sy);
		up[1] = (cr * sp * sy + -sr * cy);
		up[2] = cr * cp;
	}
}


void ProjectPointOnPlane (vec3_t dst, const vec3_t p, const vec3_t normal)
{
	float d;
	vec3_t n;
	float inv_denom;

	inv_denom = 1.0F / DotProduct (normal, normal);

	d = DotProduct (normal, p) * inv_denom;

	n[0] = normal[0] * inv_denom;
	n[1] = normal[1] * inv_denom;
	n[2] = normal[2] * inv_denom;

	dst[0] = p[0] - d * n[0];
	dst[1] = p[1] - d * n[1];
	dst[2] = p[2] - d * n[2];
}

/*
assumes "src" is normalized
*/
void PerpendicularVector (vec3_t dst, const vec3_t src)
{
	int	pos;
	int i;
	float minelem = 1.0F;
	vec3_t tempvec;

	// find the smallest magnitude axially aligned vector
	for (pos = 0, i = 0; i < 3; i++)
	{
		if (fabs (src[i]) < minelem)
		{
			pos = i;
			minelem = fabs (src[i]);
		}
	}

	tempvec[0] = tempvec[1] = tempvec[2] = 0.0F;
	tempvec[pos] = 1.0F;

	// project the point onto the plane defined by src
	ProjectPointOnPlane (dst, tempvec, src);

	// normalize the result
	VectorNormalize (dst);
}



/*
================
R_ConcatRotations
================
*/
void R_ConcatRotations (float in1[3][3], float in2[3][3], float out[3][3])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
}


/*
================
R_ConcatTransforms
================
*/
void R_ConcatTransforms (float in1[3][4], float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
				in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
				in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
				in1[2][2] * in2[2][3] + in1[2][3];
}


//============================================================================

float lerp(const float from, const float to, float f)
{
	return from + ((to - from) * f);
}

float clamp(float minf, float maxf, float value)
{
	return max(minf, min(maxf, value));
}

float Q_fabs (float f)
{
#if 0

	if (f >= 0)
		return f;

	return -f;
#else
	int tmp = * (int *) &f;
	tmp &= 0x7FFFFFFF;
	return * (float *) &tmp;
#endif
}

#if defined _M_IX86 && !defined C_ONLY
#pragma warning (disable:4035)
__declspec (naked) long Q_ftol (float f)
{
	static int tmp;
	__asm fld dword ptr [esp+4]
	__asm fistp tmp
	__asm mov eax, tmp
	__asm ret
}
#pragma warning (default:4035)
#endif

#if defined _M_IX86 && !defined C_ONLY
__declspec (naked) void Q_sincos (float angradians, float *angsin, float *angcos)
{
	__asm fld dword ptr [esp + 4]
	__asm fsincos

	__asm mov ebx, [esp + 12]
	__asm fstp dword ptr [ebx]

	__asm mov ebx, [esp + 8]
	__asm fstp dword ptr [ebx]

	__asm ret
}
#else
void Q_sincos (float angle, float *s, float *c)
{
	*s = (float)sin(angle);
	*c = (float)cos(angle);
}
#endif


/*
===============
LerpAngle

===============
*/
float LerpAngle (float a2, float a1, float frac)
{
	if (a1 - a2 > 180)
		a1 -= 360;

	if (a1 - a2 < -180)
		a1 += 360;

	return a2 + frac * (a1 - a2);
}


float	anglemod (float a)
{
#if 0

	if (a >= 0)
		a -= 360 * (int) (a / 360);
	else
		a += 360 * (1 + (int) (-a / 360));

#endif
	a = (360.0 / 65536) * ((int) (a * (65536 / 360.0)) & 65535);
	return a;
}

int		i;
vec3_t	corners[2];


// this is the slow, general version
int BoxOnPlaneSide2 (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	int		i;
	float	dist1, dist2;
	int		sides;
	vec3_t	corners[2];

	for (i = 0; i < 3; i++)
	{
		corners[0][i] = (p->normal[i] < 0) ? emins[i] : emaxs[i];
		corners[1][i] = (p->normal[i] < 0) ? emaxs[i] : emins[i];
	}

	dist1 = DotProduct (p->normal, corners[0]) - p->dist;
	dist2 = DotProduct (p->normal, corners[1]) - p->dist;
	sides = 0;

	if (dist1 >= 0) sides = 1;
	if (dist2 < 0) sides |= 2;

	return sides;
}

/*
==================
BoxOnPlaneSide

Returns 1, 2, or 1 + 2
==================
*/
#if !id386 || defined __linux__
int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	float	dist1, dist2;
	int		sides;

	// fast axial cases
	if (p->type < 3)
	{
		if (p->dist <= emins[p->type])
			return 1;

		if (p->dist >= emaxs[p->type])
			return 2;

		return 3;
	}

	// general case
	switch (p->signbits)
	{
	case 0:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		break;
	case 1:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		break;
	case 2:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		break;
	case 3:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		break;
	case 4:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		break;
	case 5:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		break;
	case 6:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		break;
	case 7:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		break;
	default:
		dist1 = dist2 = 0;		// shut up compiler
		assert (0);
		break;
	}

	sides = 0;

	if (dist1 >= p->dist)
		sides = 1;

	if (dist2 < p->dist)
		sides |= 2;

	assert (sides != 0);

	return sides;
}
#else
#pragma warning( disable: 4035 )

__declspec (naked) int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	static int bops_initialized;
	static int Ljmptab[8];

	__asm
	{

		push ebx

		cmp bops_initialized, 1
		je initialized
		mov bops_initialized, 1

		mov Ljmptab[0*4], offset Lcase0
		mov Ljmptab[1*4], offset Lcase1
		mov Ljmptab[2*4], offset Lcase2
		mov Ljmptab[3*4], offset Lcase3
		mov Ljmptab[4*4], offset Lcase4
		mov Ljmptab[5*4], offset Lcase5
		mov Ljmptab[6*4], offset Lcase6
		mov Ljmptab[7*4], offset Lcase7

		initialized:

		mov edx, ds:dword ptr[4+12+esp]
		mov ecx, ds:dword ptr[4+4+esp]
		xor eax, eax
		mov ebx, ds:dword ptr[4+8+esp]
		mov al, ds:byte ptr[17+edx]
		cmp al, 8
		jge Lerror
		fld ds:dword ptr[0+edx]
		fld st (0)
		jmp dword ptr[Ljmptab+eax*4]
		Lcase0:
		fmul ds:dword ptr[ebx]
		fld ds:dword ptr[0+4+edx]
		fxch st (2)
		fmul ds:dword ptr[ecx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[4+ebx]
		fld ds:dword ptr[0+8+edx]
		fxch st (2)
		fmul ds:dword ptr[4+ecx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[8+ebx]
		fxch st (5)
		faddp st (3), st (0)
		fmul ds:dword ptr[8+ecx]
		fxch st (1)
		faddp st (3), st (0)
		fxch st (3)
		faddp st (2), st (0)
		jmp LSetSides
		Lcase1:
		fmul ds:dword ptr[ecx]
		fld ds:dword ptr[0+4+edx]
		fxch st (2)
		fmul ds:dword ptr[ebx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[4+ebx]
		fld ds:dword ptr[0+8+edx]
		fxch st (2)
		fmul ds:dword ptr[4+ecx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[8+ebx]
		fxch st (5)
		faddp st (3), st (0)
		fmul ds:dword ptr[8+ecx]
		fxch st (1)
		faddp st (3), st (0)
		fxch st (3)
		faddp st (2), st (0)
		jmp LSetSides
		Lcase2:
		fmul ds:dword ptr[ebx]
		fld ds:dword ptr[0+4+edx]
		fxch st (2)
		fmul ds:dword ptr[ecx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[4+ecx]
		fld ds:dword ptr[0+8+edx]
		fxch st (2)
		fmul ds:dword ptr[4+ebx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[8+ebx]
		fxch st (5)
		faddp st (3), st (0)
		fmul ds:dword ptr[8+ecx]
		fxch st (1)
		faddp st (3), st (0)
		fxch st (3)
		faddp st (2), st (0)
		jmp LSetSides
		Lcase3:
		fmul ds:dword ptr[ecx]
		fld ds:dword ptr[0+4+edx]
		fxch st (2)
		fmul ds:dword ptr[ebx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[4+ecx]
		fld ds:dword ptr[0+8+edx]
		fxch st (2)
		fmul ds:dword ptr[4+ebx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[8+ebx]
		fxch st (5)
		faddp st (3), st (0)
		fmul ds:dword ptr[8+ecx]
		fxch st (1)
		faddp st (3), st (0)
		fxch st (3)
		faddp st (2), st (0)
		jmp LSetSides
		Lcase4:
		fmul ds:dword ptr[ebx]
		fld ds:dword ptr[0+4+edx]
		fxch st (2)
		fmul ds:dword ptr[ecx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[4+ebx]
		fld ds:dword ptr[0+8+edx]
		fxch st (2)
		fmul ds:dword ptr[4+ecx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[8+ecx]
		fxch st (5)
		faddp st (3), st (0)
		fmul ds:dword ptr[8+ebx]
		fxch st (1)
		faddp st (3), st (0)
		fxch st (3)
		faddp st (2), st (0)
		jmp LSetSides
		Lcase5:
		fmul ds:dword ptr[ecx]
		fld ds:dword ptr[0+4+edx]
		fxch st (2)
		fmul ds:dword ptr[ebx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[4+ebx]
		fld ds:dword ptr[0+8+edx]
		fxch st (2)
		fmul ds:dword ptr[4+ecx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[8+ecx]
		fxch st (5)
		faddp st (3), st (0)
		fmul ds:dword ptr[8+ebx]
		fxch st (1)
		faddp st (3), st (0)
		fxch st (3)
		faddp st (2), st (0)
		jmp LSetSides
		Lcase6:
		fmul ds:dword ptr[ebx]
		fld ds:dword ptr[0+4+edx]
		fxch st (2)
		fmul ds:dword ptr[ecx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[4+ecx]
		fld ds:dword ptr[0+8+edx]
		fxch st (2)
		fmul ds:dword ptr[4+ebx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[8+ecx]
		fxch st (5)
		faddp st (3), st (0)
		fmul ds:dword ptr[8+ebx]
		fxch st (1)
		faddp st (3), st (0)
		fxch st (3)
		faddp st (2), st (0)
		jmp LSetSides
		Lcase7:
		fmul ds:dword ptr[ecx]
		fld ds:dword ptr[0+4+edx]
		fxch st (2)
		fmul ds:dword ptr[ebx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[4+ecx]
		fld ds:dword ptr[0+8+edx]
		fxch st (2)
		fmul ds:dword ptr[4+ebx]
		fxch st (2)
		fld st (0)
		fmul ds:dword ptr[8+ecx]
		fxch st (5)
		faddp st (3), st (0)
		fmul ds:dword ptr[8+ebx]
		fxch st (1)
		faddp st (3), st (0)
		fxch st (3)
		faddp st (2), st (0)
		LSetSides:
		faddp st (2), st (0)
		fcomp ds:dword ptr[12+edx]
		xor ecx, ecx
		fnstsw ax
		fcomp ds:dword ptr[12+edx]
		and ah, 1
		xor ah, 1
		add cl, ah
		fnstsw ax
		and ah, 1
		add ah, ah
		add cl, ah
		pop ebx
		mov eax, ecx
		ret
		Lerror:
		int 3
	}
}
#pragma warning( default: 4035 )
#endif

void ClearBounds (vec3_t mins, vec3_t maxs)
{
	mins[0] = mins[1] = mins[2] = 9999999;
	maxs[0] = maxs[1] = maxs[2] = -9999999;
}

void AddPointToBounds (vec3_t v, vec3_t mins, vec3_t maxs)
{
	int		i;
	vec_t	val;

	for (i = 0; i < 3; i++)
	{
		val = v[i];

		if (val < mins[i])
			mins[i] = val;

		if (val > maxs[i])
			maxs[i] = val;
	}
}


int VectorCompare (vec3_t v1, vec3_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2])
		return 0;

	return 1;
}


vec_t VectorNormalize (vec3_t v)
{
	float	length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrt (length);		// FIXME

	if (length)
	{
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}

vec_t VectorNormalize2 (vec3_t v, vec3_t out)
{
	float	length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrt (length);		// FIXME

	if (length)
	{
		ilength = 1 / length;
		out[0] = v[0] * ilength;
		out[1] = v[1] * ilength;
		out[2] = v[2] * ilength;
	}

	return length;
}

void VectorMA (vec3_t addvec, float scale, vec3_t mulvec, vec3_t out)
{
	out[0] = mulvec[0] * scale + addvec[0];
	out[1] = mulvec[1] * scale + addvec[1];
	out[2] = mulvec[2] * scale + addvec[2];
}


vec_t _DotProduct (vec3_t v1, vec3_t v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void _VectorSubtract (vec3_t veca, vec3_t vecb, vec3_t out)
{
	out[0] = veca[0] - vecb[0];
	out[1] = veca[1] - vecb[1];
	out[2] = veca[2] - vecb[2];
}

void _VectorAdd (vec3_t veca, vec3_t vecb, vec3_t out)
{
	out[0] = veca[0] + vecb[0];
	out[1] = veca[1] + vecb[1];
	out[2] = veca[2] + vecb[2];
}

void _VectorCopy (vec3_t in, vec3_t out)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

void CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross)
{
	cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
	cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
	cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

double sqrt (double x);

vec_t VectorLength (vec3_t v)
{
	int		i;
	float	length;

	length = 0;

	for (i = 0; i < 3; i++)
		length += v[i] * v[i];

	length = sqrt (length);		// FIXME

	return length;
}

void VectorInverse (vec3_t v)
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

void VectorScale (vec3_t in, vec_t scale, vec3_t out)
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}


int Q_log (int val)
{
	int answer = 0;

	while (val >>= 1)
		answer++;

	return answer;
}

float Q_Clamp(float min, float max, float value)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

//====================================================================================

/*
============
COM_SkipPath
============
*/
char *COM_SkipPath (char *pathname)
{
	char	*last;

	last = pathname;

	while (*pathname)
	{
		if (*pathname == '/')
			last = pathname + 1;

		pathname++;
	}

	return last;
}

/*
============
COM_StripExtension
============
*/
void COM_StripExtension (char *in, char *out)
{
	while (*in && *in != '.')
		*out++ = *in++;

	*out = 0;
}

/*
============
COM_StripExtensionSafe
============
*/
void COM_StripExtensionSafe (const char *in, char *out, int destsize)
{
	const char *dot = strrchr(in, '.'), *slash;

	if (dot && (!(slash = strrchr(in, '/')) || slash < dot))
		destsize = (destsize < dot - in + 1 ? destsize : dot - in + 1);

	if (in == out && destsize > 1)
		out[destsize - 1] = '\0';
	else
		Q_strlcpy (out, in, destsize);
}

/*
===========
COM_StripPathFromFilename
===========
*/
char *COM_StripPathFromFilename(const char *in)
{
	char *s = strrchr(in, '/');
	if (s == NULL)
		return strdup(in);
	else
		return strdup(s + 1);
}

/*
============
COM_FileExtension
============
*/
char *COM_FileExtension (char *in)
{
	static char exten[8];
	int		i;

	while (*in && *in != '.')
		in++;

	if (!*in)
		return "";

	in++;

	for (i = 0; i < 7 && *in; i++, in++)
		exten[i] = *in;

	exten[i] = 0;
	return exten;
}

/*
============
COM_FileBase
============
*/
void COM_FileBase (char *in, char *out)
{
	char *s, *s2;

	if (!*in) {
		*out = 0;
		return;
	}
	s = in + strlen (in) - 1;

	while (s != in && *s != '.')
		s--;

	for (s2 = s; s2 != in && *s2 != '/'; s2--)
		;

	if (s - s2 < 2)
		out[0] = 0;
	else
	{
		s--;
		strncpy (out, s2 + 1, s - s2);
		out[s-s2] = 0;
	}
}

/*
==================
COM_DefaultExtension
==================
*/
void COM_DefaultExtension (char *path, char *extension)
{
	char  *src;

	// if path doesn't have a .EXT, append extension
	// (extension should include the .)
	if (!*path)
		return;

	src = path + strlen (path) - 1;

	while (*src != '/' && src != path)
	{
		if (*src == '.')
			return;         // it has an extension

		src--;
	}

	strcat (path, extension);
}

/*
============================================================================

					BYTE ORDER FUNCTIONS

============================================================================
*/

qboolean	bigendien;

// can't just use function pointers, or dll linkage can
// mess up when qcommon is included in multiple places
short	(*_BigShort) (short l);
short	(*_LittleShort) (short l);
int	(*_BigLong) (int l);
int	(*_LittleLong) (int l);
float	(*_BigFloat) (float l);
float	(*_LittleFloat) (float l);

short	BigShort (short l) {return _BigShort (l);}
short	LittleShort (short l) {return _LittleShort (l);}
int		BigLong (int l) {return _BigLong (l);}
int		LittleLong (int l) {return _LittleLong (l);}
float	BigFloat (float l) {return _BigFloat (l);}
float	LittleFloat (float l) {return _LittleFloat (l);}

short  ShortSwap (short l)
{
	byte  b1, b2;

	b1 = l & 255;
	b2 = (l >> 8) & 255;

	return (b1 << 8) + b2;
}

short	ShortNoSwap (short l)
{
	return l;
}

int  LongSwap (int l)
{
	byte  b1, b2, b3, b4;

	b1 = l & 255;
	b2 = (l >> 8) & 255;
	b3 = (l >> 16) & 255;
	b4 = (l >> 24) & 255;

	return ((int) b1 << 24) + ((int) b2 << 16) + ((int) b3 << 8) + b4;
}

int	LongNoSwap (int l)
{
	return l;
}

float FloatSwap (float f)
{
	union
	{
		float	f;
		byte	b[4];
	} dat1, dat2;


	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

float FloatNoSwap (float f)
{
	return f;
}

/*
================
Swap_Init
================
*/
void Swap_Init (void)
{
	byte	swaptest[2] = {1, 0};

	// set the byte swapping variables in a portable manner
	if (*(short *) swaptest == 1)
	{
		bigendien = false;
		_BigShort = ShortSwap;
		_LittleShort = ShortNoSwap;
		_BigLong = LongSwap;
		_LittleLong = LongNoSwap;
		_BigFloat = FloatSwap;
		_LittleFloat = FloatNoSwap;
	}
	else
	{
		bigendien = true;
		_BigShort = ShortNoSwap;
		_LittleShort = ShortSwap;
		_BigLong = LongNoSwap;
		_LittleLong = LongSwap;
		_BigFloat = FloatNoSwap;
		_LittleFloat = FloatSwap;
	}

}



/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char	*va (char *format, ...)
{
	va_list		argptr;
	static char		string[1024];

	va_start (argptr, format);
	vsprintf (string, format, argptr);
	va_end (argptr);

	return string;
}


char	com_token[MAX_TOKEN_CHARS];

/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *COM_Parse (char **data_p)
{
	int		c;
	int		len;
	char	*data;

	data = *data_p;
	len = 0;
	com_token[0] = 0;

	if (!data)
	{
		*data_p = NULL;
		return "";
	}

	// skip whitespace
skipwhite:

	while ((c = *data) <= ' ')
	{
		if (c == 0)
		{
			*data_p = NULL;
			return "";
		}

		data++;
	}

	// skip // comments
	if (c == '/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;

		goto skipwhite;
	}

	// handle quoted strings specially
	if (c == '\"')
	{
		data++;

		while (1)
		{
			c = *data++;

			if (c == '\"' || !c)
			{
				com_token[len] = 0;
				*data_p = data;
				return com_token;
			}

			if (len < MAX_TOKEN_CHARS)
			{
				com_token[len] = c;
				len++;
			}
		}
	}

	// parse a regular word
	do
	{
		if (len < MAX_TOKEN_CHARS)
		{
			com_token[len] = c;
			len++;
		}

		data++;
		c = *data;
	}
	while (c > 32);

	if (len == MAX_TOKEN_CHARS)
	{
		//		Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
		len = 0;
	}

	com_token[len] = 0;

	*data_p = data;
	return com_token;
}

/*
==================
Com_CharIsOneOfCharset
==================
*/
static qboolean Com_CharIsOneOfCharset(char c, char *set)
{
	int i;

	for (i = 0; i < strlen(set); i++)
	{
		if (set[i] == c)
			return true;
	}

	return false;
}

/*
==================
Com_SkipCharset
==================
*/
char *Com_SkipCharset(char *s, char *sep)
{
	char	*p = s;

	while (p)
	{
		if (Com_CharIsOneOfCharset(*p, sep))
			p++;
		else
			break;
	}

	return p;
}

/*
==================
Com_SkipTokens
==================
*/
char *Com_SkipTokens(char *s, int numTokens, char *sep)
{
	int		sepCount = 0;
	char	*p = s;

	while (sepCount < numTokens)
	{
		if (Com_CharIsOneOfCharset(*p++, sep))
		{
			sepCount++;
			while (Com_CharIsOneOfCharset(*p, sep))
				p++;
		}
		else if (*p == '\0')
			break;
	}

	if (sepCount == numTokens)
		return p;
	else
		return s;
}

/*
===============
Com_PageInMemory
===============
*/
int	paged_total;

void Com_PageInMemory (byte *buffer, int size)
{
	int		i;

	for (i = size - 1; i > 0; i -= 4096)
		paged_total += buffer[i];
}



/*
============================================================================

					LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/

void Q_getwd(char *out)
{
#ifdef _WIN32
	_getcwd(out, sizeof(out));
	strcat(out, "\\");
#else
	getwd(out);
	strcat(out, "/");
#endif
}

// FIXME: replace all Q_stricmp with Q_strcasecmp
int Q_stricmp (char *s1, char *s2)
{
#if defined(_WIN32)
	return _stricmp (s1, s2);
#else
	return strcasecmp (s1, s2);
#endif
}

int Q_stricmpn(const char *s1, const char *s2, int n)
{
	int		c1, c2;

	if (s1 == NULL)
	{
		if (s2 == NULL)
			return 0;
		else
			return -1;
	}
	else if (s2 == NULL)
	{
		return 1;
	}

	do {
		c1 = *s1++;
		c2 = *s2++;

		if (!n--)
			return 0; // strings are equal until end point

		if (c1 != c2)
		{
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= ('a' - 'A');
			if (c2 >= 'a' && c2 <= 'z')
				c2 -= ('a' - 'A');

			if (c1 != c2)
				return c1 < c2 ? -1 : 1;
		}
	} while (c1);

	return 0; // strings are equal
}

int Q_strncasecmp (char *s1, char *s2, int n)
{
	int		c1, c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (!n--)
			return 0;		// strings are equal until end point

		if (c1 != c2)
		{
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= ('a' - 'A');

			if (c2 >= 'a' && c2 <= 'z')
				c2 -= ('a' - 'A');

			if (c1 != c2)
				return -1;		// strings not equal
		}
	}
	while (c1);

	return 0;		// strings are equal
}

int Q_strcasecmp (char *s1, char *s2)
{
	return Q_strncasecmp (s1, s2, 99999);
}

void Com_sprintf (char *dest, int size, char *fmt, ...)
{
	int		len;
	va_list		argptr;
	char	bigbuffer[0x10000];

	va_start (argptr, fmt);
	len = vsprintf (bigbuffer, fmt, argptr);
	va_end (argptr);

	if (len >= size)
		Com_Printf ("Com_sprintf: overflow of %i in %i\n", len, size);

	strncpy (dest, bigbuffer, size - 1);
}

void Com_strcpy(char *dest, int destSize, const char *src)
{
	if (!dest)
	{
		Com_Printf("Com_strcpy: NULL dst\n");
		return;
	}
	if (!src)
	{
		Com_Printf("Com_strcpy: NULL src\n");
		return;
	}
	if (destSize < 1)
	{
		Com_Printf("Com_strcpy: dstSize < 1\n");
		return;
	}

	strncpy(dest, src, destSize - 1);
	dest[destSize - 1] = 0;
}

void Com_strcat(char *dest, int destSize, const char *src)
{
	if (!dest)
	{
		Com_Printf("Com_strcat: NULL dst\n");
		return;
	}
	if (!src)
	{
		Com_Printf("Com_strcat: NULL src\n");
		return;
	}
	if (destSize < 1)
	{
		Com_Printf("Com_strcat: dstSize < 1\n");
		return;
	}

	while (--destSize && *dest)
		dest++;

	if (destSize > 0)
	{
		while (--destSize && *src)
			*dest++ = *src++;

		*dest = 0;
	}
}


char *Q_strlwr(char *s)
{
	char *p = s;

	while (*s)
	{
		*s = tolower(*s);
		s++;
	}

	return (p);
}

int Q_strlcpy(char *dst, const char *src, int size)
{
	const char *s = src;

	while (*s)
	{
		if (size > 1)
		{
			*dst++ = *s;
			size--;
		}
		s++;
	}
	if (size > 0)
	{
		*dst = '\0';
	}

	return s - src;
}

int Q_strlcat(char *dst, const char *src, int size)
{
	char *d = dst;

	while (size > 0 && *d)
	{
		size--;
		d++;
	}

	return (d - dst) + Q_strlcpy(d, src, size);
}

/*
=====================================================================

 INFO STRINGS

=====================================================================
*/

/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
===============
*/
char *Info_ValueForKey (char *s, char *key)
{
	char	pkey[512];
	static	char value[2][512];	// use two buffers so compares
	// work without stomping on each other
	static	int	valueindex;
	char	*o;

	valueindex ^= 1;

	if (*s == '\\')
		s++;

	while (1)
	{
		o = pkey;

		while (*s != '\\')
		{
			if (!*s)
				return "";

			*o++ = *s++;
		}

		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			if (!*s)
				return "";

			*o++ = *s++;
		}

		*o = 0;

		if (!strcmp (key, pkey))
			return value[valueindex];

		if (!*s)
			return "";

		s++;
	}
}

void Info_RemoveKey (char *s, char *key)
{
	char	*start;
	char	pkey[512];
	char	value[512];
	char	*o;

	if (strstr (key, "\\"))
	{
		//		Com_Printf ("Can't use a key with a \\\n");
		return;
	}

	while (1)
	{
		start = s;

		if (*s == '\\')
			s++;

		o = pkey;

		while (*s != '\\')
		{
			if (!*s)
				return;

			*o++ = *s++;
		}

		*o = 0;
		s++;

		o = value;

		while (*s != '\\' && *s)
		{
			if (!*s)
				return;

			*o++ = *s++;
		}

		*o = 0;

		if (!strcmp (key, pkey))
		{
			strcpy (start, s);	// remove this part
			return;
		}

		if (!*s)
			return;
	}

}


/*
==================
Info_Validate

Some characters are illegal in info strings because they
can mess up the server's parsing
==================
*/
qboolean Info_Validate (char *s)
{
	if (strstr (s, "\""))
		return false;

	if (strstr (s, ";"))
		return false;

	return true;
}

void Info_SetValueForKey (char *s, char *key, char *value)
{
	char	newi[MAX_INFO_STRING], *v;
	int		c;
	int		maxsize = MAX_INFO_STRING;

	if (strstr (key, "\\") || strstr (value, "\\"))
	{
		Com_Printf ("Can't use keys or values with a \\\n");
		return;
	}

	if (strstr (key, ";"))
	{
		Com_Printf ("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strstr (key, "\"") || strstr (value, "\""))
	{
		Com_Printf ("Can't use keys or values with a \"\n");
		return;
	}

	if (strlen (key) > MAX_INFO_KEY - 1 || strlen (value) > MAX_INFO_KEY - 1)
	{
		Com_Printf ("Keys and values must be < 64 characters.\n");
		return;
	}

	Info_RemoveKey (s, key);

	if (!value || !strlen (value))
		return;

	Com_sprintf (newi, sizeof (newi), "\\%s\\%s", key, value);

	if (strlen (newi) + strlen (s) > maxsize)
	{
		Com_Printf ("Info string length exceeded\n");
		return;
	}

	// only copy ascii values
	s += strlen (s);
	v = newi;

	while (*v)
	{
		c = *v++;
		c &= 127;		// strip high bits

		if (c >= 32 && c < 127)
			*s++ = c;
	}

	*s = 0;
}

//====================================================================

/*
===============
glob_match_after_star

Like glob_match, but match PATTERN against any final segment of TEXT.
===============
*/
static int glob_match_after_star(char *pattern, char *text)
{
	register char *p = pattern, *t = text;
	register char c, c1;

	while ((c = *p++) == '?' || c == '*')
	{
		if ((c == '?') && (*t++ == '\0'))
		{
			return 0;
		}
	}

	if (c == '\0')
	{
		return 1;
	}

	if (c == '\\')
	{
		c1 = *p;
	}
	else
	{
		c1 = c;
	}

	while (1)
	{
		if (((c == '[') || (*t == c1)) && glob_match(p - 1, t))
		{
			return 1;
		}

		if (*t++ == '\0')
		{
			return 0;
		}
	}
}

/*
===============
glob_match

Match the pattern PATTERN against the string TEXT;
return 1 if it matches, 0 otherwise.

A match means the entire string TEXT is used up in matching.

In the pattern string, `*' matches any sequence of characters,
`?' matches any character, [SET] matches any character in the specified set,
[!SET] matches any character not in the specified set.

A set is composed of characters or ranges; a range looks like
character hyphen character (as in 0-9 or A-Z).
[0-9a-zA-Z_] is the set of characters allowed in C identifiers.
Any other character in the pattern must be matched exactly.

To suppress the special syntactic significance of any of `[]*?!-\',
and match the character exactly, precede it with a `\'.
===============
*/
int glob_match (char *pattern, char *text)
{
	register char *p = pattern, *t = text;
	register char c;

	while ((c = *p++) != '\0')
	{
		switch (c)
		{
		case '?':

			if (*t == '\0')
			{
				return 0;
			}
			else
			{
				++t;
			}

			break;

		case '\\':

			if (*p++ != *t++)
			{
				return 0;
			}

			break;

		case '*':
			return glob_match_after_star(p, t);

		case '[':
		{
			register char c1 = *t++;
			int invert;

			if (!c1)
			{
				return 0;
			}

			invert = ((*p == '!') || (*p == '^'));

			if (invert)
			{
				p++;
			}

			c = *p++;

			while (1)
			{
				register char cstart = c, cend = c;

				if (c == '\\')
				{
					cstart = *p++;
					cend = cstart;
				}

				if (c == '\0')
				{
					return 0;
				}

				c = *p++;

				if ((c == '-') && (*p != ']'))
				{
					cend = *p++;

					if (cend == '\\')
					{
						cend = *p++;
					}

					if (cend == '\0')
					{
						return 0;
					}

					c = *p++;
				}

				if ((c1 >= cstart) && (c1 <= cend))
				{
					goto match;
				}

				if (c == ']')
				{
					break;
				}
			}

			if (!invert)
			{
				return 0;
			}

			break;

		match:

			// Skip the rest of the [...] construct that already matched.
			while (c != ']')
			{
				if (c == '\0')
				{
					return 0;
				}

				c = *p++;

				if (c == '\0')
				{
					return 0;
				}
				else if (c == '\\')
				{
					++p;
				}
			}

			if (invert)
			{
				return 0;
			}

			break;
		}

		default:

			if (c != *t++)
			{
				return 0;
			}
		}
	}

	return *t == '\0';
}

#if defined(_WIN32)
char *strtok_r(char *s, const char *delim, char **last)
{
	const char *spanp;
	int c, sc;
	char *tok;

	if (s == NULL && (s = *last) == NULL)
		return (NULL);

	// Skip (span) leading delimiters (s += strspn(s, delim), sort of).
cont:
	c = *s++;
	for (spanp = delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		// no non-delimiter characters
		*last = NULL;
		return (NULL);
	}
	tok = s - 1;

	// Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	// Note that delim must have one NUL; we stop if we see that, too.
	for (;;) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = '\0';
				*last = s;
				return (tok);
			}
		} while (sc != 0);
	}
}
#endif

