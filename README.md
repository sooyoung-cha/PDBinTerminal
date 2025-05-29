# PDB Terminal

**PDB Terminal** is a lightweight, terminal-based protein structure visualizer designed for fast and interactive comparison of protein structures. It supports simultaneous visualization of two proteins, customizable chain selection, and even external transformation matrices (u, t) for alignment. It is built for extensibility and will support Foldseek integration in the near future.

## ✨ Features

* Render one or two proteins side-by-side in the terminal
* Compare aligned structures (via Foldseek or external u/t matrix)
* Move and rotate proteins independently or simultaneously
* Support for chain-specific views
* Helix/sheet secondary structure visualization
* Adjustable screen width and height
* Minimal dependencies, blazing fast terminal performance

## 📦 Installation

### Requirements

* C++17 compiler (e.g., g++ ≥ 7)
* CMake ≥ 3.10
* Linux or macOS (Terminal-compatible)
* Foldseek (optional, for integration)

### Build

```bash
git clone https://github.com/yourname/PDBTerminal.git
cd PDBTerminal
mkdir build && cd build
cmake ..
make -j 10
```

> Output binary will be generated at `build/src/TerminalPDB`.

## 🚀 Quick Start

### Run a single PDB file:

```bash
./TerminalPDB example1.pdb
```

### Compare two PDB files:

```bash
./TerminalPDB example1.pdb example2.pdb
```

### With options:

```bash
./TerminalPDB example1.pdb example2.pdb \
  -c A,B \                     # select chains A and B
  -m chain \                   # color mode: chain / rainbow / default
  -w 3 -h 2 \                  # terminal screen size (width x height units, 1~5)
  -u umatrix.txt \            # optional u matrix from Foldseek
  -t tmatrix.txt \            # optional t matrix from Foldseek
  -s                          # show secondary structure (helix/sheet)
```

> You can move/rotate proteins individually or together during runtime with keyboard controls.

## 🎮 Keyboard Controls

* `0` — Control both proteins
* `1` — Control only the first protein
* `2` — Control only the second protein

### Movement

* `W` / `w`: move up (+Y)
* `A` / `a`: move left (-X)
* `S` / `s`: move down (-Y)
* `D` / `d`: move right (+X)

### Rotation

* `X` / `x`: rotate around X-axis
* `Y` / `y`: rotate around Y-axis
* `Z` / `z`: rotate around Z-axis

### Zoom

* `R` / `r`: zoom in
* `F` / `f`: zoom out

### Exit

* `Q` / `q`: quit program

## 🔗 Integration with Foldseek

PDB Terminal can already accept external transformation matrices (`u`, `t`) output by **Foldseek** for protein alignment visualization.

Future releases will include:

* Direct Foldseek output parser
* Visual alignment validation
* Foldseek-based pairwise comparison interface

## 📜 License

MIT License

---

> Developed by Luna Sung-eun Jang and Soo young Cha and C++
