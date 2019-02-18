/*
   gcc distortion.c -o distortion `pkg-config --libs --cflags jack` -lm -Wall -O3
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
float 	fRec0[2];
float 	fentry0;
float 	fConst0;
float 	fConst1;
float 	fentry1;
float 	fentry2;
float 	fConst2;
float 	fConst3;
float 	fConst4;
float 	fConst5;
float 	fConst6;
float 	fConst7;
float 	fRec10[2];
float 	fslider1;
float 	fslider2;
int 	IOTA;
float 	fVec1[4096];
float 	fslider3;
float 	fRec11[2];
float 	fcheckbox0;
float 	fRec9[3];
float 	fVec2[2];
float 	fConst8;
float 	fConst9;
float 	fConst10;
float 	fConst11;
float 	fConst12;
float 	fRec8[2];
float 	fRec7[2];
float 	fRec6[3];
float 	fVec3[2];
float 	fRec5[2];
float 	fRec4[3];
float 	fVec4[2];
float 	fRec3[2];
float 	fRec2[3];
float 	fslider4;
float 	fslider5;
float 	fslider6;
float 	fslider7;
float 	fRec12[2];
float 	fRec14[2];
float 	fRec13[3];
float 	fslider8;
float 	fslider9;
float 	fRec15[2];
float 	fRec18[2];
float 	fRec17[3];
float 	fRec16[3];
float 	fslider10;
float 	fslider11;
float 	fRec19[2];
float 	fRec23[2];
float 	fRec22[3];
float 	fRec21[3];
float 	fRec20[3];
float 	fslider12;
float 	fslider13;
float 	fRec24[2];
float 	fVec5[2];
float 	fConst13;
float 	fConst14;
float 	fConst15;
float 	fConst16;
float 	fRec1[2];

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
init_distortion (uint32_t samplingFreq) {
  fConst0 = min(1.92e+05f, max(1.0f, (float)samplingFreq));
  fConst1 = (3.1415927f / fConst0);
  fConst2 = tanf((47123.89f / fConst0));
  fConst3 = (2 * (1 - (1.0f / powf(fConst2,2))));
  fConst4 = (1.0f / fConst2);
  fConst5 = (((fConst4 + -1.4142135f) / fConst2) + 1);
  fConst6 = (((fConst4 + 1.4142135f) / fConst2) + 1);
  fConst7 = (1.0f / fConst6);
  fConst8 = (1.0f / tanf((97.38937f / fConst0)));
  fConst9 = (0 - fConst8);
  fConst10 = (fConst8 + 1);
  fConst11 = (1.0f / (fConst10 * fConst6));
  fConst12 = (0 - ((1 - fConst8) / fConst10));
  fConst13 = (1.0f / tanf((20517.742f / fConst0)));
  fConst14 = (fConst13 + 1);
  fConst15 = (1.0f / fConst14);
  fConst16 = (0 - ((1 - fConst13) / fConst14));

  fcheckbox0 = 0.0;

  fentry0 = 2.5e+02f;  //   ("split_low_freq", &fentry0, 2.5e+02f, 2e+01f, 6e+02f, 1e+01f);
  fentry1 = 6.5e+02f;  //   ("split_middle_freq", &fentry1, 6.5e+02f, 6e+02f, 1.25e+03f, 1e+01f);
  fentry2 = 1.25e+03f; //   ("split_high_freq", &fentry2, 1.25e+03f, 1.25e+03f, 1.2e+04f, 1e+01f);
                       //
  fslider0 = 2.0f;     //   ("gain", &fslider0, 2.0f, -1e+01f, 1e+01f, 0.1f);
  fslider1 = 1e+02f;   //   ("wet_dry", &fslider1, 1e+02f, 0.0f, 1e+02f, 1.0f);
  fslider2 = 0.12f;    //   ("trigger", &fslider2, 0.12f, 0.0f, 1.0f, 0.01f);
  fslider3 = 1.0f;     //   ("vibrato", &fslider3, 1.0f, 0.0f, 1.0f, 0.01f);
  fslider4 = 1.0f;     //   ("low_drive", &fslider4, 1.0f, 0.0f, 1.0f, 0.01f);
  fslider5 = 0.64f;    //   ("drive", &fslider5, 0.64f, 0.0f, 1.0f, 0.01f);
  fslider6 = 0.0f;     //   ("level", &fslider6, 0.0f, 0.0f, 0.5f, 0.01f);
  fslider7 = 1e+01f;   //   ("low_gain", &fslider7, 1e+01f, -1e+01f, 2e+01f, 0.1f);
  fslider8 = 1.0f;     //   ("middle_l_drive", &fslider8, 1.0f, 0.0f, 1.0f, 0.01f);
  fslider9 = 1e+01f;   //   ("middle_l_gain", &fslider9, 1e+01f, -1e+01f, 2e+01f, 0.1
  fslider10 = 1.0f;    //   ("middle_h_drive", &fslider10, 1.0f, 0.0f, 1.0f, 0.01f);
  fslider11 = 1e+01f;  //   ("middle_h_gain", &fslider11, 1e+01f, -1e+01f, 2e+01f, 0.
  fslider12 = 1.0f;    //   ("high_drive", &fslider12, 1.0f, 0.0f, 1.0f, 0.01f);
  fslider13 = 1e+01f;  //   ("high_gain", &fslider13, 1e+01f, -1e+01f, 2e+01f, 0.1f);

  for (int i=0; i<2; i++) iVec0[i] = 0;
  for (int i=0; i<2; i++) fRec0[i] = 0;
  for (int i=0; i<2; i++) fRec10[i] = 0;
  IOTA = 0;
  for (int i=0; i<4096; i++) fVec1[i] = 0;
  for (int i=0; i<2; i++) fRec11[i] = 0;
  for (int i=0; i<3; i++) fRec9[i] = 0;
  for (int i=0; i<2; i++) fVec2[i] = 0;
  for (int i=0; i<2; i++) fRec8[i] = 0;
  for (int i=0; i<2; i++) fRec7[i] = 0;
  for (int i=0; i<3; i++) fRec6[i] = 0;
  for (int i=0; i<2; i++) fVec3[i] = 0;
  for (int i=0; i<2; i++) fRec5[i] = 0;
  for (int i=0; i<3; i++) fRec4[i] = 0;
  for (int i=0; i<2; i++) fVec4[i] = 0;
  for (int i=0; i<2; i++) fRec3[i] = 0;
  for (int i=0; i<3; i++) fRec2[i] = 0;
  for (int i=0; i<2; i++) fRec12[i] = 0;
  for (int i=0; i<2; i++) fRec14[i] = 0;
  for (int i=0; i<3; i++) fRec13[i] = 0;
  for (int i=0; i<2; i++) fRec15[i] = 0;
  for (int i=0; i<2; i++) fRec18[i] = 0;
  for (int i=0; i<3; i++) fRec17[i] = 0;
  for (int i=0; i<3; i++) fRec16[i] = 0;
  for (int i=0; i<2; i++) fRec19[i] = 0;
  for (int i=0; i<2; i++) fRec23[i] = 0;
  for (int i=0; i<3; i++) fRec22[i] = 0;
  for (int i=0; i<3; i++) fRec21[i] = 0;
  for (int i=0; i<3; i++) fRec20[i] = 0;
  for (int i=0; i<2; i++) fRec24[i] = 0;
  for (int i=0; i<2; i++) fVec5[i] = 0;
  for (int i=0; i<2; i++) fRec1[i] = 0;
}

inline void 
compute_distortion (int count, float *input, float *output) {
  float 	fSlow0 = (0.001f * powf(10,(0.05f * ((float)(fslider0) + -10))));
  float 	fSlow1 = tanf((fConst1 * (float)(fentry0)));
  float 	fSlow2 = (1.0f / fSlow1);
  float 	fSlow3 = (((fSlow2 + -1.0f) / fSlow1) + 1);
  float 	fSlow4 = (1.0f / powf(fSlow1,2));
  float 	fSlow5 = (2 * (1 - fSlow4));
  float 	fSlow6 = (((fSlow2 + 1.0f) / fSlow1) + 1);
  float 	fSlow7 = (1.0f / fSlow6);
  float 	fSlow8 = tanf((fConst1 * (float)(fentry1)));
  float 	fSlow9 = (1.0f / fSlow8);
  float 	fSlow10 = (((fSlow9 + -1.0f) / fSlow8) + 1);
  float 	fSlow11 = (1.0f / powf(fSlow8,2));
  float 	fSlow12 = (2 * (1 - fSlow11));
  float 	fSlow13 = (((fSlow9 + 1.0f) / fSlow8) + 1);
  float 	fSlow14 = (1.0f / fSlow13);
  float 	fSlow15 = tanf((fConst1 * (float)(fentry2)));
  float 	fSlow16 = (1.0f / powf(fSlow15,2));
  float 	fSlow17 = (2 * (1 - fSlow16));
  float 	fSlow18 = (1.0f / fSlow15);
  float 	fSlow19 = (((fSlow18 + -1.0f) / fSlow15) + 1);
  float 	fSlow20 = (((fSlow18 + 1.0f) / fSlow15) + 1);
  float 	fSlow21 = (1.0f / fSlow20);
  float 	fSlow22 = (0.01f * (float)(fslider1));
  float 	fSlow23 = (1.0f - (float)(fslider2));
  float 	fSlow24 = (float)(fslider3);
  int 	iSlow25 = (int)(((int)(fSlow24) & 4095));
  int 	iSlow26 = (int)(((int)((fSlow24 + -1)) & 4095));
  int 	iSlow27 = (int)((float)(fcheckbox0));
  float 	fSlow28 = (fSlow18 + 1);
  float 	fSlow29 = (1.0f / fSlow28);
  float 	fSlow30 = (0 - ((1 - fSlow18) / fSlow28));
  float 	fSlow31 = (fSlow9 + 1);
  float 	fSlow32 = (1.0f / fSlow31);
  float 	fSlow33 = (0 - ((1 - fSlow9) / fSlow31));
  float 	fSlow34 = (fSlow2 + 1);
  float 	fSlow35 = (1.0f / fSlow34);
  float 	fSlow36 = (0 - ((1 - fSlow2) / fSlow34));
  float 	fSlow37 = (float)(fslider5);
  float 	fSlow38 = (powf(1e+01f,(2 * (fSlow37 * (float)(fslider4)))) / fSlow6);
  float 	fSlow39 = (float)(fslider6);
  float 	fSlow40 = (0.001f * powf(10,(0.05f * ((float)(fslider7) + -10))));
  float 	fSlow41 = (0 - fSlow2);
  float 	fSlow42 = (1.0f / (fSlow1 * fSlow13));
  float 	fSlow43 = (2 * (0 - fSlow4));
  float 	fSlow44 = (powf(1e+01f,(2 * (fSlow37 * (float)(fslider8)))) / fSlow6);
  float 	fSlow45 = (0.001f * powf(10,(0.05f * ((float)(fslider9) + -10))));
  float 	fSlow46 = (1 - ((1.0f - fSlow2) / fSlow1));
  float 	fSlow47 = (1.0f / (((fSlow2 + 1.0f) / fSlow1) + 1));
  float 	fSlow48 = (0 - fSlow9);
  float 	fSlow49 = (1.0f / (fSlow8 * fSlow20));
  float 	fSlow50 = (2 * (0 - fSlow11));
  float 	fSlow51 = powf(1e+01f,(2 * (fSlow37 * (float)(fslider10))));
  float 	fSlow52 = (0.001f * powf(10,(0.05f * ((float)(fslider11) + -10))));
  float 	fSlow53 = (1 - ((1.0f - fSlow9) / fSlow8));
  float 	fSlow54 = (1.0f / (((fSlow9 + 1.0f) / fSlow8) + 1));
  float 	fSlow55 = (0 - fSlow18);
  float 	fSlow56 = (2 * (0 - fSlow16));
  float 	fSlow57 = powf(1e+01f,(2 * (fSlow37 * (float)(fslider12))));
  float 	fSlow58 = (0.001f * powf(10,(0.05f * ((float)(fslider13) + -10))));
  float 	fSlow59 = (1 - fSlow22);
  for (int i=0; i<count; i++) {
    iVec0[0] = 1;
    fRec0[0] = (fSlow0 + (0.999f * fRec0[1]));
    fRec10[0] = ((1e-20f * (1 - iVec0[1])) - fRec10[1]);
    float fTemp0 = (float)input[i];
    float fTemp1 = (fSlow22 * fTemp0);
    float fTemp2 = (fTemp1 + (fSlow23 * fRec11[1]));
    fVec1[IOTA&4095] = fTemp2;
    fRec11[0] = (0.5f * (fVec1[(IOTA-iSlow26)&4095] + fVec1[(IOTA-iSlow25)&4095]));
    fRec9[0] = ((((iSlow27)?fRec11[0]:fTemp1) + fRec10[0]) - (fConst7 * ((fConst5 * fRec9[2]) + (fConst3 * fRec9[1]))));
    float fTemp3 = (fRec9[0] + (fRec9[2] + (2.0f * fRec9[1])));
    fVec2[0] = fTemp3;
    fRec8[0] = ((fConst12 * fRec8[1]) + (fConst11 * ((fConst8 * fVec2[0]) + (fConst9 * fVec2[1]))));
    fRec7[0] = ((fSlow30 * fRec7[1]) + (fSlow29 * (fRec8[1] + fRec8[0])));
    fRec6[0] = (fRec7[0] - (fSlow21 * ((fSlow19 * fRec6[2]) + (fSlow17 * fRec6[1]))));
    float fTemp4 = (fRec6[0] + (fRec6[2] + (2.0f * fRec6[1])));
    float fTemp5 = (fSlow21 * fTemp4);
    fVec3[0] = fTemp5;
    fRec5[0] = ((fSlow33 * fRec5[1]) + (fSlow32 * (fVec3[0] + fVec3[1])));
    fRec4[0] = (fRec5[0] - (fSlow14 * ((fSlow12 * fRec4[1]) + (fSlow10 * fRec4[2]))));
    float fTemp6 = (fRec4[0] + (fRec4[2] + (2.0f * fRec4[1])));
    float fTemp7 = (fSlow14 * fTemp6);
    fVec4[0] = fTemp7;
    fRec3[0] = ((fSlow36 * fRec3[1]) + (fSlow35 * (fVec4[0] + fVec4[1])));
    fRec2[0] = (fRec3[0] - (fSlow7 * ((fSlow5 * fRec2[1]) + (fSlow3 * fRec2[2]))));
    float fTemp8 = max((float)-1, min((float)1, (fSlow39 + (fSlow38 * (fRec2[0] + (fRec2[2] + (2.0f * fRec2[1])))))));
    fRec12[0] = (fSlow40 + (0.999f * fRec12[1]));
    fRec14[0] = ((fSlow36 * fRec14[1]) + (fSlow35 * ((fSlow42 * fTemp6) + (fSlow41 * fVec4[1]))));
    fRec13[0] = (fRec14[0] - (fSlow7 * ((fSlow5 * fRec13[1]) + (fSlow3 * fRec13[2]))));
    float fTemp9 = max((float)-1, min((float)1, (fSlow39 + (fSlow44 * ((fSlow43 * fRec13[1]) + (fSlow4 * (fRec13[2] + fRec13[0])))))));
    fRec15[0] = (fSlow45 + (0.999f * fRec15[1]));
    float fTemp10 = (fSlow5 * fRec16[1]);
    fRec18[0] = ((fSlow33 * fRec18[1]) + (fSlow32 * ((fSlow49 * fTemp4) + (fSlow48 * fVec3[1]))));
    fRec17[0] = (fRec18[0] - (fSlow14 * ((fSlow12 * fRec17[1]) + (fSlow10 * fRec17[2]))));
    fRec16[0] = ((fSlow14 * ((fSlow50 * fRec17[1]) + (fSlow11 * (fRec17[2] + fRec17[0])))) - (fSlow47 * ((fSlow46 * fRec16[2]) + fTemp10)));
    float fTemp11 = max((float)-1, min((float)1, (fSlow39 + (fSlow51 * (fRec16[2] + (fSlow47 * (fTemp10 + (fSlow46 * fRec16[0]))))))));
    fRec19[0] = (fSlow52 + (0.999f * fRec19[1]));
    float fTemp12 = (fSlow5 * fRec20[1]);
    float fTemp13 = (fSlow12 * fRec21[1]);
    fRec23[0] = ((fSlow30 * fRec23[1]) + (fSlow29 * ((fSlow18 * fRec8[0]) + (fSlow55 * fRec8[1]))));
    fRec22[0] = (fRec23[0] - (fSlow21 * ((fSlow19 * fRec22[2]) + (fSlow17 * fRec22[1]))));
    fRec21[0] = ((fSlow21 * ((fSlow56 * fRec22[1]) + (fSlow16 * (fRec22[2] + fRec22[0])))) - (fSlow54 * ((fSlow53 * fRec21[2]) + fTemp13)));
    fRec20[0] = ((fRec21[2] + (fSlow54 * (fTemp13 + (fSlow53 * fRec21[0])))) - (fSlow47 * ((fSlow46 * fRec20[2]) + fTemp12)));
    float fTemp14 = max((float)-1, min((float)1, (fSlow39 + (fSlow57 * (fRec20[2] + (fSlow47 * (fTemp12 + (fSlow46 * fRec20[0]))))))));
    fRec24[0] = (fSlow58 + (0.999f * fRec24[1]));
    float fTemp15 = (((((fRec24[0] * fTemp14) * (1 - (0.33333334f * powf(fTemp14,2)))) + ((fRec19[0] * fTemp11) * (1 - (0.33333334f * powf(fTemp11,2))))) + ((fRec15[0] * fTemp9) * (1 - (0.33333334f * powf(fTemp9,2))))) + ((fRec12[0] * fTemp8) * (1 - (0.33333334f * powf(fTemp8,2)))));
    fVec5[0] = fTemp15;
    fRec1[0] = ((fConst16 * fRec1[1]) + (fConst15 * (fVec5[0] + fVec5[1])));
    output[i] = (float)((fSlow59 * fTemp0) + (fRec1[0] * fRec0[0]));
    fRec1[1] = fRec1[0];
    fVec5[1] = fVec5[0];
    fRec24[1] = fRec24[0];
    fRec20[2] = fRec20[1]; fRec20[1] = fRec20[0];
    fRec21[2] = fRec21[1]; fRec21[1] = fRec21[0];
    fRec22[2] = fRec22[1]; fRec22[1] = fRec22[0];
    fRec23[1] = fRec23[0];
    fRec19[1] = fRec19[0];
    fRec16[2] = fRec16[1]; fRec16[1] = fRec16[0];
    fRec17[2] = fRec17[1]; fRec17[1] = fRec17[0];
    fRec18[1] = fRec18[0];
    fRec15[1] = fRec15[0];
    fRec13[2] = fRec13[1]; fRec13[1] = fRec13[0];
    fRec14[1] = fRec14[0];
    fRec12[1] = fRec12[0];
    fRec2[2] = fRec2[1]; fRec2[1] = fRec2[0];
    fRec3[1] = fRec3[0];
    fVec4[1] = fVec4[0];
    fRec4[2] = fRec4[1]; fRec4[1] = fRec4[0];
    fRec5[1] = fRec5[0];
    fVec3[1] = fVec3[0];
    fRec6[2] = fRec6[1]; fRec6[1] = fRec6[0];
    fRec7[1] = fRec7[0];
    fRec8[1] = fRec8[0];
    fVec2[1] = fVec2[0];
    fRec9[2] = fRec9[1]; fRec9[1] = fRec9[0];
    fRec11[1] = fRec11[0];
    IOTA = IOTA+1;
    fRec10[1] = fRec10[0];
    fRec0[1] = fRec0[0];
    iVec0[1] = iVec0[0];
  }
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input, *output;

  input = jack_port_get_buffer (input_port, nframes);
  output = jack_port_get_buffer (output_port, nframes);

  jack_default_audio_sample_t distortion[nframes];

  compute_distortion(nframes, input, distortion);

  memcpy (output, distortion, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "distortion";

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

  init_distortion(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
