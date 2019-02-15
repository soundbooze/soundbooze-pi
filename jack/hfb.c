/*
   gcc hfb.c -o hfb `pkg-config --libs --cflags jack` -lm -Wall -O3
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

double fConst0;
double fConst1;
double fConst2;
double fConst3;
double fConst4;
double fConst5;
double fConst6;
double fConst7;
double fConst8;
double fConst9;
double fConst10;

double fRec0[2];
double fRec1[2];
double fRec2[3];

inline double 
min(double a, double b) {
  if (a > b)
    return b;
  return a;
}

inline double 
max(double a, double b) {
  if (a < b)
    return b;
  return a;
}

inline void 
init_hfb (double samplingFreq)
{
  int i = 0;

	fConst0 = (double) min(1.92e+05, max(1.0, samplingFreq));
	fConst1 = (2.36551040815034e-10 * fConst0);
	fConst2 = (1.32257544516629e-08 + fConst1);
	fConst3 = (1.17023754306854e-09 * fConst0);
	fConst4 = (0 - (6.50608604711861e-08 + fConst3));
	fConst5 = (1.18275520407517e-09 * fConst0);
	fConst6 = (6.61287722583147e-08 + fConst5);
	fConst7 = (fConst1 - 1.32257544516629e-08);
	fConst8 = (6.50608604711861e-08 - fConst3);
	fConst9 = (fConst5 - 6.61287722583147e-08);
	fConst10 = powf(fConst0, 2);

	for (i=0; i<2; i++) fRec0[i] = 0;
	for (i=0; i<2; i++) fRec1[i] = 0;
	for (i=0; i<3; i++) fRec2[i] = 0;
}

inline void 
compute_hfb (int count, float *input0, float *output0)
{
	double 	fSlow0 = (0.007000000000000006 * (1 - 0.28));
	double 	fSlow1 = (0.007000000000000006 * 0.32);

	for (int i=0; i<count; i++) {
		fRec0[0] = (fSlow0 + (0.993 * fRec0[1]));
		fRec1[0] = (fSlow1 + (0.993 * fRec1[1]));
		double fTemp0 = (4.40858481722098e-05 + (fConst0 * (7.88503469383447e-07 + ((fRec1[0] * (fConst6 + (fConst4 * fRec1[0]))) + (fConst2 * fRec0[0])))));
		double fTemp1 = (fConst0 * ((fRec1[0] * ((5.12666523663255e-23 * fRec1[0]) - 1.59041010538546e-09)) - (3.18082021077091e-10 * fRec0[0])));
		fRec2[0] = ((double)input0[i] - (((fRec2[1] * (8.81716963444196e-05 + (fConst10 * ((fRec1[0] * ((2.34047508613708e-09 * fRec1[0]) - 2.36551040815034e-09)) - (4.73102081630068e-10 * fRec0[0]))))) + (fRec2[2] * (4.40858481722098e-05 + (fConst0 * (((fRec1[0] * (fConst9 + (fConst8 * fRec1[0]))) + (fConst7 * fRec0[0])) - 7.88503469383447e-07))))) / fTemp0));
		output0[i] = (float)(fConst0 * (((fConst0 * (fRec2[1] * ((6.36164042154183e-10 * fRec0[0]) + (fRec1[0] * (3.18082021077092e-09 - (1.02533304732651e-22 * fRec1[0])))))) + ((fRec2[0] * (fTemp1 - 1.06027340359031e-06)) + (fRec2[2] * (1.06027340359031e-06 + fTemp1)))) / fTemp0));
		fRec2[2] = fRec2[1]; fRec2[1] = fRec2[0];
		fRec1[1] = fRec1[0];
		fRec0[1] = fRec0[0];
	}
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input1_in, *input2_in, *output1_out, *output2_out;
  int len = sizeof (jack_default_audio_sample_t) * nframes;

  input1_in = jack_port_get_buffer (input_port1, nframes);
  output1_out = jack_port_get_buffer (output_port1, nframes);

  input2_in = jack_port_get_buffer (input_port2, nframes);
  output2_out = jack_port_get_buffer (output_port2, nframes);

  jack_default_audio_sample_t hfb_l[len];
  jack_default_audio_sample_t hfb_r[len];

  compute_hfb(len, input1_in, hfb_l);
  compute_hfb(len, input2_in, hfb_r);

  memcpy (output1_out, hfb_l, len);
  memcpy (output2_out, hfb_r, len);

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
  const char *client_name = "hfb";

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

  init_hfb(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
