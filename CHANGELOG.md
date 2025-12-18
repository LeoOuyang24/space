# Changelog

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
