# notakto
> 3-board notakto in C and ncurses.

## Table of contents:
- [What is notakto?](#info)
- [Installing](#install)
- [Features](#features)

## What is notakto? <a name = "info"></a>
notakto is tic tac toe where both players use the same symbol, **X**.

The objective of the game is to avoid making three Xs in a row (vertical, horizontal or diagonal.)

Play ends when every board has **three-in-a-row**. The player that completes the **last** three-in-a-row is the **loser**.

For more about game strategy and how to play, check [The Secrets of Notakto: Winning at X-only Tic-Tac-Toe](https://arxiv.org/pdf/1301.1672.pdf)

## Installing <a name = "install"></a>
Install dependency `ncurses`, in Debian/Ubuntu:

    [sudo] apt install libncurses5-dev

To make run: 

    cd src && make

Inside src directory, run:

    ./notakto

## Features <a name = "features"></a>
- A simple, easy-to-use layout with usage instructions displayed on screen at all times. 
- A **game engine** to test your skills against. 
- Has two playing modes:
    - **Two player mode**: play against a friend (or yourself.)
    - **vs the machine**: play against playing engine.
- **Supports:**
    - Easy navigation using `h j k l` or `< v ^ >`
    - Undo / Redo for all moves.
    - Saving / Loading.
    - Restart / quit at any point.
