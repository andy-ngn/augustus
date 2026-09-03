// augustus-headless-native: run the Caesar III simulation without a window.
//
//   augustus-headless-native --c3 DIR (--scenario NAME.map | --load FILE.svx)
//       [--assets-base DIR] [--pref DIR] [--ticks N] [--years Y] [--report N]
//       [--save OUT.svx] [--hash] [--quiet]
//
// A game month is 16 days of 50 ticks (800 ticks); a year is 9600 ticks.
#include "api/aug_api.h"
#include "platform/headless/headless.h"

#include "game/file_io.h"
#include "platform/log.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TICKS_PER_YEAR AUG_TICKS_PER_YEAR

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
        ticks_done, aug_time_year(), aug_time_month() + 1, aug_time_day() + 1, aug_time_tick(),
        aug_population(), aug_treasury());
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
        absolute_path(pref_dir, pref_storage);
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

    if (!aug_init(c3_dir, pref_dir ? pref_storage : 0, !full_images)) {
        fprintf(stderr, "error: engine initialisation failed for %s\n", c3_dir);
        return 1;
    }
    int loaded = scenario ? aug_load_scenario(scenario) : aug_load_save(load);
    if (!loaded) {
        fprintf(stderr, "error: failed to load %s\n", scenario ? scenario : load);
        return 1;
    }
    print_status(0);

    long done = 0;
    while (done < ticks && !headless_exit_requested()) {
        long chunk = report_every ? report_every - (done % report_every) : ticks - done;
        if (chunk > ticks - done) {
            chunk = ticks - done;
        }
        aug_tick((int) chunk);
        done += chunk;
        if (report_every && done % report_every == 0) {
            print_status(done);
        }
    }
    if (!report_every || done % report_every) {
        print_status(done);
    }
    if (memory_roundtrip) {
        // write(A) -> read -> hash, without touching the disk; uncompressed for speed.
        int size = 0;
        uint8_t *bytes = aug_state_write(&size, 0);
        if (!bytes) {
            fprintf(stderr, "error: snapshot to memory failed\n");
            return 1;
        }
        int ticks_before = aug_ticks_total();
        int ok = aug_state_read(bytes, size);
        aug_free(bytes);
        if (!ok) {
            fprintf(stderr, "error: load from memory failed\n");
            return 1;
        }
        printf("memory roundtrip %d bytes, ticks %d -> %d, hash after reload %08x\n", size, ticks_before,
            aug_ticks_total(), aug_state_hash());
    }
    if (want_hash) {
        printf("hash %08x\n", aug_state_hash());
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
