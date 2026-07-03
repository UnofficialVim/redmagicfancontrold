#include "config.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

static void dump_config(const app_config_t *cfg) {
    printf("version: %d\n", cfg->version);
    printf("call_silence.enabled: %s\n", cfg->call_silence.enabled ? "true" : "false");
    printf("active_profile: %s\n", cfg->active_profile);
    printf("profile_count: %zu\n", cfg->profile_count);
    for (size_t i = 0; i < cfg->profile_count; i++) {
        const fan_profile_t *p = &cfg->profiles[i];
        printf("  profile[%zu] name=%s steps=%zu\n", i, p->name, p->step_count);
        for (size_t s = 0; s < p->step_count; s++) {
            printf("    %.1fC -> %d%%\n", p->steps[s].temp_c, p->steps[s].fan_pct);
        }
    }
}

int main(void) {
    app_config_t cfg;

    printf("=== load config.json ===\n");
    int rc = config_load("config.json", &cfg);
    assert(rc == 0);
    dump_config(&cfg);

    printf("\n=== typed accessors ===\n");
    assert(config_get_call_silence_enabled(&cfg) == true);
    assert(strcmp(config_get_active_profile(&cfg), "default") == 0);

    const fan_profile_t *gaming = config_find_profile(&cfg, "gaming");
    assert(gaming != NULL);
    printf("found 'gaming' profile with %zu steps\n", gaming->step_count);

    rc = config_set_active_profile(&cfg, "gaming");
    assert(rc == 0);
    assert(strcmp(config_get_active_profile(&cfg), "gaming") == 0);
    printf("active_profile switched to: %s\n", config_get_active_profile(&cfg));

    rc = config_set_active_profile(&cfg, "does-not-exist");
    assert(rc == -1);
    printf("set to unknown profile correctly rejected (rc=%d)\n", rc);

    config_set_call_silence_enabled(&cfg, false);
    assert(config_get_call_silence_enabled(&cfg) == false);

    printf("\n=== save round-trip ===\n");
    rc = config_save("config.out.json", &cfg);
    assert(rc == 0);
    printf("saved to config.out.json\n");

    app_config_t reloaded;
    rc = config_load("config.out.json", &reloaded);
    assert(rc == 0);
    dump_config(&reloaded);

    assert(reloaded.call_silence.enabled == false);
    assert(strcmp(reloaded.active_profile, "gaming") == 0);
    assert(reloaded.profile_count == cfg.profile_count);

    printf("\nALL CHECKS PASSED\n");
    return 0;
}
