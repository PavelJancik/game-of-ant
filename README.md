# Game of Life

<p align="center">
   <img src="/resources/thumbnail.png" alt="Game of Life Thumbnail" width="400"/>
</p>

This game combine Conway's Game of Life and Langton's Ant. This game differs in grid size, as it is not infinite, some patterns behave other way than in original game. Due to small resolution of the grid, the ant will not come to the state, where he can create highways. However you can use him to create initial state for Game of Life.

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
