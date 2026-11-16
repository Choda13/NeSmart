#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "graphics/Font.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace nesmart::graphics
{
    class ResourceManager
    {
    public:
        ResourceManager();

        Texture& texture(const std::string& name) const;
        Shader& shader(const std::string& name) const;
        Font& font(const std::string& name) const;

    private:
        Texture& loadTexture(const std::string& name);
        Shader& loadShader(const std::string& name);
        Font& loadFont(const std::string& name, const std::string& path, unsigned int pixelHeight);

        std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
        std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
        std::unordered_map<std::string, std::unique_ptr<Font>> m_fonts;
    };

} // namespace nesmart::graphics
