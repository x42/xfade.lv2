/* xfade -- LV2 stereo xfader control
 *
 * Copyright (C) 2013 Robin Gareus <robin@gareus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#define XFC_URI "http://gareus.org/oss/lv2/xfade"

#define IPORTS (2)

#define CHANNELS (2)
#define C_LEFT (0)
#define C_RIGHT (1)

#define FADE_LEN (64)

#define MIN(a,b) ( (a) < (b) ? (a) : (b) )
#define MAX(a,b) ( (a) > (b) ? (a) : (b) )
#define RAIL(v, min, max) (MIN((max), MAX((min), (v))))

typedef enum {
	XFC_XFADE,
	XFC_IN0L,
	XFC_IN0R,
	XFC_IN1L,
	XFC_IN1R,
	XFC_OUTL,
	XFC_OUTR
} PortIndex;

typedef struct {
	/* control ports */
	float* xfade;
	float* input[IPORTS][CHANNELS];
	float* output[CHANNELS];

	/* current settings */
	float c_amp[IPORTS];

} XfadeControl;


#define SMOOTHGAIN(AMP, TARGET_AMP) (AMP + (TARGET_AMP - AMP) * (float) MIN(pos, fade_len) / (float)fade_len)

static void
run(LV2_Handle instance, uint32_t n_samples)
{
	XfadeControl* self = (XfadeControl*)instance;
	const float xfade = *self->xfade;

	const uint32_t fade_len = (n_samples >= FADE_LEN) ? FADE_LEN : n_samples;

	float gain_0 = 1.0;
	float gain_1 = 1.0;

	if (xfade == -1.0) {
		gain_0 = 1.0;
		gain_1 = 0.0;
	} else if (xfade == 1.0) {
		gain_0 = 0.0;
		gain_1 = 1.0;
	} else {
		gain_1 = sqrt(.5 + xfade/2.0);
		gain_0 = sqrt(.5 - xfade/2.0);
	}

	for (int c = 0; c < CHANNELS; ++c) {
		uint32_t pos = 0;
		if (self->c_amp[0] == gain_0 &&  self->c_amp[1] == gain_1) {
			for (pos = 0; pos < n_samples; pos++) {
				self->output[c][pos] =
						self->input[0][c][pos] * gain_0
					+ self->input[1][c][pos] * gain_1;
			}
		} else {
			for (pos = 0; pos < n_samples; pos++) {
				self->output[c][pos] =
						self->input[0][c][pos] * SMOOTHGAIN(self->c_amp[0], gain_0)
					+ self->input[1][c][pos] * SMOOTHGAIN(self->c_amp[1], gain_1);
			}
		}
	}

	self->c_amp[0] = gain_0;
	self->c_amp[1] = gain_1;
}

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
	int i;
	XfadeControl* self = (XfadeControl*)calloc(1, sizeof(XfadeControl));
	if (!self) return NULL;

	for (i=0; i < IPORTS; ++i) {
		self->c_amp[i] = 1.0;
	}

	return (LV2_Handle)self;
}

static void
connect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
	XfadeControl* self = (XfadeControl*)instance;

	switch ((PortIndex)port) {
	case XFC_XFADE:
		self->xfade = data;
		break;
	case XFC_IN0L:
		self->input[0][C_LEFT] = data;
		break;
	case XFC_IN0R:
		self->input[0][C_RIGHT] = data;
		break;
	case XFC_IN1L:
		self->input[1][C_LEFT] = data;
		break;
	case XFC_IN1R:
		self->input[1][C_RIGHT] = data;
		break;
	case XFC_OUTL:
		self->output[C_LEFT] = data;
		break;
	case XFC_OUTR:
		self->output[C_RIGHT] = data;
		break;
	}
}

static void
cleanup(LV2_Handle instance)
{
	free(instance);
}

const void*
extension_data(const char* uri)
{
	return NULL;
}

static const LV2_Descriptor descriptor = {
	XFC_URI,
	instantiate,
	connect_port,
	NULL,
	run,
	NULL,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
	switch (index) {
	case 0:
		return &descriptor;
	default:
		return NULL;
	}
}
