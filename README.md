<a id="readme-top"></a>

## About The Project
While SNK never officially released any BIOS source for the Neo Geo platform, this project aims to provide a clean, open-source re-implementation of the Neo Geo BIOS to preserve legacy functionality and enable ongoing development. Although earlier community ports and patches delivered tremendous value, many stalled or never reached full maturity. Hopefully, this project can revamp interest and continue the journey.

Project goals:
* Open source release: Make the full BIOS code freely available under a permissive license.
* High-level implementation: Rewrite in C (instead of raw assembly) for readability, maintainability, and ease of contribution.
* Improved functionality: featuring new menus, cheat codes, and more.

Core technologies:
* Language: C
* Build system: CMake
* Compiler: GCC

By choosing these tools and languages, the project can ensure the BIOS codebase remains accessible to as many developers as possible.

Contributions and feedback are very welcome!

## Getting Started
_While development and testing are primarily conducted on Ubuntu with Visual Studio Code and MAME, the codebase is designed for maximum cross-platform compatibility_
### Prerequisites
```sh
apt-get install cmake gcc-m68k-linux-gnu
```

### Installation
1. Clone the repo
   ```sh
   git clone https://github.com/neogeo-projects/neogeo-bios.git
   ```
2. Build the ROM-file
   ```sh
   mkdir build
   cd build
   cmake -S . -B build -DSYSTEM=MVS -DREGION=EUROPE
   cmake --build build --parallel
   ```
3. Burn the ROM to a MC27C1024

### Develope

1. Clone the repo
   ```sh
   git clone https://github.com/neogeo-projects/neogeo-bios.git
   ```
2. Copy following files into the `assets` folder (you need to provide these yourself)
    ```sh
    000-lo.lo
    sfix.sfix
    sm1.sm1
    ```
3. Build the ROM-file
   ```sh
   mkdir build
   cd build
   cmake -S . -B build -DSYSTEM=MVS -DREGION=EUROPE -DCMAKE_BUILD_TYPE=Debug
   cmake --build build --parallel
   ```
4. Test the rom with MAME
    ```sh
    mame neogeo -rompath build/ -skip_gameinfo -verbose -debug -r 800x600 -cart1 [gamename] -cart2 [gamename]
    ```
#### File hierarchy
```sh
build/
└── neogeo/
    ├── sp-s2.sp1
    ├── 000-lo.lo
    ├── sfix.sfix
    ├── sm1.sm1
    ├── mslug/
    └── nam1975/
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Region settings
| Region         | Code (0xC00401) | Original BIOS | Crosshatch color | Coin 1      | Coin 2     | Start Button 1 | Start Button 2 | Comment               |
| -------------- | --------------- | ------------- | ---------------- | ----------- | ---------- | -------------- | -------------- | --------------------- |
| Japan          | 0x00            | SP-J2         | Red              | Both P1/P2  | Both P1/p2 | P1 Only        | P1 and P2      |                       |
| USA            | 0x01            | SP-U2         | Sky blue / Cyan  | P1 Only     | P2 Only    | P1 Only        | P2 Only        |                       |
| Europe (Asia)  | 0x02            | SP-S2         | Blue             | Both P1/P2  | Both P1/P2 | P1 Only        | P1 and P2      |                       |

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Game compability
Development has mainly focused on games that differ from one another in their features and BIOS usage. Hopefully, this list will grow more and more quickly as development progresses.

| Game       | Console | Status             |
| ---------- | ------- | ------------------ |
| Metal slug | MV1FZS  | Boot, Runs stage 1 |
| NAM 1975   | MV1FZS  | Boot, Runs stage 1 |

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## License

Distributed under the GNU General Public License v3.0. See `LICENSE.txt` for more information.

_This project is not officially licensed, approved or endorsed by SNK Corporation._

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Acknowledgments

Many before have done amazing contributions for the Neo Geo community and made it possible to create this project:

* [NeoGeo Development Wiki](https://wiki.neogeodev.org/index.php?title=Main_Page)
* [MAME](https://www.mamedev.org)
* Furrtek
* [neogeodev](https://github.com/neogeodev)
* [Unibios (Razoola)](http://unibios.free.fr)
* [Neo Geo Diagnostics BIOS (smkdan)](https://smkdan.eludevisibility.org/neo/diag/)

_(Appologies in advance if someone feels cheated by this list)_

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Disclaimer

This project is a completely community-powered effort — not the work of a company. Feel free to send bug reports, feature requests, and questions, but please keep feedback constructive. If you’d like to see something improved, contributions (code, documentation, tests, or even just thoughtful suggestions) are always more impactful than complaints.

**Let’s build something great together!**

<p align="right">(<a href="#readme-top">back to top</a>)</p>