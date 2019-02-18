/*
   gcc voxwah.c -o voxwah `pkg-config --libs --cflags jack` -lm -Wall -O3
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
float 	fslider0;
float 	fConst0;
float 	fConst1;
float 	fRec2[2];
float 	fRec1[2];
float 	fRec0[2];
float 	fConst2;
float 	fConst3;
float 	fRec5[2];
float 	fConst4;
float 	fConst5;
float 	fRec4[2];
float 	fRec3[2];
float 	fslider1;
float 	fRec6[2];
float 	fslider2;
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
float 	fConst17;
float 	fConst18;
float 	fConst19;
float 	fConst20;
float 	fConst21;
float 	fConst22;
float 	fConst23;
float 	fConst24;
float 	fConst25;
float 	fConst26;
float 	fConst27;
float 	fConst28;
float 	fConst29;
float 	fConst30;
float 	fConst31;
float 	fConst32;
float 	fConst33;
float 	fConst34;
float 	fslider3;
float 	fConst35;
float 	fConst36;
float 	fConst37;
float 	fVec1[2];
float 	fConst38;
float 	fConst39;
float 	fRec8[2];
float 	fRec7[5];
float 	fConst40;
float 	fConst41;
float 	fConst42;
float 	fConst43;
float 	fConst44;
float 	fConst45;
float 	fConst46;
float 	fConst47;
float 	fConst48;
float 	fConst49;
float 	fConst50;
float 	fConst51;
float 	fConst52;
float 	fConst53;
float 	fConst54;

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

inline void init_voxwah (uint32_t samplingFreq) {
  fConst0 = min(1.92e+05f, max(1.0f, (float)samplingFreq));
  fConst1 = (0.10471976f / fConst0);
  fConst2 = expf((0 - (1e+01f / fConst0)));
  fConst3 = (1.0f - fConst2);
  fConst4 = expf((0 - (1e+02f / fConst0)));
  fConst5 = (1.0f - fConst4);
  fConst6 = (float)(fConst0);
  fConst7 = (4.4365386e-20f * fConst6);
  fConst8 = ((fConst6 * (-6.3675214e-18f - fConst7)) + -6.905114e-14f);
  fConst9 = (2.9643705e-19f * fConst6);
  fConst10 = ((fConst6 * (fConst9 + 2.2065455e-17f)) + 5.226202e-14f);
  fConst11 = (5.0064296e-20f * fConst6);
  fConst12 = (fConst6 * ((fConst6 * (fConst11 + 3.442551e-17f)) + 6.238369e-13f));
  fConst13 = (1.2535493e-21f * fConst6);
  fConst14 = ((fConst6 * (fConst13 + -3.4824046e-17f)) + 5.1367795e-14f);
  fConst15 = (1.2519099e-21f * fConst6);
  fConst16 = ((fConst6 * (3.485233e-17f - fConst15)) + -5.2316993e-14f);
  fConst17 = (7.560839e-21f * fConst6);
  fConst18 = (fConst6 * ((fConst6 * (2.5031608e-16f - fConst17)) + -3.7389552e-13f));
  fConst19 = ((fConst6 * (6.3675214e-18f - fConst7)) + -6.905114e-14f);
  fConst20 = ((fConst6 * (fConst9 + -2.2065455e-17f)) + 5.226202e-14f);
  fConst21 = (fConst6 * ((fConst6 * (fConst11 + -3.442551e-17f)) + 6.238369e-13f));
  fConst22 = (1.7746155e-19f * fConst6);
  fConst23 = (fConst22 + -1.2735043e-17f);
  fConst24 = (1.1857482e-18f * fConst6);
  fConst25 = (4.413091e-17f - fConst24);
  fConst26 = powf(fConst6,2);
  fConst27 = (2.0025718e-19f * fConst6);
  fConst28 = (fConst26 * (6.885102e-17f - fConst27));
  fConst29 = (1.3810228e-13f - (2.661923e-19f * fConst26));
  fConst30 = (1.7786224e-18f * fConst26);
  fConst31 = (3.0038578e-19f * fConst26);
  fConst32 = (fConst22 + 1.2735043e-17f);
  fConst33 = (-4.413091e-17f - fConst24);
  fConst34 = (fConst26 * (-6.885102e-17f - fConst27));
  fConst35 = (716.5732f / fConst0);
  fConst36 = (fConst35 + 1);
  fConst37 = (0.01f / fConst36);
  fConst38 = (1 - fConst35);
  fConst39 = (1.0f / fConst36);
  fConst40 = (5.0141972e-21f * fConst6);
  fConst41 = (6.964809e-17f - fConst40);
  fConst42 = (5.0076395e-21f * fConst6);
  fConst43 = (fConst42 + -6.970466e-17f);
  fConst44 = (3.0243355e-20f * fConst6);
  fConst45 = (fConst26 * (fConst44 + -5.0063215e-16f));
  fConst46 = (4.536503e-20f * fConst26);
  fConst47 = (7.511459e-21f * fConst26);
  fConst48 = ((7.521296e-21f * fConst26) + -1.0273559e-13f);
  fConst49 = (-6.964809e-17f - fConst40);
  fConst50 = (fConst42 + 6.970466e-17f);
  fConst51 = (fConst26 * (fConst44 + 5.0063215e-16f));
  fConst52 = ((fConst6 * (fConst13 + 3.4824046e-17f)) + 5.1367795e-14f);
  fConst53 = ((fConst6 * (-3.485233e-17f - fConst15)) + -5.2316993e-14f);
  fConst54 = (fConst6 * ((fConst6 * (-2.5031608e-16f - fConst17)) + -3.7389552e-13f));

  fslider0 = 24.0f;  // (".wah.freq", 24.0f, 24.0f, 3.6e+02f, 1.0f)
  fslider1 = 0.14f;   // (".wah.Wah", 0.0f, 0.0f, 1.0f, 0.01f)
  fslider2 = 0.0f;   // (".wah.mode", 0.0f, 0.0f, 2.0f, 1.0f)
  fslider3 = 70.0f; // (".wah.wet_dry", 1e+02f, 0.0f, 1e+02f, 1.0f)

  for (int i=0; i<2; i++) iVec0[i] = 0;
  for (int i=0; i<2; i++) fRec2[i] = 0;
  for (int i=0; i<2; i++) fRec1[i] = 0;
  for (int i=0; i<2; i++) fRec0[i] = 0;
  for (int i=0; i<2; i++) fRec5[i] = 0;
  for (int i=0; i<2; i++) fRec4[i] = 0;
  for (int i=0; i<2; i++) fRec3[i] = 0;
  for (int i=0; i<2; i++) fRec6[i] = 0;
  for (int i=0; i<2; i++) fVec1[i] = 0;
  for (int i=0; i<2; i++) fRec8[i] = 0;
  for (int i=0; i<5; i++) fRec7[i] = 0;
}

inline void 
compute_voxwah (int count, float *input, float *output) {
  float 	fSlow0 = (fConst1 * (float)(fslider0));
  float 	fSlow1 = (0.007f * max(0.03f, (float)(fslider1)));
  int 	iSlow2 = (int)((float)(fslider2));
  float 	fSlow3 = (float)(fslider3);
  float 	fSlow4 = (fConst37 * fSlow3);
  float 	fSlow5 = (0.01f * fSlow3);
  float 	fSlow6 = (1 - fSlow5);
  for (int i=0; i<count; i++) {
    iVec0[0] = 1;
    fRec2[0] = ((fSlow0 * (0 - fRec0[1])) + fRec2[1]);
    fRec1[0] = ((fRec1[1] + (fSlow0 * fRec2[0])) + (1 - iVec0[1]));
    fRec0[0] = fRec1[0];
    float fTemp0 = (float)input[i];
    float fTemp1 = fabsf(fTemp0);
    fRec5[0] = max(fTemp1, ((fConst3 * fTemp1) + (fConst2 * fRec5[1])));
    fRec4[0] = ((fConst4 * fRec4[1]) + (fConst5 * fRec5[0]));
    fRec3[0] = ((0.993f * fRec3[1]) + (0.007f * (1 - max(0.03f, min(0.98f, fRec4[0])))));
    fRec6[0] = (fSlow1 + (0.993f * fRec6[1]));
    float fTemp2 = ((iSlow2==0)? fRec6[0] : ((iSlow2==1)?fRec3[0]:max(0.03f, min((float)1, (0.5f * (fRec0[0] + 1))))) );
    float fTemp3 = ((fConst6 * (fConst12 + ((fTemp2 * ((fConst6 * (fConst10 + (fConst8 * fTemp2))) + 1.9616802e-12f)) + 1.4860516e-11f))) + 5.5729553e-11f);
    fVec1[0] = (fSlow4 * fTemp0);
    fRec8[0] = ((fConst39 * ((fSlow5 * fTemp0) + (fConst38 * fRec8[1]))) - fVec1[1]);
    fRec7[0] = (fRec8[0] - (((((fRec7[1] * ((fConst6 * (fConst34 + ((fTemp2 * ((fConst26 * (fConst33 + (fConst32 * fTemp2))) + 3.9233603e-12f)) + 2.972103e-11f))) + 2.2291821e-10f)) + (fRec7[2] * ((fConst26 * (fConst31 + ((fTemp2 * (fConst30 + ((fConst29 * fTemp2) + -1.0452404e-13f))) + -1.2476738e-12f))) + 3.3437733e-10f))) + (fRec7[3] * ((fConst6 * (fConst28 + ((fTemp2 * ((fConst26 * (fConst25 + (fConst23 * fTemp2))) + -3.9233603e-12f)) + -2.972103e-11f))) + 2.2291821e-10f))) + (fRec7[4] * ((fConst6 * (fConst21 + ((fTemp2 * ((fConst6 * (fConst20 + (fConst19 * fTemp2))) + -1.9616802e-12f)) + -1.4860516e-11f))) + 5.5729553e-11f))) / fTemp3));
    output[i] = (float)((fSlow6 * fTemp0) + (fConst6 * ((((((fRec7[0] * (fConst54 + ((fTemp2 * ((fConst6 * (fConst53 + (fConst52 * fTemp2))) + -1.4132172e-12f)) + -2.0007925e-12f))) + (fRec7[1] * (fConst51 + ((fTemp2 * ((fConst26 * (fConst50 + (fConst49 * fTemp2))) + -2.8264344e-12f)) + -4.001585e-12f)))) + (fConst6 * (fRec7[2] * (((fTemp2 * (((fConst48 * fTemp2) + 1.0463399e-13f) - fConst47)) + 7.4779104e-13f) - fConst46)))) + (fRec7[3] * (fConst45 + ((fTemp2 * ((fConst26 * (fConst43 + (fConst41 * fTemp2))) + 2.8264344e-12f)) + 4.001585e-12f)))) + (fRec7[4] * (fConst18 + ((fTemp2 * ((fConst6 * (fConst16 + (fConst14 * fTemp2))) + 1.4132172e-12f)) + 2.0007925e-12f)))) / fTemp3)));
    for (int i=4; i>0; i--) fRec7[i] = fRec7[i-1];
    fRec8[1] = fRec8[0];
    fVec1[1] = fVec1[0];
    fRec6[1] = fRec6[0];
    fRec3[1] = fRec3[0];
    fRec4[1] = fRec4[0];
    fRec5[1] = fRec5[0];
    fRec0[1] = fRec0[0];
    fRec1[1] = fRec1[0];
    fRec2[1] = fRec2[0];
    iVec0[1] = iVec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t voxwah[nframes];

  compute_voxwah(nframes, input, voxwah);

  memcpy (output, voxwah, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "voxwah";

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

  init_voxwah(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
