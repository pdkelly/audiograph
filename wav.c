/* 
    wav.c
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

#include <sndfile.h>

struct wav_file
{
    SNDFILE *file_handle;
    SF_INFO file_info;
    float *raw_buff;
    int buff_size;
};

struct wav_file *wav_open(const char *filename)
{
    struct wav_file *wav;

    if( !(wav = calloc(1, sizeof(struct wav_file))))
        return NULL;

    wav->file_info.format = 0;

    if(strcmp(filename, "-") == 0)
        wav->file_handle = sf_open_fd(fileno(stdin), SFM_READ, &wav->file_info, 0);
    else
        wav->file_handle = sf_open(filename, SFM_READ, &wav->file_info);

    if( !wav->file_handle)
    {
        free(wav);
        return NULL;
    }

    return wav;
}

int wav_read_samples(struct wav_file *wav, float *buff, int count)
{
    int samples_read, i, j;

    /* If the source is mono, then simply read directly into the output buffer */
    if(wav->file_info.channels == 1)
        return sf_readf_float(wav->file_handle, buff, count);

    /* Otherwise resize internal buffer to hold stereo samples if necessary... */
    if(wav->buff_size < count * wav->file_info.channels)
    {
        wav->buff_size = count * wav->file_info.channels;
        if( !(wav->raw_buff = realloc(wav->raw_buff, wav->buff_size * sizeof(float))))
            return -1;
    }

    /* ...and read samples for all channels interleaved, then convert to mono. */
    samples_read = sf_readf_float(wav->file_handle, wav->raw_buff, count);
    for(i = 0; i < samples_read; i++)
    {
        buff[i] = 0;
        for(j = 0; j < wav->file_info.channels; j++)
            buff[i] += wav->raw_buff[i*wav->file_info.channels+j];
        buff[i] /= wav->file_info.channels;
    }

    return samples_read;
}

void wav_close(struct wav_file *wav)
{
    sf_close(wav->file_handle);
    free(wav->raw_buff);
    free(wav);

    return;
}
