
#include "yorel/multi_methods.hpp"

using yorel::multi_methods::virtual_;

#include "model.hpp"


using namespace si;
using namespace si::model;

namespace si {
namespace model {


void Model::handleEvent(Event* e) {
	_handleEvent(this, *e);
}


BEGIN_SPECIALIZATION(_handleEvent, void, Model* m, SetMovement& e) {
	e.entity->mov.dir = e.dir;
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handleEvent, void, Model* m, Event& e) {
	std::cout << "Unhandled Event occured in Model\n";
} END_SPECIALIZATION;


}
}
