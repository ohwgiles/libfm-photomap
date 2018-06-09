libfm-photomap
==============

A plugin for libfm, the core library of the PCManFM file manager, to display a jpeg photo's GPS EXIF tag on a map in the file properties dialog.

![screenshot](https://github.com/ohwgiles/libfm-photomap/raw/master/screenshot.png)

The plugin uses [libexif](https://libexif.github.io/) to retrieve the GPS tag, and [osm-gps-map](http://nzjrs.github.io/osm-gps-map/) to display the map widget.

Since osm-gps-map is built for gtk3, this plugin currently **only works with pcmanfm-gtk3**.

Contributions are welcome.
