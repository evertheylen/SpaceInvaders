
# Game

Game is the 'central' class. Each Game contains exactly one Model, but can **register** multiple Views or Controllers.

A Game also has a stopwatch, generating Ticks (which are Events). The stopwatch can be started and stopped through Game directly.

A Game also facilitates all the communication between Models, Views and Controllers. This way, you can easily log all the communication, among other advantages. Where it is needed, Game has to provide some Queue to remove possible concurrency problems that may arise when using multiple controlles.

One thing Game does **NOT** do, is facilitating the communication between a View/Controller and whatever extra class they may need (and share with a Controller/View, respectively). For example, a WebView and WebController may both use the same websocket, but Game has no business knowing about this communication.

Initialize a Game with an XML file, which it will pass on to the model.


## Concrete methods

	notifyViews(Event* e)
	
	notifyModel(Event* e)
	
	[un]registerView(View* v)
	
	[un]registerController(Controller* c)
	// I see no use in this yet
	
	const Model& getModel()
	// I see no use in this yet
	
	start()
	stop()
	



