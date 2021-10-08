# Designing cbump

In this page, I will talk about how this project `cbump` has come to life.
I wanted to do a 2D platformer in C++. A good collision system was necessary.
I started to search for existing physics library, finding [Chipmunk](https://chipmunk-physics.net/) and [Box2D](https://box2d.org/).
However, both simulates Newtonian physics, which does not fit well with unrealistic games such as platformers.
Moreover, for top-down games, using these libraries to simply check for collisions feels overdone.
I then found a library with the correct level of details for my use case, which is [bump.lua](https://github.com/kikito/bump.lua).
However, it is implemented in Lua. This is why I started to adapt `bump.lua` in C, which I named `cbump`.

`cbump` is a **loose** adaptation of `bump.lua`. This work is based extensively on `bump.lua` so it would be rude not to mention it.
It has fairly similar capacities but its API is modified to adapt to C particularities (memory handling, data structures...).
In Newtonian physics library, you pass numerous elements to the library, you apply forces or acceleration to these elements.
Then, you ask the simulation to be advanced by a given time, the library being responsible for integrating acceleration,
velocity, position and resolving collisions incrementally.
In both `cbump` and `bump.lua`, you create elements, then you move them one by one from a position to another position.
The library is responsible only for collision resolution. It has no concept of time.
This renders the API simpler (at the expense of Newtonian fidelity), and much more freedom to implement custom physics.


