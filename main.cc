#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Shielding.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include <unistd.h>
int main(int argc, char** argv) 
{
    // Создание объекта G4RunManager
    G4RunManager* runManager = new G4RunManager;

    // Установка описания детектора через DetectorConstruction
    runManager->SetUserInitialization(new AntiNeuDet::DetectorConstruction());

    // Установка физического списка Shielding + optics
    G4VModularPhysicsList* physicsList = new Shielding;
    physicsList->ReplacePhysics(new G4EmStandardPhysics_option4);
    G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
    physicsList->RegisterPhysics(opticalPhysics);
    runManager->SetUserInitialization(physicsList);
    
    // ...
    runManager->SetUserInitialization(new ActionInitialization);

    // Инициализация системы визуализации
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
    
    // Запуск UI-менеджера в интерактивном режиме, если файл команд не передан
    G4UIExecutive* ui = nullptr;
    if (argc == 1) 
    {
        ui = new G4UIExecutive(argc, argv);
    }

    // Доступ к менеджеру команд и выполнение начального файла визуализации
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    UImanager->ApplyCommand("/vis/scene/add/logo false"); // Отключение логотипа Geant4
    UImanager->ApplyCommand("/vis/viewer/set/view_parameters None");
    UImanager->ApplyCommand("/vis/viewer/set/annotations false");
	UImanager->ApplyCommand("/vis/viewer/set/logo false");

    if (ui) 
    {
        // Интерактивный режим
        ui->SessionStart();
        delete ui;
    }

    // Очистка ресурсов
    delete visManager;
    delete runManager;

    return 0;
}

