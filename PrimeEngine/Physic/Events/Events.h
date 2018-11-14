#ifndef _PHYSICS_EVENTS_
#define _PHYSICS_EVENTS_

#include "PrimeEngine/Events/StandardEvents.h"

namespace PE {
namespace Events {
	struct Event_PRE_PHYSIC : public Event{

		PE_DECLARE_CLASS(Event_PRE_PHYSIC);

	};

	struct Event_POST_PHYSIC : public Event {

		PE_DECLARE_CLASS(Event_POST_PHYSIC);

	};

	struct Event_UPDATE_PHYSIC : public Event {

		PE_DECLARE_CLASS(Event_UPDATE_PHYSIC);

		PrimitiveTypes::Float32 m_frameTime;

	};
}
}

#endif