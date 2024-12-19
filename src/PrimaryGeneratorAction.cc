#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4SystemOfUnits.hh"
#include "G4RandomTools.hh"
#include "Randomize.hh" 
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4Step.hh"
#include<iostream>

PrimaryGeneratorAction::PrimaryGeneratorAction() 
{
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);
    
    // Electron 1 Mev
    /*fParticleGun->SetParticleDefinition(G4Electron::ElectronDefinition());
    fParticleGun->SetParticleEnergy(1. * MeV);*/
    
    // Positron 3 Mev
    fParticleGun->SetParticleDefinition(G4Positron::PositronDefinition());
    fParticleGun->SetParticleEnergy(3. * MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() 
{
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) 
{
	auto analysisManager = G4AnalysisManager::Instance();
	// Настраиваем генератор частиц
	G4double sphereRadius = 0.6203 * m; // Радиус сферы со сцинтиллятором

	//---------------------------------------------------------------Равномерно в шаре-----------------------------------------------------------------
	// Генерация случайной позиции внутри шара
/*	G4double r = sphereRadius * std::cbrt(G4UniformRand()); // Радиус в кубической форме для равномерности
	G4double costheta = 2.0 * G4UniformRand() - 1.0;        // Равномерное распределение cos(theta)
	G4double sintheta = std::sqrt(1.0 - costheta * costheta); // sin(theta)
	G4double phi = 2.0 * CLHEP::pi * G4UniformRand();        // Угол phi

	// Преобразование сферических координат в декартовы
	G4double x = r * sintheta * std::cos(phi);
	G4double y = r * sintheta * std::sin(phi);
	G4double z = r * costheta;                                        */
	//-------------------------------------------------------------------------------------------------------------------------------------------------
	
	//--------------------------------------------------------------------По оси х---------------------------------------------------------------------
	
	/*G4int event_number = event->GetEventID(); // Извлекаем номер события
	G4int Npoints = 1000; // Количество генераций частицы в точке с одним определенным x
	G4int Nsteps = 31; // Количество точек генерации с различными x [Лучше задавать нечетное число точек из-за симеттрии с включением точки х=0]
	G4double step = (sphereRadius*2)/(Nsteps-1); // Шаг точек генерации по оси x
	G4int NbeamOn = Npoints*Nsteps; // Необходимое количесвто генерируемых частиц всего
	G4int interval_number =  event_number/Npoints; // Номер интервала, в котором находится программа
	
	G4double x = - sphereRadius + interval_number*step;
	G4double y = 0.0;
	G4double z = 0.0;
	
	analysisManager->FillNtupleDColumn(1, x);*/
	//-------------------------------------------------------------------------------------------------------------------------------------------------
	
	
		//--------------------------------------------------------------------По оси y---------------------------------------------------------------------
	
	/*G4int event_number = event->GetEventID(); // Извлекаем номер события
	G4int Npoints = 1000; // Количество генераций частицы в точке с одним определенным y
	G4int Nsteps = 31; // Количество точек генерации с различными y [Лучше задавать нечетное число точек из-за симеттрии с включением точки y=0]
	G4double step = (sphereRadius*2)/(Nsteps-1); // Шаг точек генерации по оси y
	G4int NbeamOn = Npoints*Nsteps; // Необходимое количесвто генерируемых частиц всего
	G4int interval_number =  event_number/Npoints; // Номер интервала, в котором находится программа
	
	G4double x = 0.0;
	G4double y = - sphereRadius + interval_number*step;
	G4double z = 0.0;
	
	analysisManager->FillNtupleDColumn(1, y);*/
	//-------------------------------------------------------------------------------------------------------------------------------------------------
	
		//--------------------------------------------------------------------По оси z---------------------------------------------------------------------
	
	G4int event_number = event->GetEventID(); // Извлекаем номер события
	G4int Npoints = 1000; // Количество генераций частицы в точке с одним определенным z
	G4int Nsteps = 31; // Количество точек генерации с различными z [Лучше задавать нечетное число точек из-за симеттрии с включением точки z=0]
	G4double step = (sphereRadius*2)/(Nsteps-1); // Шаг точек генерации по оси z
	G4int NbeamOn = Npoints*Nsteps; // Необходимое количесвто генерируемых частиц всего
	G4int interval_number =  event_number/Npoints; // Номер интервала, в котором находится программа
	
	G4double x = 0.0;
	G4double y = 0.0;
	G4double z = - sphereRadius + interval_number*step;
	
	analysisManager->FillNtupleDColumn(1, z);
	//-------------------------------------------------------------------------------------------------------------------------------------------------
	// Установить позицию частицы
	fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));
	
	// Генерация случайного направления импульса
	G4double dir_costheta = 2.0 * G4UniformRand() - 1.0; // Равномерное распределение cos(theta)
	G4double dir_sintheta = std::sqrt(1.0 - dir_costheta * dir_costheta); // sin(theta)
	G4double dir_phi = 2.0 * CLHEP::pi * G4UniformRand(); // phi

	G4double dir_x = dir_sintheta * std::cos(dir_phi);
	G4double dir_y = dir_sintheta * std::sin(dir_phi);
	G4double dir_z = dir_costheta;

	// Установить направление импульса
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(dir_x, dir_y, dir_z));
    fParticleGun->GeneratePrimaryVertex(event);
}

