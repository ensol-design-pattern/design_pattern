#include "Proxy.h"

int Proxy() 
{
    // IFC 파일 로딩 시 프록시만 생성 (빠름)
    std::vector<std::unique_ptr<IfcWallProxy>> model;
    model.push_back(std::make_unique<IfcWallProxy>("2O2Fr$t4X7Z", "외벽-W01", "1F"));
    model.push_back(std::make_unique<IfcWallProxy>("3K9Pq$r2A1B", "내벽-W02", "1F"));
    model.push_back(std::make_unique<IfcWallProxy>("1M7Xn$w8C3D", "외벽-W03", "2F"));

    std::cout << "=== 모델 트리 (지오메트리 로딩 없음) ===\n";
    for (auto& wall : model) 
    {
        std::cout << wall->getStorey() << " / " << wall->getName() << "\n";
    }

    std::cout << "\n=== W01만 뷰어에서 클릭 → 렌더링 ===\n";
    model[0]->render();  // 이 시점에 STEP 파싱

    std::cout << "\n=== W01 다시 클릭 ===\n";
    model[0]->render();  // 이미 로딩됨, 파싱 안 함

    return 0;
}