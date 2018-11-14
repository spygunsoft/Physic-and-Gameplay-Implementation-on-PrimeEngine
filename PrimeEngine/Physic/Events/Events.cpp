#include "PrimeEngine/PrimeEngineIncludes.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "Events.h"

using namespace PE;

namespace PE {
namespace Events {
	
	PE_IMPLEMENT_CLASS1(Event_PRE_PHYSIC, Event);
	PE_IMPLEMENT_CLASS1(Event_POST_PHYSIC, Event);
	PE_IMPLEMENT_CLASS1(Event_UPDATE_PHYSIC, Event);

};
};