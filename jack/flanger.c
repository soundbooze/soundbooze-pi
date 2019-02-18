/*
   gcc flanger.c -o flanger `pkg-config --libs --cflags jack` -lm -Wall -O3
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
int 	iVec0[2];
int 	IOTA;
float 	fVec1[2048];
float 	fslider2;
float 	fConst0;
float 	fConst1;
float 	fRec1[2];
float 	fRec2[2];
float 	fRec0[2];

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
init_flanger (uint32_t samplingFreq) {
  fConst0 = min(1.92e+05f, max(1.0f, (float)samplingFreq));
  fConst1 = (6.2831855f / fConst0);

  fslider0 = -2.0f;    // ("level", 0.0f, -6e+01f, 1e+01f, 0.1f)
  fslider1 = 80.0f;  // ("wet_dry", 1e+02f, 0.0f, 1e+02f, 1.0f)
  fslider2 = 0.2f;    // ("freq", 0.2f, 0.0f, 5.0f, 0.01f)

  for (int i=0; i<2; i++) iVec0[i] = 0;
  IOTA = 0;
  for (int i=0; i<2048; i++) fVec1[i] = 0;
  for (int i=0; i<2; i++) fRec1[i] = 0;
  for (int i=0; i<2; i++) fRec2[i] = 0;
  for (int i=0; i<2; i++) fRec0[i] = 0;
}

inline void 
compute_flanger (int count, float *input, float *output) {
  float 	fSlow0 = (float)(fslider1);
  float 	fSlow1 = (0.01f * (fSlow0 * powf(10,(0.05f * (float)(fslider0)))));
  float 	fSlow2 = (fConst1 * (float)(fslider2));
  float 	fSlow3 = cosf(fSlow2);
  float 	fSlow4 = sinf(fSlow2);
  float 	fSlow5 = (0 - fSlow4);
  float 	fSlow6 = (1 - (0.01f * fSlow0));
  for (int i=0; i<count; i++) {
    float fTemp0 = (float)input[i];
    float fTemp1 = (fSlow1 * fTemp0);
    iVec0[0] = 1;
    float fTemp2 = ((0.5f * fRec0[1]) - fTemp1);
    fVec1[IOTA&2047] = fTemp2;
    fRec1[0] = ((fSlow4 * fRec2[1]) + (fSlow3 * fRec1[1]));
    fRec2[0] = (((fSlow3 * fRec2[1]) + (fSlow5 * fRec1[1])) + (1 - iVec0[1]));
    float fTemp3 = (fConst0 * ((0.005f * (fRec1[0] + 1)) + 0.001f));
    int iTemp4 = (int)(fTemp3);
    float fTemp5 = floorf(fTemp3);
    fRec0[0] = ((fVec1[(IOTA-(int)((iTemp4 & 2047)))&2047] * (fTemp5 + (1 - fTemp3))) + ((fTemp3 - fTemp5) * fVec1[(IOTA-(int)(((int)((iTemp4 + 1)) & 2047)))&2047]));
    output[i] = (float)((fSlow6 * fTemp0) + (0.5f * (fTemp1 - fRec0[0])));
    fRec0[1] = fRec0[0];
    fRec2[1] = fRec2[0];
    fRec1[1] = fRec1[0];
    IOTA = IOTA+1;
    iVec0[1] = iVec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t flanger[nframes];

  compute_flanger(nframes, input, flanger);

  memcpy (output, flanger, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "flanger";

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

  init_flanger(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
