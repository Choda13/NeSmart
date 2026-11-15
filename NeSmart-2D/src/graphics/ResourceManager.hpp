#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "graphics/Font.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace nesmart::graphics
{
    // Owns textures, shaders and fonts under string names. Each load* call creates the resource once; the returned
    // reference (and later lookups) stay valid until the manager is destroyed, so users can keep it.
    // Needs a current GL context, and must be destroyed before it. That is why it is a normal member, not a static.
    // Throws std::runtime_error if a name is already taken, a name is unknown, or the resource cannot be loaded.
    class ResourceManager
    {
    public:
        Texture& loadTexture(const std::string& name, const std::string& path);
        Shader& loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
        Font& loadFont(const std::string& name, const std::string& path, unsigned int pixelHeight);

        Texture& texture(const std::string& name) const;
        Shader& shader(const std::string& name) const;
        Font& font(const std::string& name) const;

    private:
        std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
        std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
        std::unordered_map<std::string, std::unique_ptr<Font>> m_fonts;
    };

} // namespace nesmart::graphics
