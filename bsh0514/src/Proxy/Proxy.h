#include <iostream>
#include <memory>
#include <string>
#include <vector>

// IFC Entity Intrface
class IfcElement 
{
public:
    virtual ~IfcElement() = default;
    virtual void render() = 0;
    virtual std::string getGeometry() = 0;
};

// Real Subject
class IfcWall : public IfcElement 
{
    std::string guid_;
    std::string geometry_;  // 무거운 B-Rep 데이터
public:
    IfcWall(const std::string& guid) : guid_(guid) 
    {
        // IFC 파일에서 지오메트리 파싱 (비용 큰 작업)
        std::cout << "[IfcWall] STEP 파싱 중... GUID: " << guid_ << "\n";
        geometry_ = "SweptSolid(width=200, height=3000, length=5000)";
    }
    void render() override 
    {
        std::cout << "[IfcWall] 렌더링: " << geometry_ << "\n";
    }
    std::string getGeometry() override { return geometry_; }
};

// Proxy - 속성만 먼저 들고 있고, 지오메트리는 필요할 때 로딩
class IfcWallProxy : public IfcElement 
{
    std::string guid_;
    std::string name_;        // 가벼운 메타데이터는 바로 보유
    std::string storeyName_;
    std::unique_ptr<IfcWall> real_;

    void ensureLoaded() 
    {
        if (!real_) 
        {
            real_ = std::make_unique<IfcWall>(guid_);
        }
    }
public:
    IfcWallProxy(const std::string& guid, const std::string& name, const std::string& storey)
        : guid_(guid), name_(name), storeyName_(storey) {}

    // 메타데이터 조회는 실제 객체 없이 가능
    std::string getName() const { return name_; }
    std::string getStorey() const { return storeyName_; }

    // 지오메트리가 필요한 순간에만 실제 객체 생성
    void render() override 
    {
        ensureLoaded();
        real_->render();
    }
    std::string getGeometry() override 
    {
        ensureLoaded();
        return real_->getGeometry();
    }
};
