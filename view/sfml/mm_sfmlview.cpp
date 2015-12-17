

#include "yorel/multi_methods.hpp"

using yorel::multi_methods::virtual_;

#include "sfmlview.hpp"

using namespace si;
using namespace si::view;

namespace si {
namespace view {


BEGIN_SPECIALIZATION(_handleEvent, void, SfmlView* m, Event& e) {
	std::cout << &e << " Unhandled Event occured in Model\n";
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_handleEvent, void, SfmlView* m, GameStart& e) {
	std::cout << "SfmlView Init\n";
	m->handle->window.setActive(true);
} END_SPECIALIZATION;

}
}
