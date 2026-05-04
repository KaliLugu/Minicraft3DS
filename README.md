# Minicraft3DS

3DS Homebrew port of Notch's ludum dare game "Minicraft"  
Current Version: Version 2.0.0

----------

**Download:**

If you just want to download the game prebuilt check the releases tab in Github:
<https://github.com/KaliLugu/Minicraft3DS/releases>

For building the game yourself look below.  

----------
**Setup repository**

- git clone `git@github.com:KaliLugu/Minicraft3DS.git`
- make init

----------

**Building via Docker (recommended):**

- have Act and Docker installed
- type "act push"

----------

**Building without Docker:**
**1. Install devkitPro and select 3DS Development**

- Visit <https://devkitpro.org/wiki/Getting_Started> for more infos
- On Windows I recommend using the graphical installer
- And install atleast Minimal System, 3DS Development
- Use the msys2.exe found in "devkitPro"/msys2 to perform all following actions

**2. Install further dependencies using (dpk-)pacman**

- For 3ds build install: 3ds-libpng 3ds-curl 3ds-mbedtls

**4. You can now build Minicraft Homebrew Edition**

- Run "make"

----------

You can do anything with the source code (besides sell it) as long as you give proper credit to the right people.
If you are going to make a mod of this version, be sure to give credit to Markus "Notch" Perrson because he did create the original game after all.

This source code is subject to a lot of change for better optimization/cleanliness.

----------

<a href="https://www.star-history.com/?repos=KaliLugu%2FMinicraft3DS&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/chart?repos=KaliLugu/Minicraft3DS&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/chart?repos=KaliLugu/Minicraft3DS&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/chart?repos=KaliLugu/Minicraft3DS&type=date&legend=top-left" />
 </picture>
</a>
