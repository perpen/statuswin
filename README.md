Plan 9 "file viewer"

![screenshot](screenshot.png?raw=true)

Simply displays lines read from the file passed as param.
If you run `statuswin blah.out` and blah.out contains "x\ny" then the window will show x and y in 2 boxes.

I use this to display on 9 some system info from my linux host (cpu usage, volume, pending package updates, etc)

On linux I start `./status-bar > /var/tmp/status-bar.out`,
then on 9 `statuswin /mnt/term/var/tmp/status-bar.out`

`status-bar` is an old ruby program of mine I've been dragging for years, check the `monitors` list at the end for configuration.
