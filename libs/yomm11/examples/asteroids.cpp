// asteroids.cpp
// Copyright (c) 2013 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <yorel/multi_methods.hpp>

#include <iostream>
#include <string>

using namespace std;
using yorel::multi_methods::virtual_;

struct object : yorel::multi_methods::selector {
  MM_CLASS(object);
  object() {
    MM_INIT();
  }
};

struct ship : object {
  MM_CLASS(ship, object);
  ship() {
    MM_INIT();
  }
};

struct asteroid : object {
  MM_CLASS(asteroid, object);
  asteroid() {
    MM_INIT();
  }
};

struct saucer : object {
  MM_CLASS(saucer, object);
  saucer() {
    MM_INIT();
  }
};

struct bullet : object {
  MM_CLASS(bullet, object);
  bullet() {
    MM_INIT();
  }
};

MULTI_METHOD(collide, string, virtual_<object>&, virtual_<object>&, bool swapped);

BEGIN_SPECIALIZATION(collide, string, object& a, object& b, bool swapped) {
  if (swapped) {
    return "kaboom!";
  } else {
    return collide(b, a, true);
  }
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(collide, string, asteroid& a, asteroid& b, bool swapped) {
  return "traverse";
} END_SPECIALIZATION;

int main() {
  yorel::multi_methods::initialize(); // IMPORTANT!

  ship player;
  asteroid as;
  saucer small;
  bullet b;

  cout << collide(player, as, false) << endl; // kaboom!
  cout << collide(as, as, false) << endl; // traverse

  return 0;
}
