/*
   gcc thickdistortion.c -o thickdistortion `pkg-config --libs --cflags jack` -lm -Wall -O3
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

float	fslider0;
float	fRec0[2];
float	fslider1;
float	fRec2[2];
float	fslider2;
float	fRec3[2];
float	fRec1[2];

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
init_dist() {
  fslider0 = 0.8f; // ("volume", 0.0f, -9e+01f, 4e+01f, 0.1f)
  fslider1 = 0.8f; // ("thickness", 0.8f, 0.0f, 1.0f, 0.01f)
  fslider2 = 0.8f; // ("input_gain", 0.0f, 0.0f, 1.2e+02f, 0.1f)

  for (int i=0; i<2; i++) fRec0[i] = 0;
  for (int i=0; i<2; i++) fRec2[i] = 0;
  for (int i=0; i<2; i++) fRec3[i] = 0;
  for (int i=0; i<2; i++) fRec1[i] = 0;
}

inline void 
compute_dist (int count, float *input, float *output) {
  float 	fSlow0 = (0.001f * powf(10,(0.05f * (fslider0))));
  float 	fSlow1 = (0.001f * (fslider1));
  float 	fSlow2 = (0.001f * powf(10,(0.05f * (fslider2))));
  for (int i=0; i<count; i++) {
    fRec0[0] = (fSlow0 + (0.999f * fRec0[1]));
    fRec2[0] = (fSlow1 + (0.999f * fRec2[1]));
    fRec3[0] = (fSlow2 + (0.999f * fRec3[1]));
    fRec1[0] = ((fRec1[1] * fRec2[0]) + (max((float)-1, min((float)1, ((float)input[i] * fRec3[0]))) * (1 - fRec2[0])));
    output[i] = (float)(fRec1[0] * fRec0[0]);
    fRec1[1] = fRec1[0];
    fRec3[1] = fRec3[0];
    fRec2[1] = fRec2[0];
    fRec0[1] = fRec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t dist[nframes];

  compute_dist(nframes, input, dist);

  memcpy (output, dist, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "thickdistortion";

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

  init_dist();

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
