/*
   gcc stereodelay.c -o stereodelay `pkg-config --libs --cflags jack` -lm -Wall -O3
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

int 	  IOTA;
float 	fVec0[262144];
int 	  iVec1[2];
float 	fslider0;
float 	fConst0;
float 	fConst1;
float 	fRec0[2];
float 	fRec1[2];
float 	fRec2[2];
float 	fRec3[2];
float 	fslider1;
float 	fConst2;
float 	fRec4[2];
float 	fRec5[2];
float 	fcheckbox0;
float 	fslider2;
float 	fRec6[2];
float 	fVec2[262144];
float 	fslider3;
float 	fRec7[2];
float 	fRec8[2];
float 	fRec9[2];
float 	fRec10[2];
float 	fslider4;
float 	fRec11[2];

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
init_stereodelay (uint32_t samplingFreq) {
  fConst0 = min(1.92e+05f, max(1.0f, (float)samplingFreq));
  fConst1 = (60 * fConst0);
  fConst2 = (0.10471976f / fConst0);
  IOTA = 0;
  for (int i=0; i<262144; i++) fVec0[i] = 0;
  for (int i=0; i<2; i++) iVec1[i] = 0;
  for (int i=0; i<2; i++) fRec0[i] = 0;
  for (int i=0; i<2; i++) fRec1[i] = 0;
  for (int i=0; i<2; i++) fRec2[i] = 0;
  for (int i=0; i<2; i++) fRec3[i] = 0;
  for (int i=0; i<2; i++) fRec4[i] = 0;
  for (int i=0; i<2; i++) fRec5[i] = 0;
  for (int i=0; i<2; i++) fRec6[i] = 0;
  for (int i=0; i<262144; i++) fVec2[i] = 0;
  for (int i=0; i<2; i++) fRec7[i] = 0;
  for (int i=0; i<2; i++) fRec8[i] = 0;
  for (int i=0; i<2; i++) fRec9[i] = 0;
  for (int i=0; i<2; i++) fRec10[i] = 0;
  for (int i=0; i<2; i++) fRec11[i] = 0;

  // param argv || auto adjustlive
  fslider0 = 1.2e+02f; // delayL ("lbpm", 1.2e+02f, 24.0f, 3.6e+02f, 1.0f)
  fslider1 = 24.0f; // LFO freq  ("lfobpm", 24.0f, 24.0f, 3.6e+02f, 1.0f)
  fcheckbox0 = 1.0; // 0.0; // invert
  fslider2 = -9.2; // 0.0f; // gainL      ("l_gain", 0.0f, -2e+01f, 2e+01f, 0.1f)
  fslider3 = 1.2e+02f; //delayR  ("rbpm", 1.2e+02f, 24.0f, 3.6e+02f, 1.0f)
  fslider4 = -9.2; // 0.0f; // gainR      ("r_gain", 0.0f, -2e+01f, 2e+01f, 0.1f)
}

inline void 
compute_stereodelay (int count, float *input, float *output0, float *output1) {
  float 	fSlow0 = (fConst1 / (float)(fslider0));
  float 	fSlow1 = (fConst2 * (float)(fslider1));
  float 	fSlow2 = cosf(fSlow1);
  float 	fSlow3 = sinf(fSlow1);
  float 	fSlow4 = (0 - fSlow3);
  float 	fSlow5 = (float)(fcheckbox0);
  float 	fSlow6 = (0.001f * powf(10,(0.05f * (float)(fslider2))));
  float 	fSlow7 = (fConst1 / (float)(fslider3));
  float 	fSlow8 = (0.001f * powf(10,(0.05f * (float)(fslider4))));
  for (int i=0; i<count; i++) {
    float fTemp0 = (float)input[i];
    fVec0[IOTA&262143] = fTemp0;
    iVec1[0] = 1;
    float fTemp1 = (((int)((fRec0[1] != 0.0f)))?(((int)(((fRec1[1] > 0.0f) & (fRec1[1] < 1.0f))))?fRec0[1]:0):(((int)(((fRec1[1] == 0.0f) & (fSlow0 != fRec2[1]))))?0.0009765625f:(((int)(((fRec1[1] == 1.0f) & (fSlow0 != fRec3[1]))))?-0.0009765625f:0)));
    fRec0[0] = fTemp1;
    fRec1[0] = max(0.0f, min(1.0f, (fRec1[1] + fTemp1)));
    fRec2[0] = (((int)(((fRec1[1] >= 1.0f) & (fRec3[1] != fSlow0))))?fSlow0:fRec2[1]);
    fRec3[0] = (((int)(((fRec1[1] <= 0.0f) & (fRec2[1] != fSlow0))))?fSlow0:fRec3[1]);
    fRec4[0] = ((fSlow3 * fRec5[1]) + (fSlow2 * fRec4[1]));
    fRec5[0] = (((fSlow2 * fRec5[1]) + (fSlow4 * fRec4[1])) + (1 - iVec1[1]));
    fRec6[0] = (fSlow6 + (0.999f * fRec6[1]));
    output0[i] = (float)(fVec0[IOTA&262143] + ((fRec6[0] * (1 - (fSlow5 * fRec4[0]))) * ((fVec0[(IOTA-(int)(((int)(fRec2[0]) & 262143)))&262143] * (1.0f - fRec1[0])) + (fRec1[0] * fVec0[(IOTA-(int)(((int)(fRec3[0]) & 262143)))&262143]))));
    float fTemp2 = (float)input[i];
    fVec2[IOTA&262143] = fTemp2;
    float fTemp3 = (((int)((fRec7[1] != 0.0f)))?(((int)(((fRec8[1] > 0.0f) & (fRec8[1] < 1.0f))))?fRec7[1]:0):(((int)(((fRec8[1] == 0.0f) & (fSlow7 != fRec9[1]))))?0.0009765625f:(((int)(((fRec8[1] == 1.0f) & (fSlow7 != fRec10[1]))))?-0.0009765625f:0)));
    fRec7[0] = fTemp3;
    fRec8[0] = max(0.0f, min(1.0f, (fRec8[1] + fTemp3)));
    fRec9[0] = (((int)(((fRec8[1] >= 1.0f) & (fRec10[1] != fSlow7))))?fSlow7:fRec9[1]);
    fRec10[0] = (((int)(((fRec8[1] <= 0.0f) & (fRec9[1] != fSlow7))))?fSlow7:fRec10[1]);
    fRec11[0] = (fSlow8 + (0.999f * fRec11[1]));
    output1[i] = (float)(fVec2[IOTA&262143] + ((fRec11[0] * (1 - (fSlow5 * (0 - fRec4[0])))) * ((fVec2[(IOTA-(int)(((int)(fRec9[0]) & 262143)))&262143] * (1.0f - fRec8[0])) + (fRec8[0] * fVec2[(IOTA-(int)(((int)(fRec10[0]) & 262143)))&262143]))));
    fRec11[1] = fRec11[0];
    fRec10[1] = fRec10[0];
    fRec9[1] = fRec9[0];
    fRec8[1] = fRec8[0];
    fRec7[1] = fRec7[0];
    fRec6[1] = fRec6[0];
    fRec5[1] = fRec5[0];
    fRec4[1] = fRec4[0];
    fRec3[1] = fRec3[0];
    fRec2[1] = fRec2[0];
    fRec1[1] = fRec1[0];
    fRec0[1] = fRec0[0];
    iVec1[1] = iVec1[0];
    IOTA = IOTA+1;
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input1_in, /**input2_in,*/ *output1_out, *output2_out;

  input1_in = jack_port_get_buffer (input_port1, nframes);
  output1_out = jack_port_get_buffer (output_port1, nframes);

  //input2_in = jack_port_get_buffer (input_port2, nframes);
  output2_out = jack_port_get_buffer (output_port2, nframes);

  jack_default_audio_sample_t stereodelay_l[nframes];
  jack_default_audio_sample_t stereodelay_r[nframes];

  compute_stereodelay(nframes, input1_in, stereodelay_l, stereodelay_r);

  memcpy (output1_out, stereodelay_l, sizeof (jack_default_audio_sample_t) * nframes);
  memcpy (output2_out, stereodelay_r, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "stereodelay";

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

  init_stereodelay(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
