// Input stubs: the headless engine never receives keyboard, mouse, touch or joystick events.
#include "game/system.h"
#include "input/cursor.h"
#include "input/joystick.h"
#include "input/keys.h"
#include "platform/cursor.h"
#include "platform/joystick.h"
#include "platform/keyboard_input.h"
#include "platform/touch.h"

// cursor
void system_init_cursors(int scale_percentage) {}
void system_set_cursor(int cursor_id) {}
void system_show_cursor(void) {}
void system_hide_cursor(void) {}
int platform_cursor_is_disabled(void) { return 1; }
void platform_cursor_force_software_mode(void) {}
int platform_cursor_is_forced_software_cursor(void) { return 0; }
int platform_cursor_has_hardware_cursor(void) { return 0; }
cursor_shape platform_cursor_get_current_shape(void) { return CURSOR_ARROW; }
cursor_scale platform_cursor_get_current_scale(void) { return CURSOR_SCALE_1; }
int platform_cursor_get_texture_size(const cursor *c) { return 32; }

// mouse
void system_mouse_get_relative_state(int *x, int *y) { *x = 0; *y = 0; }
void system_mouse_set_relative_mode(int enabled) {}
void system_move_mouse_cursor(int delta_x, int delta_y) {}
void system_set_mouse_position(int *x, int *y) {}

// keyboard
void platform_handle_key_down(void *event) {}
void platform_handle_key_up(void *event) {}
void platform_handle_editing_text(void *event) {}
void platform_handle_text(void *event) {}
key_type system_keyboard_key_for_symbol(const char *name) { return KEY_TYPE_NONE; }
const char *system_keyboard_key_name(key_type key) { return ""; }
const char *system_keyboard_key_modifier_name(key_modifier_type modifier) { return ""; }
void system_keyboard_set_input_rect(int x, int y, int width, int height) {}
void system_keyboard_show(void) {}
void system_keyboard_hide(void) {}
void system_start_text_input(void) {}
void system_stop_text_input(void) {}

// joystick
void platform_joystick_init(int force_enable) {}
int platform_joystick_is_enabled(void) { return 0; }
void platform_joystick_device_changed(long long id, int is_connected) {}
joystick_hat_position platform_joystick_convert_hat_position(int value) { return JOYSTICK_HAT_CENTERED; }

// touch
void platform_touch_start(void *event) {}
void platform_touch_move(void *event) {}
void platform_touch_end(void *event) {}
