# Minicraft3DS
3DS Homebrew port of Notch's ludum dare game "Minicraft"  
Current Version: Version 2.0.0


----------

**Download:**

If you just want to download the game prebuilt check the releases tab in Github:  
https://github.com/KaliLugu/Minicraft3DS/releases
For building the game yourself look below.  

----------


**Building:**

**1. Install devkitPro and select atleast 3DS and Switch Development**
- Visit https://devkitpro.org/wiki/Getting_Started for more infos
- On Windows I recommend using the graphical installer
- And install atleast Minimal System, 3DS Development and Switch Development
- Use the msys2.exe found in "devkitPro"/msys2 to perform all following actions

**2. Install further dependencies using (dpk-)pacman**
- For 3ds build install: 3ds-libpng
- For switch build install: switch-libpng switch-glad

**3. Install raylib-nx for Switch build**
- Follow the instructions on https://github.com/luizpestana/raylib-nx/wiki/Working-on-Switch for building raylib-nx
- Add RAYLIB_NX environment variable pointing to raylib-nx install location (the root directory, not "src")
- If you followed these instructions, a possible location to do this is: "devkitPro"/msys2/etc/profile.d/devkit-env.sh

**4. You can now build Minicraft Homebrew Edition**
- Run "make T=3ds" for building Minicraft3DS.3dsx
- Run "make T=switch" for building MinicraftSwitch.nro


----------


You can do anything with the source code (besides sell it) as long as you give proper credit to the right people. 
If you are going to make a mod of this version, be sure to give credit to Markus "Notch" Perrson because he did create the original game after all.

This source code is subject to a lot of change for better optimization/cleanliness.


----------


(Old) Forum thread: https://gbatemp.net/threads/release-minicraft3ds-the-multiplayer-update.495945/
