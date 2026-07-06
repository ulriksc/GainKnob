# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

# Gain Plugin

JUCE 8 VST3 + Standalone gain plugin. One big custom-drawn knob, -60 dB (-inf) to +12 dB.

## Toolchain

- Compiler: VS Build Tools 2019 (MSVC 14.29) — generator `Visual Studio 16 2019`, x64
- CMake: portable install at `%LOCALAPPDATA%\Programs\cmake-4.3.3-windows-x86_64\bin\cmake.exe` (the Build Tools' bundled CMake 3.20 is too old for JUCE 8)
- JUCE 8.0.14 fetched via CMake FetchContent on first configure (needs network)

## Build

```powershell
$cmake = "$env:LOCALAPPDATA\Programs\cmake-4.3.3-windows-x86_64\bin\cmake.exe"
& $cmake -B build -G "Visual Studio 16 2019" -A x64
& $cmake --build build --config Release
```

There are no tests or lint setup; verification is building and running the Standalone app.

## Artifacts

- VST3: `build\GainKnob_artefacts\Release\VST3\GainKnob.vst3` (copy to `C:\Program Files\Common Files\VST3` — needs admin)
- Standalone (for testing without a DAW): `build\GainKnob_artefacts\Release\Standalone\GainKnob.exe`

## Architecture

Four files in `Source/`, two classes plus a LookAndFeel:

- `PluginProcessor.{h,cpp}` — `GainAudioProcessor`. Owns a public `AudioProcessorValueTreeState` (`apvts`) with a single float parameter `"gain"` (skewed so 0 dB sits at the knob's centre). `processBlock` converts dB → linear via `juce::Decibels::decibelsToGain(x, minGainDb)` and applies it through a 50 ms `SmoothedValue`. State save/load is the APVTS tree serialized as XML. Mono and stereo layouts only (in == out).
- `PluginEditor.{h,cpp}` — `GainAudioProcessorEditor` plus `KnobLookAndFeel`, which custom-draws the rotary slider (track arc, orange value arc, gradient knob body, pointer). The slider binds to the parameter via `SliderAttachment`; double-click resets to 0 dB. Colours live in the `colours` namespace at the top of the .cpp.

The "-60 dB means -inf" convention is load-bearing: the range endpoints are `GainAudioProcessor::minGainDb`/`maxGainDb` constants, `decibelsToGain` is always called with `minGainDb` as the -inf floor, and the "-inf dB" display threshold (`value <= minGainDb + 0.05f`) is duplicated in the processor's `stringFromValue` lambda and the editor's `updateValueLabel()` — change one, change both.
