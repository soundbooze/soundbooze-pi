/*
   gcc tonestackmesa.c -o tonestackmesa `pkg-config --libs --cflags jack` -lm -Wall -O3
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <jack/jack.h>

jack_port_t *input_port1, *input_port2;
jack_port_t *output_port1, *output_port2;
jack_client_t *client;

float 	fslider0;
float 	fslider1;
float 	fConst0;
float 	fConst1;
float 	fConst2;
float 	fConst3;
float 	fRec0[4];
float 	fslider2;
float 	fRec1[4];

inline float 
min(float a, float b) {
  if (a > b)
    return b;
  return a;
}

inline float 
max(float a, float b) {
  if (a < b)
    return b;
  return a;
}

inline void 
init_mesa (uint32_t samplingFreq) {
  fConst0 = (float)(min(1.92e+05f, max(1.0f, (float)samplingFreq)));
  fConst1 = (2 * fConst0);
  fConst2 = powf(fConst1,2);
  fConst3 = (6 * fConst0);
  fslider0 = 0.5f; // (".amp.tonestack.Middle", 0.5f, 0.0f, 1.0f, 0.01f)
  fslider1 = 0.5f; // (".amp.tonestack.Bass", 0.5f, 0.0f, 1.0f, 0.01f)
  fslider2 = 0.5f; // (".amp.tonestack.Treble", 0.5f, 0.0f, 1.0f, 0.01f)
  for (int i=0; i<4; i++) fRec0[i] = 0;
  for (int i=0; i<4; i++) fRec1[i] = 0;
}

inline void 
compute_mesa (int count, float *input0, float *input1, float *output0, float *output1) {
  float 	fSlow0 = (float)(fslider0);
  float 	fSlow1 = expf((3.4f * ((float)(fslider1) + -1)));
  float 	fSlow2 = (2.5703124e-09f * fSlow1);
  float 	fSlow3 = (7.34375e-09f * fSlow1);
  float 	fSlow4 = ((fSlow3 + (fSlow0 * (fSlow2 + (-4.773438e-10f - (2.5703126e-10f * fSlow0))))) + 7.34375e-10f);
  float 	fSlow5 = (fConst1 * fSlow4);
  float 	fSlow6 = (2.9448438e-06f * fSlow0);
  float 	fSlow7 = ((0.00011998125f * fSlow1) + (fSlow0 * ((2.9448438e-05f * fSlow1) + (-8.7317185e-06f - fSlow6))));
  float 	fSlow8 = (0.0250625f * fSlow1);
  float 	fSlow9 = (0.001175f * fSlow0);
  float 	fSlow10 = (fConst1 * ((fSlow9 + fSlow8) + 0.01726875f));
  float 	fSlow11 = (fSlow10 + (-1 - (fConst2 * (fSlow7 + (1.2916875e-05f - fSlow5)))));
  float 	fSlow12 = (fConst3 * fSlow4);
  float 	fSlow13 = ((fSlow10 + (fConst2 * (fSlow7 + (1.2916875e-05f - fSlow12)))) + -3);
  float 	fSlow14 = ((fConst2 * ((fSlow7 + fSlow12) + 1.2916875e-05f)) + (-3 - fSlow10));
  float 	fSlow15 = (1.0f / (-1 - (fSlow10 + (fConst2 * ((fSlow7 + fSlow5) + 1.2916875e-05f)))));
  float 	fSlow16 = (1.0f - fSlow0);
  float 	fSlow17 = (float)(fslider2);
  float 	fSlow18 = ((fSlow17 * (fSlow3 + (7.34375e-10f * fSlow16))) + (fSlow0 * (fSlow2 + (2.5703126e-10f * fSlow16))));
  float 	fSlow19 = (fConst1 * fSlow18);
  float 	fSlow20 = (((9.1875e-07f * fSlow17) + (fSlow0 * (3.0182812e-06f - fSlow6))) + (fSlow1 * ((2.9448438e-05f * fSlow0) + 2.48125e-06f)));
  float 	fSlow21 = (fSlow8 + (fSlow9 + (6.25e-05f * fSlow17)));
  float 	fSlow22 = (fConst1 * (fSlow21 + 0.00250625f));
  float 	fSlow23 = (fSlow22 - (fConst2 * (fSlow20 + (2.48125e-07f - fSlow19))));
  float 	fSlow24 = (fConst3 * fSlow18);
  float 	fSlow25 = (fSlow22 + (fConst2 * (fSlow20 + (2.48125e-07f - fSlow24))));
  float 	fSlow26 = (fConst1 * (-0.00250625f - fSlow21));
  float 	fSlow27 = (fSlow26 + (fConst2 * ((fSlow20 + fSlow24) + 2.48125e-07f)));
  float 	fSlow28 = (fSlow26 - (fConst2 * ((fSlow20 + fSlow19) + 2.48125e-07f)));
  for (int i=0; i<count; i++) {
    fRec0[0] = ((float)input0[i] - (fSlow15 * (((fSlow14 * fRec0[1]) + (fSlow13 * fRec0[2])) + (fSlow11 * fRec0[3]))));
    output0[i] = (float)(fSlow15 * ((((fSlow28 * fRec0[0]) + (fSlow27 * fRec0[1])) + (fSlow25 * fRec0[2])) + (fSlow23 * fRec0[3])));
    fRec1[0] = ((float)input1[i] - (fSlow15 * (((fSlow14 * fRec1[1]) + (fSlow13 * fRec1[2])) + (fSlow11 * fRec1[3]))));
    output1[i] = (float)(fSlow15 * ((((fSlow28 * fRec1[0]) + (fSlow27 * fRec1[1])) + (fSlow25 * fRec1[2])) + (fSlow23 * fRec1[3])));
    for (int i=3; i>0; i--) fRec1[i] = fRec1[i-1];
    for (int i=3; i>0; i--) fRec0[i] = fRec0[i-1];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input1_in, *input2_in, *output1_out, *output2_out;

  input1_in = jack_port_get_buffer (input_port1, nframes);
  output1_out = jack_port_get_buffer (output_port1, nframes);

  input2_in = jack_port_get_buffer (input_port2, nframes);
  output2_out = jack_port_get_buffer (output_port2, nframes);

  jack_default_audio_sample_t mesa_l[nframes];
  jack_default_audio_sample_t mesa_r[nframes];

  compute_mesa(nframes, input1_in, input2_in, mesa_l, mesa_r);

  memcpy (output1_out, mesa_l, sizeof (jack_default_audio_sample_t) * nframes);
  memcpy (output2_out, mesa_r, sizeof (jack_default_audio_sample_t) * nframes);

  return 0;      
}

void
jack_shutdown (void *arg)
{
  exit (1);
}

int
main (int argc, char *argv[])
{
  const char *client_name = "mesa";

  jack_options_t options = JackNullOption;
  jack_status_t status;

  client = jack_client_open (client_name, options, &status, NULL);
  if (client == NULL) {
    fprintf (stderr, "jack_client_open() failed, " "status = 0x%2.0x\n", status);
    if (status & JackServerFailed) {
      fprintf (stderr, "Unable to connect to JACK server\n");
    }
    exit (1);
  }

  if (status & JackServerStarted) {
    fprintf (stderr, "JACK server started\n");
  }

  if (status & JackNameNotUnique) {
    client_name = jack_get_client_name(client);
    fprintf (stderr, "unique name `%s' assigned\n", client_name);
  }

  jack_set_process_callback (client, process, 0);

  jack_on_shutdown (client, jack_shutdown, 0);

  input_port1 = jack_port_register (client, "input1",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  input_port2 = jack_port_register (client, "input2",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  output_port1 = jack_port_register (client, "output1",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  output_port2 = jack_port_register (client, "output2",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  if ((input_port1 == NULL) || (output_port1 == NULL) || (input_port2 == NULL) || (output_port2 == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
    exit (1);
  }

  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
    exit (1);
  }

  init_mesa(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
