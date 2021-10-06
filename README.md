# MOrgan - "More than Organ"
This is a small collection of open-source audio plug-ins inspired by the basic design of [Hammond tone-wheel organs](https://en.wikipedia.org/wiki/Hammond_organ). These are NOT intended to serve as a "B-3 emulator"--if that's what you're looking for, have a look at the excellent (and free) [CollaB3 by Sampleson](https://sampleson.com/collab3-free-tonewheel-organ.html), or any of the many B-3 and "clonewheel" organ plug-ins available commercially.

## Goals of the MOrgan project

The purposes of the *MOrgan* project are:

1. To illustrate how the basics of the Hammond design can be roughly modeled using one instrument plug-in, one audio-effect plug-in, and one MIDI-effect plug-in, which can be combined in a multi-plug-in chaining tool such as [Unify](https://www.pluginguru.com/products/unify-standard/), [Element](https://kushview.net/), [Gig Performer](https://gigperformer.com/), or in a digital audio workstation (DAW).
2. To provide open-source implementations of the three basic plug-in types using the [JUCE plug-in development framework](https://juce.com/), including aspects such as automation parameters and custom GUI controls, which are not present in most JUCE-based code samples.
3. To allow sound-designers to create new multi-plug-in combination presets (using e.g., Unify) which support keyboard performance techniques familiar to the Hammond player, but with extended timbral range to become "more than organ".
4. To publish, for the first time in a generally-usable form, several pieces of code which I originally developed as part of the [AudioKit project](https://audiokit.io/).

## The three MOrgan plug-ins

The basic MOrgan plug-in collection comprises three plug-ins as follows:

1. **MOrgan Osc** is an instrument plug-in, which receives MIDI input and produces audio output, based on a simple tonewheel-organ oscillator model. The name "Osc" is slightly misleading, as this is in fact a full 32-voice polysynth.
2. **[MOrgan Cab](morgan-cab.md)** is an audio-effect plug-in, which processes incoming audio using two different *rotary speaker* models, reminiscent of the type of ["Leslie" speaker cabinet system](https://en.wikipedia.org/wiki/Leslie_speaker) often used with Hammond organs.
   - The first model adapts code from the [setBfree project](https://setbfree.org/) (see e.g. https://github.com/pantherb/setBfree), which was itself based on the paper "[Discrete Time Emulation of the Leslie Speaker](https://www.aes.org/e-lib/browse.cfm?elib=15120)" by Jorge Herrera, Craig Hanson, and Jonathan S. Abel, Presented at the 127th Convention of the Audio Engineering Society, 2009 October 9–12 New York NY, USA.
   - The second model uses code originally developed by Paul Kellett as part of the [mda-vst project](http://mda.smartelectronix.com/) (see also https://github.com/topics/mda-plugins). Although the MDA Leslie effect was not a true Leslie speaker emulation, it works nicely in conjunction with the more realistic *setBfree* version.
   - The plug-in includes a simple mixer to set the levels of the direct (unprocessed) signal and of the two separate Leslie effects.
3. **MOrgan Perc** is a MIDI-effect plug-in which can be routed before any instrument plug-in, allowing it to behave similarly to the characteristic "percussion" system of the classic Hammond organ models, which was unique in that it was both *polyphonic* and *mono-triggered*. The first MIDI note-on event after a no-keys-down condition passes through the plug-in, and triggers a timed *gate*. Subsequent note-on events will only pass through the plug-in if they follow the trigger within a preset *gate time*, and furthermore, their MIDI velocity values will be reduced according to an *exponential decay curve* which roughly models the response of the Hammond percussion circuit.

## Downloading and using the MOrgan plug-ins

If you simply want to *use* the MOrgan plug-ins, click on **Releases** at right for download links. You can ignore the two sections below; these are only related to compiling the open-source code yourself.

[Click here for description and usage details](https://getdunne.github.io/MOrgan/).

## Licensing and legal concerns

The [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0.en.html) (GPL3) is used as the primary license for the *MOrgan* code, in order to comply with the specific licensing conditions specified for JUCE and the Steinberg VST3 SDK for open-source projects. Note that *the GPL3 forbids you to use any of the code in a closed-source product which you distribute to others*.

The core DSP code files originally published as part of [AudioKit](https://audiokit.io/) are (as noted in each file) themselves licensed under the more permissive [MIT License](https://opensource.org/licenses/MIT), which *does permit use in closed-source projects*.

The rotary-speaker emulation code from the [setBfree project](https://setbfree.org/) was originally published under the [GPL version 2 license](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html), whose terms are a bit more restrictive than the later GPL v3. In response to my written request, [Dr. Robin Gareus](https://github.com/x42) has provided the following points of clarification:

> setBfree is licensed in terms of GPLv2+ "either version 2, or (at your option) any later version" (b_whirl/whirl.c:8-11).
>
> It is entirely valid to mix GPLv2+ and GPLv3 code in the same codebase. The resulting binary however has to be licensed in terms of the GPLv3.

It is up to you to familiarize yourself with the legal ramifications of how you choose to use this code. Shane Dunne and Shane Dunne & Associates, Inc. provides this code *on an as-is basis, with no warranty of any kind*, and *will not be responsible for any legal consequences arising from your own use of the source code*.

Note **VST is a registered trademark of Steinberg Media Technologies GmbH**.

## Dependencies

To compile the *MOrgan* code yourself, you will need to obtain your own copy of the the [JUCE plug-in development framework](https://juce.com/) and (if you want to compile VST3 plug-ins) the [VST3 Audio Plug-Ins SDK](https://www.steinberg.net/developers/).

Details of setting up to work with JUCE and the VST3 SDK are beyond the scope of this document.

