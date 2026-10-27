import andromeda;
import std;

class MyExampleApp : public andromeda::Application {
public:
 void loop() {
 }
};

int main() {
    andromeda::ResourceManager rm;
    rm.Load<andromeda::TestResource>("TestingTesting");
    auto test = rm.GetResource<andromeda::TestResource>("TestingTesting");
    std::cout << "Test resource message: " << test->TestMessage() << "\n";
    auto test2 = rm.GetResource<andromeda::TestResource>("FakeResource");
    std::cout << "Fake resource handle: " << test2 << std::endl;
    MyExampleApp app;
    app.run();
}