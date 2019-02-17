/*
   gcc tone.c -o tone `pkg-config --libs --cflags jack` -lm -Wall -O3
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <jack/jack.h>

jack_port_t *input_port;
jack_port_t *output_port;
jack_client_t *client;

float 	fslider0;
float 	fslider1;
float 	fConst0;
float 	fConst1;
float 	fConst2;
float 	fConst3;
float 	fConst4;
float 	fConst5;
float 	fConst6;
float 	fVec0[3];
float 	fslider2;
float 	fRec3[3];
float 	fRec2[3];
float 	fRec1[3];
float 	fRec0[3];

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
init_tone (uint32_t samplingFreq) {
  fConst0 = min(1.92e+05f, max(1.0f, (float)samplingFreq));
  fConst1 = (15079.645f / fConst0);
  fConst2 = cosf(fConst1);
  fConst3 = (1.4142135f * sinf(fConst1));
  fConst4 = (3769.9111f / fConst0);
  fConst5 = cosf(fConst4);
  fConst6 = (1.4142135f * sinf(fConst4));

  fslider0 = 4.0f; // -20.0 20.0 middle
  fslider1 = 3.0f; // -20.0 20.0 treble
  fslider2 = 5.0f; // -20.0 20.0 bass

  for (int i=0; i<3; i++) fVec0[i] = 0;
  for (int i=0; i<3; i++) fRec3[i] = 0;
  for (int i=0; i<3; i++) fRec2[i] = 0;
  for (int i=0; i<3; i++) fRec1[i] = 0;
  for (int i=0; i<3; i++) fRec0[i] = 0;
}

inline void 
compute_tone (int count, float *input, float *output) {
  float 	fSlow0 = (float)(fslider0);
  float 	fSlow1 = (0.5f * fSlow0);
  float 	fSlow2 = powf(10,(0.025f * ((float)(fslider1) - fSlow1)));
  float 	fSlow3 = (fConst2 * (fSlow2 + 1));
  float 	fSlow4 = (2 * (fSlow2 + (-1 - fSlow3)));
  float 	fSlow5 = (fConst3 * sqrtf(fSlow2));
  float 	fSlow6 = (fConst2 * (fSlow2 + -1));
  float 	fSlow7 = (fSlow2 + (1 - (fSlow6 + fSlow5)));
  float 	fSlow8 = powf(10,(0.0125f * fSlow0));
  float 	fSlow9 = (fSlow8 + 1);
  float 	fSlow10 = (fConst5 * fSlow9);
  float 	fSlow11 = (2 * (fSlow8 + (-1 - fSlow10)));
  float 	fSlow12 = sqrtf(fSlow8);
  float 	fSlow13 = (fConst6 * fSlow12);
  float 	fSlow14 = (fSlow8 + -1);
  float 	fSlow15 = (fConst5 * fSlow14);
  float 	fSlow16 = (fSlow8 + (1 - (fSlow15 + fSlow13)));
  float 	fSlow17 = powf(10,(0.025f * ((float)(fslider2) - fSlow1)));
  float 	fSlow18 = (fConst6 * sqrtf(fSlow17));
  float 	fSlow19 = (fConst5 * (fSlow17 + -1));
  float 	fSlow20 = (fSlow17 + (1 - (fSlow19 + fSlow18)));
  float 	fSlow21 = (fConst5 * (fSlow17 + 1));
  float 	fSlow22 = (2 * (fSlow17 + (-1 - fSlow21)));
  float 	fSlow23 = ((fSlow17 + fSlow18) + (1 - fSlow19));
  float 	fSlow24 = (fSlow17 + fSlow19);
  float 	fSlow25 = (fSlow24 + (1 - fSlow18));
  float 	fSlow26 = (0 - (2 * ((fSlow17 + fSlow21) + -1)));
  float 	fSlow27 = (1.0f / ((fSlow18 + fSlow24) + 1));
  float 	fSlow28 = (fConst3 * fSlow12);
  float 	fSlow29 = (fConst2 * fSlow14);
  float 	fSlow30 = (fSlow8 + (1 - (fSlow29 + fSlow28)));
  float 	fSlow31 = ((fSlow8 + fSlow28) + (1 - fSlow29));
  float 	fSlow32 = (fConst2 * fSlow9);
  float 	fSlow33 = (2 * (fSlow8 + (-1 - fSlow32)));
  float 	fSlow34 = (fSlow8 + fSlow29);
  float 	fSlow35 = (fSlow34 + (1 - fSlow28));
  float 	fSlow36 = (0 - (2 * ((fSlow8 + fSlow32) + -1)));
  float 	fSlow37 = (1.0f / ((fSlow28 + fSlow34) + 1));
  float 	fSlow38 = (fSlow8 + fSlow15);
  float 	fSlow39 = (fSlow38 + (1 - fSlow13));
  float 	fSlow40 = ((fSlow38 + fSlow13) + 1);
  float 	fSlow41 = ((0 - (2 * fSlow8)) * ((fSlow8 + fSlow10) + -1));
  float 	fSlow42 = (1.0f / ((fSlow8 + fSlow13) + (1 - fSlow15)));
  float 	fSlow43 = (fSlow2 + fSlow6);
  float 	fSlow44 = (fSlow43 + (1 - fSlow5));
  float 	fSlow45 = ((fSlow43 + fSlow5) + 1);
  float 	fSlow46 = ((0 - (2 * fSlow2)) * ((fSlow2 + fSlow3) + -1));
  float 	fSlow47 = (1.0f / ((fSlow2 + fSlow5) + (1 - fSlow6)));
  for (int i=0; i<count; i++) {
    float fTemp0 = ((float)input[i] + 1e-20f);
    fVec0[0] = fTemp0;
    fRec3[0] = (0 - (fSlow27 * (((fSlow26 * fRec3[1]) + (fSlow25 * fRec3[2])) - (fSlow17 * (((fSlow23 * fVec0[0]) + (fSlow22 * fVec0[1])) + (fSlow20 * fVec0[2]))))));
    fRec2[0] = (0 - (fSlow37 * (((fSlow36 * fRec2[1]) + (fSlow35 * fRec2[2])) - (fSlow8 * ((fSlow33 * fRec3[1]) + ((fSlow31 * fRec3[0]) + (fSlow30 * fRec3[2])))))));
    fRec1[0] = (fSlow42 * (((fSlow41 * fRec2[1]) + (fSlow8 * ((fSlow40 * fRec2[0]) + (fSlow39 * fRec2[2])))) - ((fSlow16 * fRec1[2]) + (fSlow11 * fRec1[1]))));
    fRec0[0] = (fSlow47 * (((fSlow46 * fRec1[1]) + (fSlow2 * ((fSlow45 * fRec1[0]) + (fSlow44 * fRec1[2])))) - ((fSlow7 * fRec0[2]) + (fSlow4 * fRec0[1]))));
    output[i] = (float)fRec0[0];

    fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
    fRec1[2] = fRec1[1]; fRec1[1] = fRec1[0];
    fRec2[2] = fRec2[1]; fRec2[1] = fRec2[0];
    fRec3[2] = fRec3[1]; fRec3[1] = fRec3[0];
    fVec0[2] = fVec0[1]; fVec0[1] = fVec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t tone[nframes];

  compute_tone(nframes, input, tone);

  memcpy (output, tone, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "tone";

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

  input_port = jack_port_register (client, "in",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  output_port = jack_port_register (client, "out",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  if ((input_port == NULL) || (output_port == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
    exit (1);
  }

  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
    exit (1);
  }

  init_tone(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
