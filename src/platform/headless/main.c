// augustus-headless-native: run the Caesar III simulation without a window.
//
//   augustus-headless-native --c3 DIR (--scenario NAME.map | --load FILE.svx)
//       [--assets-base DIR] [--pref DIR] [--ticks N] [--years Y] [--report N]
//       [--save OUT.svx] [--hash] [--quiet]
//
// A game month is 16 days of 50 ticks (800 ticks); a year is 9600 ticks.
#include "platform/headless/headless.h"

#include "city/finance.h"
#include "city/population.h"
#include "core/image.h"
#include "core/log.h"
#include "core/time.h"
#include "game/file.h"
#include "game/file_io.h"
#include "game/game.h"
#include "game/system.h"
#include "game/tick.h"
#include "game/time.h"
#include "platform/file_manager.h"
#include "platform/log.h"
#include "platform/screen.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MILLIS_PER_TICK 16
#define TICKS_PER_YEAR (12 * 16 * 50)

static void usage(void)
{
    fputs("usage: augustus-headless-native --c3 DIR (--scenario NAME.map | --load FILE.svx)\n"
        "         [--assets-base DIR] [--pref DIR] [--ticks N] [--years Y] [--report N]\n"
        "         [--save OUT.svx] [--snapshot OUT.svx] [--memory-roundtrip]\n"
        "         [--hash] [--hash-pieces] [--full-images] [--quiet]\n", stderr);
}

static void silent_log(const char *message, int is_error)
{
    if (is_error) {
        fputs(message, stderr);
        fputc('\n', stderr);
    }
}

static void print_status(long ticks_done)
{
    printf("ticks %ld  date %d-%02d-%02d.%02d  population %d  treasury %d\n",
        ticks_done, game_time_year(), game_time_month() + 1, game_time_day() + 1, game_time_tick(),
        city_population(), city_finance_treasury());
    fflush(stdout);
}

static const char *absolute_path(const char *path, char *storage)
{
    if (realpath(path, storage)) {
        size_t len = strlen(storage);
        if (len && storage[len - 1] != '/' && len + 1 < PATH_MAX) {
            storage[len] = '/';
            storage[len + 1] = 0;
        }
        return storage;
    }
    return path;
}

int main(int argc, char **argv)
{
    const char *c3_dir = 0, *assets_base = 0, *pref_dir = 0, *scenario = 0, *load = 0, *save = 0;
    long ticks = 0, report_every = 0;
    int want_hash = 0, want_pieces = 0, quiet = 0, memory_roundtrip = 0, full_images = 0;
    const char *snapshot = 0, *dump_dir = 0;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        const char *value = i + 1 < argc ? argv[i + 1] : 0;
        if (!strcmp(arg, "--hash")) {
            want_hash = 1;
        } else if (!strcmp(arg, "--quiet")) {
            quiet = 1;
        } else if (!strcmp(arg, "--hash-pieces")) {
            want_pieces = 1;
        } else if (!strcmp(arg, "--memory-roundtrip")) {
            memory_roundtrip = 1;
        } else if (!strcmp(arg, "--full-images")) {
            full_images = 1;
        } else if (!value) {
            usage();
            return 2;
        } else if (!strcmp(arg, "--c3")) {
            c3_dir = value; i++;
        } else if (!strcmp(arg, "--assets-base")) {
            assets_base = value; i++;
        } else if (!strcmp(arg, "--pref")) {
            pref_dir = value; i++;
        } else if (!strcmp(arg, "--scenario")) {
            scenario = value; i++;
        } else if (!strcmp(arg, "--load")) {
            load = value; i++;
        } else if (!strcmp(arg, "--save")) {
            save = value; i++;
        } else if (!strcmp(arg, "--snapshot")) {
            snapshot = value; i++;
        } else if (!strcmp(arg, "--dump-pieces")) {
            dump_dir = value; i++;
        } else if (!strcmp(arg, "--ticks")) {
            ticks += strtol(value, 0, 10); i++;
        } else if (!strcmp(arg, "--years")) {
            ticks += strtol(value, 0, 10) * TICKS_PER_YEAR; i++;
        } else if (!strcmp(arg, "--report")) {
            report_every = strtol(value, 0, 10); i++;
        } else {
            usage();
            return 2;
        }
    }
    if (!c3_dir || (!scenario && !load) || (scenario && load)) {
        usage();
        return 2;
    }

    static char pref_storage[PATH_MAX], assets_storage[PATH_MAX], save_storage[PATH_MAX], load_storage[PATH_MAX];
    if (quiet) {
        platform_log_set_output_function(silent_log);
    }
    if (pref_dir) {
        headless_set_pref_path(absolute_path(pref_dir, pref_storage));
    }
    if (assets_base) {
        headless_set_assets_base_path(absolute_path(assets_base, assets_storage));
    }
    if (load) {
        load = absolute_path(load, load_storage);
        size_t len = strlen(load_storage);
        if (len && load_storage[len - 1] == '/') {
            load_storage[len - 1] = 0; // realpath helper appends a slash; files must not have one
        }
    }
    if (save && save[0] != '/') {
        // Saves are written relative to the C3 directory after chdir, so anchor them to the caller's cwd.
        char cwd[PATH_MAX];
        if (getcwd(cwd, PATH_MAX)) {
            snprintf(save_storage, PATH_MAX, "%s/%s", cwd, save);
            save = save_storage;
        }
    }

    platform_log_setup();
    log_info("Augustus headless", system_version(), 0);

    if (!platform_file_manager_set_base_path(c3_dir)) {
        fprintf(stderr, "error: cannot use Caesar III directory %s\n", c3_dir);
        return 1;
    }
    if (!game_pre_init()) {
        fprintf(stderr, "error: game_pre_init failed (is %s a Caesar III directory?)\n", c3_dir);
        return 1;
    }
    if (!platform_screen_create("headless", 100, 0)) {
        fprintf(stderr, "error: platform_screen_create failed\n");
        return 1;
    }
    headless_set_ticks(0);
    time_set_millis(0);
    image_set_index_only(!full_images);
    if (!game_init()) {
        fprintf(stderr, "error: game_init failed\n");
        return 1;
    }
    game_file_set_disk_saves_enabled(0);

    int loaded = scenario
        ? game_file_start_scenario_by_name((const uint8_t *) scenario)
        : game_file_load_saved_game(load) == 1;
    if (!loaded) {
        fprintf(stderr, "error: failed to load %s\n", scenario ? scenario : load);
        return 1;
    }
    print_status(0);

    uint64_t millis = 0;
    long done = 0;
    for (; done < ticks; done++) {
        millis += MILLIS_PER_TICK;
        headless_set_ticks(millis);
        time_set_millis((time_millis) millis);
        game_tick_run();
        if (report_every && (done + 1) % report_every == 0) {
            print_status(done + 1);
        }
        if (headless_exit_requested()) {
            break;
        }
    }
    if (!report_every || done % report_every) {
        print_status(done);
    }
    if (memory_roundtrip) {
        // write(A) -> read -> hash, without touching the disk; uncompressed for speed.
        uint8_t *bytes = 0;
        int size = 0;
        if (!game_file_io_write_saved_game_to_memory(&bytes, &size, 0)) {
            fprintf(stderr, "error: snapshot to memory failed\n");
            return 1;
        }
        int result = game_file_load_saved_game_from_memory(bytes, size);
        free(bytes);
        if (result != 1) {
            fprintf(stderr, "error: load from memory failed (%d)\n", result);
            return 1;
        }
        printf("memory roundtrip %d bytes, hash after reload %08x\n", size, game_file_io_state_hash());
    }
    if (save) {
        if (!game_file_io_write_saved_game(save)) {
            fprintf(stderr, "error: failed to save %s\n", save);
            return 1;
        }
        printf("saved %s\n", save);
    }
    if (want_hash) {
        printf("hash %08x\n", game_file_io_state_hash());
    }
    if (want_pieces) {
        game_file_io_state_hash_dump();
    }
    if (dump_dir) {
        game_file_io_dump_pieces(dump_dir);
    }
    if (snapshot) {
        // Same bytes as --save, produced through the in-memory path.
        uint8_t *bytes = 0;
        int size = 0;
        if (!game_file_io_write_saved_game_to_memory(&bytes, &size, 1)) {
            fprintf(stderr, "error: snapshot to memory failed\n");
            return 1;
        }
        FILE *fp = fopen(snapshot, "wb");
        if (!fp || fwrite(bytes, 1, size, fp) != (size_t) size) {
            fprintf(stderr, "error: cannot write %s\n", snapshot);
            return 1;
        }
        fclose(fp);
        free(bytes);
        printf("snapshot %d bytes -> %s\n", size, snapshot);
    }
    return 0;
}
