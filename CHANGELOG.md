# Changelog

## UNRELEASED:

* Added music
* Created Sound Library for sounds
* Added help text for resetting and interacting with objects
* Added main menu
  * Hitting play plays a zoom out "cutscene"
  * Moved level 1 stuff around to better suit the cutscene
  * TODO: add ability to skip cutscenes 

## 12/25/2025:

* Added respawning and resetting objects that were collected in the air
* Made terrain detection more accurate
  * Whether a block at a point is of a certain type now is now dependeng on not only itself, but also its neighbors, if the given point is on an edge
* Terrain is now rendered before entities as a temporary workaround to the depth problem
* Added boosting
* Added levels 2 and 3

## 12/17/2025:

* Levels now load player position on the 2nd line
* Added collectibles:
  * Added interface counter for collectibles
* Portal now has a gray tint if locked
* Portal locked conditions have been added, and can be deserialized/serialized
* Added Antigravity and support for more than just binary blocks
* Added long jump
* Added signs and their serializations
* Layer 1 is now fully playable!
* Turned on Antialiasing
* New background! based off a radial way of generating background stars

## 11/28/2025:

* Fixed some bugs with friction not actually affecting the individual forces
* Moving against the edge of the screen bounces players in the opposite direction, like a pong paddle
* Player movement is now a force, to make it consistent with the aforementioned behavior
* Some player movement tweeks
* Gravity affect radius is now affected by whether object is in freefall or not.
* Gravity is now a constant value
