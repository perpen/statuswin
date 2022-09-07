Plan 9 "file viewer"

![screenshot](screenshot.png?raw=true)

Simply displays lines read from the file passed as param.
If you run `statuswin lines.out` and lines.out contains "x\ny" then the window will show x and y in 2 boxes.

I use this to display on 9 some system info from my linux host (cpu usage, volume, pending package updates, etc)

On linux I start `my-status-bar > /var/tmp/status-bar.out`,
then on 9 `statuswin /mnt/term/var/tmp/status-bar.out`
