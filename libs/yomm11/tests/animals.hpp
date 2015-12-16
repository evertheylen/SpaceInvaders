// animals.hpp
// Copyright (c) 2013 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

struct Animal : yorel::multi_methods::selector {
  MM_CLASS(Animal);
  Animal() {
    MM_INIT();
  }
};

struct Herbivore : Animal {
  MM_CLASS(Herbivore, Animal);
  Herbivore() {
    MM_INIT();
  }
};

struct Carnivore : Animal {
  MM_CLASS(Carnivore, Animal);
  Carnivore() {
    MM_INIT();
  }
};

struct Cow : Herbivore {
  MM_CLASS(Cow, Herbivore);
  Cow() {
    MM_INIT();
  }
};

struct Wolf : Carnivore {
  MM_CLASS(Wolf, Carnivore);
  Wolf() {
    MM_INIT();
  }
};

struct Tiger : Carnivore {
  MM_CLASS(Tiger, Carnivore);
  Tiger() {
    MM_INIT();
  }
};

struct Interface : selector {
  MM_CLASS(Interface);
  Interface() {
    MM_INIT();
  }
};

struct Terminal : Interface {
  MM_CLASS(Terminal, Interface);
  Terminal() {
    MM_INIT();
  }
};

struct Window : Interface {
  MM_CLASS(Window, Interface);
  Window() {
    MM_INIT();
  }
};
