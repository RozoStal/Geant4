#ifndef ACTION_INIT_H
#define ACTION_INIT_H

#include "G4VUserActionInitialization.hh"

class ActionInitialization : public G4VUserActionInitialization
{
public:
	void Build() const override;
};

#endif  // ACTION_INIT_H
