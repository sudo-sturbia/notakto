# notakto
3-board notakto in C and ncurses

## What is notakto?
notakto is tic tac toe where both players use the same symbol, **X**.

## How to install?
first install *libncurses*.
in Debian/Ubuntu:

    [sudo] apt install libncurses5-dev

then, inside src directory run:

    make

## Features:
- An simple layout with general usage **instructions** displayed at the screen. 
- A **game engine** to test yourself against. 
- Has two playing modes:
    - **Two player mode**: play against a friend (or yourself.)
    - **vs the machine**: play against playing engine.
- **Supports:**
    - Undo / Redo for all moves.
    - Saving / Loading.
    - Restart / quit at any point.
