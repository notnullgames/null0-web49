// TODO: check these are all used
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "physfs.h"

#include "../api/api.h"
#include "../ast.h"
#include "../interp/interp.h"
#include "../opt/tee.h"
#include "../opt/tree.h"
#include "../read_bin.h"

static web49_interp_data_t web49_main_import_silly(web49_interp_t interp) {
  return (web49_interp_data_t){0};
}

web49_env_func_t web49_main_import_func(void* state, const char* mod, const char* func) {
  if (!strcmp(mod, "wasi_snapshot_preview1")) {
    return web49_api_import_wasi(func);
  } else if (!strcmp(mod, "env")) {
    if (!strcmp(func, "emscripten_asm_const_int")) {
      return &web49_main_import_silly;
    }
    web49_env_func_t ret = web49_api_import_raylib(func);
    if (ret == NULL) {
      __builtin_trap();
    }
    return ret;
  } else {
    return NULL;
  }
}

// can take a dir, zip, or wasm binary file as input and setup rootfs, load the main.wasm for it
int web49_file_main(const char* inarg, const char** args) {
  PHYSFS_init("null0");
  bool isZip = false;
  bool isWasm = false;
  bool isDir = false;

  DIR* dirptr;
  if (access(inarg, F_OK) != -1) {
    if ((dirptr = opendir(inarg)) != NULL) {
      isDir = true;
    } else {
      FILE* fptr1 = fopen(inarg, "r");
      char str[4];
      if (fptr1 == NULL) {
        fprintf(stderr, "Could not open file: %s\n", inarg);
        return 1;
      }
      fread(str, 4, 1, fptr1);
      fclose(fptr1);
      isZip = memcmp(str, "PK\3\4", 4) == 0;
      isWasm = memcmp(str, "\0asm", 4) == 0;
    }
  } else {
    fprintf(stderr, "Could not open file: %s\n", inarg);
    return 1;
  }

  if (!isDir && !isZip && !isWasm) {
    fprintf(stderr, "Unknown filetype: %s\n", inarg);
    return 1;
  }

  PHYSFS_init("null0");
  web49_io_input_t infile;

  if (isZip || isDir) {
    PHYSFS_mount(inarg, NULL, 0);
    PHYSFS_File* wasmFile = PHYSFS_openRead("main.wasm");
    PHYSFS_uint64 wasmLen = PHYSFS_fileLength(wasmFile);
    unsigned char* wasmBuffer[wasmLen];
    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(wasmFile, wasmBuffer, wasmLen);
    if (bytesRead == -1) {
      fprintf(stderr, "Could not open file: %s\n", inarg);
      return 1;
    }

    infile = (web49_io_input_t){
        .byte_index = 0,
        .byte_len = wasmLen,
        .byte_buf = (unsigned char*)wasmBuffer,
    };
  } else {
    infile = web49_io_input_open(inarg);
  }

  web49_module_t mod = web49_readbin_module(&infile);

  uint32_t cart_load = 0;
  uint32_t cart_update = 0;
  web49_section_export_t exports = web49_module_get_section(mod, WEB49_SECTION_ID_EXPORT).export_section;
  for (size_t j = 0; j < exports.num_entries; j++) {
    web49_section_export_entry_t entry = exports.entries[j];
    if (!strcmp(entry.field_str, "load")) {
      cart_load = entry.index;
    }
    if (!strcmp(entry.field_str, "update")) {
      cart_update = entry.index;
    }
  }

  web49_opt_tee_module(&mod);
  web49_opt_tree_module(&mod);

  web49_interp_t interp = web49_interp_module(mod, args);
  web49_free_module(mod);
  interp.import_func = web49_main_import_func;
  interp.import_state = NULL;
  if (cart_load) {
    web49_interp_block_run(&interp, &interp.funcs[cart_load]);
  }
  web49_free_interp(interp);

  return 0;
}

int main(int argc, const char** argv) {
  const char* inarg = NULL;
  const char** args = argv + 1;
  for (int i = 1; i <= argc; i += 1) {
    if (inarg == NULL) {
      inarg = argv[i];
      args = &argv[i];
    } else {
      break;
    }
  }
  return web49_file_main(inarg, args);
}