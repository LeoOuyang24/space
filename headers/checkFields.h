#ifndef CHECKFUNCTIONS_H_INCLUDED
#define CHECKFUNCTIONS_H_INCLUDED

#include "blackMagic.h"

CHECK_FOR(collideWith);
CHECK_FOR(ObjectName);
CHECK_FOR(dead);
CHECK_FOR(interactWith)
CHECK_FOR(key);

//macro that makes my getter functions. life is too short brah
#define make_getter(field,type) \
type get_##field() { return field; }


#endif // CHECKFUNCTIONS_H_INCLUDED
