//
// Created by finnw on 13.11.2022.
//

#ifndef FORGOTTENENGINE_FONT_HPP
#define FORGOTTENENGINE_FONT_HPP

#include <string>
#include <cstring>
#include <memory>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"

namespace pmlike::render {

    struct BlockInfo {
        uint8_t type;
        uint32_t size;
    };

    struct BlockFontInfo {
        int16_t fontSize;
        uint8_t bitField;
        uint8_t charSet;
        uint16_t stretchH;
        uint8_t aa;
        uint8_t paddingUp;
        uint8_t paddingRight;
        uint8_t paddingDown;
        uint8_t paddingLeft;
        uint8_t spacingHoriz;
        uint8_t spacingVert;
        uint8_t outline;
        std::string fontName;
    };

    struct BlockCommon {
        uint16_t lineHeight;
        uint16_t base;
        uint16_t scaleW;
        uint16_t scaleH;
        uint16_t pages;
        uint8_t bitField;
        uint8_t alphaChnl;
        uint8_t redChnl;
        uint8_t greenChnl;
        uint8_t blueChnl;
    };

    struct BlockPage {
        char *pageNames;
    };

    struct BlockChar {
        uint32_t id;
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        int16_t xoffset;
        int16_t yoffset;
        int16_t xadvance;
        uint8_t page;
        uint8_t chnl;
    };

    struct BlockKerning {
        uint32_t first;
        uint32_t second;
        int16_t amount;
    };

    struct BitmapFontPage {
        ~BitmapFontPage();

        std::string name;
        Texture *texture;
    };

    enum Alignment {
        BEGIN,
        CENTER,
        END
    };

    class Font {
        public:
            Font(const Font &font); //Copy constructor
            explicit Font(std::string &path);

            explicit Font(const char *path);

            explicit Font(std::string &path, bool kerning);

            explicit Font(const char *ptah, bool kerning);

            ~Font();

            void renderText(const char *text, int x, int y, int fontSize, int lineSpacing, glm::mat4 projection,
                            glm::vec4 color);

            void renderText(const char *text, int x, int y, int fontSize, int lineSpacing, glm::mat4 projection,
                            glm::vec4 color, Alignment horizontal, Alignment vertical);

            void renderText(const char *text, int x, int y, int width, int height, int fontSize, int lineSpacing,
                            glm::mat4 projection, glm::vec4 color);

            void renderText(const char *text, int x, int y, int width, int height, int fontSize, int lineSpacing,
                            glm::mat4 projection, glm::vec4 color, Alignment horizontal, Alignment vertical);

            void renderText(const char32_t *text, int x, int y, int fontSize, int lineSpacing, glm::mat4 projection,
                            glm::vec4 color);

            void renderText(const char32_t *text, int x, int y, int width, int height, int fontSize, int lineSpacing,
                            glm::mat4 projection, glm::vec4 color);

            void renderText(std::vector<uint32_t> strChars, int x, int y, int width, int height, int fontSize,
                            int lineSpacing, glm::mat4 projection, glm::vec4 color, Alignment horizontal,
                            Alignment vertical);

            void updateDisplaySize(int width, int height);

        private:
            const std::string path;

            std::vector<std::shared_ptr<BitmapFontPage>> pages;
            std::map<uint32_t, BlockChar> chars;
            std::map<uint32_t, std::map<uint32_t, int16_t>> kernings;

            BlockCommon commonInfo;

            void load(bool useKernings);

            void load();

            static std::vector<uint32_t> convertText(const char *text);


            /* OpenGL stuff */
            GLuint vertexArrayObject;
            GLuint vertexBufferObject;
            GLuint indexBufferObject;

            glm::mat4x4 projectionMatrix;

            std::shared_ptr<ShaderProgram> shaderProgram;

    };

} // Engine

#endif //FORGOTTENENGINE_FONT_HPP