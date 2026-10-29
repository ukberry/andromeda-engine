import andromeda;
import std;

using namespace andromeda;

class MyExampleApp : public Application< Context<GLFWWindowPolicy>> {
public:
 void loop() {
 }
};

int main() {
    andromeda::ResourceManager rm;

    auto resource1 = rm.Load<andromeda::TestResource>("TestingTesting");
    auto resource2 = rm.Load<andromeda::TestResource>("OneTwoOneTwo");

    auto shader1 = rm.Load<andromeda::Shader>("Generic", "shaders/slang.spv");

    {
        auto resource_temp { std::move(resource1) };    
        std::cout << resource_temp->TestMessage() << std::endl;
    }

    std::cout << resource2->TestMessage() << std::endl;

    MyExampleApp app;
    app.run();
}