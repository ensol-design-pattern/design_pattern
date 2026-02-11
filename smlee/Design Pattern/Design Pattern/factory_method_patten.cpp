#include <iostream>
#include <vector>
#include <string>

class Report {
public:
    virtual void write() = 0;
    virtual ~Report() {}
};

class SalesReportF : public Report {
public:
    void write() override { std::cout << "Sales report generated\n"; }
};

class DevReportF : public Report {
public:
    void write() override { std::cout << "Development report generated\n"; }
};

// Factory interface
class ReportFactory {
public:
    virtual Report* createReport() = 0;
    virtual ~ReportFactory() {}
};

class SalesReportFactory : public ReportFactory {
public:
    Report* createReport() override { return new SalesReportF(); }
};

class DevReportFactory : public ReportFactory {
public:
    Report* createReport() override { return new DevReportF(); }
};

// Client code
void FactoryMethodPattern() {
    std::vector<ReportFactory*> factories;
    factories.push_back(new SalesReportFactory());
    factories.push_back(new DevReportFactory());

    for (auto factory : factories) {
        Report* r = factory->createReport();
        std::cout << "=== Factory Method ===\n";
        r->write();
        std::cout << "=====================\n\n";
        delete r;
    }

    for (auto f : factories) delete f;
}
