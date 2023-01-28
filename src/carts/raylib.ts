export function load():void {
  InitWindow(320, 240, "Welcome to WASM")

  while (!WindowShouldClose()) {
    BeginDrawing()
    ClearBackground(RAYWHITE)
    DrawText("this IS a texture!", 10, 10, 10, GRAY)
    EndDrawing();
  }

  CloseWindow()
}