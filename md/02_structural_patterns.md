# C++ 디자인 패턴 - 구조 패턴 (Structural Patterns)

---

## Adapter
> 호환되지 않는 인터페이스를 가진 클래스를 기존 인터페이스에 맞게 변환하는 패턴

```cpp
#include <iostream>
#include <string>

// 기존 시스템이 기대하는 인터페이스
class Logger {
public:
    virtual void log(const std::string& message) = 0;
    virtual ~Logger() = default;
};

// 서드파티 라이브러리 (인터페이스가 다름)
class ThirdPartyLogger {
public:
    void writeLog(int level, const std::string& msg) {
        std::cout << "[Level " << level << "] " << msg << "\n";
    }
};

// 어댑터: ThirdPartyLogger를 Logger 인터페이스로 변환
class LoggerAdapter : public Logger {
    ThirdPartyLogger adaptee;
public:
    void log(const std::string& message) override {
        adaptee.writeLog(1, message);  // 기본 레벨 1로 매핑
    }
};

// 활용 예시
int main() {
    LoggerAdapter logger;
    logger.log("서버가 시작되었습니다");
    logger.log("사용자 로그인 성공");
    // [Level 1] 서버가 시작되었습니다
    // [Level 1] 사용자 로그인 성공
}
```

---

## Bridge
> 추상화와 구현을 분리하여 독립적으로 확장할 수 있게 하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>

// 구현 계층
class Renderer {
public:
    virtual void renderCircle(double x, double y, double r) = 0;
    virtual ~Renderer() = default;
};

class OpenGLRenderer : public Renderer {
public:
    void renderCircle(double x, double y, double r) override {
        std::cout << "OpenGL로 원 렌더링 (" << x << "," << y << " r=" << r << ")\n";
    }
};

class VulkanRenderer : public Renderer {
public:
    void renderCircle(double x, double y, double r) override {
        std::cout << "Vulkan으로 원 렌더링 (" << x << "," << y << " r=" << r << ")\n";
    }
};

// 추상화 계층
class Shape {
protected:
    Renderer& renderer;
public:
    Shape(Renderer& r) : renderer(r) {}
    virtual void draw() = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double x, y, radius;
public:
    Circle(Renderer& r, double x, double y, double radius)
        : Shape(r), x(x), y(y), radius(radius) {}
    void draw() override { renderer.renderCircle(x, y, radius); }
};

// 활용 예시: 렌더러를 바꿔도 도형 코드는 그대로
int main() {
    OpenGLRenderer gl;
    VulkanRenderer vk;

    Circle c1(gl, 10, 20, 5);
    Circle c2(vk, 10, 20, 5);

    c1.draw(); // OpenGL로 원 렌더링 (10,20 r=5)
    c2.draw(); // Vulkan으로 원 렌더링 (10,20 r=5)
}
```

---

## Composite
> 개별 객체와 복합 객체를 동일한 인터페이스로 다룰 수 있게 트리 구조를 구성하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class FileSystemNode {
public:
    virtual void print(int indent = 0) const = 0;
    virtual int getSize() const = 0;
    virtual ~FileSystemNode() = default;
};

class File : public FileSystemNode {
    std::string name;
    int size;
public:
    File(const std::string& n, int s) : name(n), size(s) {}
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "📄 " << name << " (" << size << "KB)\n";
    }
    int getSize() const override { return size; }
};

class Directory : public FileSystemNode {
    std::string name;
    std::vector<std::shared_ptr<FileSystemNode>> children;
public:
    Directory(const std::string& n) : name(n) {}
    void add(std::shared_ptr<FileSystemNode> node) { children.push_back(node); }
    int getSize() const override {
        int total = 0;
        for (auto& c : children) total += c->getSize();
        return total;
    }
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "📁 " << name
                  << " (총 " << getSize() << "KB)\n";
        for (auto& c : children) c->print(indent + 2);
    }
};

// 활용 예시
int main() {
    auto root = std::make_shared<Directory>("project");
    auto src  = std::make_shared<Directory>("src");
    src->add(std::make_shared<File>("main.cpp", 15));
    src->add(std::make_shared<File>("utils.cpp", 8));
    root->add(src);
    root->add(std::make_shared<File>("README.md", 3));

    root->print();
    // 📁 project (총 26KB)
    //   📁 src (총 23KB)
    //     📄 main.cpp (15KB)
    //     📄 utils.cpp (8KB)
    //   📄 README.md (3KB)
}
```

---

## Decorator
> 객체에 동적으로 새로운 책임(기능)을 추가하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>

class Coffee {
public:
    virtual std::string description() const = 0;
    virtual int cost() const = 0;
    virtual ~Coffee() = default;
};

class Americano : public Coffee {
public:
    std::string description() const override { return "아메리카노"; }
    int cost() const override { return 4000; }
};

class CoffeeDecorator : public Coffee {
protected:
    std::unique_ptr<Coffee> base;
public:
    CoffeeDecorator(std::unique_ptr<Coffee> b) : base(std::move(b)) {}
};

class MilkDecorator : public CoffeeDecorator {
public:
    using CoffeeDecorator::CoffeeDecorator;
    std::string description() const override { return base->description() + " + 우유"; }
    int cost() const override { return base->cost() + 500; }
};

class ShotDecorator : public CoffeeDecorator {
public:
    using CoffeeDecorator::CoffeeDecorator;
    std::string description() const override { return base->description() + " + 샷추가"; }
    int cost() const override { return base->cost() + 700; }
};

// 활용 예시: 옵션을 자유롭게 조합
int main() {
    std::unique_ptr<Coffee> order = std::make_unique<Americano>();
    order = std::make_unique<MilkDecorator>(std::move(order));
    order = std::make_unique<ShotDecorator>(std::move(order));

    std::cout << order->description() << "\n"; // 아메리카노 + 우유 + 샷추가
    std::cout << order->cost() << "원\n";       // 5200원
}
```

---

## Facade
> 복잡한 서브시스템을 단순한 인터페이스 뒤에 숨기는 패턴

```cpp
#include <iostream>
#include <string>

// 복잡한 서브시스템들
class VideoDecoder {
public:
    void decode(const std::string& file) { std::cout << "비디오 디코딩: " << file << "\n"; }
};
class AudioDecoder {
public:
    void decode(const std::string& file) { std::cout << "오디오 디코딩: " << file << "\n"; }
};
class SubtitleLoader {
public:
    void load(const std::string& file) { std::cout << "자막 로딩: " << file << "\n"; }
};
class Display {
public:
    void render() { std::cout << "화면 렌더링 시작\n"; }
};

// 퍼사드: 하나의 인터페이스로 통합
class MediaPlayer {
    VideoDecoder video;
    AudioDecoder audio;
    SubtitleLoader subtitle;
    Display display;
public:
    void play(const std::string& filename) {
        std::cout << "=== 재생 시작 ===\n";
        video.decode(filename);
        audio.decode(filename);
        subtitle.load(filename);
        display.render();
        std::cout << "=== 재생 중 ===\n";
    }
};

// 활용 예시: 복잡한 내부를 몰라도 한 줄로 사용
int main() {
    MediaPlayer player;
    player.play("movie.mp4");
}
```

---

## Flyweight
> 다수의 유사 객체에서 공유 가능한 데이터를 분리하여 메모리 사용을 최적화하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// Flyweight: 공유되는 불변 데이터
class TreeType {
    std::string name;
    std::string texture;
public:
    TreeType(const std::string& n, const std::string& t) : name(n), texture(t) {
        std::cout << "  [TreeType 생성] " << name << "\n";
    }
    void draw(int x, int y) const {
        std::cout << name << " at (" << x << "," << y << ")\n";
    }
};

// Flyweight Factory: 캐싱
class TreeTypeFactory {
    std::unordered_map<std::string, std::shared_ptr<TreeType>> cache;
public:
    std::shared_ptr<TreeType> getType(const std::string& name, const std::string& texture) {
        std::string key = name + "_" + texture;
        if (cache.find(key) == cache.end()) {
            cache[key] = std::make_shared<TreeType>(name, texture);
        }
        return cache[key];
    }
    size_t typeCount() const { return cache.size(); }
};

// Context: 개별 인스턴스 고유 데이터
struct Tree {
    int x, y;
    std::shared_ptr<TreeType> type;
    void draw() const { type->draw(x, y); }
};

// 활용 예시: 1000그루의 나무지만 타입 객체는 3개만 생성
int main() {
    TreeTypeFactory factory;
    std::vector<Tree> forest;

    for (int i = 0; i < 400; ++i)
        forest.push_back({i, i * 2, factory.getType("소나무", "pine.png")});
    for (int i = 0; i < 350; ++i)
        forest.push_back({i, i * 3, factory.getType("참나무", "oak.png")});
    for (int i = 0; i < 250; ++i)
        forest.push_back({i, i * 4, factory.getType("단풍나무", "maple.png")});

    std::cout << "나무 수: " << forest.size() << "\n";        // 1000
    std::cout << "타입 객체 수: " << factory.typeCount() << "\n"; // 3
}
```

---

## Proxy
> 다른 객체에 대한 접근을 제어하는 대리 객체를 제공하는 패턴

```cpp
#include <iostream>
#include <memory>
#include <string>

class Image {
public:
    virtual void display() = 0;
    virtual ~Image() = default;
};

// 실제 객체: 생성 시 무거운 로딩
class HighResImage : public Image {
    std::string filename;
public:
    HighResImage(const std::string& f) : filename(f) {
        std::cout << "⏳ 고해상도 이미지 로딩 중... " << filename << "\n";
    }
    void display() override {
        std::cout << "🖼️  이미지 표시: " << filename << "\n";
    }
};

// 프록시: 실제 필요할 때까지 로딩 지연 (Lazy Loading)
class ImageProxy : public Image {
    std::string filename;
    std::unique_ptr<HighResImage> realImage;
public:
    ImageProxy(const std::string& f) : filename(f) {
        std::cout << "✅ 프록시 생성 (아직 로딩 안 함): " << filename << "\n";
    }
    void display() override {
        if (!realImage) {
            realImage = std::make_unique<HighResImage>(filename);
        }
        realImage->display();
    }
};

// 활용 예시: 이미지 갤러리에서 보이는 것만 로딩
int main() {
    std::vector<std::unique_ptr<Image>> gallery;
    gallery.push_back(std::make_unique<ImageProxy>("photo1.jpg"));
    gallery.push_back(std::make_unique<ImageProxy>("photo2.jpg"));
    gallery.push_back(std::make_unique<ImageProxy>("photo3.jpg"));

    std::cout << "\n--- 첫 번째만 표시 ---\n";
    gallery[0]->display(); // 이때 로딩됨
    gallery[0]->display(); // 이미 로딩됨, 바로 표시

    // photo2, photo3는 아직 로딩되지 않음
}
```
