/*
   gcc expander.c -o expander `pkg-config --libs --cflags jack` -lm -Wall -O3
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

float 	fConst0;
float 	fConst1;
float 	fConst2;
float 	fRec1[2];
float 	fConst3;
float 	fslider0;
float 	fslider1;
float 	fRec0[2];
float 	fentry0;
float 	fentry1;
float 	fentry2;

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
init_expander (uint32_t samplingFreq) {
  fConst0 = min(1.92e+05f, max(1.0f, (float)samplingFreq));
  fConst1 = expf((0 - (1e+01f / fConst0)));
  fConst2 = (1 - fConst1);
  fConst3 = (1.0f / fConst0);

  fslider0 = 0.1f;   // ("release", 0.1f, 0.0f, 1e+01f, 0.01f);
  fslider1 = 0.001f; // ("attack", 0.001f, 0.0f, 1.0f, 0.001f);
  fentry0 = -4e+01f; // ("threshold", -4e+01f, -96.0f, 1e+01f, 0.1f);
  fentry1 = 3.0f;    // ("knee", 3.0f, 0.0f, 2e+01f, 0.1f);
  fentry2 = 2.0f;    // ("ratio", 2.0f, 1.0f, 2e+01f, 0.1f);

  for (int i=0; i<2; i++) fRec1[i] = 0;
  for (int i=0; i<2; i++) fRec0[i] = 0;
}

inline void 
compute_expander (int count, float *input, float *output) {
  float 	fSlow0 = expf((0 - (fConst3 / max(fConst3, (float)(fslider0)))));
  float 	fSlow1 = expf((0 - (fConst3 / max(fConst3, (float)(fslider1)))));
  float 	fSlow2 = (float)(fentry1);
  float 	fSlow3 = (fSlow2 + (float)(fentry0));
  float 	fSlow4 = (1.0f / (fSlow2 + 0.001f));
  float 	fSlow5 = (0.05f * (1 - (float)(fentry2)));
  for (int i=0; i<count; i++) {
    float fTemp0 = (float)input[i];
    fRec1[0] = ((fConst2 * fabsf(fTemp0)) + (fConst1 * fRec1[1]));
    float fTemp1 = max(fRec1[0], fTemp0);
    float fTemp2 = ((fSlow1 * (fRec0[1] < fTemp1)) + (fSlow0 * (fRec0[1] >= fTemp1)));
    fRec0[0] = ((fRec0[1] * fTemp2) + (fTemp1 * (1 - fTemp2)));
    float fTemp3 = max((float)0, (fSlow3 - (20 * log10f(fRec0[0]))));
    output[i] = (float)(fTemp0 * powf(10,(fSlow5 * (fTemp3 * min((float)1, max((float)0, (fSlow4 * fTemp3)))))));
    fRec0[1] = fRec0[0];
    fRec1[1] = fRec1[0];
  }
}



int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t expander[nframes];

  compute_expander(nframes, input, expander);

  memcpy (output, expander, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "expander";

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

  init_expander(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
