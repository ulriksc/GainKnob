# Moving to Mac (M4 Mini) — Step by Step

Goal: build GainKnob as an AU plugin and load it in Logic Pro.

## 1. Install prerequisites

```bash
xcode-select --install
```

Installs the Xcode command-line tools (compiler, git, etc.) — needed even though you already have Xcode.

```bash
brew install cmake
```

If you don't have Homebrew yet, install it first from [brew.sh](https://brew.sh). Any recent CMake version works (no special pin needed like on Windows).

## 2. Clone the repo

```bash
git clone https://github.com/ulriksc/GainKnob.git
cd GainKnob
```

## 3. Configure and build

```bash
cmake -B build -G Xcode
cmake --build build --config Release
```

First configure will fetch JUCE 8.0.14 via CMake FetchContent — needs network access and takes a few minutes.

## 4. Install the AU into Logic

```bash
cp -R build/GainKnob_artefacts/Release/AU/GainKnob.component ~/Library/Audio/Plug-Ins/Components/
```

## 5. Validate the AU

```bash
auval -v aufx Gain Uufe
```

Confirms Logic will actually be able to load it (checks parameter ranges, threading, etc.) before you go looking for it in the DAW. Look for `PASSED` at the end of the output.

## 6. Open Logic Pro

- Restart Logic (or rescan plugins if it's already running).
- Find it under **Audio Units → UlrikSC → GainKnob** on an audio track's Channel Strip / Audio FX slot.

## Other artifacts (optional)

- VST3: `build/GainKnob_artefacts/Release/VST3/GainKnob.vst3`
- Standalone app (test without any DAW): `build/GainKnob_artefacts/Release/Standalone/GainKnob.app`

## If something's wrong

- `auval` fails → rebuild in Release, re-copy the `.component`, re-run `auval`.
- Logic doesn't show it → fully quit Logic, remove `~/Library/Caches/AudioUnitCache`, relaunch.
- Build errors mentioning JUCE → delete the `build/` folder and reconfigure from scratch (`rm -rf build`, then step 3 again).
