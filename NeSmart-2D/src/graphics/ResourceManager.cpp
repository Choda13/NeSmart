#include "graphics/ResourceManager.hpp"

#include <stdexcept>
#include <utility>

namespace nesmart::graphics
{
    namespace
    {
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

    Texture& ResourceManager::loadTexture(const std::string& name, const std::string& path)
    {
        return store(m_textures, name, std::make_unique<Texture>(path));
    }

    Shader& ResourceManager::loadShader(const std::string& name, const std::string& vertexPath,
                                        const std::string& fragmentPath)
    {
        return store(m_shaders, name, std::make_unique<Shader>(vertexPath, fragmentPath));
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
