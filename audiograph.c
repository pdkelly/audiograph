/* 
    audiograph.c
    Audio waveform plotting utility by Paul Kelly.
    Copyright (C) 2012 NP Broadcast Ltd.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <stdio.h>
#include <stdlib.h>

#include <cairo.h>

#define PROG_NAME    "audiograph"
#define PROG_VERSION "0.6"

#include "audiograph.h"

#define CHUNK_SIZE 2048

static int parse_arguments(int, char **, int *, int *, double *);

int main(int argc, char **argv)
{
    int width, height;
    double colour[3];

    struct wav_file *wav;

    struct graph *gr;
    cairo_surface_t *surface;


    /* Validate and parse command-line arguments */
    if(parse_arguments(argc, argv, &width, &height, colour) != 0)
        return 1;

    /* Open input file */
    if( !(wav = wav_open(argv[1])))
    {
        fprintf(stderr, "Unable to open input audio file %s\n", argv[1]);
        return 1;
    }

    /* Buffer all samples from input file. */
    if( !(gr = graph_init()))
    {
        fprintf(stderr, "Unable to initialise sample buffer\n");
        return 1;
    }
    while(1)
    {
        float samples[CHUNK_SIZE];
        int sample_count = wav_read_samples(wav, samples, CHUNK_SIZE);

        if(sample_count < 0)
        {
            fprintf(stderr, "Error reading samples; out of memory?\n");
            return 1;
        }
        else if(sample_count == 0) /* end of input */
            break;

        if(graph_buffer_samples(gr, samples, sample_count) < 0)
        {
            fprintf(stderr, "Error buffering samples; out of memory?\n");
            return 1;
        }
    }

    /* Close input file */
    wav_close(wav);

    /* Draw graph and output to PNG file using Cairo */
    if( !(surface = graph_draw(gr, width, height, colour)))
    {
        fprintf(stderr, "Error initialising Cairo; out of memory?\n");
        return 1;
    }
    if(cairo_surface_write_to_png(surface, argv[2]) != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr, "Error writing graph to PNG file\n");
        return 1;
    }

    graph_surface_destroy(surface);
    graph_destroy(gr);

    return 0;
}

static int parse_arguments(int argc, char **argv, int *width, int *height, double *colour)
{
    unsigned char icol[3];
    int i;

    if(argc != 6)
    {
        fprintf(stderr, "%s v%s\n", PROG_NAME, PROG_VERSION);
        fprintf(stderr, "Usage: %s <input.wav> <output.png> <width> <height> <rrggbb>\n", argv[0]);
        fprintf(stderr, "Plots the audio waveform contained in the specified file, as a transparent\n"
                        "PNG image with the specified width, height and foreground colour.\n"
                        "The audio file may be in any format supported by libsndfile, e.g. WAV or FLAC.\n");
        return 1;
    }

    /* Parse width as integer */
    if((*width = atoi(argv[3])) <= 0)
    {
        fprintf(stderr, "ERROR: Invalid value for width %s; should be integer > 0\n", argv[3]);
        return 1;
    }

    /* Parse height as integer */
    if((*height = atoi(argv[4])) <= 0)
    {
        fprintf(stderr, "ERROR: Invalid value for height %s; should be integer > 0\n", argv[4]);
        return 1;
    }

    /* Parse colour as hexadecimal triplet in range 0-255 and convert to 
     * array of double values in range 0-1 */
    if(sscanf(argv[5], "%2hhx%2hhx%2hhx", &icol[0], &icol[1], &icol[2]) != 3)
    {
        fprintf(stderr, "ERROR: Invalid value for colour %s; should be RRGGBB hex triplet\n", argv[5]);
        return 1;
    }
    for(i = 0; i < 3; i++)
        colour[i] = (double)icol[i] / 255;

    return 0;
}
