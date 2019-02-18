/*
   gcc tremolo.c -o tremolo `pkg-config --libs --cflags jack` -lm -Wall -O3
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
float 	fConst0;
float 	fConst1;
float 	fslider0;
float 	fConst2;
int 	iRec2[2];
int 	iRec1[2];
float 	fConst3;
float 	fRec5[2];
float 	fRec4[2];
float 	fRec3[2];
float 	fcheckbox0;
float 	fslider1;
float 	fRec0[2];
float 	fslider2;

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
init_tremolo (uint32_t samplingFreq) {
  fConst0 = min(1.92e+05f, max(1.0f, (float)samplingFreq));
  fConst1 = (1.0f / fConst0);
  fConst2 = (0.5f * fConst0);
  fConst3 = (6.2831855f / fConst0);

  fcheckbox0 = 0.0;  // ("enum", "os.triangle|sine")
  fslider0 = 4.8f;   // ("freq", 5.0f, 0.1f, 5e+01f, 0.1f)
  fslider1 = 0.48f;   // ("depth", 0.5f, 0.0f, 1.0f, 0.01f)
  fslider2 = 70.0f; // ("wet_dry", 1e+02f, 0.0f, 1e+02f, 1.0f)

  for (int i=0; i<2; i++) iVec0[i] = 0;
  for (int i=0; i<2; i++) iRec2[i] = 0;
  for (int i=0; i<2; i++) iRec1[i] = 0;
  for (int i=0; i<2; i++) fRec5[i] = 0;
  for (int i=0; i<2; i++) fRec4[i] = 0;
  for (int i=0; i<2; i++) fRec3[i] = 0;
  for (int i=0; i<2; i++) fRec0[i] = 0;
}

inline void 
compute_tremolo (int count, float *input, float *output) {
  float 	fSlow0 = (float)(fslider0);
  int 	iSlow1 = (int)((fConst2 / (float)(fSlow0)));
  float 	fSlow2 = (1.0f / (float)(iSlow1));
  float 	fSlow3 = (fConst3 * fSlow0);
  int 	iSlow4 = (int)((float)(fcheckbox0));
  float 	fSlow5 = (float)(fslider1);
  float 	fSlow6 = (float)(fslider2);
  for (int i=0; i<count; i++) {
    iVec0[0] = 1;
    float fTemp0 = (fRec0[1] * (1 - (fConst1 / (fConst1 + (0.06f * expf((0 - (2.4849067f * fRec0[1]))))))));
    iRec2[0] = (((int)((iRec2[1] > 0)))?((2 * (iRec1[1] < iSlow1)) + -1):(1 - (2 * (iRec1[1] > 0))));
    iRec1[0] = (iRec1[1] + iRec2[0]);
    fRec5[0] = ((fSlow3 * (0 - fRec3[1])) + fRec5[1]);
    fRec4[0] = ((fRec4[1] + (fSlow3 * fRec5[0])) + (1 - iVec0[1]));
    fRec3[0] = fRec4[0];
    fRec0[0] = (fTemp0 + (fConst1 * (powf((1 - (fSlow5 * (1 - ((iSlow4)?max((float)0, (0.5f * (fRec3[0] + 1))):(fSlow2 * iRec1[0]))))),1.9f) / (fConst1 + (0.06f * expf((0 - (2.4849067f * fTemp0))))))));
    output[i] = (float)((float)input[i] * ((fSlow6 * ((27.0f / (expf((13.815511f / logf(((8.551968f * fRec0[0]) + 2.7182817f)))) + 2700)) + -0.01f)) + 1));

    fRec0[1] = fRec0[0];
    fRec3[1] = fRec3[0];
    fRec4[1] = fRec4[0];
    fRec5[1] = fRec5[0];
    iRec1[1] = iRec1[0];
    iRec2[1] = iRec2[0];
    iVec0[1] = iVec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t tremolo[nframes];

  compute_tremolo(nframes, input, tremolo);

  memcpy (output, tremolo, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "tremolo";

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

  init_tremolo(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
