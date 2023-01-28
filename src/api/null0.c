#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "../interp/interp.h"

static web49_interp_data_t null0_log(web49_interp_t interp) {
  web49_interp_data_t ret;
  printf("%s\n", (const char*)&interp.memory[interp.locals[0].i32_u]);
  return ret;
}

web49_env_func_t web49_api_import_null0(const char* name) {
  if (!strcmp(name, "null0_log")) {
    return &null0_log;
  }
  return NULL;
}
