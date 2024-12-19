#ifndef PHOTON_COUNTER_HH
#define PHOTON_COUNTER_HH

#include "G4VSensitiveDetector.hh"

class PhotonCounter : public G4VSensitiveDetector
{
	protected:
		G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override;
	public:
		void EndOfEvent(G4HCofThisEvent*) override;
		PhotonCounter(G4String name) : G4VSensitiveDetector(name){};
	private:
		G4int count{0};
		std::vector<G4double> photonEnergies;
};

#endif // PHOTON_COUNTER_HH
