#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include<iostream>

RunAction::RunAction()
{
	auto analysisManager = G4AnalysisManager::Instance();
  
  	// Настраиваем структуру данных
	analysisManager->CreateNtuple("PhotonData", "Photon Count and Energies");
    //analysisManager->CreateNtupleIColumn("EventID");  // ID события
    analysisManager->CreateNtupleIColumn("PhotonCount"); // Число фотонов
    //analysisManager->CreateNtupleDColumn("Energy");   // Энергия фотона
    analysisManager->CreateNtupleDColumn("Coordinate"); // Координата генерации частицы (x, y или z)
    analysisManager->FinishNtuple();
}

void RunAction::BeginOfRunAction(const G4Run*) 
{
    auto analysisManager = G4AnalysisManager::Instance();
    G4String fileName = "../3_MeV_Positron_ZAxis.csv"; // Название выходного файла с данными
    analysisManager->OpenFile(fileName);
}

void RunAction::EndOfRunAction(const G4Run*) 
{
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}

