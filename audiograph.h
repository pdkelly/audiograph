/* 
    audiograph.h
    Audio waveform plotting utility by Paul Kelly.
    Copyright (C) 2012 NP Broadcast Ltd.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef AUDIOGRAPH_H
#define AUDIOGRAPH_H


/* wav.c */

/* Open a libsndfile-supported audio file for reading. Parameter
 * "filename" should be the path to the file to be opened, or a string
 * constisting of a single - character - which indicates that audio
 * should be read from standard input. */
struct wav_file *wav_open(const char *filename);

/* This function accepts a pointer to the structure returned by a previous
 * call to wav_open(), and attempts to read a maximum of "count" audio
 * samples, as single-precision floating point numbers in the range -1.0 to 
 * +1.0, into the user-supplied buffer "buff". If the audio source is multi-
 * channel, the samples will be automatically down-mixed to mono. The function
 * returns the number of mono samples actually read. */
int wav_read_samples(struct wav_file *, float *buff, int count);

/* Close an audio file that was previously opened by wav_open(). */
void wav_close(struct wav_file *);


/* graph.c */

/* Initialise sample buffer and allocate memory. */
struct graph *graph_init(void);

/* Buffer a number of mono floating point audio samples 
 * in the range -1.0 to 1.0. */
int graph_buffer_samples(struct graph *, float *, int);

/* Create a visualisation, using Cairo, of the samples buffered so
 * far. Graph will have dimensions of the given width and height;
 * input audio data will be appropriately binned in pixel intervals
 * and plotted in the given colour against a transparent background.
 * Logarithmic scale used for y-axis (amplitude).
 * This function returns a Cairo ARGB32 surface, which can be output to 
 * various disk and in-memory graphics formats using Cairo functions, 
 * e.g. cairo_surface_write_to_png(). */
cairo_surface_t *graph_draw(struct graph *, int width, int height, double *colour);

/* Free the memory used by a Cairo surface returned from graph_draw(). */
void graph_surface_destroy(cairo_surface_t *);

/* Free the memory used by the sample buffer */
void graph_destroy(struct graph *);

#endif /* AUDIOGRAPH_H */
