2016-09-28.
Bill Brinson

HeatSourceClass.txt

This is a small sample of a heat transfer program I'm writing. It has more
OOP flaver, makes better use of inheritance than the othe codes in my
GitHub collection.

The full code has everything except nuclear reactions*: 
conduction, natural convection, forced convection, chemical reactions,
thermal radiation. Combustion may or may not be a linear function of
temperature, but it's definitely nonlinear with burn history. We'll burn
that bridge when we get there (sorry--I had no choice. ;^>)
Thermal radiation goes with T^4--highly nonlinear. It's a dynamic
simulation (time dependent). The current problem is really only 2D,
but the project manager wants 3D.

I not going to put the whole code out for all to see any time soon,
probably not ever.

*I could easily include  nuclear reactions, but I'm holding the line
 against requirement creep. Lol