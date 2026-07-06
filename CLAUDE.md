# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

# Gain Plugin

JUCE 8 VST3 + AU + Standalone gain plugin. One big custom-drawn knob, -60 dB (-inf) to +12 dB.

Built cross-platform: Windows (VST3 + Standalone) and macOS (VST3 + AU + Standalone, for Logic Pro). `FORMATS` in `CMakeLists.txt` lists `VST3 AU Standalone` — JUCE silently skips AU when configuring on non-Apple platforms, so the same CMakeLists.txt works unmodified on both.

## Toolchain — Windows

- Compiler: VS Build Tools 2019 (MSVC 14.29) — generator `Visual Studio 16 2019`, x64
- CMake: portable install at `%LOCALAPPDATA%\Programs\cmake-4.3.3-windows-x86_64\bin\cmake.exe` (the Build Tools' bundled CMake 3.20 is too old for JUCE 8)
- JUCE 8.0.14 fetched via CMake FetchContent on first configure (needs network)

## Build — Windows

```powershell
$cmake = "$env:LOCALAPPDATA\Programs\cmake-4.3.3-windows-x86_64\bin\cmake.exe"
& $cmake -B build -G "Visual Studio 16 2019" -A x64
& $cmake --build build --config Release
```

## Artifacts — Windows

- VST3: `build\GainKnob_artefacts\Release\VST3\GainKnob.vst3` (copy to `C:\Program Files\Common Files\VST3` — needs admin)
- Standalone (for testing without a DAW): `build\GainKnob_artefacts\Release\Standalone\GainKnob.exe`

## Toolchain — macOS

- Xcode + command-line tools (`xcode-select --install`)
- CMake (`brew install cmake` — no special version pin needed, unlike Windows)
- JUCE 8.0.14 fetched via CMake FetchContent on first configure (needs network)

## Build — macOS

```bash
cmake -B build -G Xcode
cmake --build build --config Release
```

## Artifacts — macOS

- AU: `build/GainKnob_artefacts/Release/AU/GainKnob.component` — copy to `~/Library/Audio/Plug-Ins/Components/` to make it visible in Logic
- VST3: `build/GainKnob_artefacts/Release/VST3/GainKnob.vst3`
- Standalone: `build/GainKnob_artefacts/Release/Standalone/GainKnob.app`
- Validate the AU before trusting Logic to load it: `auval -v aufx Gain Uufe` (codes from `PLUGIN_CODE`/`PLUGIN_MANUFACTURER_CODE` in `CMakeLists.txt`)

There are no automated tests or lint setup on either platform; verification is building and running the Standalone app (or `auval` + Logic on macOS).

## Architecture

Four files in `Source/`, two classes plus a LookAndFeel:

- `PluginProcessor.{h,cpp}` — `GainAudioProcessor`. Owns a public `AudioProcessorValueTreeState` (`apvts`) with a single float parameter `"gain"` (skewed so 0 dB sits at the knob's centre). `processBlock` converts dB → linear via `juce::Decibels::decibelsToGain(x, minGainDb)` and applies it through a 50 ms `SmoothedValue`. State save/load is the APVTS tree serialized as XML. Mono and stereo layouts only (in == out).
- `PluginEditor.{h,cpp}` — `GainAudioProcessorEditor` plus `KnobLookAndFeel`, which custom-draws the rotary slider (track arc, orange value arc, gradient knob body, pointer). The slider binds to the parameter via `SliderAttachment`; double-click resets to 0 dB. Colours live in the `colours` namespace at the top of the .cpp.

The "-60 dB means -inf" convention is load-bearing: the range endpoints are `GainAudioProcessor::minGainDb`/`maxGainDb` constants, `decibelsToGain` is always called with `minGainDb` as the -inf floor, and the "-inf dB" display threshold (`value <= minGainDb + 0.05f`) is duplicated in the processor's `stringFromValue` lambda and the editor's `updateValueLabel()` — change one, change both.
