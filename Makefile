.PHONY: help clean config

help:
	@grep -E '^[a-zA-Z_0-9]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

./build:
	@cmake -B build .

config: ./build ## Configure host-build (cmake)

./build/null0: ./build
	@make -C build

./build/rimage_libretro.dylib:
	@make -C build libretro

null0: ./build/null0 ## Build standalone host

libretro: ./build/rimage_libretro.dylib ## Build retroarch core

./build/log.null0: carts/log.ts
	@echo "building cart:log"
	./carts/build_assemblyscript_cart carts/log.ts

./build/raylib.null0: carts/raylib.ts
	@echo "building cart:raylib"
	./carts/build_assemblyscript_cart carts/raylib.ts

./build/raylib2.null0: carts/raylib2.c
	@echo "building cart:raylib"
	./carts/build_c_cart carts/raylib2.c

carts: ./build/log.null0 ./build/raylib.null0 ./build/raylib2.null0 ## Build some demo carts

clean: ## Delete built files
	@rm -rf build