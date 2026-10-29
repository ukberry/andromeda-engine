export module andromeda:shaders;
import :resources;

import vulkan;

export namespace andromeda {

class Shader final : public Resource {
public:
    explicit Shader(resource_id id, std::string_view Filename) : filename(Filename)  {
        this->resourceId = id;
    }

    Shader(Shader&&) = default;
    Shader& operator=(Shader&&) = default;

    ~Shader() {
        std::cout << "Shader resource " << filename << " freed. ";
        std::cout << "The type id for this class is " << TypeRegistry::getTypeID<ResourceCategory,Shader>() << "\n";
    }

    bool Load() { 
        if (loaded) return true;

        std::ifstream source{filename, std::ios::binary | std::ios::in | std::ios::end};
        if (!source.is_open()) {
            std::cerr << "Could not open file: " << filename << "\n";
            return false;
        }
        const auto filesize {static_cast<std::size_t>(source.tellg())};
        std::cout << std::format("Loading shader - filesize: {} bytes\n", filesize);

        std::vector<char> buffer(filesize);
        source.seekg(0, std::ios::beg);
        source.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        source.close();

        vk::ShaderModuleCreateInfo createInfo {
            .codeSize = buffer.size() * sizeof(char),
            .pCode = reinterpret_cast<const std::uint32_t*>(buffer.data())
        };

        (void)createInfo;
        // TODO - Pass device
        shaderModule = vk::raii::ShaderModule{nullptr};

        loaded = true;
        return loaded;
    }
    void Unload() {
        if (!loaded) return;
    }
private:
    std::string filename{};
    vk::raii::ShaderModule shaderModule{nullptr};
};

} // namespace: andromeda
