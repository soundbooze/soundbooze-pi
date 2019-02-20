#define ALSA_PCM_NEW_HW_PARAMS_API

#include <signal.h>
#include <pthread.h>
#include <alsa/asoundlib.h>

#define FRAMES        128
#define SAMPLERATE  44100

struct Capture {
  int channel;
  int rc;
  unsigned int val;
  int dir;
  int size;
  int16_t *buffer;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_uframes_t frames;
} c;

struct Play {
  int channel;
  int rc;
  unsigned int val;
  int dir;
  int size;
  int16_t *buffer;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_uframes_t frames;
} p;

pthread_t live_thread;

int 
capture (const char *name) 
{
  c.rc = snd_pcm_open(&c.handle, name, SND_PCM_STREAM_CAPTURE, 0);
  if (c.rc < 0) {
    fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(c.rc));
    return -1;
  }

  snd_pcm_hw_params_alloca(&c.params);
  snd_pcm_hw_params_any(c.handle, c.params);
  snd_pcm_hw_params_set_access(c.handle, c.params, SND_PCM_ACCESS_MMAP_INTERLEAVED);
  snd_pcm_hw_params_set_format(c.handle, c.params, SND_PCM_FORMAT_S16_LE);

  c.channel = 1;
  snd_pcm_hw_params_set_channels(c.handle, c.params, c.channel);

  c.val = SAMPLERATE;
  snd_pcm_hw_params_set_rate_near(c.handle, c.params, &c.val, &c.dir);

  c.frames = FRAMES;
  snd_pcm_hw_params_set_period_size_near(c.handle, c.params, &c.frames, &c.dir);

  c.rc = snd_pcm_hw_params(c.handle, c.params);
  if (c.rc < 0) {
    fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(c.rc));
    return -1;
  }

  snd_pcm_hw_params_get_period_size(c.params, &c.frames, &c.dir);

  c.size = c.frames * (2 * c.channel); /* 2 bytes/sample, 1 channels */
  c.buffer = (int16_t *) malloc(c.size * sizeof(int16_t));

  snd_pcm_hw_params_get_period_time(c.params, &c.val, &c.dir);

  return 0;
}

int 
play (const char *name) 
{
  p.rc = snd_pcm_open(&p.handle, name, SND_PCM_STREAM_PLAYBACK, 0);
  if (p.rc < 0) {
    fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(p.rc));
    return -1;
  }

  snd_pcm_hw_params_alloca(&p.params);
  snd_pcm_hw_params_any(p.handle, p.params);
  snd_pcm_hw_params_set_access(p.handle, p.params, SND_PCM_ACCESS_MMAP_INTERLEAVED);
  snd_pcm_hw_params_set_format(p.handle, p.params, SND_PCM_FORMAT_S16_LE);

  p.channel = 2;
  snd_pcm_hw_params_set_channels(p.handle, p.params, p.channel);

  p.val = SAMPLERATE;
  snd_pcm_hw_params_set_rate_near(p.handle, p.params, &p.val, &p.dir);

  p.frames = FRAMES;
  snd_pcm_hw_params_set_period_size_near(p.handle, p.params, &p.frames, &p.dir);

  p.rc = snd_pcm_hw_params(p.handle, p.params);
  if (p.rc < 0) {
    fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(p.rc));
    return -1;
  }

  snd_pcm_hw_params_get_period_size(p.params, &p.frames, &p.dir);

  p.size = p.frames * (2 * p.channel); /* 2 bytes/sample, 2 channels */
  p.buffer = (int16_t *) malloc(p.size * sizeof(int16_t));

  snd_pcm_hw_params_get_period_time(p.params, &p.val, &p.dir);

  return 0;
}

void 
sighandler (int signum) {
  pthread_cancel(live_thread);

  snd_pcm_drain(c.handle);
  snd_pcm_close(c.handle);
  free(c.buffer);

  snd_pcm_drain(p.handle);
  snd_pcm_close(p.handle);
  free(p.buffer);

  /*
  snd_pcm_hw_params_free(c.params);
  snd_pcm_hw_params_free(p.params);
  */

  exit(0);
}

void *
process (void *data)
{
  while (1) {

    c.rc = snd_pcm_mmap_readi(c.handle, c.buffer, c.frames);
    if (c.rc == -EPIPE) {
      fprintf(stderr, "overrun occurred\n");
      snd_pcm_prepare(c.handle);
    } else if (c.rc < 0) {
      fprintf(stderr, "error from read: %s\n", snd_strerror(c.rc));
    } else if (c.rc != c.frames) {
      fprintf(stderr, "short read: read %d frames\n", c.rc);
    }

    register_t i = 0, j = 0;
    for(i = 0; i < c.size; i++, j+=2) {
      p.buffer[j] = c.buffer[i]; 
      p.buffer[j+1] = c.buffer[i];
    }

    p.rc = snd_pcm_mmap_writei(p.handle, p.buffer, p.frames);
    if (p.rc == -EPIPE) {
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(p.handle);
    } else if (p.rc < 0) {
      fprintf(stderr, "error from writei: %s\n", snd_strerror(p.rc));
    } else if (p.rc != p.frames) {
      fprintf(stderr, "short write: wrote %d frames\n", p.rc);
    }

  }

  return NULL;
}

int
main (int argc, char *argv[])
{
  signal(SIGINT, sighandler);

  capture("hw:1");
  play("hw:0");

  // priority - attributes  ...
  if (pthread_create(&live_thread, NULL, process, NULL)) {
    fprintf(stderr, "Error creating thread\n");
    return -1;
  }

  if (pthread_join(live_thread, NULL)) {
    fprintf(stderr, "Error joining thread\n");
    return -1;
  }

  return 0;
}
