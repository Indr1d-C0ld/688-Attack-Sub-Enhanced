# 688 Attack Sub — Enhanced Edition

**Tagline (116 characters)**

> The 1989 classic 688 Attack Sub, rebuilt from its own code, with a deep and realistic Cold War submarine sim on top.

**Short description (273 characters)**

> A non-commercial remake of EA's 688 Attack Sub (1989). Classic mode reproduces the original rules exactly,
> reverse-engineered from the DOS executable; Enhanced mode adds real acoustics and sonar, weather, damage, careers,
> an editor and Cold War missions drawn from history.

---

## What the project is

*688 Attack Sub* (John W. Ratcliff and Paul Grace, Electronic Arts, 1989) was one of the first submarine simulators
to let you command both sides of the Cold War: a US Navy Los Angeles-class boat or a Soviet Alfa. This project
rebuilds it from scratch as a new program, in two layers.

- **Classic mode** plays the 22 original missions with the original rules. Those rules were recovered from the DOS
  executable and its mission scripts, not imitated by eye. Results are checked bit for bit against reference runs:
  the same orders and the same random seed must give the same game.
- **Enhanced mode** keeps the same missions and boats and adds what a 1989 home computer could not do. Sound is
  modelled physically, sonar and electronic warfare work as on the real boats, and the weather and sea come from
  climate data. Damage, a career with real ranks and medals, a mission editor and a library of new missions based on
  Cold War events complete it.

It is a non-commercial fan project, not affiliated with or endorsed by Electronic Arts. The original game's assets
(graphics, font, texts, mission files) are not redistributed: they are read from the player's own copy.

The game is fully in Italian, with English throughout the data, so a second language is a translation pass rather
than a rewrite.

## How the work is organised

The project is developed with **Claude Code** (Anthropic), run as a small team of parallel AI sessions coordinated
by a human designer, who is the author of the project. The designer sets the direction, approves every research
proposal before code is written, and judges the results.

- **The main session** coordinates. It keeps the roadmap and a backlog of open items (oldest first), assigns work so
  that no two sessions edit the same files, approves interfaces between modules, and wires the pieces into the game
  screens.
- **Four specialist sessions** work at the same time:
  - **Reverse engineering, acoustics and sonar:** the x86 disassembly of the original, the mission virtual machine,
    the sound model, the sonar stations, the weapons load-out, the career, achievements, replay data and statistics.
  - **Mission runtime:** the simulation, the radio room, airborne anti-submarine warfare, collisions, naval gunfire,
    mines and the AI's doctrine.
  - **Graphics kit:** 3D models built from public-domain photographs, ship damage and sinking, the visual effects,
    the console panels of both boats and the in-game encyclopedia.
  - **Missions and media:** the quick-mission generator, the mission editor, the mission library and the trailers.
- **Contracts first.** When a feature spans several sessions (for example naval gunfire: data, firing model, damage,
  graphics, audio, game screens), its interface is written down and approved before anyone codes, and each piece is
  delivered against it.
- **Research first.** Every major feature starts from a written research dossier with sources and a confidence rating
  for each number. 23 dossiers so far, from platforms and weapons to acoustics, radio, periscopes, weather, careers
  and Cold War incidents. Values that cannot be sourced are marked **ESTIMATE** in code and data, so they can be found
  and replaced later.
- **Regression as a rule.** Every change is checked against:
  - about 600 automated test cases (some 2.6 million assertions);
  - a 110-game «classic survey», which must stay identical to the reference;
  - snapshot fingerprints of the simulation and screenshots.

  The classic game is never allowed to drift.
- **Shared files and stale builds.** Headers shared by several sessions are changed only with point edits announced
  to the others. Each session verifies in its own private build, because several sessions compile at once.

## Reverse engineering

- The DOS executables were unpacked (EXEPACK) and analysed with **Ghidra 12** in headless mode, through
  **REA (rea-agents)**, an MCP server that gives an AI agent function-by-function access to Ghidra. The work also used
  **Capstone** for quick 16-bit disassembly and **Python** scripts for the data formats.
- The mission files (`SCENE*.LZA`, compressed bytecode for the game's own scripting virtual machine) were decoded, and
  the VM was rebuilt. All 22 missions were converted to readable **Lua** that the new engine runs with the original
  semantics.
- Every graphic and data asset was extracted and converted: sprites, palettes, fonts, screens, the OPL2 music and the
  4-bit PCM samples. The original ship database, sensor ranges, damage tables and messages were mapped, with
  addresses documented.
- The original manual (about 21,000 words) and all in-game texts were translated into Italian. Briefings and orders
  have short forms where the screen is narrow.
- Side-by-side comparisons with the original running in **DOSBox Staging** keep the classic mode honest. They are also
  how the «Before and after» trailer was made.

## What the Enhanced mode adds

- **Acoustics and sonar.** Sound is propagated with real sound-speed profiles per theatre and month, ambient noise
  depends on weather and traffic, and the sonar equation is solved per sensor. Each class has its own radiated noise,
  tonal lines and cavitation.
  - The sonar station has broadband waterfalls, LOFAR/DEMON analysis, trackers and a classification library.
  - The headphones play a mix synthesised from the model, plus licensed real recordings.
  - A realistic towed array is deployed and recovered, and goes blind in turns.
- **Weather and sea.** Data from the ERA5 reanalysis (1981-1990) drives the forecast. Waves, foam, spray and rain
  behave by Beaufort force at the periscope; lightning and thunder are heard on board and on the sonar.
- **Periscopes and electronic warfare.** The real periscope heads include TV, low-light and thermal modes, exposure
  time and the risk of being seen. The ESM receiver identifies radar emitters and estimates detection risk, and the AI
  manages its own emissions.
- **The ocean floor.** Bathymetry comes from EMODnet (European theatres) and NOAA ETOPO 2022, shown on period
  nautical charts for each side,
  with shaded relief. Seven theatres: Barents, Faroe-Iceland gap, Celtic Sea, Gibraltar, Otranto, Baltic and Kunashir.
- **Radio room.** VLF/ELF broadcasts and a floating wire run on real schedules, with authentication codes and
  deception. Orders and rules of engagement change during the mission, and transmitting gives away your position.
- **Weapons and countermeasures.** The load-out is historically checked for 1989: Mk 48 and ADCAP, Harpoon and
  Tomahawk in capsules, Mk 67 mines, static and mobile decoys (what-if items are marked as such). Wire guidance and
  realistic reloads are modelled.
- **The enemy.** Airborne ASW (SH-60, Ka-25/27, P-3C, Il-38, Tu-142, Be-12) follows real search doctrine, with
  sonobuoys, dipping sonar, MAD and air-dropped torpedoes. Ships fire their guns at a surfaced boat or a raised
  periscope according to the rules of engagement. The AI uses Crazy Ivan manoeuvres, diesel boats must snorkel, and
  minefields, mine-detecting sonar and towed-array cables are all modelled.
- **Damage.** Compartments flood with flow rates that depend on depth, and fires, explosions and the crew's damage
  control follow. Ships break, list and sink as their 3D models say, and collisions are physical.
- **Both boats' consoles.** The 688's control room is reconstructed with period US Navy labels, and the Alfa's with its
  automated systems (Boksit, Tan, Akkord) in transliterated Russian stencil.
- **Career.** The real rank ladders of both navies apply, from Ensign to Captain and from Leitenant to Kapitan 1-go
  ranga. Period decorations come with award ceremonies. There are fitness reports and selection boards, and you can be
  relieved or killed in action.
  - A roster of commanders and a personal service record.
  - 41 achievements in bronze, silver and gold.
  - A hall of honour, which includes historical commanders (without scores).
- **After the mission.** The debriefing works in the style of F-19 and F-117A: highlighted events with snapshots, the
  patrol report and «Resume from here». A full replay shows «ground truth» or «what the captain knew», on a timeline.
  Per-mission and cumulative statistics include heatmaps.
- **Making missions.** Quick missions come from a shareable code. The mission editor has units, rules, radio and
  weather, and import/export of mission packages. The mission library has 19 missions: historically grounded scenarios
  from the Cold War for both sides, plus original ones.
- **Encyclopedia.** About 200 entries cover every unit, weapon and sensor in the simulation, with sources, confidence,
  free-licence photos and in-game images. It can be consulted from inside a mission.

## Sources

- **Declassified intelligence:** CIA National Intelligence Estimates on Soviet naval and ASW forces, DTIC technical
  reports, NATO publications.
- **Naval references:** Norman Polmar and K. J. Moore (*Cold War Submarines*), Norman Friedman, *Jane's* for the
  period, the Naval History and Heritage Command (DANFS), the Federation of American Scientists, GlobalSecurity,
  NavWeaps, and Russian-language references such as deepstorm.ru, militaryrussia.ru and the works of Apalkov and
  Pavlov.
- **Acoustics:** R. J. Urick, *Principles of Underwater Sound*; Ross, *Mechanics of Underwater Noise*; Waite,
  *Sonar for Practising Engineers*; Owen R. Coté Jr., *The Third Battle* (Cold War ASW and Soviet submarine
  quieting).
- **Ocean and weather data:**
  - EMODnet Bathymetry (CC BY 4.0) and NOAA ETOPO 2022;
  - World Ocean Atlas 2023 for monthly sound-speed profiles (TEOS-10);
  - Copernicus ERA5 (CC BY 4.0) and Open-Meteo (CC BY 4.0).
- **Images and sound:** US Navy and Department of Defense photographs in the public domain, Wikimedia Commons files
  under CC0, CC BY or CC BY-SA (each credited), and audio samples under CC0, public domain or CC BY only.
- Every number in the data files carries its source and a confidence level. The credits screen and `docs/CREDITI.md`
  are generated automatically from the same records.

## Software

- **Engine:** C++20, SDL3, a 640×480 indexed 256-colour canvas, CMake and Ninja, the doctest unit-test framework, Lua
  5.4 for missions, toml++ for data, and ymfm (Aaron Giles) to emulate the OPL2 FM chip of the original music.
- **Data pipeline:** Python 3.13 with NumPy, SciPy, Pillow, xarray, netCDF4 and gsw (TEOS-10 seawater equations). The
  pipeline turns research data, bathymetry, climate data and 3D models into the engine's formats.
- **Reverse engineering:** Ghidra 12.2 and 12.1 (headless) with JDK 25 and 21, REA (rea-agents) via Node.js,
  Capstone, and DOSBox Staging for reference captures.
- **Media:** FFmpeg for the trailers; FluidSynth with the MuseScore General soundfont (MIT) for the music, written in
  ABC and MIDI.
- **Development:** Claude Code (Anthropic), run as coordinated parallel sessions.

## In numbers

- About 103,000 lines of C++ in the engine and 16,000 lines of Python in the tools.
- About 600 automated test cases and a 110-game survey of the classic mode, kept identical.
- 23 research dossiers, 7 theatres, more than 40 ship, submarine and aircraft classes, and 28 3D models built from
  photographs.
- 22 original missions plus a 19-mission library, quick missions and an editor.
- About 200 encyclopedia entries and 41 achievements.

## Legal note

*688 Attack Sub* is a trademark of its respective owners. This is an amateur, non-commercial project, not affiliated
with Electronic Arts. No original asset is distributed: the game reads them from the player's own copy. Third-party
data and media are used under their licences, listed in the credits.
