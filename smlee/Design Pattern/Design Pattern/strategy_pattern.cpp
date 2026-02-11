#include <iostream>
#include <vector>
#include <string>

class AnalysisStrategy {
public:
    virtual void analyze() = 0;
    virtual ~AnalysisStrategy() {}
};

class SalesAnalysis : public AnalysisStrategy {
public:
    void analyze() override { std::cout << "Sales KPI analysis done\n"; }
};

class DevAnalysis : public AnalysisStrategy {
public:
    void analyze() override { std::cout << "Development KPI analysis done\n"; }
};

class ReportContext {
private:
    AnalysisStrategy* strategy;
public:
    ReportContext(AnalysisStrategy* s) : strategy(s) {}
    void execute() {
        std::cout << "=== Strategy Pattern ===\n";
        strategy->analyze();
        std::cout << "=======================\n\n";
    }
};

// Client code
void StrategyPattern() {
    std::vector<ReportContext*> contexts;
    contexts.push_back(new ReportContext(new SalesAnalysis()));
    contexts.push_back(new ReportContext(new DevAnalysis()));

    for (auto ctx : contexts) ctx->execute();
    for (auto ctx : contexts) delete ctx;
}
