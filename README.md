# Drift

Author: Jiasheng Zhou \<jiashen2@andrew.cmu.edu\>

Design: 

Screen Shot:

![Screen Shot](screenshot.png)

Asset Pipeline:

1. Authoring Tools: GIMP
2. Use GIMP to create 128px * 128px tile map in png format: tiles.png, half of the first 8px * 8px tile is used as the palette, the project is saved as tiles.xcf
3. png_to_tilemap.exe: Code that converts tiles.png to tiles.tile and tiles.palette
4. Runtime code that loads tiles and palette into memory and upload it to PPU466

How To Play:

Goals:
You are drifting on a raft in a boundless ocean. Survive as long as possible and get the most score!

Rules:

The ocean will continuously scroll and you will gain points alongside. You have 5 lives initially and touching the stone will lose 1 life each time.\
Once you run out of life, the score and lives will be reset.

Items:

1. Faster Ocean: \
   <img src="faster_ocean.png" width="16" height="16" />\
   Increase the ocean speed for 5 seconds and gain 5 points.
2. Slower Ocean:\
   <img src="slower_ocean.png" width="16" height="16" />\
    Decrease the ocean speed for 5 second and gain 1 point.
3. Faster Raft:\
   <img src="faster_raft.png" width="16" height="16" />\
    Increase the raft speed for 5 seconds and gain 1 point.
4. Slower Raft:\
   <img src="slower_raft.png" width="16" height="16" />\
    Decrease the raft speed for 5 seconds and gain 5 points.
5. Life:\
   <img src="life.png" width="16" height="16" />\
    Add one life and gain 10 points.
6. Invulnerable potion:\
   <img src="invulnerable_potion.png" width="16" height="16" />\
    Become invulnerable for 5 seconds and gain 3 points.

Controls:

Up arrow: Move the raft up\
Down arrow: Move the raft down\
Left arrow: Move the raft left\
Right arrow: Move the raft right


This game was built with [NEST](NEST.md).

