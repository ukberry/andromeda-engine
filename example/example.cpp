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
    rm.Load<andromeda::TestResource>("OneTwoOneTwo");

    auto test = rm.GetResource<andromeda::TestResource>("TestingTesting");
    std::cout << test->TestMessage() << "\n";

    auto test2 = rm.GetResource<andromeda::TestResource>("FakeResource");
    std::cout << "Fake resource handle: " << test2 << std::endl; // Should be (nil)/nullptr

    rm.Release<andromeda::TestResource>("TestingTesting");
    
    auto test3 = rm.GetResource<andromeda::TestResource>("OneTwoOneTwo");
    std::cout << test3->TestMessage() << std::endl;

    // MyExampleApp app;
    // app.run();
}