/*
   gcc mxrdist.c -o mxrdist `pkg-config --libs --cflags jack` -lm -Wall -O3
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
float 	fConst3;
float 	fConst4;
float 	fConst5;
float 	fConst6;
float 	fConst7;
float 	fConst8;
float 	fConst9;
float 	fConst10;
float 	fConst11;
float 	fConst12;
float 	fConst13;
float 	fConst14;
float 	fConst15;
float 	fConst16;
float 	fRec3[3];
float 	fConst17;
float 	fConst18;
float 	fConst19;
float 	fConst20;
float 	fVec0[2];
float 	fslider0;
float 	fRec5[2];
float 	fConst21;
float 	fConst22;
float 	fConst23;
float 	fConst24;
float 	fRec4[2];
float 	fVec1[2];
float 	fConst25;
float 	fConst26;
float 	fConst27;
float 	fRec2[2];
float 	fRec1[3];
float 	fRec0[3];
float 	fslider1;
float 	fRec6[2];
float 	fConst28;

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
init_mxrdist (uint32_t samplingFreq) {
  fConst0 = min(1.92e+05f, max(1.0f, (float)samplingFreq*2.0));
  fConst1 = (float)(fConst0);
  fConst2 = (1.4066874e-10f * fConst1);
  fConst3 = ((fConst1 * (fConst2 + -1.4073908e-05f)) + 0.00035167186f);
  fConst4 = powf(fConst1,2);
  fConst5 = (0.0007033437f - (2.8133748e-10f * fConst4));
  fConst6 = ((fConst1 * (fConst2 + 1.4073908e-05f)) + 0.00035167186f);
  fConst7 = (1.0f / fConst6);
  fConst8 = tanf((50893.8f / fConst0));
  fConst9 = (2 * (1 - (1.0f / powf(fConst8,2))));
  fConst10 = (1.0f / fConst8);
  fConst11 = (((fConst10 + -1.0f) / fConst8) + 1);
  fConst12 = (1.0f / (((fConst10 + 1.0f) / fConst8) + 1));
  fConst13 = (4.335671e-10f * fConst1);
  fConst14 = ((fConst1 * (fConst13 + -2.2111923e-08f)) + 2.1463718e-08f);
  fConst15 = (4.2927436e-08f - (8.671342e-10f * fConst4));
  fConst16 = (1.0f / ((fConst1 * (fConst13 + 2.2111923e-08f)) + 2.1463718e-08f));
  fConst17 = (4.2927437e-10f * fConst1);
  fConst18 = (fConst17 + -6.4391154e-10f);
  fConst19 = (fConst17 + 6.4391154e-10f);
  fConst20 = (0 - (8.5854873e-10f * fConst4));
  fConst21 = (9.4e-08f * fConst0);
  fConst22 = (0.0479118f * fConst0);
  fConst23 = (1 - fConst22);
  fConst24 = (fConst22 + 1);
  fConst25 = (fConst10 + 1);
  fConst26 = (1.0f / fConst25);
  fConst27 = (0 - ((1 - fConst10) / fConst25));
  fConst28 = (fConst1 / fConst6);

  fslider0 = 0.8f; // ("drive", 0.5f, 0.0f, 1.0f, 0.01f)
  fslider1 = 0.4f; // ("Volume", 0.5f, 0.0f, 1.0f, 0.01f)

  for (int i=0; i<3; i++) fRec3[i] = 0;
  for (int i=0; i<2; i++) fVec0[i] = 0;
  for (int i=0; i<2; i++) fRec5[i] = 0;
  for (int i=0; i<2; i++) fRec4[i] = 0;
  for (int i=0; i<2; i++) fVec1[i] = 0;
  for (int i=0; i<2; i++) fRec2[i] = 0;
  for (int i=0; i<3; i++) fRec1[i] = 0;
  for (int i=0; i<3; i++) fRec0[i] = 0;
  for (int i=0; i<2; i++) fRec6[i] = 0;
}

const double cliptable[] =
{
	0.0,-0.0297117955828,-0.0600180054016,-0.0909366474689,-0.122486475452,
	-0.154687016603,-0.187558612066,-0.221122459724,-0.255400659785,-0.290416263311,
	-0.32619332391,-0.362756952842,-0.400133377793,-0.43835000562,-0.477435489371,
	-0.517419799931,-0.558334302664,-0.600211839473,-0.64308681672,-0.686995299506,
	-0.731975112846,-0.778065950352,-0.825309491059,-0.873749525136,-0.923432089265,
	-0.974405612576,-1.0267210741,-1.08043217285,-1.13559551155,-1.19227079528,
	-1.25052104388,-1.31041280223,-1.3720162023,-1.43540358623,-1.50063467874,
	-1.5676109484,-1.63471748928,-1.69222628207,-1.72818769616,-1.74925333761,
	-1.76321031985,-1.77347868583,-1.78157546286,-1.78826553445,-1.79397934372,
	-1.79898035985,-1.8034405747,-1.80747806592,-1.81117723448,-1.81460043341,
	-1.81779501238,-1.82079777708,-1.8236379189,-1.8263390017,-1.8289203466,
	-1.83139802075,-1.83378555835,-1.83609449674,-1.83833478175,-1.84051507916,
	-1.84264301759,-1.84472538062,-1.84676826077,-1.8487771847,-1.85075721643,
	-1.85271304361,-1.85464905097,-1.85656938373,-1.85847800364,-1.86037873945,
	-1.86227533346,-1.86417148579,-1.86607089734,-1.86797731307,-1.8698945665,
	-1.87182662708,-1.87377765176,-1.8757520425,-1.87775451194,-1.87979015981,
	-1.88186456341,-1.88398388656,-1.88615501283,-1.88838571081,-1.89068484223,
	-1.89306262786,-1.89553099258,-1.89810402058,-1.90079856664,-1.90363509344,
	-1.90663884415,-1.90984152641,-1.91328380224,-1.91701909716,-1.92111966903,
	-1.92568676559,-1.93086870081,-1.93689565223,-1.94415405144,-1.95337103981
};

inline double 
opamp2(float x) {
  float low = 0.0;
  /* float high = 0.970874; */
  float istep = 101.97;
  int size = 100.0;
  float f = fabsf(x);
  f = (f/(3.0 + f) - low) * istep;
  int i = (int)f;
  if (i < 0) {
    f = (float)cliptable[0];
  } else if (i >= size-1) {
    f = (float)cliptable[size-1];
  } else {
    f -= i;
    f = (float)cliptable[i]*(1-f) + (float)cliptable[i+1]*f;
  }
  return copysign(f, -x);
}

inline void 
compute_mxrdist (int count, float *input, float *output) {
  float 	fSlow0 = (0.007f * ((0.25f * (float)(fslider0)) + 0.75f));
  float 	fSlow1 = (0.007f * (float)(fslider1));
  for (int i=0; i<count; i++) {
    fRec3[0] = ((float)input[i] - (fConst16 * ((fConst15 * fRec3[1]) + (fConst14 * fRec3[2]))));
    float fTemp0 = ((fConst20 * fRec3[1]) + (fConst1 * ((fConst19 * fRec3[0]) + (fConst18 * fRec3[2]))));
    fVec0[0] = fTemp0;
    float fTemp1 = (fConst16 * fVec0[0]);
    fRec5[0] = (fSlow0 + (0.993f * fRec5[1]));
    float fTemp2 = (fConst21 * ((500000 * (1.0f - fRec5[0])) + 4700));
    float fTemp3 = (fTemp2 + 1);
    fRec4[0] = ((fRec4[1] * (0 - ((1 - fTemp2) / fTemp3))) + (fConst16 * (((fConst24 * fVec0[0]) + (fConst23 * fVec0[1])) / fTemp3)));
    float fTemp4 = opamp2((fRec4[0] - fTemp1));
    fVec1[0] = (fTemp1 - fTemp4);
    fRec2[0] = ((fConst27 * fRec2[1]) + (fConst26 * ((fTemp1 + fVec1[1]) - fTemp4)));
    fRec1[0] = (fRec2[0] - (fConst12 * ((fConst11 * fRec1[2]) + (fConst9 * fRec1[1]))));
    fRec0[0] = ((fConst12 * (fRec1[0] + (fRec1[2] + (2.0f * fRec1[1])))) - (fConst7 * ((fConst5 * fRec0[1]) + (fConst3 * fRec0[2]))));
    fRec6[0] = (fSlow1 + (0.993f * fRec6[1]));
    output[i] = (float)(fConst28 * ((fRec0[2] * (0 - (7.0334368e-06f * fRec6[0]))) + (7.0334368e-06f * (fRec6[0] * fRec0[0]))));
    fRec6[1] = fRec6[0];
    fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
    fRec1[2] = fRec1[1]; fRec1[1] = fRec1[0];
    fRec2[1] = fRec2[0];
    fVec1[1] = fVec1[0];
    fRec4[1] = fRec4[0];
    fRec5[1] = fRec5[0];
    fVec0[1] = fVec0[0];
    fRec3[2] = fRec3[1]; fRec3[1] = fRec3[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t mxrdist[nframes];

  compute_mxrdist(nframes, input, mxrdist);

  memcpy (output, mxrdist, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "mxrdist";

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

  init_mxrdist(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
