# Ratchet & Clank: Railfinity

An infinite, procedural 3D action grindrail runner built in **C++20** with **Raylib 5.0** for Windows and Android, inspired by *Ratchet & Clank: Rift Apart* and *Before the Nexus*. Grind high-speed plasma rails across 8 shuffled galactic sectors, deploy Clank's Heli-Pack and Swingshot grapples, smash Bolt Crates and Raritanium caches with the OmniWrench 8000, rotate across 6 upgradeable Gadgetron weapons, customize **Ratchet, Rivet, Clank, and Kit** in an interactive 3D Wardrobe Lab, and take on daily Dreadzone mutators and live radio dares from Captain Qwark and Dr. Nefarious.

> **Studio:** Sebiny Labs  
> **IP Disclaimer:** Non-commercial fan tribute created for educational and portfolio purposes. *Ratchet & Clank*, *Rift Apart*, and *Before the Nexus* are registered trademarks and copyright of **Sony Interactive Entertainment LLC / Insomniac Games**.  
> **Playable Builds:** [Download Windows `.zip` & Android `.apk` (GitHub Releases)](https://github.com/sebiny12p/Ratchet_and_Clank_Railfinity/releases)

![Ratchet & Clank: Railfinity Gameplay](docs/gameplay.gif)

---

## Game Modes, Daily Mutators & Live Radio Dares

### 1. Endless Grind & Daily Arena Challenge
- **Endless Grind:** Randomized 8-sector tour with escalating rail speeds, dynamic 3-rail corridor shifts, `20x` Bolt Multiplier progression, and Boss Dropship ambushes.
- **Daily Challenge (8 Rotating Arena Mutators):** Seeded daily track featuring one of **8 Hazard + Perk mutators** (*Armored Convoy*, *High-Voltage Grid*, *Volatile Cargo*, *Overclocked Rails*, *Aerial Assault*, *Gadgetron Overload*, *Raritanium Rush*, and *Deadlocked Gauntlet*), awarding bonus **Raritanium** on sector clears and `2x` contract progress.

### 2. Daily Dreadzone Contracts, Skill Points & Radio Dares
- **12 Rotating Bounties (`Records -> Bounties`):** 3 daily contracts per day awarding substantial **Bolts** and **Raritanium**.
- **7 Live Mid-Run Radio Dares:** Captain Qwark, Dr. Nefarious, and Clank broadcast spontaneous 30-second stunt dares mid-run (*Wrench Fu*, *Untouchable*, *Drone Safari*, *Aerial Calibration*, *Pyrotechnics*, *Rail Weaver*, *Combo Showtime*) that shower bonus Bolts & Raritanium directly onto the rails.
- **10 Insomniac Skill Points & Career Dossier:** Classic skill challenges and full lifetime telemetry tracking distance grinded, crates smashed, bosses defeated, and weapon mastery.

---

## Character & Armor Systems (3D Wardrobe Lab)

Preview and rotate heroes `360°` in real time in **Arsenal -> Wardrobe**, featuring custom 3-point studio lighting, glossy corneal eye catchlights, natural blinking, eye saccades, and expressive Lombax animations:

### 1. Lombax Hero Suits

| Suit Name | Origin & Lore | Perk & Signature Effect | Cost |
| :--- | :--- | :--- | :--- |
| **Classic Commando** | Solana & Bogon Aviator | Ratchet's iconic pilot vest & Bogon denim. Balanced, agile, and timeless. | Default (Free) |
| **Carbonox Armor** | Going Commando / Up Your Arsenal | Nanotech graphite alloy. **+1 Extra Nanotech HP** & cyan electric grind sparks. | 2,000 B / 8 R |
| **Rebel Lombax** | Sargasso Rebel Cell // Rift Apart | Playable **Rivet** with 3D cybernetic arm, platinum fur, and cyan/violet sparks. | 2,500 B / 0 R |
| **Infernox Armor** | Megacorp Thermal Weapons Lab | Active volcanic core plates. Radiates magma embers & **magnetizes crates (+0.65m)**. | 4,500 B / 15 R |
| **Deadlocked Marauder** | Dreadzone Combat Arena | Heavy gladiator battlegear. **1.25x Wrench Melee Damage** & shockwaves. | 6,500 B / 25 R |
| **Lombax Praetorian** | Ancient Dimensional Archive | Ceremonial pearl-gold regalia. **1.5x Bolt Multiplier Escalation Rate**. | 9,000 B / 35 R |

### 2. Companion Robot Outfits

| Outfit Name | Origin & Finish | Optics & Beacon |
| :--- | :--- | :--- |
| **Titanium Chassis** | Insomniac Standard Issue | Chrome silver with dark gunmetal trim, emerald camera lenses, and red beacon. |
| **24-Karat Golden Clank** | Gold Weapon Foundry | Electroplated solid 24K mirror gold with brilliant sapphire-blue optics. |
| **Secret Agent Clank** | Holomedia Entertainment | Matte stealth-tuxedo black with formal red bowtie and golden spectacles. |
| **Dread Pirate Clank** | Space Pirate Flotilla | Weathered oxidized bronze with brass rivets and glowing ruby optics. |
| **KT-74-610 // Kit** | Dimension-Hopper Warbot | Warm copper chassis, dual angled sensor horns, and sunny gold optics. |

---

## Gadgetron Arsenal, Tactical Ammo & Raritanium Lab

Level up weapons through combat usage from **V1** to **V5 Omega**, then forge them with Raritanium into **Golden Mastery** forms:

1. **OmniWrench 8000 (Unlimited Melee):** Heavy forward melee strike that shatters crates/drones, deflects incoming enemy plasma bolts, and earns `+2` bonus Bolts per crate.
2. **Magma Combuster:** Rapid-fire twin/triple plasma stream dealing `2.5x` shield-melter damage to armored Enforcers.
3. **Constructo Shotgun:** 3-rail concussive shockwave spread that vaporizes incoming enemy shots mid-air.
4. **Buzz Blades:** High-RPM saw discs with smart cross-rail pinball ricochet across all 3 active lanes.
5. **Plasma Striker:** Instant rail-piercing sniper hyperbeam that arcs EMP chain-lightning to adjacent rails.
6. **R.Y.N.O. Infinite:** Locks homing micro-missiles across all 3 rails with explosive AoE blasts and `+50%` Bolt jackpots.

### Tactical Ammo Economy & Arsenal Rotation
- **Scarce Gadgetron Ammo Crates:** Standard Bolt, Nanotech, and Raritanium crates never refund bullets; instead, dedicated steel-and-orange **Gadgetron Ammo Crates** (and close-quarters OmniWrench takedowns on armored Enforcers) drop finite ammo packs.
- **Cross-Arsenal Drops:** Ammo pickups weight `80%` toward your unequipped firearms and auto-cycle to your next loaded gun when empty, rewarding active switching across all 6 weapons.
- **Raritanium Mod Lab:** Invest Raritanium crystals into Nanotech Armor HP, Box-Breaker Magnet range, Heli-Pack & Boost Thrusters, Multiplier Escalation, Overdrive Hoverboots, and Golden Weapons.

---

## 8 Shuffled Galactic Sectors & 4 Boss Encounters

Each run shuffles all 8 sectors with smooth 40m atmospheric crossfades, spatial upcoming landmarks, and 3D Warp Gate rings:

1. **Metropolis Kerwan (Solana Galaxy):** Ringed gas giant horizon, gleaming skyscrapers, and cruising hover-traffic.
2. **Corson V — Nefarious City (Polaris):** Neon-magenta cyberpunk skyline, diagonal rain, and weaving hover-taxis.
3. **Planet Gaspar — Volcanic Core:** Molten magma basin with obsidian spires and erupting lava geysers.
4. **Pokitaru — Jowai Resort Coast:** Sunlit azure ocean, palm-tree islands, and leaping cyber-piranhas.
5. **Torren IV — Molonoth Scrap Fields:** Rusted wasteland canyons with industrial scrap towers and swinging crane debris.
6. **Megacorp Weapons Park (Bogon Galaxy):** Industrial weapons foundry with overhead catwalks and shielded Enforcers.
7. **Zordoom Prison Maximum Security:** High-security galactic penitentiary with crimson lockdown laser grids.
8. **Dimensionator Rift Anomaly:** Psionic violet dimensional tear with floating crystal obelisks and rift rings.

### Boss Dropships
1. **Prog Twins // Command Warship:** Fires synchronized 2-lane plasma carpet-bombing volleys.
2. **Emperor Nefarious // Dread-Frigate:** Sweeps dual Imperial ground lasers across the rails alongside rapid bolts.
3. **Captain Slag // Sky-Galleon:** Space-pirate warship firing staggered 3-rail broadside cannon waves.
4. **Bioliterator // Nefarious Dread-Mech:** Tracks your lane with a crimson telegraph laser and unleashes rapid 3-shot bursts.

---

## Audio, Presentation & Accessibility

- **Soundtrack Jukebox:** 4 dynamic Insomniac-inspired chiptune/synth themes (*Rift Apart Main Theme*, *A Crack in Time*, *Going Commando - Tabora*, *Up Your Arsenal - Marcadia*).
- **Crisp Native-Resolution 3D & UI:** Native display-resolution backbuffer with `4x MSAA` anti-aliasing, custom 32-bit `highp` OpenGL ES shaders on Android, and display-scaled `Roboto` typography.
- **Accessibility & QOL:** Borderless native fullscreen (`F11 / Alt+Enter`), `0.29s` committed rail hops with input buffering, left-handed touch controls, configurable swipe sensitivity, focus-loss auto-pause, smooth `0.75s` unpause speed ramp, `Q`/`E` (`LB`/`RB`) tab cycling, and live `"!"` upgrade badges.

---

## Controls

| Action | PC (Keyboard & Mouse) | Gamepad (Xbox / PS) | Android / Touch |
| :--- | :--- | :--- | :--- |
| **Switch Rail Left / Right** | `A` / `D` or `LEFT` / `RIGHT` | `D-Pad Left / Right` or `L-Stick` | Swipe Left / Right |
| **Jump / Clank Heli-Glide** | `SPACE` / `W` / `UP` (Hold to glide) | `A` / `Cross` (Hold to glide) | `[JUMP]` Button or Swipe Up |
| **Wrench Slide / Duck** | `S` / `DOWN` / `L-CTRL` | `B` / `Circle` or `L-Stick Down` | Swipe Down |
| **Fire / Swing Weapon** | `F` / `J` / `ENTER` / `Tap` | `RT` / `R2` or `X` / `Square` | `[FIRE]` Button or Tap |
| **Overdrive Hoverboots** | `SHIFT` | `LT` / `L2` or `Y` / `Triangle` | `[BOOST]` Button |
| **Select / Cycle Weapon** | `1` - `6` / `Q` / `E` / `Wheel` | `LB` / `RB` (`L1` / `R1`) | Tap Weapon Hotbar (`[1]`–`[6]`) |
| **Pause Game** | `ESC` / `P` | `START` / `Options` | Top-Right `[II]` Button |
| **Soundtrack Selector** | `N` (Next Track) / `M` (Mute) | Via Pause / Settings Menu | Via Settings Menu |
| **Toggle Fullscreen** | `F11` or `ALT + ENTER` | — | Native Immersive Fullscreen |

---

## Technical Specifications

- **Engine:** Raylib 5.0 with modern C++20.
- **3D Camera System:** Custom third-person `Camera3D` tracking with rail curvature look-ahead, velocity-based roll tilt, and dynamic FOV warp.
- **Fixed-Timestep Physics:** `120 Hz` accumulator loop ensuring deterministic, stutter-free motion across 60 Hz–240 Hz displays.
- **Zero Runtime Heap Allocations:** All 3D track segments, bolts, hazards, particles, and weapon projectiles run on pre-allocated static pools.
- **Cross-Platform Parity:** Single C++20 codebase compiling natively to Windows 64-bit and Android `arm64-v8a` (OpenGL ES 2.0 / 3.0).

---

## Building & Deliverables

### Windows Executable & Standalone Package
```cmd
cmake -B build -G Ninja
ninja -C build
.\build\RatchetAndClankRailfinity.exe
```
Standalone distribution archive: `RatchetAndClankRailfinity_Windows.zip` (contains `.exe` and self-contained `assets/` folder).

### Android APK Build
Requires Android SDK + NDK (`r26b`+):
```cmd
cmd.exe /c Build_Android_APK.bat
```
Output: `RatchetAndClankRailfinity.apk` (aligned and signed with v2/v3 signatures, ready for direct sideloading).
