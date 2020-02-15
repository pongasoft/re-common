# Introduction

This project contains the common code used across pongasoft rack extensions (for Reason DAW by Reason Studios)

# History

In 2015, I started working on Rack Extensions for Reason and created this mini framework to share code across the various rack extensions subsequently released.

In January 2020, Reason studios opened up the RE platform to allow sharing and distributing this kind of project.

As a result, I have decided to make the code publicly available in the hope that it can help other people to bootstrap their efforts in learning the platform.

Note that this is not a full fledged and well designed framework (like the Jamba VST framework). More a set of concepts and common code used throughout the other projects. The code is, at this time, not well documented.

Note that this project was designed with the [SDK version 2.2](https://developer.reasonstudios.com/documentation/JukeboxReadme#_whats_new_in_sdk_2_2_0) in mind (which has the advantage of allowing REs to run with Reason 8). As a result the code does not use C++17 which is a new feature since 4.0.

Depending on feedback and which direction Reason Studios decide to go, this may turn into something bigger...

# List of projects using this mini framework

* [CVA-7](https://github.com/pongasoft/re-cva-7): free rack extension utility which lets you analyze a CV signal in various ways

# License

This project is released under the terms of the [Rack Extension License agreement](RE_License.txt)
