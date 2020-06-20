Introduction
------------

This project contains the common code used across pongasoft rack extensions (for Reason DAW by Reason Studios)

History
-------

In 2015, I started working on Rack Extensions for Reason and created this mini framework to share code across the various rack extensions subsequently released.

In January 2020, Reason studios opened up the RE platform to allow sharing and distributing this kind of project.

As a result, I have decided to make the code publicly available in the hope that it can help other people to bootstrap their efforts in learning the platform.

> #### Note
> This is not a full fledged and well designed framework (like the Jamba VST framework). More a set of concepts and common code used throughout the other projects. The code is, at this time, not well documented.

> #### Note
> This project was originally created with RE SDK version 2.2. Although the latest version of this project assumes 4.1+ and some features requires C++17, at this time, not everything has been updated to use the latest features.

Depending on feedback and which direction Reason Studios decide to go, this may turn into something bigger...

List of projects using this mini framework
------------------------------------------

- [CVA-7](https://github.com/pongasoft/re-cva-7): free rack extension utility which lets you analyze a CV signal in various ways

Release notes
-------------

#### 2.0.0 - 2020/06/20

- Assumes RE SDK Version 4.1.0+
- Use C++17 features 
- Added logging (loguru) and `re-common.cmake` for inclusion in other projects

#### 1.0.1 - 2020/02/21

- Added details and README.

#### 1.0.0 - 2020/02/15

- First release.

Misc
----

- This project uses [loguru](https://github.com/emilk/loguru) for logging (included under `src/cpp/logging`)


License
-------

- This project is released under the terms of the [Apache 2.0 license](LICENSE.txt)

- This project uses the Rack extension SDK released under the [Rack Extension License agreement](RE_License.txt)
