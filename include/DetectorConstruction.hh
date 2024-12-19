#ifndef AntiNeuDetDetectorConstruction_h
#define AntiNeuDetDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

namespace AntiNeuDet
{

/// Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction() = default;
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;

    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }
    void ConstructSDandField() override;

  protected:
    G4LogicalVolume* fScoringVolume = nullptr;
};

}  // namespace AntiNeuDet

#endif

