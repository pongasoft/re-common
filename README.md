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

#### 3.1.0 - xxx

- Added `maybeWriteAudio` call to check for silent buffers when writing
- Added `SPLerp`/`DPLerp` and `SampleRateBasedClock` (imported from Jamba)
- Added `CVInProperty`
- Added unit test for re-common itself/working on it

#### 3.0.1 - 2021/09/27

- Fixed loguru included in universal45 and failing build

#### 3.0.0 - 2021/02/02

- Added `fmt` namespace that implements a simplified version of `printf` mostly for use in property name creation
- Added `jbox` namespace that implements the `JBox_GetMotherboardObjectRef` and `JBox_MakePropertyRef` Jukebox API
  calls using `fmt::printf` syntax. For example:
 
```cpp
int N = 4;
for(int i = 0; i < N; i++)
{
  fSampleNativeObjectRefs[i] = 
    jbox::get_property_ref("/custom_properties/sample_sound_native_object%d", i);
}
```

- Added `StaticString` which encapsulates an immutable array of characters (underlying type varies depending on debug 
  or release mode) to provide a simple syntax to create strings: 

```cpp
auto s = StaticString<100>::printf("test %s / %d", "abc", 12);

// ObjectPath is StaticString<kJBox_MaxObjectNameLen + 1>
fSampleObjectRef = JBox_GetMotherboardObjectRef(ObjectPath::printf("/user_samples/%d", 1));

// which is 100% equivalent to
fSampleObjectRef = jbox::get_object_ref("/user_samples/%d", 1);
```  

##### Warning Warning Warning : Breaking changes

- The `AudioInSocket`,  `AudioOutSocket`, `StereoInPair` and `StereoOutPair` constructors now takes socket name 
  instead of full socket path since the path can be computed by the class (and is less error prone):
  
```cpp
// old code
StereoOutPair audioOut{"/audio_outputs/mainOutLeft", "/audio_outputs/mainOutRight"};

// new code
StereoOutPair audioOut{"mainOutLeft", "mainOutRight"};

// which can also be simplified to (new in 3.0.0)
StereoOutPair audioOut{"mainOut"};
```

- The `CVInSocket` and `CVOutSocket` constructors now takes socket name instead of full socket path since the path 
  can be computed by the class (and is less error prone):
  
```cpp
// old code
CVOutSocket cvOut{"/cv_outputs/cv_out_socket"};

// new code
CVOutSocket cvOut{"cv_out_socket"};
```

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
