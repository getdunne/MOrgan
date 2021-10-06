# MOrgan = "More than Organ"

This is a small collection of open-source audio plug-ins inspired by the basic design of [Hammond tone-wheel organs](https://en.wikipedia.org/wiki/Hammond_organ). These are NOT intended to serve as a "B-3 emulator"--if that's what you're looking for, have a look at the excellent (and free) [CollaB3 by Sampleson](https://sampleson.com/collab3-free-tonewheel-organ.html), or any of the many B-3 and "clonewheel" organ plug-ins available commercially.

## Goals of the MOrgan project

The purposes of the *MOrgan* project are:

1. To illustrate how the basics of the Hammond design can be roughly modeled using one instrument plug-in, one audio-effect plug-in, and one MIDI-effect plug-in, which can be combined in a multi-plug-in chaining tool such as [Unify](https://www.pluginguru.com/products/unify-standard/), [Element](https://kushview.net/), [Gig Performer](https://gigperformer.com/), or in a digital audio workstation (DAW).
2. To provide open-source implementations of the three basic plug-in types using the [JUCE plug-in development framework](https://juce.com/), including aspects such as automation parameters and custom GUI controls, which are not present in most JUCE-based code samples.
3. To allow sound-designers to create new multi-plug-in combination presets (using e.g., Unify) which support keyboard performance techniques familiar to the Hammond player, but with extended timbral range to become "more than organ".
4. To publish, for the first time in a generally-usable form, several pieces of code which I originally developed as part of the [AudioKit project](https://audiokit.io/).

## The three MOrgan plug-ins

The basic MOrgan plug-in collection comprises three plug-ins. Click on the plug-in name for more information.

1. **[MOrgan Osc](morgan-osc.md)** is an instrument plug-in, which receives MIDI input and produces audio output, based on a simple tonewheel-organ oscillator model.
2. **[MOrgan Cab](morgan-cab.md)** is an audio-effect plug-in, which processes incoming audio using two different *rotary speaker* models, reminiscent of the type of ["Leslie" speaker cabinet system](https://en.wikipedia.org/wiki/Leslie_speaker) often used with Hammond organs.
3. **[MOrgan Perc](morgan-perc.md)** is a MIDI-effect plug-in which can be routed before any instrument plug-in, allowing it to behave similarly to the characteristic "percussion" system of the classic Hammond organ models, which was unique in that it was both *polyphonic* and *mono-triggered*.

Other plug-ins may be added to the collection in future.

