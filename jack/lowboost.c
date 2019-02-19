/*
   gcc lowboost.c -o lowboost `pkg-config --libs --cflags jack` -lm -Wall -O3
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
float 	fConst0;
float 	fRec0[3];
float 	fslider1;

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

inline void init_lowboost(uint32_t samplingFreq) {
 	fConst0 = (3.1415927f / min(1.92e+05f, max(1.0f, (float)samplingFreq)));
  fslider0 = 10990.0f; // ("freq", &fslider0, 1e+03f, 2e+01f, 2e+04f, 0.1f)
  fslider1 = -9.6f;    // ("gain", &fslider1, 0.0f, -2e+01f, 2e+01f, 0.1f);

  for (int i=0; i<3; i++) fRec0[i] = 0;
}

inline void compute_lowboost (int count, float *input, float *output) {
  float 	fSlow0 = tanf((fConst0 * (float)(fslider0)));
  float 	fSlow1 = powf(fSlow0,2);
  float 	fSlow2 = (2 * (fSlow1 + -1));
  float 	fSlow3 = ((fSlow0 * (fSlow0 + -1.4142135f)) + 1);
  float 	fSlow4 = (1.0f / ((fSlow0 * (fSlow0 + 1.4142135f)) + 1));
  float 	fSlow5 = powf(10,(0.05f * (float)(fslider1)));
  float 	fSlow6 = (fSlow5 * fSlow0);
  float 	fSlow7 = sqrtf((2 * fSlow5));
  float 	fSlow8 = (1 - (fSlow0 * (fSlow7 - fSlow6)));
  float 	fSlow9 = ((fSlow0 * (fSlow7 + fSlow6)) + 1);
  float 	fSlow10 = (2 * ((fSlow5 * fSlow1) + -1));
  for (int i=0; i<count; i++) {
    fRec0[0] = ((float)input[i] - (fSlow4 * ((fSlow3 * fRec0[2]) + (fSlow2 * fRec0[1]))));
    output[i] = (float)(fSlow4 * ((fSlow10 * fRec0[1]) + ((fSlow9 * fRec0[0]) + (fSlow8 * fRec0[2]))));
    fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t lowboost[nframes];

  compute_lowboost(nframes, input, lowboost);

  memcpy (output, lowboost, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "lowboost";

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

  init_lowboost(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
