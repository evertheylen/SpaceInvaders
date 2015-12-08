
#include "yorel/multi_methods.hpp"

using yorel::multi_methods::virtual_;

#include "model.hpp"

// TODO remove this!
#include "SFML/Window.hpp"

using namespace si;
using namespace si::model;

namespace si {
namespace model {

/*

MULTI_METHOD(...)

  results in

template<typename Sig> struct handle_Event_specialization;
constexpr ::yorel::methods::detail::method<handle_Event_specialization, void(__VA_ARGS__)> handle_Event;

*/


void Model::handleEvent(Event* e) {
	handle_Event(this, *e);
}

BEGIN_SPECIALIZATION(handle_Event, void, Model* m, Event& e) {
	std::cout << "Unhandled Event occured\n";
	std::cout << "I can access player! yay" << m->player << "\n";
} END_SPECIALIZATION;


}
}
