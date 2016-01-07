
# Event

Events are the way of communication for a lot of the things in this game. There are a lot of different types:

- Player started ones:
  - Move (with a direction)
  - Shoot
  - Tick
- Events that may 'happen' as a result of the above
  - Collision
  - Death
  - Win
  - Lose
  - ...

These events will have dispatchers for them in Model. Each event has a dispatcher, like `collide`.
  
Some of these, like Collision, might not actually need a class. Within a model, Events are not instantiated, they are instantly executed through the corresponding dispatcher. If needed, an Event is created, for communication with the Views/Game.

Events have RTTI, because they may be queued and we need to remember their type. The hierarchy of Events is flat (no (public) inheritance). This allows for the following efficient technique of dispatching: each event has a function `getType()`, which returns some value from the enum `EventTypes`. A simple `case` switch is all you need.

