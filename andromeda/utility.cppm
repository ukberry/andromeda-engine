export module andromeda:utility;
import std;

export namespace andromeda {
    
    // Resource ID's will be based on the FNV-1a hash of a string
    typedef std::uint64_t resource_id;
    
    class TypeRegistry {
    public:
        template <typename Category, class T>
        static int getTypeID() noexcept {
            static const int id = nextID<Category>();
            return id;
        }
    private:
        template <class Category>
        static inline int nextID() noexcept {
            static int current_id{};
            return current_id++;
        }
    };

    template <typename T>
    struct FNVConstants;

    template <>
    struct FNVConstants<std::uint64_t> {
        static constexpr std::uint64_t offset =  0xCBF29CE48422232ull;
        static constexpr std::uint64_t prime = 0x100000001B3ull;
    };

    template <>
    struct FNVConstants<std::uint32_t> {
        static constexpr std::uint32_t offset = 0x811C9DC5u;
        static constexpr std::uint32_t prime = 0x01000193u;
    };

    template <class T = resource_id>
    struct FNVHashBasic {
        const static T offset { FNVConstants<T>::offset };
        const static T prime { FNVConstants<T>::prime };
        constexpr static T hash(std::string_view str) noexcept {
            T hash {offset};
            for (char c : str) {
                hash ^= static_cast<T>(c);
                hash *= prime;
            }
            return hash;
        }
    };

    using FNVHash = FNVHashBasic<std::uint64_t>;

    inline void assert(bool condition, std::string_view message = "") {
        if(!condition) {
            #ifdef NDEBUG
                std::cerr << "Assertion failed! " << message << std::endl;
                std::terminate();
            #else
                std::unreachable();
            #endif
        }
    }
} // namespace: andromeda

// Custom string literal operator for convenience
export constexpr andromeda::resource_id operator""_fnv(const char* str, std::size_t size) noexcept {
    return andromeda::FNVHash::hash(std::string_view(str,size));
}
