README
======

README and code base best viewed on [Github](https://github.com/mikenorthorp/OpenGL_SolarSystem)

This is assignment #2 for my OpenGL Animation class, written in C.

It is a Solar System simulation that shows all of the planets rotating around the sun. As well as stars in the distance and a
space ship of the enterprise. The sun has a firery corona and there is also a shield that pops up around the enterprise. A klingon fleet
also attacks when you press a certain button.

Requirements
------------

This program requires `C`, `freeGlut`, and `GLEW`.

This program is made to run in Visual Studio 2012, and you will have to configure the
freeglut libraries and linkers when running this on your own copy most likely.

A basic tutorial on how to get set up is found here [Setup](http://www.wyzant.com/resources/blogs/226103/opengl_freeglut_and_visual_studio_2012)

Installation
------------

1. Open the project in Visual studio, or grab the Planets.c file and set it up in
   your own environment.
2. Ensure you have `freeGlut`, and `GLEW` installed properly. As well as all includes and static libraries linked

Making the Program Do Things
----------------------------

1. Run the Planets.exe found in the debug folder or run it in visual studio as a project. Or use the source code in the Planets
   folder called Planets.c
2. View the output to console to see the various keys that control the program.
	- Scene Controls

	r: toggle orbit rings

	s: toggle stars

	c: toggle the sun's corona

	k: toggle shields

	b: toggle bonus klingon attack

	- Camera Controls

	Up    Arrow: move up

	Down  Arrow: move down

	Right Arrow: move right

	Left  Arrow: move left

	PAGE  UP   : forward

	PAGE  DOWN : backward

3. To exit the program just hit the exit button in the corner.

Bonus
-----

The bonus for this assignment is a klingon fleet attack from the distance. The klingons travle to the enterprise, lay in wait,
and then cloak (dissapear). This repeates until it is turned off with the bonus button.

The suns corona is also in full 3d, and the enterprise tilts when you move it around the universe.


Citations
=========
Lecture notes used. Code examples in class used in some parts of program, but modifed.




