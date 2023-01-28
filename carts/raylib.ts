export function load():void {
    log("raylib wasm loaded")
}

export function update():void {
    ClearBackground(RAYWHITE)
    DrawText("Hello!", 10, 10, 10, GRAY)
}
