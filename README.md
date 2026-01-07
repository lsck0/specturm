<div>
    <h1 align="center">
        specturm
    </h1>
    <h3 align="center">
        SDL3 w/ GPU API 2D Engine + Game
    </h3>
</div>

```bash
git clone --recursive http://github.com/lsck0/specturm.git
cd specturm
clang build.c -o build -std=c23 -fenable-matrix -mavx -mavx2 -Wno-gcc-compat -Wno-initializer-overrides -I./src -lm -lSDL3
./build
```
