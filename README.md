# gr-nfc - GNURadio blocks allowing decode NFC transactions


## Synopsis

The gr-nfc package is a collection of GNURadio blocks allowing to decode an NFC communication. It has been designed to work with an __rtl-sdr__ compatible dongle, but it should work with any kind of SDR.


## Build instructions

In order to use this module, you need to have SWIG installed.
For instance on Ubuntu, you just need to do:

```
$ sudo apt-get install swig
```

To build and install the module:

```
$ mkdir gr-nfc/build
$ cd gr-nfc/build
$ cmake ../
$ make
$ sudo make install
$ sudo ldconfig
```


## Usage

Once installed, you will get an NFC category in GNURadio Companion. Just add and configure the block you need as usual.


## License

gr-nfc is distributed under the GPLv3 license. See the LICENSE file for more information.


## Miscellaneous

Fell free to visit my [blog](http://blog.rona.fr), and/or send me an email !

__  
Copyright (C) 2017 Jean-Christophe Rona
