
# Model

The model contains all the actual logic. It is made up of different parts. Multiple Controllers can send messages aka Events to it, so Game has a queue of sorts in place to catch the messages and pass them to the Model one by one, so there is no need to make the parts of the Model capable of concurrency.

A Model contains multiple (or one) level(s) (read from XML). A World is a way of storing exactly one level, containing all the Entities.

A Model initializes with an XML File and the parent Game. A Model can be send messages from any Controller, and may notify the parent Game of changes. The parent Game will then distribute the messages (again, those are just Events) to all the registered Views.

## Concrete methods

	handleEvent(Event* e)
	
	
