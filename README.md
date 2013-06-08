xfade.lv2 - Stereo DJ X-fade
=============================

xfade.lv2 is an audio-plugin for stereo cross-fading
2 x 2 input channels to 2 output channels.

Install
-------

Compiling this plugin requires LV2 SDK, gnu-make and a c-compiler.

```bash
  git clone git://github.com/x42/xfade.lv2.git
  cd xfade.lv2
  make
  sudo make install PREFIX=/usr

  # test run
  jalv.gtk http://gareus.org/oss/lv2/xfade
```

Note to packagers: The Makefile honors `PREFIX` and `DESTDIR` variables as well
as `CFLAGS`, `LDFLAGS` and `OPTIMIZATIONS` (additions to `CFLAGS`).

Signal Controls
----------------------

The plugin has 3 parameters which are interpolated and can be operated
in realtime without introducing clicks or similar effects.

### Signal A/B
Fade between Input A (left-end, -1.0) and Input B (right-end, +1.0).

### Fade Shape
Allows to smoothly choose the A/B behaviour:

* Linear (default): constant Amplitude.  Out ∝ InA + InB
* Equal Power: retain signal power. Out<sup>2</sup> ∝ InA<sup>2</sup> + InB<sup>2</sup>

### Fade Mode
* X-fade (default): Inputs are fade over the complete range of the _Signal A/B_ control range.
* V-fade: Input A is only faded if _Signal A/B_ is > 0.0, Input B if _Signal A/B_ < 0.0

Consider the following simple diagrams:

```
     Vol
      ^
  1.0 |AA           BB
      |   A       B
      |     A   B          +-------------+
      |       X            | X-fade mode |
      |     B   A          +-------------+
      |   B       A
  0.0 |BB           AA
      +-----------------> (A/B control)
       -1     0     +1



     Vol
      ^
  1.0 |AAAAAAAXBBBBBBB
      |      B A
      |     B   A          +-------------+
      |    B     A         | V-fade mode |
      |   B       A        +-------------+
      |  B         A
  0.0 |AB           AA
      +-----------------> (A/B control)
       -1     0     +1
```

Screenshots
-----------

xfade.lv2 running in jalv.gtk:

![screenshot](https://raw.github.com/x42/xfade.lv2/master/screenshot_jalv.png "Basic Controls in jalv")

With Ardour3, use a 4 channel bus, bypass the VBAP-pan control and only connect Bus outputs 1&2 to the master bus:

![screenshot](https://raw.github.com/x42/xfade.lv2/master/screenshot_ardour.png "Ardour3 and xfade.lv2")

