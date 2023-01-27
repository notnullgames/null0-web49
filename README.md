# null0

This is a wasm-based game-engine, based on [web49](https://github.com/FastVM/Web49).

- The exposed API is essentually WASI + raylib, which fun & easy to use.
- you can make your games in any programming language that compiles (or can be interpreted by a runtime compiled) to wasm (most programming languages)
- carts are just zip files with assets + wasm entry-point, or a standalone wasm, if you don't need assets
- You can run your cart in retroarch



## supported languages

These are the programming languages currently supported for making your game. You can use any language, but these have headers to make it nice & ergonomic:

- C/C++
- assemblyscript


```
# configure the build for your system
cmake -B build .

# make all targets
make -C build

# make just the native runtime
make -C build null0

# make just the libretro core
make -C build libretro

# build some demo-carts
make -C build cart:log
make -C build cart:draw
```