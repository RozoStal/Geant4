#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"

void ActionInitialization::Build() const 
{
	SetUserAction(new PrimaryGeneratorAction());
	SetUserAction(new RunAction);
}
