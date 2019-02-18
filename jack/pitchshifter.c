/*
   gcc pitchshifter.c -o pitchshifter `pkg-config --libs --cflags jack` -lm -Wall -O3
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <jack/jack.h>

jack_port_t *input_port1, *input_port2;
jack_port_t *output_port1, *output_port2;
jack_client_t *client;

float 	fslider0;
float 	fslider1;
float 	fslider2;
float 	fRec0[2];
float 	fslider3;
int 	  IOTA;
float 	fVec0[65536];
float 	fslider4;
float 	fVec1[65536];

inline float 
min(float a, float b) {
  if (a > b)
    return b;
  return a;
}

inline void 
init_pitchshifter() {
  fslider0 = 0.0f;   // ("Pitch Shift", 0.0f, -12.0f, 12.0f, 0.1f) (octave)
  fslider1 = 0.0f;   // ("Super Pitch", 0.0f, 0.0f, 12.0f, 0.01f)  (octave)
  fslider2 = 1e+03f; // ("Window", 1e+03f, 5e+01f, 1e+04f, 1.0f)
  fslider3 = 1e+01f; // ("Crossfade", &fslider3, 1e+01f, 1.0f, 1e+04f, 1.0f)
  fslider4 = 0.5f;   // ("Dry/Wet", &fslider4, 0.5f, 0.0f, 1.0f, 0.01f)
  for (int i=0; i<2; i++) fRec0[i] = 0;
  IOTA = 0;
  for (int i=0; i<65536; i++) fVec0[i] = 0;
  for (int i=0; i<65536; i++) fVec1[i] = 0;
}

unsigned int index1 = 0;

inline void 
compute_pitchshifter (int count, float *input0, float *input1, float *output0, float *output1) {
  //float 	fSlow0 = powf(2,(0.083333336f * ((float)(fslider1) + (float)(fslider0))));
  float rf = fslider1; //((float)rand()/(float)(RAND_MAX)) * 12;
  if (index1 == 1024) {
    rf = ((float)rand()/(float)(RAND_MAX)) * 12;
    index1 = 0;
  }
  index1++;
  float 	fSlow0 = powf(2,(0.083333336f * ((float)(rf) + (float)(rf - 2))));
  float 	fSlow1 = (float)(fslider2);
  float 	fSlow2 = (1.0f / (float)(fslider3));
  float 	fSlow3 = (float)(fslider4);
  float 	fSlow4 = (1 - fSlow3);
  for (int i=0; i<count; i++) {
    fRec0[0] = fmodf((fSlow1 + ((fRec0[1] + 1) - fSlow0)),fSlow1);
    float fTemp0 = min((fSlow2 * fRec0[0]), (float)1);
    float fTemp1 = (1 - fTemp0);
    float fTemp2 = (float)input0[i];
    fVec0[IOTA&65535] = fTemp2;
    float fTemp3 = (fSlow1 + fRec0[0]);
    int iTemp4 = (int)(fTemp3);
    int iTemp5 = (int)(((int)((iTemp4 + 1)) & 65535));
    float fTemp6 = floorf(fTemp3);
    float fTemp7 = (fSlow1 + (fRec0[0] - fTemp6));
    float fTemp8 = (1 - fRec0[0]);
    float fTemp9 = ((fTemp6 + fTemp8) - fSlow1);
    int iTemp10 = (int)((iTemp4 & 65535));
    int iTemp11 = (int)(fRec0[0]);
    int iTemp12 = (int)(((int)((iTemp11 + 1)) & 65535));
    float fTemp13 = floorf(fRec0[0]);
    float fTemp14 = (fRec0[0] - fTemp13);
    float fTemp15 = (fTemp13 + fTemp8);
    int iTemp16 = (int)((iTemp11 & 65535));
    output0[i] = (float)((fSlow4 * fVec0[IOTA&65535]) + (fSlow3 * ((((fVec0[(IOTA-iTemp16)&65535] * fTemp15) + (fTemp14 * fVec0[(IOTA-iTemp12)&65535])) * fTemp0) + (((fVec0[(IOTA-iTemp10)&65535] * fTemp9) + (fTemp7 * fVec0[(IOTA-iTemp5)&65535])) * fTemp1))));
    float fTemp17 = (float)input1[i];
    fVec1[IOTA&65535] = fTemp17;
    output1[i] = (float)((fSlow4 * fVec1[IOTA&65535]) + (fSlow3 * ((fTemp0 * ((fTemp15 * fVec1[(IOTA-iTemp16)&65535]) + (fTemp14 * fVec1[(IOTA-iTemp12)&65535]))) + (fTemp1 * ((fTemp9 * fVec1[(IOTA-iTemp10)&65535]) + (fTemp7 * fVec1[(IOTA-iTemp5)&65535]))))));
    IOTA = IOTA+1;
    fRec0[1] = fRec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input1_in, *input2_in, *output1_out, *output2_out;

  input1_in = jack_port_get_buffer (input_port1, nframes);
  output1_out = jack_port_get_buffer (output_port1, nframes);

  input2_in = jack_port_get_buffer (input_port2, nframes);
  output2_out = jack_port_get_buffer (output_port2, nframes);

  jack_default_audio_sample_t pitchshifter_l[nframes];
  jack_default_audio_sample_t pitchshifter_r[nframes];

  compute_pitchshifter(nframes, input1_in, input2_in, pitchshifter_l, pitchshifter_r);

  memcpy (output1_out, pitchshifter_l, sizeof (jack_default_audio_sample_t) * nframes);
  memcpy (output2_out, pitchshifter_r, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "pitchshifter";

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

  input_port1 = jack_port_register (client, "input1",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  input_port2 = jack_port_register (client, "input2",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  output_port1 = jack_port_register (client, "output1",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  output_port2 = jack_port_register (client, "output2",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  if ((input_port1 == NULL) || (output_port1 == NULL) || (input_port2 == NULL) || (output_port2 == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
    exit (1);
  }

  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
    exit (1);
  }

  init_pitchshifter();

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
