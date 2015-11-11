
# World

A world contains Entities, and that's about it. It structures the world, but it'll probably be a bunch of pointers to entities.

All entities have (x,y)-coordinates, ordened like this:

    y ^            
      |            
      | .(2,5)     
      |            
      |            
      |      .(7,2)
      |             
      +------------>
                   x    

x and y are integers, as such the world is discrete.

I will give the description about what kind of world we'll try to emulate here (mainly based on the original Space Invaders):

- Different kinds of Aliens (Big, Medium, Small), each throwing their respective kind of Bomb.
- There is also a special kind of alien, with a higher score when you hit it.
- Shields may be extended to be fancy (own collision/tile system), but for now a simple entity suffices.
- The Player shoots Bullets.
- The world has a certain set of things it needs to update on a tick. (Bomb/bullet movements). It also stores some stuff on it's own (like the slightly more complex movement of Aliens)


