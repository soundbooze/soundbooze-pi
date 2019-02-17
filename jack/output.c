/*
   gcc output.c -o output `pkg-config --libs --cflags jack` -lm -Wall -O3
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <jack/jack.h>

jack_port_t *input1_port, *input2_port;
jack_port_t *output1_port, *output2_port;
jack_client_t *client;

float 	level;
float 	fRec0[2];

inline void 
init_output() {
  level = 19.7f; // min = 0.0 max = 50.0
  for (int i=0; i<2; i++) fRec0[i] = 0;
}

inline void 
compute_output (int count, float *input0, float *input1, float *output0, float *output1) {
  float 	fSlow0 = (0.001f * powf(10,(0.05f * (float)(level))));
  for (int i=0; i<count; i++) {
    fRec0[0] = (fSlow0 + (0.999f * fRec0[1]));
    output0[i] = (float)((float)input0[i] * fRec0[0]);
    output1[i] = (float)((float)input1[i] * fRec0[0]);
    fRec0[1] = fRec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input1, *input2, *output1, *output2;

  input1 = jack_port_get_buffer (input1_port, nframes);
  output1 = jack_port_get_buffer (output1_port, nframes);
  input2 = jack_port_get_buffer (input2_port, nframes);
  output2 = jack_port_get_buffer (output2_port, nframes);

  jack_default_audio_sample_t level1[nframes];
  jack_default_audio_sample_t level2[nframes];

  compute_output(nframes, input1, input2, level1, level2);

  memcpy (output1, level1, sizeof (jack_default_audio_sample_t) * nframes);
  memcpy (output2, level2, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "output";

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

  input1_port = jack_port_register (client, "in1",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  input2_port = jack_port_register (client, "in2",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  output1_port = jack_port_register (client, "out1",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  output2_port = jack_port_register (client, "out2",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  if ((input1_port == NULL) || (output1_port == NULL) || (input2_port == NULL) || (output2_port == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
    exit (1);
  }

  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
    exit (1);
  }

  init_output();

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
