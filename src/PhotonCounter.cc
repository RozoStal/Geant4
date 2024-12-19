#include "PhotonCounter.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include<iostream>

G4bool PhotonCounter::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist)
{
	G4Track *track = aStep->GetTrack();
	if(track->GetParticleDefinition()->GetParticleName() == "opticalphoton")
	{
		count++;
		//G4double energy = track->GetKineticEnergy();
		//photonEnergies.push_back(energy*1000000);  // Сохранение энергии
	}
	track->SetTrackStatus(fStopAndKill);
	return true;
}

void PhotonCounter::EndOfEvent(G4HCofThisEvent*)
{
	auto analysisManager = G4AnalysisManager::Instance();

    // Запись общего числа фотонов
    
    //analysisManager->FillNtupleIColumn(0, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());
    //analysisManager->FillNtupleIColumn(1, count);
    
    analysisManager->FillNtupleIColumn(0, count);
    analysisManager->AddNtupleRow();

    // Сохранение энергий фотонов
  /*  for (const auto& energy : photonEnergies) 
    {
        analysisManager->FillNtupleDColumn(2, energy);
        analysisManager->AddNtupleRow();
    } */
    
	std::cout << "PMT Count: " << count << std::endl;
	count = 0;
	//photonEnergies.clear();
}
