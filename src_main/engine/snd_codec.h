/*
 * Audio Codecs: Adapted from ioquake3 with changes.
 *
 * Copyright (C) 1999-2005 Id Software, Inc.
 * Copyright (C) 2005 Stuart Dalton <badcdev@gmail.com>
 * Copyright (C) 2010 O.Sezer <sezero@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef _SND_CODEC_H_
#define _SND_CODEC_H_

typedef struct snd_info_s
{
	int rate;
	int width;
	int channels;
	int	loopstart;
	int samples;
	int size;
	int dataofs;
} snd_info_t;

typedef enum {
	STREAM_NONE = -1,
	STREAM_INIT,
	STREAM_PAUSE,
	STREAM_PLAY
} stream_status_t;

typedef struct codec_fileHandle_s
{
	FILE *file;
	qboolean pak;		// is the file read from a pak
	long start;			// file or data start position
	long length;		// file or data size
	long pos;			// current position relative to start
} codec_fileHandle_t;

typedef struct snd_codec_s snd_codec_t;

typedef struct snd_stream_s
{
	codec_fileHandle_t fh;
	qboolean pak;
	snd_info_t info;
	stream_status_t status;
	snd_codec_t *codec;	// codec handling this stream
	void *priv;			// data private to the codec
} snd_stream_t;

void S_CodecInit (void);
void S_CodecShutdown (void);

void *S_CodecLoad (char *filename, snd_info_t *info);

// Callers of the following S_CodecOpenStream* functions
// are reponsible for attaching any path to the filename

snd_stream_t *S_CodecOpenStreamType (char *filename, unsigned int type);
// Decides according to the required type.

snd_stream_t *S_CodecOpenStreamAny (char *filename);
// Decides according to file extension. if the name has no extension, try all available.

snd_stream_t *S_CodecOpenStreamExt (char *filename);
// Decides according to file extension. the name MUST have an extension.

void S_CodecCloseStream (snd_stream_t *stream);
int S_CodecReadStream (snd_stream_t *stream, int bytes, void *buffer);
int S_CodecRewindStream (snd_stream_t *stream);

snd_stream_t *S_CodecUtilOpen(char *filename, snd_codec_t *codec);
void S_CodecUtilClose(snd_stream_t **stream);

#define CODECTYPE_NONE		0
#define CODECTYPE_MOD		(1 << 0)
#define CODECTYPE_WAV		(1 << 1)
#define CODECTYPE_MP3		(1 << 2)

int S_CodecIsAvailable (unsigned int type);
// return 1 if available, 0 if codec failed init or -1 if no such codec is present.

#endif	/* _SND_CODEC_H_ */