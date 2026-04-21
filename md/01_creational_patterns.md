# C++ 디자인 패턴 - 생성 패턴 (Creational Patterns)

---

## Singleton
> 클래스의 인스턴스를 하나만 생성하고, 전역 접근점을 제공하는 패턴

```cpp
#include <iostream>
#include <string>

class DatabaseConnection {
private:
    static DatabaseConnection* instance;
    std::string connectionString;
    DatabaseConnection(const std::string& connStr) : connectionString(connStr) {}
public:
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    static DatabaseConnection* getInstance(const std::string& connStr = "") {
        if (!instance) {
            instance = new DatabaseConnection(connStr);
        }
        return instance;
    }
    void query(const std::string& sql) {
        std::cout << "[" << connectionString << "] 쿼리 실행: " << sql << "\n";
    }
};
DatabaseConnection* DatabaseConnection::instance = nullptr;

// 활용 예시
int main() {
    auto* db = DatabaseConnection::getInstance("mysql://localhost:3306/mydb");
    db->query("SELECT * FROM users");

    // 어디서든 같은 인스턴스
    auto* db2 = DatabaseConnection::getInstance();
    db2->query("INSERT INTO logs VALUES('login')");

    std::cout << std::boolalpha << (db == db2) << "\n"; // true
}
```

---

## Factory Method
> 객체 생성을 서브클래스에 위임하여, 생성할 구체 클래스를 런타임에 결정하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>

// 제품 인터페이스
class Document {
public:
    virtual void open() = 0;
    virtual void save() = 0;
    virtual ~Document() = default;
};

class PDFDocument : public Document {
public:
    void open() override { std::cout << "PDF 문서 열기\n"; }
    void save() override { std::cout << "PDF 문서 저장 (.pdf)\n"; }
};

class WordDocument : public Document {
public:
    void open() override { std::cout << "Word 문서 열기\n"; }
    void save() override { std::cout << "Word 문서 저장 (.docx)\n"; }
};

// 팩토리 인터페이스
class DocumentCreator {
public:
    virtual std::unique_ptr<Document> createDocument() = 0;
    virtual ~DocumentCreator() = default;

    void processDocument() {
        auto doc = createDocument();
        doc->open();
        doc->save();
    }
};

class PDFCreator : public DocumentCreator {
public:
    std::unique_ptr<Document> createDocument() override {
        return std::make_unique<PDFDocument>();
    }
};

class WordCreator : public DocumentCreator {
public:
    std::unique_ptr<Document> createDocument() override {
        return std::make_unique<WordDocument>();
    }
};

// 활용 예시
int main() {
    std::unique_ptr<DocumentCreator> creator;

    std::string type = "pdf";
    if (type == "pdf")
        creator = std::make_unique<PDFCreator>();
    else
        creator = std::make_unique<WordCreator>();

    creator->processDocument();
    // 출력: PDF 문서 열기 / PDF 문서 저장 (.pdf)
}
```

---

## Abstract Factory
> 관련 객체들의 군(family)을 구체 클래스 지정 없이 생성할 수 있는 인터페이스를 제공하는 패턴

```cpp
#include <iostream>
#include <memory>

// 제품군 인터페이스
class Button {
public:
    virtual void render() = 0;
    virtual ~Button() = default;
};
class TextBox {
public:
    virtual void render() = 0;
    virtual ~TextBox() = default;
};

// Windows 제품군
class WinButton : public Button {
public:
    void render() override { std::cout << "[Windows] 버튼 렌더링\n"; }
};
class WinTextBox : public TextBox {
public:
    void render() override { std::cout << "[Windows] 텍스트박스 렌더링\n"; }
};

// Mac 제품군
class MacButton : public Button {
public:
    void render() override { std::cout << "[Mac] 버튼 렌더링\n"; }
};
class MacTextBox : public TextBox {
public:
    void render() override { std::cout << "[Mac] 텍스트박스 렌더링\n"; }
};

// 추상 팩토리
class GUIFactory {
public:
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<TextBox> createTextBox() = 0;
    virtual ~GUIFactory() = default;
};

class WinFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override { return std::make_unique<WinButton>(); }
    std::unique_ptr<TextBox> createTextBox() override { return std::make_unique<WinTextBox>(); }
};

class MacFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override { return std::make_unique<MacButton>(); }
    std::unique_ptr<TextBox> createTextBox() override { return std::make_unique<MacTextBox>(); }
};

// 활용 예시: OS에 따라 팩토리만 바꾸면 전체 UI가 변경됨
int main() {
    std::unique_ptr<GUIFactory> factory;

    std::string os = "mac";
    if (os == "windows")
        factory = std::make_unique<WinFactory>();
    else
        factory = std::make_unique<MacFactory>();

    auto btn = factory->createButton();
    auto txt = factory->createTextBox();
    btn->render();  // [Mac] 버튼 렌더링
    txt->render();  // [Mac] 텍스트박스 렌더링
}
```

---

## Builder
> 복잡한 객체의 생성 과정과 표현을 분리하여, 같은 절차로 다양한 객체를 만들 수 있는 패턴

```cpp
#include <iostream>
#include <string>

class QueryBuilder {
    std::string table;
    std::string conditions;
    std::string order;
    int limitCount = -1;
public:
    QueryBuilder& from(const std::string& t) { table = t; return *this; }
    QueryBuilder& where(const std::string& cond) { conditions = cond; return *this; }
    QueryBuilder& orderBy(const std::string& col) { order = col; return *this; }
    QueryBuilder& limit(int n) { limitCount = n; return *this; }

    std::string build() const {
        std::string sql = "SELECT * FROM " + table;
        if (!conditions.empty()) sql += " WHERE " + conditions;
        if (!order.empty())      sql += " ORDER BY " + order;
        if (limitCount > 0)      sql += " LIMIT " + std::to_string(limitCount);
        return sql + ";";
    }
};

// 활용 예시: SQL 쿼리를 체이닝으로 조립
int main() {
    std::string query = QueryBuilder()
        .from("employees")
        .where("department = 'Engineering'")
        .orderBy("hire_date DESC")
        .limit(10)
        .build();

    std::cout << query << "\n";
    // SELECT * FROM employees WHERE department = 'Engineering' ORDER BY hire_date DESC LIMIT 10;
}
```

---

## Prototype
> 기존 객체를 복제(clone)하여 새 객체를 생성하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

class Shape {
public:
    std::string color;
    virtual std::unique_ptr<Shape> clone() const = 0;
    virtual void draw() const = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r, const std::string& c) : radius(r) { color = c; }
    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Circle>(*this);
    }
    void draw() const override {
        std::cout << color << " 원 (r=" << radius << ")\n";
    }
};

class Rectangle : public Shape {
    double w, h;
public:
    Rectangle(double w, double h, const std::string& c) : w(w), h(h) { color = c; }
    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Rectangle>(*this);
    }
    void draw() const override {
        std::cout << color << " 사각형 (" << w << "x" << h << ")\n";
    }
};

// 활용 예시: 프로토타입 레지스트리로 템플릿 도형 복제
int main() {
    // 원본 등록
    std::unordered_map<std::string, std::unique_ptr<Shape>> registry;
    registry["red_circle"] = std::make_unique<Circle>(5.0, "빨강");
    registry["blue_rect"] = std::make_unique<Rectangle>(10, 20, "파랑");

    // 복제해서 사용
    auto shape1 = registry["red_circle"]->clone();
    auto shape2 = registry["blue_rect"]->clone();
    shape2->color = "초록"; // 복제 후 수정 가능

    shape1->draw(); // 빨강 원 (r=5)
    shape2->draw(); // 초록 사각형 (10x20)
}
```
