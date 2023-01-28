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

./build/log.null0:
	@echo "building cart:log"
	@npx -y --package=assemblyscript -c 'asc --use abort=fatal --lib src/carts/null0.ts --runtime stub --exportRuntime --optimize --stats --bindings esm \
		src/carts/log.ts -t build/log.wat -o build/log.wasm' && \
	mkdir -p build/log && cd build/log && cp ../log.wasm main.wasm && zip -r ../log.null0 .

./build/raylib.null0:
	@echo "building cart:raylib"
	@npx -y --package=assemblyscript -c 'asc --use abort=fatal --lib src/carts/null0.ts --runtime stub --exportRuntime --optimize --stats --bindings esm \
		src/carts/raylib.ts -t build/raylib.wat -o build/raylib.wasm' && \
	mkdir -p build/raylib && cd build/raylib && cp ../raylib.wasm main.wasm && zip -r ../raylib.null0 .

carts: ./build/log.null0 ./build/raylib.null0 ## Build some demo carts

clean: ## Delete built files
	@rm -rf build