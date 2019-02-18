/*
   gcc biquad.c -o biquad `pkg-config --libs --cflags jack` -lm -Wall -O3
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

float fslider0;
float fConst0;
float fRec0[3];

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
init_biquad (uint32_t samplingFreq) {
  fConst0 = (6.2831855f / min(1.92e+05f, max(1.0f, (float)samplingFreq)));
  fslider0 = 900.0f; // 300-3000hz 1.2e+03f; // ("Freq", &fslider0, 1.2e+03f, 3e+02f, 3e+03f, 5.0f)
  for (int i=0; i<3; i++) fRec0[i] = 0;
}

inline void 
compute_biquad (int count, float *input, float *output) {
  float fSlow0 = logf((fConst0 * (float)(fslider0)));
  float fSlow1 = (0 - (1.8442f * cosf(expf(((fSlow0 * ((fSlow0 * ((fSlow0 * ((fSlow0 * ((0.00506158f * fSlow0) + 0.06446806f)) + 0.27547622f)) + 0.43359432f)) + 1.3128225f)) + 0.07238887f)))));
  for (int i=0; i<count; i++) {
    fRec0[0] = ((float)input[i] - ((fSlow1 * fRec0[1]) + (0.8502684f * fRec0[2])));
    output[i] = (float)(0.31622776f * (fRec0[0] - (1.059f * fRec0[1])));
    fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t biquad[nframes];

  compute_biquad(nframes, input, biquad);

  memcpy (output, biquad, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "biquad";

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

  init_biquad(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
