// Audio stubs: nothing is ever played headlessly.
#include "sound/device.h"

void sound_device_open(void) {}
void sound_device_close(void) {}
void sound_device_init_channels(void) {}
int sound_device_is_file_playing_on_channel(const char *filename, sound_type type) { return 0; }
void sound_device_set_music_volume(int volume_pct) {}
void sound_device_set_volume_for_type(sound_type type, int volume_pct) {}
int sound_device_play_music(const char *filename, int volume_pct, int loop) { return 0; }
int sound_device_play_track(const char *filename, int volume_pct, void (*on_finish)(void)) { return 0; }
int sound_device_play_file_on_channel_panned(const char *filename, sound_type type,
    int volume_pct, int left_pct, int right_pct, int loop) { return 0; }
int sound_device_play_file_on_channel(const char *filename, sound_type type, int volume_pct) { return 0; }
int sound_device_pause_music(void) { return 0; }
int sound_device_resume_music(void) { return 0; }
void sound_device_stop_music(void) {}
void sound_device_stop_type(sound_type type) {}
void sound_device_on_audio_finished(void (*callback)(sound_type)) {}
void sound_device_fadeout_music(int milisseconds) {}
void sound_device_use_custom_music_player(int bitdepth, int num_channels, int rate, const void *audio_data, int len) {}
void sound_device_write_custom_music_data(const void *audio_data, int len) {}
void sound_device_use_default_music_player(void) {}
