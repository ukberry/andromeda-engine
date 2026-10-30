import andromeda;
import std;

using namespace andromeda;

class MyExampleApp : public Application< Context<GLFWWindowPolicy, VulkanGraphicsPolicy>> {
public:
 void Loop() override {
 }
};

int main() {
    MyExampleApp app;
    app.Run();
}