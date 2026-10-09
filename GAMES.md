# Yazan Game OS - The 5 Built-in 2D Games Guide

This document details the mechanics, controls, scoring, and architecture of all 5 ready-to-play games in Yazan Game OS.

---

### Game 1: Yazan's Quest (مغامرات يزن)
- **Genre**: 2D Top-Down Dungeon Adventure / RPG
- **Theme**: A heroic dungeon crawler where Yazan the Warrior ventures into ancient catacombs.
- **Mechanics**:
  - Grid map with walls, chests, keys, potions, and doors.
  - Skeletons patrol corridors. Getting touched costs 1 Heart.
  - Space or 'Z': Swing sword to strike adjacent enemies.
  - Keys open locked doors to access the golden exit portal.
- **Controls**:
  - `Arrows` / `W, A, S, D`: Move character.
  - `Space` / `Z`: Swing sword attack.
  - `R`: Restart level.

---

### Game 2: Desert Speed (سباق الصحراء)
- **Genre**: 2D Top-Down High-Speed Racing
- **Theme**: High-speed highway racing through a desert highway.
- **Mechanics**:
  - Dynamic scrolling asphalt road.
  - 5 traffic cars traveling at differing velocities.
  - Fuel consumption: pickup fuel canisters before the tank runs dry.
  - Nitro acceleration boosts speed up to 250 km/h.
  - Collision with cars triggers a crash event with sound effects.
- **Controls**:
  - `Left / Right` or `A / D`: Steer car.
  - `Up` or `W` or `Space`: Engage Nitro Boost.
  - `Down` or `S`: Brake.
  - `R`: Retry race.

---

### Game 3: Cyber Jumper (قفزة السايبر)
- **Genre**: 2D Physics Platformer
- **Theme**: Cyberpunk ninja leaping between floating platforms.
- **Mechanics**:
  - Real vertical gravity and horizontal momentum.
  - Double jump mechanic: Jump once from ground, jump again in mid-air!
  - 5 spinning golden coins to collect.
  - Red spikes hazard on floor: loses 1 Life.
  - Reach the purple portal at the top right to complete the stage.
- **Controls**:
  - `A / D` or `Left / Right`: Run left/right.
  - `W` / `Up` / `Space`: Jump / Double Jump.
  - `R`: Restart stage.

---

### Game 4: Galactic Defender (حامي المجرة)
- **Genre**: 2D Vertical Arcade Space Shooter
- **Theme**: Defending the home star from waves of hostile invaders.
- **Mechanics**:
  - Starfighter equipped with dual plasma laser cannons.
  - Waves of alien cruisers descend and oscillate.
  - Explosions create fiery expanding particle sparks.
  - Ship has 100% Shield health.
- **Controls**:
  - `Arrows` or `W, A, S, D`: Fly starship.
  - `Space` / `Z` / `Mouse Click`: Fire plasma lasers.
  - `R`: Restart game.

---

### Game 5: Mind Blocks (مكعبات الذكاء)
- **Genre**: 2D Number Sliding Puzzle (2048)
- **Theme**: Clean modern number fusion puzzle.
- **Mechanics**:
  - 4x4 grid of tiles.
  - Sliding tiles pushes them to the borders.
  - Adjacent tiles with matching values merge into double their value (2 -> 4 -> 8 -> 16 ... -> 2048).
  - Target: Reach 2048!
- **Controls**:
  - `Arrows` or `W, A, S, D`: Slide tiles in direction.
  - On-screen D-Pad buttons: Click with mouse to slide.
  - `R`: New game.
