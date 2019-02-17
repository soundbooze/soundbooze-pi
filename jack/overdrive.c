/*
   gcc overdrive.c -o overdrive `pkg-config --libs --cflags jack` -lm -Wall -O3
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
float 	fRec0[2];

inline void 
init_overdrive() {
  fslider0 = 90; // 1e+02f; // ("wet_dry", &fslider0, 1e+02f, 0.0f, 1e+02f, 1.0f)
  fslider1 = 19; // 1.0f;   // ("drive", &fslider1, 1.0f, 1.0f, 2e+01f, 0.1f)
  for (int i=0; i<2; i++) fRec0[i] = 0;
}

inline void 
compute_overdrive (int count, float *input, float *output) {
  float 	fSlow0 = (float)(fslider0);
  float 	fSlow1 = (0.01f * fSlow0);
  float 	fSlow2 = (float)(fslider1);
  float 	fSlow3 = (fSlow2 + -1);
  float 	fSlow4 = (0.0001f * powf(fSlow0,2));
  float 	fSlow5 = (0.001f * powf(10,(0.05f * (0 - (0.5f * fSlow2)))));
  for (int i=0; i<count; i++) {
    float fTemp0 = (float)input[i];
    float fTemp1 = fabsf((fSlow1 * fTemp0));
    fRec0[0] = (fSlow5 + (0.999f * fRec0[1]));
    output[i] = (float)(fTemp0 * (1 - (fSlow1 * (1.0f - (((fSlow2 + fTemp1) * fRec0[0]) / (((fSlow4 * powf(fTemp0,2)) + (fSlow3 * fTemp1)) + 1))))));
    fRec0[1] = fRec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t overdrive[nframes];

  compute_overdrive(nframes, input, overdrive);

  memcpy (output, overdrive, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "overdrive";

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

  init_overdrive();

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
