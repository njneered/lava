# the-floor-is-lava

"The Floor is Lava!!!" made by Reptile Dysfunction (Group 77) is a tile-based isometric pathfinding game built with **SFML**, featuring:
pixel art made by Group 77, DIJKSTRA'S ALGORITHM, A* ALGORITHM, real-time path comparison, walking animation, dynamic UI with coordinate input, LAVA!!!!

## HOW-TO

1. Launch the program
2. In title screen, click anywhere to continue to prompt map
3. Type in your chosen destination tile coordinates
4. Choose your pathfinding mode: 1 for DIJKSTRA'S, 2 for A*, 3 to compare both
5. Move your NaviGator using WASD in isometric space
6. Press R to reset and enter a new destination

## FEATURES
1. Isometric tile rendering (32x16 diamond tiles)
2. Character movement aligned to tile grid
3. Path visualizer with glowing color-coded lines
4. Walk animation with frame-switching
5. User-friendly UI for input, instructions, and feedback
6. Dynamic camera following NaviGator
7. Background music from Uppbeat.io

## ALGORITHMS
1. DIJKSTRA'S = shortest, uniform, blue
2. A* = fastest, heuristic-based, green
3. Compare = DIJSKTRA'S and A* side-by-side, green + blue

Each path is recalculated in real time based on NaviGator's position and destination


---

## Built With

- [SFML 2.5.x](https://www.sfml-dev.org/) – graphics, input, and audio
- C++17
- Tiled (.tmx) map format

---

## Music Credit

> **Pixel Playground**  
> by Color Parade  
> [uppbeat.io/t/color-parade/pixel-playground](https://uppbeat.io/t/color-parade/pixel-playground)  
> License code: `YXK8LCR0AWJD0S3M`

---

## Next Features (Coming Soon?)

- [ ] Lava spreading logic 🌋
- [ ] Tile cost weights (danger zones)
- [ ] Obstacles and impassable tiles
- [ ] Diagonal movement and smarter AI
- [ ] Leaderboard system

---

## Author
@andonthenguyen | @njneered | Reptile Dysfunction - Group 77

Made with fun, lava, and pathfinding magic ✨  
Feel free to fork, improve, or remix it!


