# Snake AI — DQN in C++

A Snake game that learns to play itself using Deep Q-Network (DQN), built with **tiny-dnn** and **Raylib**.

## Dependencies

- [tiny-dnn](https://github.com/tiny-dnn/tiny-dnn) — header-only neural network library
- [Raylib](https://www.raylib.com/) — game rendering

## Network

```
Input (8)  →  Dense (256)  →  Dense (128)  →  Output (4)
```

- **Input** — snake direction (one-hot x4) + strawberry relative position (x4)
- **Output** — Q-value for each direction (Up / Down / Left / Right)


