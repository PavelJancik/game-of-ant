# Game of Life & Langton's Ant

<p align="center">
   <img src="/resources/thumbnail.png" alt="Game of Life Thumbnail" width="400"/>
</p>

This game combine Conway's Game of Life and Langton's Ant. This game differs in grid size, as it is not infinite, some patterns behave other way than in original game. Due to small resolution of the grid, the ant will not come to the state, where he can create highways. However you can use him to create initial state for Game of Life.

Tip: Load this file to game and use it as initial pattern: "28.06.2021-01.38.20-Glider_gun.txt"

## Description

### Game of Life

Game of life was created in 1970 by John Horton Conway. Rules are quite simple. There is an infinite grid. Each cell has 8 neightbours and can be in one of two possible states: dead or alive. These states changes acording following rules:
- If the cell is alive and has 2 or 3 alive neightbours, the cell will life to next generation. Otherwise it dies.
- If the cell is dead and has exactly 3 alive neightbours, the cell will revive.
The state of all cells changes at the same time. Player sets just initial state, the rest of the game play itselfs. Because of that we can call this game "zero-player game".

### Langton's Ant

Chris Langton's Ant was created 16 years later than Game of Life. The grid is same, but the cell changes states in the moment, when ant leaves it.
- If the cell is alive, the ant goes to the left cell.
- If the cell is dead, the ant goes to the right one.
After thousend of cycles, the ant starts to create highways. 

## Features

- Interactive grid for creating and editing patterns
- Support for classic Game of Life rules
- Predefined patterns (gliders, spaceships, pulsars, etc.)
- Start, pause, and reset simulation controls
- Customizable grid size and speed

## Getting Started

### Prerequisites

- Qt (recommended: Qt 5 or Qt 6)
- C++ compiler (e.g., g++, clang++)

### Building

1. Open the `Game_of_Life.pro` file in Qt Creator or run `qmake` and `make` from the command line:
   ```bash
   qmake Game_of_Life.pro
   make
   ```
2. Run the generated executable.

## Project Structure

- `main.cpp`, `mainwindow.cpp`, `mainwindow.h`, `mainwindow.ui`: Main application code and UI
- `ant.cpp`, `ant.h`: Additional features or patterns
- `resources/`: Images and pattern files

## License

See [LICENCE.md](LICENCE.md) for license information.
