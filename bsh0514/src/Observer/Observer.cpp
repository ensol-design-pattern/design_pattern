#include "Observer.h"

int Observer() 
{
    IfcWall wall("2O2Fr$t4X7Z", "외벽-W01", 200.0, "콘크리트");

    QuantityTakeoff  qto;
    StructuralChecker strCheck;
    ChangeLogger     logger;

    wall.subscribe(&qto);
    wall.subscribe(&strCheck);
    wall.subscribe(&logger);

    std::cout << "=== 두께 변경: 200 → 250mm ===\n";
    wall.setThickness(250.0);

    std::cout << "\n=== 두께 변경: 250 → 120mm ===\n";
    wall.setThickness(120.0);

    std::cout << "\n=== 재료 변경 ===\n";
    wall.setMaterial("경량콘크리트");

    return 0;
}