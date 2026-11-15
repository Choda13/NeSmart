#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "graphics/Font.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace nesmart::graphics
{
    // Owns textures, shaders and fonts under string names. All assets are loaded on construction; nobody else can load
    // or add resources. The references returned by the lookups stay valid until the manager is destroyed, so users can
    // keep them.
    // Needs a current GL context, and must be destroyed before it. That is why it is a normal member, not a static.
    // Throws std::runtime_error if a resource cannot be loaded, or if a looked-up name is unknown.
    class ResourceManager
    {
    public:
        ResourceManager();

        Texture& texture(const std::string& name) const;
        Shader& shader(const std::string& name) const;
        Font& font(const std::string& name) const;

    private:
        // Loads assets/images/<name>.png.
        Texture& loadTexture(const std::string& name);
        // Loads assets/shaders/<name>.vert and <name>.frag.
        Shader& loadShader(const std::string& name);
        Font& loadFont(const std::string& name, const std::string& path, unsigned int pixelHeight);

        std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
        std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
        std::unordered_map<std::string, std::unique_ptr<Font>> m_fonts;
    };

} // namespace nesmart::graphics
