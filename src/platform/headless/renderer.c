// No-op renderer for the headless engine. Nothing is drawn, but the image loader still
// expects atlas buffers to write pixels into, so those are honoured (and freed right away
// when the loader allows it).
#include "platform/renderer.h"

#include "graphics/renderer.h"

#include <stdlib.h>
#include <string.h>

#define MAX_TEXTURE_SIZE 4096

static struct {
    image_atlas_data atlas[ATLAS_MAX];
    int atlas_exists[ATLAS_MAX];
    struct {
        color_t *buffer;
        int width;
        int height;
    } custom[CUSTOM_IMAGE_MAX];
    graphics_renderer_interface renderer_interface;
} data;

static void clear_screen(void) {}
static void set_viewport(int x, int y, int width, int height) {}
static void reset_viewport(void) {}
static void set_clip_rectangle(int x, int y, int width, int height) {}
static void reset_clip_rectangle(void) {}
static void draw_line(int x_start, int x_end, int y_start, int y_end, color_t color) {}
static void draw_rect(int x_start, int x_end, int y_start, int y_end, color_t color) {}
static void fill_rect(int x_start, int x_end, int y_start, int y_end, color_t color) {}
static void draw_image(const image *img, int x, int y, color_t color, float scale) {}
static void draw_image_advanced(const image *img, float x, float y, color_t color,
    float scale_x, float scale_y, double angle, int disable_coord_scaling) {}
static void draw_silhouette(const image *img, int x, int y, color_t color, float scale) {}

static void create_custom_image(custom_image_type type, int width, int height, int is_yuv)
{
    free(data.custom[type].buffer);
    data.custom[type].buffer = calloc((size_t) width * height, sizeof(color_t));
    data.custom[type].width = width;
    data.custom[type].height = height;
}

static int has_custom_image(custom_image_type type)
{
    return data.custom[type].buffer != 0;
}

static color_t *get_custom_image_buffer(custom_image_type type, int *actual_texture_width)
{
    if (actual_texture_width) {
        *actual_texture_width = data.custom[type].width;
    }
    return data.custom[type].buffer;
}

static void release_custom_image_buffer(custom_image_type type) {}
static void update_custom_image(custom_image_type type) {}
static void update_custom_image_from(custom_image_type type, const color_t *buffer,
    int x_offset, int y_offset, int width, int height) {}
static void update_custom_image_yuv(custom_image_type type, const uint8_t *y_data, int y_width,
    const uint8_t *cb_data, int cb_width, const uint8_t *cr_data, int cr_width) {}
static void draw_custom_image(custom_image_type type, int x, int y, float scale, int disable_filtering) {}
static int supports_yuv_image_format(void) { return 0; }

static int start_tooltip_creation(int width, int height) { return 0; }
static void finish_tooltip_creation(void) {}
static int has_tooltip(void) { return 0; }
static void set_tooltip_position(int x, int y) {}
static void set_tooltip_opacity(int opacity) {}

static int save_image_from_screen(int image_id, int x, int y, int width, int height) { return 0; }
static void draw_image_to_screen(int image_id, int x, int y) {}
static int save_screen_buffer(color_t *pixels, int x, int y, int width, int height, int row_width) { return 0; }

static void get_max_image_size(int *width, int *height)
{
    *width = MAX_TEXTURE_SIZE;
    *height = MAX_TEXTURE_SIZE;
}

static void free_atlas_buffers(image_atlas_data *atlas)
{
    if (atlas->buffers) {
        for (int i = 0; i < atlas->num_images; i++) {
            free(atlas->buffers[i]);
        }
    }
    free(atlas->buffers);
    atlas->buffers = 0;
}

static void free_image_atlas(atlas_type type)
{
    image_atlas_data *atlas = &data.atlas[type];
    free_atlas_buffers(atlas);
    free(atlas->image_widths);
    free(atlas->image_heights);
    memset(atlas, 0, sizeof(image_atlas_data));
    data.atlas_exists[type] = 0;
}

static const image_atlas_data *prepare_image_atlas(atlas_type type, int num_images, int last_width, int last_height)
{
    free_image_atlas(type);
    image_atlas_data *atlas = &data.atlas[type];
    atlas->type = type;
    atlas->num_images = num_images;
    atlas->buffers = calloc(num_images, sizeof(color_t *));
    atlas->image_widths = calloc(num_images, sizeof(int));
    atlas->image_heights = calloc(num_images, sizeof(int));
    if (!atlas->buffers || !atlas->image_widths || !atlas->image_heights) {
        free_image_atlas(type);
        return 0;
    }
    for (int i = 0; i < num_images; i++) {
        int width = i == num_images - 1 ? last_width : MAX_TEXTURE_SIZE;
        int height = i == num_images - 1 ? last_height : MAX_TEXTURE_SIZE;
        atlas->image_widths[i] = width;
        atlas->image_heights[i] = height;
        atlas->buffers[i] = calloc((size_t) width * height, sizeof(color_t));
        if (!atlas->buffers[i]) {
            free_image_atlas(type);
            return 0;
        }
    }
    return atlas;
}

static int create_image_atlas(const image_atlas_data *atlas_data, int delete_buffers)
{
    if (!atlas_data) {
        return 0;
    }
    image_atlas_data *atlas = &data.atlas[atlas_data->type];
    if (delete_buffers) {
        free_atlas_buffers(atlas);
    }
    data.atlas_exists[atlas_data->type] = 1;
    return 1;
}

static const image_atlas_data *get_image_atlas(atlas_type type)
{
    return data.atlas_exists[type] ? &data.atlas[type] : 0;
}

static int has_image_atlas(atlas_type type)
{
    return data.atlas_exists[type];
}

static void load_unpacked_image(const image *img, const color_t *pixels) {}
static void free_unpacked_image(const image *img) {}
static int should_pack_image(int width, int height) { return 1; }
static void update_scale(int city_scale) {}

static void create_renderer_interface(void)
{
    graphics_renderer_interface *r = &data.renderer_interface;
    r->clear_screen = clear_screen;
    r->set_viewport = set_viewport;
    r->reset_viewport = reset_viewport;
    r->set_clip_rectangle = set_clip_rectangle;
    r->reset_clip_rectangle = reset_clip_rectangle;
    r->draw_line = draw_line;
    r->draw_rect = draw_rect;
    r->fill_rect = fill_rect;
    r->draw_image = draw_image;
    r->draw_image_advanced = draw_image_advanced;
    r->draw_silhouette = draw_silhouette;
    r->create_custom_image = create_custom_image;
    r->has_custom_image = has_custom_image;
    r->get_custom_image_buffer = get_custom_image_buffer;
    r->release_custom_image_buffer = release_custom_image_buffer;
    r->update_custom_image = update_custom_image;
    r->update_custom_image_from = update_custom_image_from;
    r->update_custom_image_yuv = update_custom_image_yuv;
    r->draw_custom_image = draw_custom_image;
    r->supports_yuv_image_format = supports_yuv_image_format;
    r->start_tooltip_creation = start_tooltip_creation;
    r->finish_tooltip_creation = finish_tooltip_creation;
    r->has_tooltip = has_tooltip;
    r->set_tooltip_position = set_tooltip_position;
    r->set_tooltip_opacity = set_tooltip_opacity;
    r->save_image_from_screen = save_image_from_screen;
    r->draw_image_to_screen = draw_image_to_screen;
    r->save_screen_buffer = save_screen_buffer;
    r->get_max_image_size = get_max_image_size;
    r->prepare_image_atlas = prepare_image_atlas;
    r->create_image_atlas = create_image_atlas;
    r->get_image_atlas = get_image_atlas;
    r->has_image_atlas = has_image_atlas;
    r->free_image_atlas = free_image_atlas;
    r->load_unpacked_image = load_unpacked_image;
    r->free_unpacked_image = free_unpacked_image;
    r->should_pack_image = should_pack_image;
    r->update_scale = update_scale;
    graphics_renderer_set_interface(r);
}

int platform_renderer_init(void *window)
{
    create_renderer_interface();
    return 1;
}

int platform_renderer_create_render_texture(int width, int height) { return 1; }
int platform_renderer_lost_render_texture(void) { return 0; }
void platform_renderer_invalidate_target_textures(void) {}
void platform_renderer_generate_mouse_cursor_texture(int cursor_id, int size, const color_t *pixels,
    int hotspot_x, int hotspot_y) {}
void platform_renderer_clear(void) {}
void platform_renderer_render(void) {}
void platform_renderer_pause(void) {}
void platform_renderer_resume(void) {}

void platform_renderer_destroy(void)
{
    for (int i = 0; i < ATLAS_MAX; i++) {
        free_image_atlas(i);
    }
    for (int i = 0; i < CUSTOM_IMAGE_MAX; i++) {
        free(data.custom[i].buffer);
        data.custom[i].buffer = 0;
    }
}
