/* 
    graph.c
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

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <cairo.h>

struct graph
{
    float *buff;
    int sample_count, max_samples;
};

struct graph *graph_init(void)
{
    struct graph *gr = calloc(1, sizeof(struct graph));

    return gr;
}

int graph_buffer_samples(struct graph *gr, float *samples, int num_samples)
{
    if(gr->sample_count + num_samples > gr->max_samples)
    {
        gr->max_samples += (num_samples + 100000);
        if( !(gr->buff = realloc(gr->buff, gr->max_samples * sizeof(float))))
            return -1;
    }

    memcpy(gr->buff+gr->sample_count, samples, num_samples * sizeof(float));
    gr->sample_count += num_samples;

    return num_samples;
}

/* Helper function for calculating logarithmetic vertical scale */
static double log_scale(double linear)
{
    if(linear < 0)
        return -log(1.-linear) / log(2.);
    else
        return log(1.+linear) / log(2.);
}

cairo_surface_t *graph_draw(struct graph *gr, int width, int height, double *colour)
{
    cairo_surface_t *surface;
    cairo_t *cr;
    int i;

    /* Create Cairo surface and set background as transparent */
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
        return NULL;
    cr = cairo_create(surface);
    if(cairo_status(cr) != CAIRO_STATUS_SUCCESS)
        return NULL;
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0); /* technically white, but transparent */
    cairo_paint(cr);

    /* Set drawing colour and line width */
    cairo_set_source_rgb(cr, colour[0], colour[1], colour[2]);
    cairo_set_line_width(cr, 1.0);

    /* Draw initial horizontal line */
    cairo_move_to(cr, 0.0, height / 2.0);
    cairo_rel_line_to(cr, width, 0);
    cairo_stroke(cr);

    /* Plot the samples as a verical line between min and max value for 
     * each pixel (with logarithmic scale) and return the surface */
    for(i = 0; i < width; i ++)
    {
        double x, y_min, y_max;
        double min_amp = 1.0, max_amp = -1.0;
        int min_sample, max_sample, j;

        min_sample = (int)floor((double)i / width * gr->sample_count);
        max_sample = (int)ceil((double)(i+1) / width * gr->sample_count);
        for(j = min_sample; j < max_sample; j++)
        {
            if(gr->buff[j] < min_amp)
                min_amp = gr->buff[j];
            if(gr->buff[j] > max_amp)
                max_amp = gr->buff[j];
        }
        /* Add 0.5 so line will be centred in middle of pixel */
        x = i + 0.5;
        y_min = (1. - log_scale(min_amp)) * height / 2.0;
        y_max = (1. - log_scale(max_amp)) * height / 2.0;

        cairo_move_to(cr, x, y_min);
        cairo_line_to(cr, x, y_max);
        cairo_stroke(cr);
    }
    cairo_destroy(cr);

    return surface;
}

void graph_surface_destroy(cairo_surface_t *surface)
{
    cairo_surface_destroy(surface);

    return;
}

void graph_destroy(struct graph *gr)
{
    free(gr->buff);
    free(gr);

    return;
}
