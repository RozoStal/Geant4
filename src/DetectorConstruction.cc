#include "DetectorConstruction.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4Orb.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4OpticalSurface.hh"
#include "G4MaterialPropertiesTable.hh"
#include <cmath>
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4UnionSolid.hh"
#include "G4RotationMatrix.hh"
#include <vector>
#include "PhotonCounter.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"



std::vector<G4ThreeVector> GetDodecahedronVertices(G4double a) 
{
	const G4double phi = (1.0 + std::sqrt(5.0)) / 2.0; // Золотое сечение

	std::vector<G4ThreeVector> vertices;

	// Первая группа: (±1, ±1, ±1)
	for (G4double x : {1, -1}) 
	{
		for (G4double y : {1, -1}) 
		{
			for (G4double z : {1, -1}) 
			{
				vertices.emplace_back(a*x/std::sqrt(3), a*y/std::sqrt(3), a*z/std::sqrt(3));
			}
		}
	}

	// Вторая группа: (0, ±phi, ±1/phi)
	for (G4double y : {phi, -phi}) 
	{
		for (G4double z : {1/phi, -1/phi}) 
		{
			vertices.emplace_back(0, a*y/std::sqrt(3), a*z/std::sqrt(3));
		}
	}

	// Третья группа: (±1/phi, 0, ±phi)
	for (G4double x : {1/phi, -1/phi}) 
	{
		for (G4double z : {phi, -phi}) 
		{
			vertices.emplace_back(a*x/std::sqrt(3), 0, a*z/std::sqrt(3));
		}
	}

	// Четвёртая группа: (±phi, ±1/phi, 0)
	for (G4double x : {phi, -phi}) 
	{
		for (G4double y : {1/phi, -1/phi}) 
		{
			vertices.emplace_back(a*x/std::sqrt(3), a*y/std::sqrt(3), 0);
		}
	}
	return vertices;
}

G4RotationMatrix* GetRotationMatrix(G4ThreeVector pmtPosition, G4ThreeVector center = G4ThreeVector(0, 0, 0))
{
	G4ThreeVector direction = center - pmtPosition;
	direction = direction.unit();
	G4double angle = std::acos(direction.dot(G4ThreeVector(0, 0, 1)));
	G4ThreeVector axis = direction.cross(G4ThreeVector(0, 0, 1));
	axis = axis.unit();
	G4RotationMatrix* rotation = new G4RotationMatrix();
	rotation->rotate(angle, axis);
	return rotation;
}

namespace AntiNeuDet
{
	G4VPhysicalVolume* DetectorConstruction::Construct()
	{
		G4NistManager* nist = G4NistManager::Instance();
		
		G4int z; // z=mean number of protons;
		G4double a; // mass of a mole;
		G4int natoms;
		G4double fractionmass;
		
		// Примесь гадолиния
		G4Isotope* Gd152 = new G4Isotope("Gd152", 64, 152, 152.0*g/mole);
	  	G4Isotope* Gd154 = new G4Isotope("Gd154", 64, 154, 154.0*g/mole);
	  	G4Isotope* Gd155 = new G4Isotope("Gd155", 64, 155, 155.0*g/mole);
	  	G4Isotope* Gd156 = new G4Isotope("Gd156", 64, 156, 156.0*g/mole);
	  	G4Isotope* Gd157 = new G4Isotope("Gd157", 64, 157, 157.0*g/mole);
	  	G4Isotope* Gd158 = new G4Isotope("Gd158", 64, 158, 158.0*g/mole);
	  	G4Isotope* Gd160 = new G4Isotope("Gd160", 64, 160, 160.0*g/mole);
	  	
	  	G4Element* Gd = new G4Element("Gadolinium","Gd",7);
	  	Gd->AddIsotope(Gd152,  0.2*perCent);
	  	Gd->AddIsotope(Gd154,  2.2*perCent);
	  	Gd->AddIsotope(Gd155, 14.9*perCent);   //beware: it is abundance, not fractionMass
	  	Gd->AddIsotope(Gd156, 20.6*perCent);   
	  	Gd->AddIsotope(Gd157, 15.7*perCent);
	  	Gd->AddIsotope(Gd158, 24.7*perCent);
	  	Gd->AddIsotope(Gd160, 21.7*perCent);

		// Опции проверки перекрытия
		G4bool checkOverlaps = true;

		// Параметры сцинтиллятора
		std::vector<G4double> photonEnergy = {3.20 * eV, 3.30 * eV, 3.40 * eV};
		std::vector<G4double> scint_rind = {1.51, 1.52, 1.53};
		std::vector<G4double> scint_frac = {0.1, 0.8, 0.1};
		G4double LAB_density = 0.853 * g / cm3;
		G4double scint_radius = 0.6203 * m;
		G4double scint_yield = 0.28 * 5000. / MeV;
		std::vector<G4double> scint_abs_length = {5. * m, 5. * m, 5. * m};
		std::vector<G4double> pmma_abs_length = {9. * m, 9. * m, 9. * m};
		G4double Gd_density = 0.5 * g / liter;
		G4double PPO_density = 3 * g / liter;
		G4double Scint_density = LAB_density + Gd_density + PPO_density;

		// Параметры ПММА оболочки
		G4double pmma_thickness = 10 * mm;

		// Параметры внешнего бака
		G4double tank_diameter = 1858 * mm;
		G4double tank_height = 1858 * mm;
		G4double tank_thickness = 2 * mm;
		std::vector<G4double> LAB_abs_length = {12. * m, 12. * m, 12. * m};

		// Параметры мира
		G4double world_size = 10 * m;
		G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

		// Создание объема мира
		auto solidWorld = new G4Box("World", 0.5 * world_size, 0.5 * world_size, 0.5 * world_size);
		auto logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
		auto physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, checkOverlaps);

		// Создание LAB сцинтиллятора (C_6 H_5 C H Cn H*2n+1), n=10
		G4Element* H = new G4Element("Hydrogen", "H", z=1 , a=1.01*g/mole);
		G4Element* C = new G4Element("Carboneum", "C", z=6 , a=12.01*g/mole);
		G4Element* O = new G4Element("Oxygen", "O", z=8, a=15.999*g/mole);
		G4Element* N = new G4Element("Nitrogen", "N", z=7, a=14.01*g/mole);
		G4Element* B = new G4Element("Boron", "B", z=5, a=10.81*g/mole);
		G4Element* Si = new G4Element("Silicon","Si", z=14, a=28.09*g/mole);
		
		G4Material* LAB = new G4Material("LAB", LAB_density, 2);
		LAB->AddElement(C, natoms=17);
		LAB->AddElement(H, natoms=27);
		
		G4Material* fGd = new G4Material("fGd", Gd_density, 1);
		fGd->AddElement(Gd, natoms=1);
		
		G4Material* PPO = new G4Material("PPO", PPO_density, 4);
		PPO->AddElement(C, natoms=15);
		PPO->AddElement(H, natoms=11);
		PPO->AddElement(N, natoms=1);
		PPO->AddElement(O, natoms=1);
		
		G4Material* Scint = new G4Material("Scint", Scint_density, 3);
		Scint->AddMaterial(LAB, fractionmass = (LAB_density*1000)/(LAB_density*1000 + Gd_density + PPO_density));
		Scint->AddMaterial(fGd, fractionmass = Gd_density/(LAB_density*1000 + Gd_density + PPO_density));
		Scint->AddMaterial(PPO, fractionmass = PPO_density/(LAB_density*1000 + Gd_density + PPO_density));

		// Добавление свойств сцинтиллятора
		G4MaterialPropertiesTable* labMPT = new G4MaterialPropertiesTable();
		labMPT->AddConstProperty("SCINTILLATIONYIELD", scint_yield);
		labMPT->AddProperty("ABSLENGTH", photonEnergy, scint_abs_length);
		labMPT->AddProperty("RINDEX", photonEnergy, scint_rind);
		labMPT->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, scint_frac);
		labMPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
		labMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 0.0 * ns);                           
		Scint->SetMaterialPropertiesTable(labMPT);

		// Создание сферы со сцинтиллятором
		auto solidScintSphere = new G4Orb("ScintSphere", scint_radius);
		auto logicScintSphere = new G4LogicalVolume(solidScintSphere, Scint, "ScintSphere");
		new G4PVPlacement(nullptr, G4ThreeVector(), logicScintSphere, "ScintSphere", logicWorld, false, 0, checkOverlaps);

		// Создание ПММА оболочки вокруг сцинтиллятора как полой сферы
		G4Material* PMMA = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
		G4MaterialPropertiesTable* PPMAProp = new G4MaterialPropertiesTable();
		PPMAProp->AddProperty("ABSLENGTH", photonEnergy, pmma_abs_length);
		PPMAProp->AddProperty("RINDEX", photonEnergy, scint_rind);                              
		PMMA->SetMaterialPropertiesTable(PPMAProp);
		G4double pmma_outer_radius = scint_radius + pmma_thickness;

		// Внутренняя сфера для сцинтиллятора и полая оболочка для ПММА
		auto solidOuterPMMASphere = new G4Orb("OuterPMMAShell", pmma_outer_radius);
		auto solidInnerScintSphere = new G4Orb("InnerPMMAShell", scint_radius);
		auto solidPMMAShell = new G4SubtractionSolid("PMMAShell", solidOuterPMMASphere, solidInnerScintSphere);

		auto logicPMMAShell = new G4LogicalVolume(solidPMMAShell, PMMA, "PMMAShell");
		new G4PVPlacement(nullptr, G4ThreeVector(), logicPMMAShell, "PMMAShell", logicWorld, false, 0, checkOverlaps);


		// Создание LAB без добавок для цилиндрического бака (C_6 H_5 C H Cn H*2n+1), n=10
		G4Material* LAB_no_additives = new G4Material("LAB_no_additives", LAB_density, 2);
		LAB_no_additives->AddElement(C, natoms=17);
		LAB_no_additives->AddElement(H, natoms=27);

		// Добавление свойств для LAB (без сцинтилляции)
		G4MaterialPropertiesTable* labPureMPT = new G4MaterialPropertiesTable();
		labPureMPT->AddProperty("ABSLENGTH", photonEnergy, LAB_abs_length);
		labPureMPT->AddProperty("RINDEX", photonEnergy, scint_rind);                             
		LAB_no_additives->SetMaterialPropertiesTable(labPureMPT);

		// Создание цилиндрического бака
		auto solidOuterTank = new G4Tubs("OuterTank", 0, (0.5*tank_diameter) + tank_thickness, (0.5*tank_height) + tank_thickness, 0.0 * deg, 360.0 * deg);
		auto solidInnerTankFull = new G4Tubs("InnerTankFull", 0, 0.5 * tank_diameter, 0.5 * tank_height, 0.0 * deg, 360.0 * deg);
		auto solidOuterShell = new G4SubtractionSolid("OuterShell", solidOuterTank, solidInnerTankFull);
		
		auto solidInnerSphere = new G4Orb("InnerSphere", pmma_outer_radius);
		auto solidInnerTank = new G4SubtractionSolid("InnerTank", solidInnerTankFull, solidInnerSphere);
		
		auto logicInnerTank = new G4LogicalVolume(solidInnerTank, LAB_no_additives, "InnerTank");
		auto logicOuterShell = new G4LogicalVolume(solidOuterShell, nist->FindOrBuildMaterial("G4_STAINLESS-STEEL"), "OuterShell");
		
		new G4PVPlacement(nullptr, G4ThreeVector(), logicOuterShell, "OuterShell", logicWorld, false, 0, checkOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(), logicInnerTank, "InnerTank", logicWorld, false, 0, checkOverlaps);
		
		//Настройка цветов и прозрачности элементов модели
		auto outerShellVisAttr = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.3)); // Красный цвет с прозрачностью 0.3
		outerShellVisAttr->SetForceSolid(true);
		logicOuterShell->SetVisAttributes(outerShellVisAttr);

		auto innerTankVisAttr = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.3)); // Синий цвет с прозрачностью 0.3
		innerTankVisAttr->SetForceSolid(true);
		logicInnerTank->SetVisAttributes(innerTankVisAttr);
		
		auto ScintSphereVisAttr = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 1)); // Зеленый цвет с прозрачностью 1
		ScintSphereVisAttr->SetForceSolid(true);
		logicScintSphere->SetVisAttributes(ScintSphereVisAttr);
		
		// Добавляем систему из ФЭУ.................................................................................
		
		// Создаем материал боросиликатного стекла
		G4Material* borosilicateGlass = new G4Material("BorosilicateGlass", 2.23*g/cm3, 2);
		borosilicateGlass->AddElement(B, natoms=1);
		borosilicateGlass->AddElement(Si, natoms=1);
		
		G4MaterialPropertiesTable* GlassProp = new G4MaterialPropertiesTable();
		GlassProp->AddProperty("ABSLENGTH", photonEnergy, LAB_abs_length);
		GlassProp->AddProperty("RINDEX", photonEnergy, scint_rind);
		borosilicateGlass->SetMaterialPropertiesTable(GlassProp);
		
		// Размеры для ФЭУ
		G4double WindowRadius = 101*mm;  // Радиус полусферы
		G4double WindowSensRadius = 95*mm;  // Радиус чувствительной части полусферы
		G4double PMTRadius = 131*mm; // Радиус сферы для окна ФЭУ
		G4double theta = std::acos(WindowRadius/PMTRadius); // Угол, начиная с которого формируется окно ФЭУ
		G4double RPMTTube1 = 42.25*mm; // Радиус большего цилиндра
		G4double RPMTHeight1 = 89*mm; // Высота большего цилиндра
		G4double RPMTTube2 = 25.6*mm; // Радиус меньшего цилиндра
		G4double RPMTHeight2 = 30*mm; // Высота меньшего цилиндра

		// Создаем ФЭУ
		auto pmtWindowSolid = new G4Sphere("PMTWindow", 0, WindowRadius, 0, CLHEP::pi*2, 0, CLHEP::pi/2-theta);
		auto pmtTube1 = new G4Tubs("PMTTube1", 0, RPMTTube1, 0.5*RPMTHeight1, 0.0 * deg, 360.0 * deg);
		auto pmtTube2 = new G4Tubs("PMTTube2", 0, RPMTTube2, 0.5*RPMTHeight2, 0.0 * deg, 360.0 * deg);
		
		G4ThreeVector pmtTube1Position(0, 0, -RPMTHeight1*0.5);
		G4ThreeVector pmtTube2Position(0, 0, -RPMTHeight1-RPMTHeight2*0.5);
		
		G4UnionSolid* Solid1 = new G4UnionSolid("Solid1", pmtWindowSolid, pmtTube1, 0, pmtTube1Position);
		G4UnionSolid* PMTSoild = new G4UnionSolid("PMTSoild", Solid1, pmtTube2, 0, pmtTube2Position);
		auto PMTLogical = new G4LogicalVolume(PMTSoild, borosilicateGlass, "PMTLogical");
		
		// Помещаем 20 ФЭУ в вершины додекаэдра
		
		G4double shell_add_radius = 0.15*m; // Расстояние от границы оболочки сцинтиллятора и середины ФЭУ
		G4double dod_radius = pmma_outer_radius + shell_add_radius; // Радиус сферы, описанной вокруг додекаэдра
		auto dod_vert = GetDodecahedronVertices(dod_radius); // Координаты вершин додекаэдра
		for(int i=0; i<20; i++)
		{
			new G4PVPlacement(GetRotationMatrix(dod_vert[i]), dod_vert[i], PMTLogical, "PMT", logicInnerTank, false, 0, true);
		}

		return physWorld;
	}
	
	void DetectorConstruction::ConstructSDandField()
	{
		PhotonCounter* SD = new PhotonCounter("PhotonCounter");
		G4SDManager::GetSDMpointer()->AddNewDetector(SD);
		G4LogicalVolumeStore::GetInstance()->GetVolume("PMTLogical")->SetSensitiveDetector(SD);
	}
}  // namespace AntiNeuDet
