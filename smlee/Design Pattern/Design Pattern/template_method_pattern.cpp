#include <iostream>
#include <string>
#include <vector>

// -------------------
// 부모 클래스: 보고서 작성 템플릿
// -------------------
class WeeklyReport {
public:
    // 템플릿 메서드: 보고서 작성 골격
    void createReport() {
        collectData();       // 팀별로 다름 → 추상 단계
        cleanData();         // 공통 로직 → 기본 구현
        analyzeData();       // 공통 로직 → 기본 구현
        generateReport();    // 공통 로직 → 기본 구현
    }

    virtual ~WeeklyReport() {}

protected:
    // 추상 단계 (팀마다 구현)
    virtual void collectData() = 0;

    // 기본 구현 단계 (공통)
    virtual void cleanData() {
        std::cout << "[공통] 데이터 정리 완료\n";
    }

    virtual void analyzeData() {
        std::cout << "[공통] KPI 계산 및 성과 분석 완료\n";
    }

    virtual void generateReport() {
        std::cout << "[공통] 보고서 생성 완료\n";
    }
};

// -------------------
// 영업팀 보고서
// -------------------
class SalesReport : public WeeklyReport {
protected:
    void collectData() override {
        std::cout << "영업팀 데이터 수집: 매출, 신규 계약, 잠재 고객\n";
    }
};

// -------------------
// 개발팀 보고서
// -------------------
class DevReport : public WeeklyReport {
protected:
    void collectData() override {
        std::cout << "개발팀 데이터 수집: 완료된 이슈, 배포된 기능, 버그 수\n";
    }
};

// -------------------
// 클라이언트 코드
// -------------------
void TemplateMethodPattern() {
    std::vector<WeeklyReport*> reports;
    reports.push_back(new SalesReport());
    reports.push_back(new DevReport());

    for (auto report : reports) {
        std::cout << "=== 주간 보고서 작성 시작 ===\n";
        report->createReport();
        std::cout << "=============================\n\n";
    }

    for (auto report : reports) delete report;

    return;
}
