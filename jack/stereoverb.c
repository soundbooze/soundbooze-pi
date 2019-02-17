/*
   gcc stereoverb.c -o stereoverb `pkg-config --libs --cflags jack` -lm -Wall -O3
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

int 	iVec0[2];
float 	fslider0;
float 	fRec9[2];
float 	fslider1;
float 	fslider2;
int 	IOTA;
float 	fVec1[2048];
float 	fRec8[2];
float 	fRec11[2];
float 	fVec2[2048];
float 	fRec10[2];
float 	fRec13[2];
float 	fVec3[2048];
float 	fRec12[2];
float 	fRec15[2];
float 	fVec4[2048];
float 	fRec14[2];
float 	fRec17[2];
float 	fVec5[2048];
float 	fRec16[2];
float 	fRec19[2];
float 	fVec6[2048];
float 	fRec18[2];
float 	fRec21[2];
float 	fVec7[2048];
float 	fRec20[2];
float 	fRec23[2];
float 	fVec8[2048];
float 	fRec22[2];
float 	fVec9[1024];
float 	fRec6[2];
float 	fVec10[512];
float 	fRec4[2];
float 	fVec11[512];
float 	fRec2[2];
float 	fVec12[256];
float 	fRec0[2];
float 	fslider3;
float 	fConst0;
float 	fRec24[2];
float 	fRec25[2];
float 	fcheckbox0;
float 	fRec35[2];
float 	fVec13[2048];
float 	fRec34[2];
float 	fRec37[2];
float 	fVec14[2048];
float 	fRec36[2];
float 	fRec39[2];
float 	fVec15[2048];
float 	fRec38[2];
float 	fRec41[2];
float 	fVec16[2048];
float 	fRec40[2];
float 	fRec43[2];
float 	fVec17[2048];
float 	fRec42[2];
float 	fRec45[2];
float 	fVec18[2048];
float 	fRec44[2];
float 	fRec47[2];
float 	fVec19[2048];
float 	fRec46[2];
float 	fRec49[2];
float 	fVec20[2048];
float 	fRec48[2];
float 	fVec21[1024];
float 	fRec32[2];
float 	fVec22[512];
float 	fRec30[2];
float 	fVec23[512];
float 	fRec28[2];
float 	fVec24[256];
float 	fRec26[2];

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
init_stereoverb (uint32_t samplingFreq) {
  fConst0 = (6.2831855f / min(1.92e+05f, max(1.0f, (float)samplingFreq)));
  fslider0 = 0.70f; // 0.5f;   // ("damp", 0.5f, 0.0f, 1.0f, 0.025f)
  fslider1 = 0.77f; // 0.5f;   // ("RoomSize", 0.5f, 0.0f, 1.0f, 0.025f)
  fslider2 = 70.0f; // 5e+01f; // ("wet_dry", 5e+01f, 0.0f, 1e+02f, 1.0f)
  fslider3 = 1.46f; // 0.2f;   // ("LFO freq", 0.2f, 0.0f, 5.0f, 0.01f)
  fcheckbox0 = 0.0; // 1.0 == invert

  for (int i=0; i<2; i++) iVec0[i] = 0;
  for (int i=0; i<2; i++) fRec9[i] = 0;
  IOTA = 0;
  for (int i=0; i<2048; i++) fVec1[i] = 0;
  for (int i=0; i<2; i++) fRec8[i] = 0;
  for (int i=0; i<2; i++) fRec11[i] = 0;
  for (int i=0; i<2048; i++) fVec2[i] = 0;
  for (int i=0; i<2; i++) fRec10[i] = 0;
  for (int i=0; i<2; i++) fRec13[i] = 0;
  for (int i=0; i<2048; i++) fVec3[i] = 0;
  for (int i=0; i<2; i++) fRec12[i] = 0;
  for (int i=0; i<2; i++) fRec15[i] = 0;
  for (int i=0; i<2048; i++) fVec4[i] = 0;
  for (int i=0; i<2; i++) fRec14[i] = 0;
  for (int i=0; i<2; i++) fRec17[i] = 0;
  for (int i=0; i<2048; i++) fVec5[i] = 0;
  for (int i=0; i<2; i++) fRec16[i] = 0;
  for (int i=0; i<2; i++) fRec19[i] = 0;
  for (int i=0; i<2048; i++) fVec6[i] = 0;
  for (int i=0; i<2; i++) fRec18[i] = 0;
  for (int i=0; i<2; i++) fRec21[i] = 0;
  for (int i=0; i<2048; i++) fVec7[i] = 0;
  for (int i=0; i<2; i++) fRec20[i] = 0;
  for (int i=0; i<2; i++) fRec23[i] = 0;
  for (int i=0; i<2048; i++) fVec8[i] = 0;
  for (int i=0; i<2; i++) fRec22[i] = 0;
  for (int i=0; i<1024; i++) fVec9[i] = 0;
  for (int i=0; i<2; i++) fRec6[i] = 0;
  for (int i=0; i<512; i++) fVec10[i] = 0;
  for (int i=0; i<2; i++) fRec4[i] = 0;
  for (int i=0; i<512; i++) fVec11[i] = 0;
  for (int i=0; i<2; i++) fRec2[i] = 0;
  for (int i=0; i<256; i++) fVec12[i] = 0;
  for (int i=0; i<2; i++) fRec0[i] = 0;
  for (int i=0; i<2; i++) fRec24[i] = 0;
  for (int i=0; i<2; i++) fRec25[i] = 0;
  for (int i=0; i<2; i++) fRec35[i] = 0;
  for (int i=0; i<2048; i++) fVec13[i] = 0;
  for (int i=0; i<2; i++) fRec34[i] = 0;
  for (int i=0; i<2; i++) fRec37[i] = 0;
  for (int i=0; i<2048; i++) fVec14[i] = 0;
  for (int i=0; i<2; i++) fRec36[i] = 0;
  for (int i=0; i<2; i++) fRec39[i] = 0;
  for (int i=0; i<2048; i++) fVec15[i] = 0;
  for (int i=0; i<2; i++) fRec38[i] = 0;
  for (int i=0; i<2; i++) fRec41[i] = 0;
  for (int i=0; i<2048; i++) fVec16[i] = 0;
  for (int i=0; i<2; i++) fRec40[i] = 0;
  for (int i=0; i<2; i++) fRec43[i] = 0;
  for (int i=0; i<2048; i++) fVec17[i] = 0;
  for (int i=0; i<2; i++) fRec42[i] = 0;
  for (int i=0; i<2; i++) fRec45[i] = 0;
  for (int i=0; i<2048; i++) fVec18[i] = 0;
  for (int i=0; i<2; i++) fRec44[i] = 0;
  for (int i=0; i<2; i++) fRec47[i] = 0;
  for (int i=0; i<2048; i++) fVec19[i] = 0;
  for (int i=0; i<2; i++) fRec46[i] = 0;
  for (int i=0; i<2; i++) fRec49[i] = 0;
  for (int i=0; i<2048; i++) fVec20[i] = 0;
  for (int i=0; i<2; i++) fRec48[i] = 0;
  for (int i=0; i<1024; i++) fVec21[i] = 0;
  for (int i=0; i<2; i++) fRec32[i] = 0;
  for (int i=0; i<512; i++) fVec22[i] = 0;
  for (int i=0; i<2; i++) fRec30[i] = 0;
  for (int i=0; i<512; i++) fVec23[i] = 0;
  for (int i=0; i<2; i++) fRec28[i] = 0;
  for (int i=0; i<256; i++) fVec24[i] = 0;
  for (int i=0; i<2; i++) fRec26[i] = 0;
}

inline void 
compute_stereoverb (int count, float *input1, float *input2, float *output1, float *output2) {
  float 	fSlow0 = (float)(fslider0);
  float 	fSlow1 = (1 - fSlow0);
  float 	fSlow2 = ((0.28f * (float)(fslider1)) + 0.7f);
  float 	fSlow3 = (float)(fslider2);
  float 	fSlow4 = (0.00015f * fSlow3);
  float 	fSlow5 = (1 - (0.01f * fSlow3));
  float 	fSlow6 = (fSlow3 * ((0.01f * fSlow5) + 0.00015f));
  float 	fSlow7 = (fConst0 * (float)(fslider3));
  float 	fSlow8 = cosf(fSlow7);
  float 	fSlow9 = sinf(fSlow7);
  float 	fSlow10 = (0 - fSlow9);
  float 	fSlow11 = (float)(fcheckbox0);
  for (int i=0; i<count; i++) {
    iVec0[0] = 1;
    fRec9[0] = ((fSlow1 * fRec8[1]) + (fSlow0 * fRec9[1]));
    float fTemp0 = (float)input1[i];
    float fTemp1 = (fSlow4 * fTemp0);
    fVec1[IOTA&2047] = (fTemp1 + (fSlow2 * fRec9[0]));
    fRec8[0] = fVec1[(IOTA-1640)&2047];
    fRec11[0] = ((fSlow1 * fRec10[1]) + (fSlow0 * fRec11[1]));
    fVec2[IOTA&2047] = (fTemp1 + (fSlow2 * fRec11[0]));
    fRec10[0] = fVec2[(IOTA-1580)&2047];
    fRec13[0] = ((fSlow1 * fRec12[1]) + (fSlow0 * fRec13[1]));
    fVec3[IOTA&2047] = (fTemp1 + (fSlow2 * fRec13[0]));
    fRec12[0] = fVec3[(IOTA-1514)&2047];
    fRec15[0] = ((fSlow1 * fRec14[1]) + (fSlow0 * fRec15[1]));
    fVec4[IOTA&2047] = (fTemp1 + (fSlow2 * fRec15[0]));
    fRec14[0] = fVec4[(IOTA-1445)&2047];
    fRec17[0] = ((fSlow1 * fRec16[1]) + (fSlow0 * fRec17[1]));
    fVec5[IOTA&2047] = (fTemp1 + (fSlow2 * fRec17[0]));
    fRec16[0] = fVec5[(IOTA-1379)&2047];
    fRec19[0] = ((fSlow1 * fRec18[1]) + (fSlow0 * fRec19[1]));
    fVec6[IOTA&2047] = (fTemp1 + (fSlow2 * fRec19[0]));
    fRec18[0] = fVec6[(IOTA-1300)&2047];
    fRec21[0] = ((fSlow1 * fRec20[1]) + (fSlow0 * fRec21[1]));
    fVec7[IOTA&2047] = (fTemp1 + (fSlow2 * fRec21[0]));
    fRec20[0] = fVec7[(IOTA-1211)&2047];
    fRec23[0] = ((fSlow1 * fRec22[1]) + (fSlow0 * fRec23[1]));
    fVec8[IOTA&2047] = (fTemp1 + (fSlow2 * fRec23[0]));
    fRec22[0] = fVec8[(IOTA-1139)&2047];
    fVec9[IOTA&1023] = (fRec22[0] + (fRec20[0] + (fRec18[0] + (fRec16[0] + (fRec14[0] + (fRec12[0] + (fRec10[0] + ((0.5f * fRec6[1]) + fRec8[0]))))))));
    fRec6[0] = fVec9[(IOTA-579)&1023];
    float 	fRec7 = (fRec6[1] - (fRec22[0] + (fRec20[0] + (fRec18[0] + (fRec16[0] + (fRec14[0] + (fRec12[0] + (fRec8[0] + fRec10[0]))))))));
    fVec10[IOTA&511] = ((0.5f * fRec4[1]) + fRec7);
    fRec4[0] = fVec10[(IOTA-464)&511];
    float 	fRec5 = (fRec4[1] - fRec7);
    fVec11[IOTA&511] = ((0.5f * fRec2[1]) + fRec5);
    fRec2[0] = fVec11[(IOTA-364)&511];
    float 	fRec3 = (fRec2[1] - fRec5);
    fVec12[IOTA&255] = ((0.5f * fRec0[1]) + fRec3);
    fRec0[0] = fVec12[(IOTA-248)&255];
    float 	fRec1 = (fRec0[1] - fRec3);
    fRec24[0] = ((fSlow9 * fRec25[1]) + (fSlow8 * fRec24[1]));
    fRec25[0] = (((fSlow8 * fRec25[1]) + (fSlow10 * fRec24[1])) + (1 - iVec0[1]));
    output1[i] = (float)((fSlow5 * fTemp0) + ((1 - (fSlow11 * fRec24[0])) * ((fSlow6 * fTemp0) + fRec1)));
    fRec35[0] = ((fSlow1 * fRec34[1]) + (fSlow0 * fRec35[1]));
    float fTemp2 = (float)input2[i];
    float fTemp3 = (fSlow4 * fTemp2);
    fVec13[IOTA&2047] = (fTemp3 + (fSlow2 * fRec35[0]));
    fRec34[0] = fVec13[(IOTA-1640)&2047];
    fRec37[0] = ((fSlow1 * fRec36[1]) + (fSlow0 * fRec37[1]));
    fVec14[IOTA&2047] = (fTemp3 + (fSlow2 * fRec37[0]));
    fRec36[0] = fVec14[(IOTA-1580)&2047];
    fRec39[0] = ((fSlow1 * fRec38[1]) + (fSlow0 * fRec39[1]));
    fVec15[IOTA&2047] = (fTemp3 + (fSlow2 * fRec39[0]));
    fRec38[0] = fVec15[(IOTA-1514)&2047];
    fRec41[0] = ((fSlow1 * fRec40[1]) + (fSlow0 * fRec41[1]));
    fVec16[IOTA&2047] = (fTemp3 + (fSlow2 * fRec41[0]));
    fRec40[0] = fVec16[(IOTA-1445)&2047];
    fRec43[0] = ((fSlow1 * fRec42[1]) + (fSlow0 * fRec43[1]));
    fVec17[IOTA&2047] = (fTemp3 + (fSlow2 * fRec43[0]));
    fRec42[0] = fVec17[(IOTA-1379)&2047];
    fRec45[0] = ((fSlow1 * fRec44[1]) + (fSlow0 * fRec45[1]));
    fVec18[IOTA&2047] = (fTemp3 + (fSlow2 * fRec45[0]));
    fRec44[0] = fVec18[(IOTA-1300)&2047];
    fRec47[0] = ((fSlow1 * fRec46[1]) + (fSlow0 * fRec47[1]));
    fVec19[IOTA&2047] = (fTemp3 + (fSlow2 * fRec47[0]));
    fRec46[0] = fVec19[(IOTA-1211)&2047];
    fRec49[0] = ((fSlow1 * fRec48[1]) + (fSlow0 * fRec49[1]));
    fVec20[IOTA&2047] = (fTemp3 + (fSlow2 * fRec49[0]));
    fRec48[0] = fVec20[(IOTA-1139)&2047];
    fVec21[IOTA&1023] = (fRec48[0] + (fRec46[0] + (fRec44[0] + (fRec42[0] + (fRec40[0] + (fRec38[0] + (fRec36[0] + ((0.5f * fRec32[1]) + fRec34[0]))))))));
    fRec32[0] = fVec21[(IOTA-579)&1023];
    float 	fRec33 = (fRec32[1] - (fRec48[0] + (fRec46[0] + (fRec44[0] + (fRec42[0] + (fRec40[0] + (fRec38[0] + (fRec34[0] + fRec36[0]))))))));
    fVec22[IOTA&511] = ((0.5f * fRec30[1]) + fRec33);
    fRec30[0] = fVec22[(IOTA-464)&511];
    float 	fRec31 = (fRec30[1] - fRec33);
    fVec23[IOTA&511] = ((0.5f * fRec28[1]) + fRec31);
    fRec28[0] = fVec23[(IOTA-364)&511];
    float 	fRec29 = (fRec28[1] - fRec31);
    fVec24[IOTA&255] = ((0.5f * fRec26[1]) + fRec29);
    fRec26[0] = fVec24[(IOTA-248)&255];
    float 	fRec27 = (fRec26[1] - fRec29);
    output2[i] = (float)((fSlow5 * fTemp2) + ((1 - (fSlow11 * (0 - fRec24[0]))) * ((fSlow6 * fTemp2) + fRec27)));
    fRec26[1] = fRec26[0];
    fRec28[1] = fRec28[0];
    fRec30[1] = fRec30[0];
    fRec32[1] = fRec32[0];
    fRec48[1] = fRec48[0];
    fRec49[1] = fRec49[0];
    fRec46[1] = fRec46[0];
    fRec47[1] = fRec47[0];
    fRec44[1] = fRec44[0];
    fRec45[1] = fRec45[0];
    fRec42[1] = fRec42[0];
    fRec43[1] = fRec43[0];
    fRec40[1] = fRec40[0];
    fRec41[1] = fRec41[0];
    fRec38[1] = fRec38[0];
    fRec39[1] = fRec39[0];
    fRec36[1] = fRec36[0];
    fRec37[1] = fRec37[0];
    fRec34[1] = fRec34[0];
    fRec35[1] = fRec35[0];
    fRec25[1] = fRec25[0];
    fRec24[1] = fRec24[0];
    fRec0[1] = fRec0[0];
    fRec2[1] = fRec2[0];
    fRec4[1] = fRec4[0];
    fRec6[1] = fRec6[0];
    fRec22[1] = fRec22[0];
    fRec23[1] = fRec23[0];
    fRec20[1] = fRec20[0];
    fRec21[1] = fRec21[0];
    fRec18[1] = fRec18[0];
    fRec19[1] = fRec19[0];
    fRec16[1] = fRec16[0];
    fRec17[1] = fRec17[0];
    fRec14[1] = fRec14[0];
    fRec15[1] = fRec15[0];
    fRec12[1] = fRec12[0];
    fRec13[1] = fRec13[0];
    fRec10[1] = fRec10[0];
    fRec11[1] = fRec11[0];
    fRec8[1] = fRec8[0];
    IOTA = IOTA+1;
    fRec9[1] = fRec9[0];
    iVec0[1] = iVec0[0];
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

  jack_default_audio_sample_t sv1[nframes];
  jack_default_audio_sample_t sv2[nframes];

  compute_stereoverb(nframes, input1, input2, sv1, sv2);

  memcpy (output1, sv1, sizeof (jack_default_audio_sample_t) * nframes);
  memcpy (output2, sv2, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "stereoverb";

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

  init_stereoverb(jack_get_sample_rate(client));

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
