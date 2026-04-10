#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

// Observer 인터페이스
class IfcModelObserver 
{
public:
    virtual ~IfcModelObserver() = default;
    virtual void onElementChanged(const std::string& guid, const std::string& property, const std::string& oldVal, const std::string& newVal) = 0;
};

// Subject - IFC 벽 요소
class IfcWall 
{
    std::string guid_;
    std::string name_;
    double thickness_;
    std::string material_;
    std::vector<IfcModelObserver*> observers_;

    void notify(const std::string& prop, const std::string& oldV, const std::string& newV) 
    {
        for (auto* obs : observers_) obs->onElementChanged(guid_, prop, oldV, newV);
    }

public:
    IfcWall(const std::string& guid, const std::string& name, double thickness, const std::string& material)
        : guid_(guid), name_(name), thickness_(thickness), material_(material) {}

    void subscribe(IfcModelObserver* obs) { observers_.push_back(obs); }
    void unsubscribe(IfcModelObserver* obs) 
    {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), obs), observers_.end());
    }

    void setThickness(double t) 
    {
        auto old = std::to_string(thickness_);
        thickness_ = t;
        notify("Thickness", old, std::to_string(t));
    }
    void setMaterial(const std::string& m) 
    {
        auto old = material_;
        material_ = m;
        notify("Material", old, m);
    }

    std::string guid() const { return guid_; }
    double thickness() const { return thickness_; }
};

// Observers

// 1) 물량산출 - 두께 바뀌면 체적 재계산
class QuantityTakeoff : public IfcModelObserver 
{
public:
    void onElementChanged(const std::string& guid, const std::string& prop, const std::string& oldVal, const std::string& newVal) override 
    {
        if (prop == "Thickness") 
        {
            std::cout << "[QTO] " << guid << " 두께 변경 " << oldVal << "→" << newVal << "mm → 물량 재산출\n";
        }
    }
};

// 2) 구조검토 - 두께 변경 시 내력 재검토
class StructuralChecker : public IfcModelObserver 
{
public:
    void onElementChanged(const std::string& guid, const std::string& prop, const std::string& oldVal, const std::string& newVal) override 
    {
        if (prop == "Thickness") 
        {
            double t = std::stod(newVal);
            if (t < 150.0)
                std::cout << "[구조] ⚠ " << guid << " 두께 " << t << "mm < 최소 150mm!\n";
            else
                std::cout << "[구조] ✓ " << guid << " 두께 적합\n";
        }
    }
};

// 3) 변경이력 로그
class ChangeLogger : public IfcModelObserver 
{
public:
    void onElementChanged(const std::string& guid, const std::string& prop, const std::string& oldVal, const std::string& newVal) override 
    {
        std::cout << "[LOG] " << guid << "." << prop << ": \"" << oldVal << "\" → \"" << newVal << "\"\n";
    }
};
