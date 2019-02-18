/*
   gcc preamp.c -o preamp `pkg-config --libs --cflags jack` -lm -Wall -O3
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

int 	iVec0[2];
float 	fRec0[2];
float 	fVec1[3];
float 	fslider0;
float 	fVec2[3];
int fSamplingFreq;

inline void 
init_preamp() {
  fslider0 = 1.0f; // ("atan", "Drive" : input level, 1.0f, 1.0f, 1e+01f, 1.0f)
  for (int i=0; i<2; i++) iVec0[i] = 0;
  for (int i=0; i<2; i++) fRec0[i] = 0;
  for (int i=0; i<3; i++) fVec1[i] = 0;
  for (int i=0; i<3; i++) fVec2[i] = 0;
}
inline void 
compute_preamp (int count, float *input, float *output) {
  float 	fSlow0 = (float)(fslider0);
  float 	fSlow1 = (0.75f / atanf(fSlow0));
  for (int i=0; i<count; i++) {
    iVec0[0] = 1;
    fRec0[0] = ((1e-20f * (1 - iVec0[1])) - fRec0[1]);
    float fTemp0 = (float)input[i];
    fVec1[0] = fTemp0;
    float fTemp1 = ((fVec1[0] + fVec1[1]) + fVec1[2]);
    float fTemp2 = (fTemp1 * (0.33333334f - (0.016666668f * (fTemp1 * (((0.33333334f * fTemp1) + fRec0[0]) + 1.0f)))));
    float fTemp3 = (fSlow1 * atanf((fSlow0 * (fTemp2 * (1.5f - (0.5f * (fTemp2 * (fRec0[0] + fTemp2))))))));
    fVec2[0] = fTemp3;
    output[i] = (float)(0.33333334f * ((fVec2[0] + fVec2[1]) + fVec2[2]));
    fVec2[2] = fVec2[1]; fVec2[1] = fVec2[0];
    fVec1[2] = fVec1[1]; fVec1[1] = fVec1[0];
    fRec0[1] = fRec0[0];
    iVec0[1] = iVec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t preamp[nframes];

  compute_preamp(nframes, input, preamp);

  memcpy (output, preamp, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "preamp";

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

  init_preamp();

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
