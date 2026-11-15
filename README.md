<h1 align="center">
  CATONAUTICS
</h1>

<div align="center">
  <strong>Lunar Base Educational Mini-Games</strong><br>
  🎮🚀🌱
</div>

<div align="center"> 
  <a href="https://isocpp.org/">
    <img src="https://img.shields.io/badge/C%2B%2B-00599C?logo=c%2B%2B&logoColor=fff">
  </a>
  <a>
    <img src="https://img.shields.io/badge/Raylib-F5F5F5?logo=raylib&logoColor=000">
  </a>
  </br>This project is a collection of 4 mini-games developed in C++ using the Raylib library. It is designed for children aged 7–12 to introduce them to operations on a lunar base.
</div>

## Table of contents

* [Table of contents](#table-of-contents)
* [Tech stack](#tech-stack)
* [Prerequisites](#prerequisites)
* [Project Mini-Games](#project-mini-games)
* [Cloning the project](#cloning-the-project)
* [Installation and setup](#installation-and-setup)
* [Project structure](#project-structure)

## Tech stack

* C++
* Raylib
* CMake / Make (optional)
* **Target Resolution:** 2550 x 1600 (Optimal for Version 1)

## Prerequisites

Before you begin, make sure you have installed:
* **Git** — [download](https://git-scm.com/)
* **C++ Compiler** (GCC, Clang, or MSVC supporting C++17)
* **Raylib** — [installation guide](https://www.raylib.com/)

## Project Mini-Games

1. **Energy Balance**
   * *How to play:* The player manages the power supply for the lunar base. There are 4 systems on the screen, each requiring a specific amount of energy (oxygen, heating, greenhouse, and lighting). The player must distribute energy among these four systems using sliders. The player wins if they manage to keep the base running for 5 cycles.
   * *Goal:* To teach the player how to allocate resources.

2. **Restoration of Life Support**
   * *How to play:* The player manages water circulation on the base. By clicking on pipes, the player rotates them 90 degrees; the objective is to connect the water source, the filter, and the clean water reservoir.
   * *Goal:* To demonstrate how a water circulation system works.

3. **Radiation Alarm**
   * *How to play:* A radiation storm begins, and the player must distribute limited energy among various modules—pressing the "switch" button at the right time to toggle between modules.
   * *Goal:* The player learns that space radiation can be dangerous to humans and that protecting the base requires special materials, shelters, and energy.

4. **Greenhouse on the Moon**
   * *How to play:* The screen displays plant beds, each requiring water, light, and time. The player can water each bed and toggle the lighting on or off for individual beds across several cycles.
   * *Goal:* To explain and show how astronauts grow food.

## Cloning the project

* Run the following command in your console:
```bash
git clone https://github.com/c4nerRr/NASA_pe67/
```
* Then, navigate into the project folder:
```bash
cd catonautics
```

## Installation and setup

1. **Compile and run (Example for Linux / GCC)**
```bash
g++ main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o catonautics
./catonautics
```

## Project structure

* `main.cpp`: Main game entry point containing menu logic and mini-game switching.
* `assets/`: Graphical and sound resources for the game.
