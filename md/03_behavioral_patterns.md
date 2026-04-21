# C++ 디자인 패턴 - 행동 패턴 (Behavioral Patterns)

---

## Chain of Responsibility
> 요청을 처리할 수 있는 핸들러들을 체인으로 연결하여 순서대로 처리 기회를 넘기는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>

class SupportHandler {
protected:
    std::shared_ptr<SupportHandler> next;
public:
    void setNext(std::shared_ptr<SupportHandler> n) { next = n; }
    virtual void handle(const std::string& issue, int severity) {
        if (next) next->handle(issue, severity);
        else std::cout << "처리 불가: " << issue << "\n";
    }
    virtual ~SupportHandler() = default;
};

class FAQ : public SupportHandler {
public:
    void handle(const std::string& issue, int severity) override {
        if (severity <= 1) std::cout << "[FAQ] 자동 응답: " << issue << "\n";
        else SupportHandler::handle(issue, severity);
    }
};

class Engineer : public SupportHandler {
public:
    void handle(const std::string& issue, int severity) override {
        if (severity <= 3) std::cout << "[엔지니어] 기술 지원: " << issue << "\n";
        else SupportHandler::handle(issue, severity);
    }
};

class Manager : public SupportHandler {
public:
    void handle(const std::string& issue, int severity) override {
        std::cout << "[매니저] 직접 대응: " << issue << "\n";
    }
};

// 활용 예시
int main() {
    auto faq = std::make_shared<FAQ>();
    auto eng = std::make_shared<Engineer>();
    auto mgr = std::make_shared<Manager>();
    faq->setNext(eng);
    eng->setNext(mgr);

    faq->handle("비밀번호 초기화", 1);  // FAQ 처리
    faq->handle("API 오류", 3);         // 엔지니어 처리
    faq->handle("서비스 장애", 5);       // 매니저 처리
}
```

---

## Command
> 요청을 객체로 캡슐화하여 매개변수화, 큐잉, 실행취소 등을 가능하게 하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <stack>
#include <string>

class TextEditor {
    std::string content;
public:
    void insert(const std::string& text) { content += text; }
    void removeLast(int n) { content = content.substr(0, content.size() - n); }
    void show() const { std::cout << "내용: [" << content << "]\n"; }
};

class Command {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual ~Command() = default;
};

class InsertCommand : public Command {
    TextEditor& editor;
    std::string text;
public:
    InsertCommand(TextEditor& e, const std::string& t) : editor(e), text(t) {}
    void execute() override { editor.insert(text); }
    void undo() override    { editor.removeLast(text.size()); }
};

class CommandHistory {
    std::stack<std::unique_ptr<Command>> history;
public:
    void execute(std::unique_ptr<Command> cmd) {
        cmd->execute();
        history.push(std::move(cmd));
    }
    void undo() {
        if (!history.empty()) {
            history.top()->undo();
            history.pop();
        }
    }
};

// 활용 예시: 텍스트 편집 + Undo
int main() {
    TextEditor editor;
    CommandHistory history;

    history.execute(std::make_unique<InsertCommand>(editor, "Hello"));
    history.execute(std::make_unique<InsertCommand>(editor, " World"));
    editor.show(); // 내용: [Hello World]

    history.undo();
    editor.show(); // 내용: [Hello]

    history.undo();
    editor.show(); // 내용: []
}
```

---

## Interpreter
> 언어의 문법 규칙을 클래스로 표현하고, 해당 언어의 문장을 해석하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

using Context = std::unordered_map<std::string, int>;

class Expression {
public:
    virtual int interpret(const Context& ctx) = 0;
    virtual ~Expression() = default;
};

class NumberExpr : public Expression {
    int value;
public:
    NumberExpr(int v) : value(v) {}
    int interpret(const Context&) override { return value; }
};

class VariableExpr : public Expression {
    std::string name;
public:
    VariableExpr(const std::string& n) : name(n) {}
    int interpret(const Context& ctx) override { return ctx.at(name); }
};

class AddExpr : public Expression {
    std::unique_ptr<Expression> left, right;
public:
    AddExpr(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left(std::move(l)), right(std::move(r)) {}
    int interpret(const Context& ctx) override {
        return left->interpret(ctx) + right->interpret(ctx);
    }
};

class MultiplyExpr : public Expression {
    std::unique_ptr<Expression> left, right;
public:
    MultiplyExpr(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left(std::move(l)), right(std::move(r)) {}
    int interpret(const Context& ctx) override {
        return left->interpret(ctx) * right->interpret(ctx);
    }
};

// 활용 예시: (x + 3) * y 를 해석
int main() {
    // 식: (x + 3) * y
    auto expr = std::make_unique<MultiplyExpr>(
        std::make_unique<AddExpr>(
            std::make_unique<VariableExpr>("x"),
            std::make_unique<NumberExpr>(3)
        ),
        std::make_unique<VariableExpr>("y")
    );

    Context ctx = {{"x", 5}, {"y", 4}};
    std::cout << "(x + 3) * y = " << expr->interpret(ctx) << "\n"; // 32
}
```

---

## Iterator
> 컬렉션의 내부 구조를 노출하지 않고 요소를 순차적으로 접근할 수 있게 하는 패턴

```cpp
#include <iostream>
#include <string>
#include <vector>

template<typename T>
class Iterator {
public:
    virtual bool hasNext() = 0;
    virtual T next() = 0;
    virtual ~Iterator() = default;
};

class Playlist {
    std::vector<std::string> songs;
public:
    void add(const std::string& song) { songs.push_back(song); }
    size_t size() const { return songs.size(); }
    const std::string& at(size_t i) const { return songs[i]; }

    // 순방향 이터레이터
    class ForwardIterator : public Iterator<std::string> {
        const Playlist& playlist;
        size_t index = 0;
    public:
        ForwardIterator(const Playlist& p) : playlist(p) {}
        bool hasNext() override { return index < playlist.size(); }
        std::string next() override { return playlist.at(index++); }
    };

    // 역방향 이터레이터
    class ReverseIterator : public Iterator<std::string> {
        const Playlist& playlist;
        int index;
    public:
        ReverseIterator(const Playlist& p) : playlist(p), index(p.size() - 1) {}
        bool hasNext() override { return index >= 0; }
        std::string next() override { return playlist.at(index--); }
    };

    ForwardIterator forwardIterator() const { return ForwardIterator(*this); }
    ReverseIterator reverseIterator() const { return ReverseIterator(*this); }
};

// 활용 예시
int main() {
    Playlist pl;
    pl.add("Bohemian Rhapsody");
    pl.add("Hotel California");
    pl.add("Stairway to Heaven");

    std::cout << "--- 정방향 ---\n";
    auto fwd = pl.forwardIterator();
    while (fwd.hasNext()) std::cout << fwd.next() << "\n";

    std::cout << "--- 역방향 ---\n";
    auto rev = pl.reverseIterator();
    while (rev.hasNext()) std::cout << rev.next() << "\n";
}
```

---

## Mediator
> 객체들 간의 직접 통신을 제거하고, 중재자를 통해 상호작용하게 하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class User;

class ChatRoom {
    std::vector<User*> users;
public:
    void join(User* user) { users.push_back(user); }
    void broadcast(const std::string& msg, User* sender);
};

class User {
    std::string name;
    ChatRoom& room;
public:
    User(const std::string& n, ChatRoom& r) : name(n), room(r) { room.join(this); }
    const std::string& getName() const { return name; }
    void send(const std::string& msg) {
        std::cout << name << " 전송: " << msg << "\n";
        room.broadcast(msg, this);
    }
    void receive(const std::string& msg, const std::string& from) {
        std::cout << "  " << name << " 수신 [" << from << "]: " << msg << "\n";
    }
};

void ChatRoom::broadcast(const std::string& msg, User* sender) {
    for (auto* u : users) {
        if (u != sender) u->receive(msg, sender->getName());
    }
}

// 활용 예시: 유저들은 서로 직접 참조 없이 채팅
int main() {
    ChatRoom room;
    User alice("Alice", room);
    User bob("Bob", room);
    User charlie("Charlie", room);

    alice.send("안녕하세요!");
    bob.send("반갑습니다!");
}
```

---

## Memento
> 객체의 내부 상태를 저장하고, 필요 시 이전 상태로 복원할 수 있게 하는 패턴

```cpp
#include <iostream>
#include <string>
#include <vector>

class EditorMemento {
    std::string content;
    friend class Editor;
    EditorMemento(const std::string& c) : content(c) {}
public:
    std::string getSummary() const {
        return content.substr(0, 20) + (content.size() > 20 ? "..." : "");
    }
};

class Editor {
    std::string content;
public:
    void type(const std::string& text) { content += text; }
    void show() const { std::cout << "내용: [" << content << "]\n"; }
    EditorMemento save() const { return EditorMemento(content); }
    void restore(const EditorMemento& m) { content = m.content; }
};

class History {
    std::vector<EditorMemento> snapshots;
public:
    void push(const EditorMemento& m) { snapshots.push_back(m); }
    EditorMemento pop() {
        auto last = snapshots.back();
        snapshots.pop_back();
        return last;
    }
    bool empty() const { return snapshots.empty(); }
};

// 활용 예시: 에디터 상태 저장/복원 (Ctrl+Z)
int main() {
    Editor editor;
    History history;

    editor.type("첫 번째 문장. ");
    history.push(editor.save());

    editor.type("두 번째 문장. ");
    history.push(editor.save());

    editor.type("세 번째 문장.");
    editor.show(); // 전체 내용

    // Undo 2회
    editor.restore(history.pop());
    editor.show(); // 두 번째까지

    editor.restore(history.pop());
    editor.show(); // 첫 번째까지
}
```

---

## Observer
> 객체 상태 변화 시 등록된 관찰자들에게 자동으로 알림을 보내는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Observer {
public:
    virtual void update(const std::string& event, double data) = 0;
    virtual ~Observer() = default;
};

class StockMarket {
    std::vector<Observer*> observers;
    std::string stockName;
    double price = 0;
public:
    StockMarket(const std::string& name) : stockName(name) {}
    void subscribe(Observer* obs) { observers.push_back(obs); }
    void setPrice(double p) {
        price = p;
        for (auto* obs : observers) obs->update(stockName, price);
    }
};

class PriceDisplay : public Observer {
    std::string name;
public:
    PriceDisplay(const std::string& n) : name(n) {}
    void update(const std::string& event, double data) override {
        std::cout << "[" << name << "] " << event << " 현재가: " << data << "원\n";
    }
};

class AlertSystem : public Observer {
    double threshold;
public:
    AlertSystem(double t) : threshold(t) {}
    void update(const std::string& event, double data) override {
        if (data > threshold)
            std::cout << "🚨 경고! " << event << " 가격 " << data << "원 (기준: " << threshold << "원 초과)\n";
    }
};

// 활용 예시
int main() {
    StockMarket samsung("삼성전자");

    PriceDisplay screen("메인화면");
    PriceDisplay mobile("모바일");
    AlertSystem alert(80000);

    samsung.subscribe(&screen);
    samsung.subscribe(&mobile);
    samsung.subscribe(&alert);

    samsung.setPrice(75000);  // 디스플레이만 반응
    samsung.setPrice(82000);  // 디스플레이 + 경고 발생
}
```

---

## State
> 객체의 내부 상태에 따라 행동을 변경하는 패턴 (상태 전이를 명시적으로 표현)

```cpp
#include <iostream>
#include <memory>

class VendingMachine;

class State {
public:
    virtual void insertCoin(VendingMachine& vm) = 0;
    virtual void pressButton(VendingMachine& vm) = 0;
    virtual ~State() = default;
};

class VendingMachine {
    std::unique_ptr<State> state;
public:
    VendingMachine();
    void setState(std::unique_ptr<State> s) { state = std::move(s); }
    void insertCoin() { state->insertCoin(*this); }
    void pressButton() { state->pressButton(*this); }
};

class IdleState : public State {
public:
    void insertCoin(VendingMachine& vm) override;
    void pressButton(VendingMachine&) override {
        std::cout << "동전을 먼저 넣어주세요\n";
    }
};

class HasCoinState : public State {
public:
    void insertCoin(VendingMachine&) override {
        std::cout << "이미 동전이 있습니다\n";
    }
    void pressButton(VendingMachine& vm) override {
        std::cout << "🥤 음료 배출!\n";
        vm.setState(std::make_unique<IdleState>());
    }
};

VendingMachine::VendingMachine() : state(std::make_unique<IdleState>()) {}
void IdleState::insertCoin(VendingMachine& vm) {
    std::cout << "동전 투입 완료\n";
    vm.setState(std::make_unique<HasCoinState>());
}

// 활용 예시
int main() {
    VendingMachine vm;
    vm.pressButton();  // 동전을 먼저 넣어주세요
    vm.insertCoin();   // 동전 투입 완료
    vm.insertCoin();   // 이미 동전이 있습니다
    vm.pressButton();  // 🥤 음료 배출!
    vm.pressButton();  // 동전을 먼저 넣어주세요
}
```

---

## Strategy
> 알고리즘을 캡슐화하여 런타임에 교체할 수 있게 하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

class SortStrategy {
public:
    virtual void sort(std::vector<int>& data) = 0;
    virtual ~SortStrategy() = default;
};

class BubbleSort : public SortStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "버블 정렬 수행 (소규모 데이터)\n";
        for (size_t i = 0; i < data.size(); ++i)
            for (size_t j = 0; j + 1 < data.size() - i; ++j)
                if (data[j] > data[j + 1]) std::swap(data[j], data[j + 1]);
    }
};

class QuickSort : public SortStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "퀵 정렬 수행 (대규모 데이터)\n";
        std::sort(data.begin(), data.end());
    }
};

class Sorter {
    std::unique_ptr<SortStrategy> strategy;
public:
    void setStrategy(std::unique_ptr<SortStrategy> s) { strategy = std::move(s); }
    void sort(std::vector<int>& data) { strategy->sort(data); }
};

// 활용 예시: 데이터 크기에 따라 전략 교체
int main() {
    Sorter sorter;
    std::vector<int> data = {5, 2, 8, 1, 9, 3};

    if (data.size() < 100)
        sorter.setStrategy(std::make_unique<BubbleSort>());
    else
        sorter.setStrategy(std::make_unique<QuickSort>());

    sorter.sort(data);
    for (int x : data) std::cout << x << " "; // 1 2 3 5 8 9
    std::cout << "\n";
}
```

---

## Template Method
> 알고리즘의 뼈대를 상위 클래스에서 정의하고, 세부 단계를 하위 클래스에서 구현하게 하는 패턴

```cpp
#include <iostream>
#include <string>

class DataExporter {
public:
    // 템플릿 메서드: 알고리즘 뼈대
    void exportData() {
        openConnection();
        fetchData();
        processData();
        writeOutput();
        closeConnection();
    }
    virtual ~DataExporter() = default;
protected:
    virtual void openConnection() = 0;
    virtual void fetchData() = 0;
    virtual void processData() { std::cout << "기본 데이터 가공\n"; } // 선택적 오버라이드
    virtual void writeOutput() = 0;
    virtual void closeConnection() { std::cout << "연결 종료\n"; }
};

class CSVExporter : public DataExporter {
protected:
    void openConnection() override { std::cout << "DB 연결\n"; }
    void fetchData() override      { std::cout << "테이블 데이터 조회\n"; }
    void writeOutput() override    { std::cout << "CSV 파일 생성 완료\n"; }
};

class APIExporter : public DataExporter {
protected:
    void openConnection() override { std::cout << "REST API 연결\n"; }
    void fetchData() override      { std::cout << "API 응답 수신\n"; }
    void processData() override    { std::cout << "JSON 파싱 및 변환\n"; }
    void writeOutput() override    { std::cout << "JSON 파일 생성 완료\n"; }
};

// 활용 예시
int main() {
    std::cout << "=== CSV 내보내기 ===\n";
    CSVExporter csv;
    csv.exportData();

    std::cout << "\n=== API 내보내기 ===\n";
    APIExporter api;
    api.exportData();
}
```

---

## Visitor
> 객체 구조를 변경하지 않고 새로운 연산을 추가할 수 있게 하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Circle;
class Rectangle;

class ShapeVisitor {
public:
    virtual void visit(Circle& c) = 0;
    virtual void visit(Rectangle& r) = 0;
    virtual ~ShapeVisitor() = default;
};

class Shape {
public:
    virtual void accept(ShapeVisitor& v) = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
public:
    double radius;
    Circle(double r) : radius(r) {}
    void accept(ShapeVisitor& v) override { v.visit(*this); }
};

class Rectangle : public Shape {
public:
    double width, height;
    Rectangle(double w, double h) : width(w), height(h) {}
    void accept(ShapeVisitor& v) override { v.visit(*this); }
};

// 새 연산 1: 면적 계산
class AreaCalculator : public ShapeVisitor {
public:
    void visit(Circle& c) override {
        std::cout << "원 면적: " << 3.14159 * c.radius * c.radius << "\n";
    }
    void visit(Rectangle& r) override {
        std::cout << "사각형 면적: " << r.width * r.height << "\n";
    }
};

// 새 연산 2: JSON 직렬화 (구조 변경 없이 추가)
class JsonSerializer : public ShapeVisitor {
public:
    void visit(Circle& c) override {
        std::cout << R"({"type":"circle","radius":)" << c.radius << "}\n";
    }
    void visit(Rectangle& r) override {
        std::cout << R"({"type":"rect","w":)" << r.width << R"(,"h":)" << r.height << "}\n";
    }
};

// 활용 예시
int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5));
    shapes.push_back(std::make_unique<Rectangle>(4, 6));

    AreaCalculator area;
    JsonSerializer json;

    std::cout << "=== 면적 ===\n";
    for (auto& s : shapes) s->accept(area);

    std::cout << "=== JSON ===\n";
    for (auto& s : shapes) s->accept(json);
}
```
