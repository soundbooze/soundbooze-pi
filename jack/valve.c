/*
   gcc valve.c -o valve `pkg-config --libs --cflags jack` -lm -Wall -O3
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

inline float 
max(float a, float b) {
  if (a < b)
    return b;
  return a;
}

inline void 
init_valve() {
  fslider0 = -0.6f; //  ("q", 0.0f, -1.7f, 1.7f, 0.01f)
  fslider1 = 1.32f; // ("dist", 0.0f, -2.0f, 2.0f, 0.01f)
}

inline void 
compute_valve (int count, float *input, float *output) {
  float 	fSlow0 = powf(fslider0,3);
  float 	fSlow1 = powf(10,(float)(fslider1));
  float 	fSlow2 = (fSlow1 * (0 - fSlow0));
  float 	fSlow3 = max((float)-600, fSlow2);
  float 	fSlow4 = (0 - fSlow3);
  float 	fSlow5 = (((int)((fabsf(fSlow2) > 0.0001f)))?(((int)((fSlow3 < -50)))?(fSlow4 * expf(fSlow3)):(fSlow3 / (1 - expf(fSlow4)))):((fSlow2 * ((0.083333336f * fSlow2) + 0.5f)) + 1));
  float 	fSlow6 = (0.083333336f * fSlow1);
  float 	fSlow7 = (1.0f / fSlow1);
  for (int i=0; i<count; i++) {
    float fTemp0 = ((float)input[i] - fSlow0);
    float fTemp1 = (fSlow1 * fTemp0);
    float fTemp2 = max((float)-600, fTemp1);
    float fTemp3 = (0 - fTemp2);
    float fTemp4 = (0 - (fSlow0 + (fSlow7 * ((((int)((fabsf(fTemp1) > 0.0001f)))?(((int)((fTemp2 < -50)))?(fTemp3 * expf(fTemp2)):(fTemp2 / (1 - expf(fTemp3)))):((fSlow1 * (fTemp0 * ((fSlow6 * fTemp0) + 0.5f))) + 1)) - fSlow5))));
    float fTemp5 = (fSlow1 * fTemp4);
    float fTemp6 = max((float)-600, fTemp5);
    float fTemp7 = (0 - fTemp6);
    output[i] = (float)(0 - (fSlow7 * ((((int)((fabsf(fTemp5) > 0.0001f)))?(((int)((fTemp6 < -50)))?(fTemp7 * expf(fTemp6)):(fTemp6 / (1 - expf(fTemp7)))):((fSlow1 * (fTemp4 * ((fSlow6 * fTemp4) + 0.5f))) + 1)) - fSlow5)));
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t valve[nframes];

  compute_valve(nframes, input, valve);

  memcpy (output, valve, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "valve";

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

  init_valve();

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
