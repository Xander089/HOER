# HOER

**Engine:** Unreal Engine 5.5 (C++) · **Context:** Master's course final project · **Team:** 4 programmers

HOER is a light-driven horror game built with Unreal Engine 5.5. The player explores dark corridors with a plush companion while an enemy, guided by an AI Director, hunts them. I owned four gameplay systems: **Audio**, **AI**, **Save System** and **UI**. Each one is mostly built in C++, configured through data assets, and exposed to Blueprints so designers can tune it without touching code.

---

## 🔊 Audio

- **Component-based audio architecture:** I wrote an abstract `UBaseAudioComponent` with specialised subclasses for the player character, the companion, enemies and world props (doors, pushable blocks, puzzles).
- **MetaSound integration:** MetaSound assets stream in asynchronously through the `StreamableManager`. Sounds are triggered by named MetaSound input triggers, and stops use ADSR release fade-outs instead of hard cuts.
- **Event-driven:** each component subscribes to its owner's gameplay events (footsteps, jumps, throws, grabs, landings, enemy growls), so the gameplay code never refers to the audio code.
- **Adaptive music:** a `MusicActor` crossfades between *Calm*, *Mid* and *High tension* layers. The AI chase logic drives the tension level.
- **Designer-friendly tooling:** data assets hold the event names and concurrency/attenuation settings, editor dropdowns list the available MetaSound trigger pins, and editor data validation catches misconfigured components.

## 🧠 AI

- **Two-layer AI design:** an **AI Director** controller manages pacing and tells the **Enemy** controller where to search. Both run on Behavior Trees and share Blackboard keys.
- **Dynamic "risk awareness":** the Director reacts to the lights the player switches on and whether the player is lit, and turns this into a single risk value. Curve tables map that value to the enemy's walk speed, sight radius, peripheral vision angle, hearing range and other perception parameters.
- **AI Perception (sight + hearing):** the enemy detects and forgets the player, starts chases, broadcasts detection events (which drive music and tension) and ignores players inside safe zones.
- **Custom Behavior Tree tasks:** I wrote nine C++ BT tasks, covering patrol-location assignment, waypoint and random NavMesh patrols, chase handling, reacting to risk, and teleporting the enemy to a corridor spawn point the player can't see.
- **Environmental storytelling:** an `NPCLightSwitcherComponent` turns off nearby lights as the enemy approaches and restores them after a delay, so the player can sense where the enemy is.

## 💾 Save System

- **`USaveSubsystem` (Game Instance Subsystem):** saves asynchronously to disk, loads from named slots, and applies the save to the world after a level transition (load-on-next-map).
- **Interface-driven persistence:** actors take part through `ISavableActorInterface` and are captured generically. This covers the player and enemy positions and rotations, interactables that have been used, and unlocked doors (whose key pickups are hidden again on load).
- **Hashed save records:** `FActorSaveData` entries are stored in `TSet`s keyed by actor name, which keeps restore lookups fast. The world state is rebuilt on every save so no stale entries are left behind.
- **Persistent user settings:** a custom `UGameUserSettings` class stores music and SFX volume in config and applies them to the engine's Sound Mix and Sound Classes at runtime.

## 🖥️ UI

- **Page-stack HUD:** a custom `AHOERHUD` pushes and pops full-screen pages. It caches and restores focus, switches the input mapping context between UI and gameplay, handles Back/Cancel, and shows timed toast notifications.
- **Complete menu flow:** Main Menu (New Game / Continue), Pause, Settings, Controls, auto-scrolling Credits, Game Over and Game End screens.
- **Reusable widget library:** a shared base widget plus a button, slider row, command row, credit row, page footer and in-world interaction prompt. Each has Blueprint hooks for focus and hover animations.
- **Full gamepad support:** keyboard/gamepad focus navigation, and footer prompt icons that change when a controller is connected or disconnected.
- **In-game HUD:** a real-time **panic bar** and gameplay overlay, plus a settings page whose volume sliders write to the persistent user settings.
