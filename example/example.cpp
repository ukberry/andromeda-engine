import andromeda;
import std;

class MyExampleApp : public andromeda::Application {
public:
 void loop() {
 }
};

int main() {
    andromeda::ResourceManager rm;

    auto resource1 = rm.Load<andromeda::TestResource>("TestingTesting");

    {
        auto resource2 = rm.Load<andromeda::TestResource>("OneTwoOneTwo");
        std::cout << resource2->TestMessage() << std::endl;
    }

    std::cout << resource1->TestMessage() << std::endl;

    // MyExampleApp app;
    // app.run();
}