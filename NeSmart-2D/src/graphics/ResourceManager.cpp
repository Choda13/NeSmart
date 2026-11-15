#include "graphics/ResourceManager.hpp"

#include <stdexcept>
#include <utility>

namespace nesmart::graphics
{
    namespace
    {
        constexpr const char* FONT_NAME = "roboto";
        constexpr const char* FONT_PATH = "assets/fonts/roboto/Roboto-Regular.ttf";
        constexpr unsigned int FONT_PIXEL_HEIGHT = 56;

        template <typename T>
        T& store(std::unordered_map<std::string, std::unique_ptr<T>>& map, const std::string& name,
                 std::unique_ptr<T> resource)
        {
            const auto [it, inserted] = map.emplace(name, std::move(resource));
            if (!inserted)
            {
                throw std::runtime_error("ResourceManager: name already in use: " + name);
            }
            return *it->second;
        }

        template <typename T>
        T& find(const std::unordered_map<std::string, std::unique_ptr<T>>& map, const std::string& name)
        {
            const auto it = map.find(name);
            if (it == map.end())
            {
                throw std::runtime_error("ResourceManager: unknown resource: " + name);
            }
            return *it->second;
        }
    } // namespace

    ResourceManager::ResourceManager()
    {
        loadShader("sprite");
        loadShader("solid");
        loadShader("text");

        loadTexture("background");
        loadTexture("circle");
        loadTexture("bus");
        loadTexture("doors_closed");
        loadTexture("doors_opened");
        loadTexture("passengers_hud");
        loadTexture("fines_hud");
        loadTexture("inspection");

        loadFont(FONT_NAME, FONT_PATH, FONT_PIXEL_HEIGHT);
    }

    Texture& ResourceManager::loadTexture(const std::string& name)
    {
        return store(m_textures, name, std::make_unique<Texture>("assets/images/" + name + ".png"));
    }

    Shader& ResourceManager::loadShader(const std::string& name)
    {
        const std::string basePath = "assets/shaders/" + name;
        return store(m_shaders, name, std::make_unique<Shader>(basePath + ".vert", basePath + ".frag"));
    }

    Font& ResourceManager::loadFont(const std::string& name, const std::string& path, unsigned int pixelHeight)
    {
        return store(m_fonts, name, std::make_unique<Font>(path, pixelHeight));
    }

    Texture& ResourceManager::texture(const std::string& name) const
    {
        return find(m_textures, name);
    }

    Shader& ResourceManager::shader(const std::string& name) const
    {
        return find(m_shaders, name);
    }

    Font& ResourceManager::font(const std::string& name) const
    {
        return find(m_fonts, name);
    }

} // namespace nesmart::graphics
