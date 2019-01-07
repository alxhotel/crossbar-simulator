#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

class Subscriber {

public:
	
	virtual void notified() = 0;
	
	virtual void notified_resize() = 0;
	
};

#endif /* SUBSCRIBER_H */

