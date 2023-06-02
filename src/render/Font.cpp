//
// Created by finnw on 13.11.2022.
//

#include <codecvt>
#include "Font.hpp"
#include "Util/Log.hpp"
#include "Texture.hpp"

pmlike::render::Font::Font(std::string &path) : path(path) {
    this->load();
}

pmlike::render::Font::Font(const char *path) : path(std::string(path)) {
    this->load();
}

pmlike::render::Font::Font(std::string &path, bool kernings) : path(path) {
    this->load(kernings);
}

pmlike::render::Font::Font(const char *path, bool kernings) : path(std::string(path)) {
    this->load(kernings);
}

//Copy constructor
pmlike::render::Font::Font(const Font &font) : path(font.path) {
    this->pages = font.pages;
    this->load();
}

pmlike::render::Font::~Font() {
    //Do nothing
}

void pmlike::render::Font::load() {
    this->load(true);
}

void pmlike::render::Font::load(bool enableKernings) {

    FILE *file = fopen(this->path.c_str(), "rb");
    if (file == nullptr) {
        LOG_EF("Failed to open font file: %s", this->path.c_str());
        return;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    uint8_t *buffer = (uint8_t *) std::calloc(size, sizeof(uint8_t));
    fread(buffer, sizeof(uint8_t), size, file);
    fclose(file);

    if (buffer[0] != 'B' || buffer[1] != 'M' || buffer[2] != 'F') {
        LOG_EF("Invalid font file (%c%c%c): %s", buffer[0], buffer[1], buffer[2], this->path.c_str());
        return;
    }

    uint8_t *data = buffer;

    BlockInfo fontInfoBlockInfo{};
    uint8_t *fontInfoBlockData = data + 3 + 1; //(BMF + Version)
    fontInfoBlockInfo.type = *((uint8_t *) (fontInfoBlockData + 0));
    fontInfoBlockInfo.size = *((uint32_t *) (fontInfoBlockData + 1));

    BlockInfo commonBlockInfo{};
    uint8_t *commonBlockInfoData = fontInfoBlockData + (1 + 4) + fontInfoBlockInfo.size;
    commonBlockInfo.type = *((uint8_t *) (commonBlockInfoData + 0));
    commonBlockInfo.size = *((uint32_t *) (commonBlockInfoData + 1));

    BlockInfo pageBlockInfo{};
    uint8_t *pageBlockInfoData = commonBlockInfoData + (1 + 4) + commonBlockInfo.size;
    pageBlockInfo.type = *((uint8_t *) (pageBlockInfoData + 0));
    pageBlockInfo.size = *((uint32_t *) (pageBlockInfoData + 1));

    BlockInfo charBlockInfo{};
    uint8_t *charBlockInfoData = pageBlockInfoData + (1 + 4) + pageBlockInfo.size;
    charBlockInfo.type = *((uint8_t *) (charBlockInfoData + 0));
    charBlockInfo.size = *((uint32_t *) (charBlockInfoData + 1));

    BlockInfo kerningBlockInfo{};
    uint8_t *kerningBlockInfoData = charBlockInfoData + (1 + 4) + charBlockInfo.size;
    kerningBlockInfo.type = *((uint8_t *) (kerningBlockInfoData + 0));
    kerningBlockInfo.size = *((uint32_t *) (kerningBlockInfoData + 1));

    //Loading common info
    this->commonInfo.lineHeight = *((uint16_t *) (commonBlockInfoData + (1 + 4) + 0));
    this->commonInfo.base = *((uint16_t *) (commonBlockInfoData + (1 + 4) + 2));
    this->commonInfo.scaleW = *((uint16_t *) (commonBlockInfoData + (1 + 4) + 4));
    this->commonInfo.scaleH = *((uint16_t *) (commonBlockInfoData + (1 + 4) + 6));
    this->commonInfo.pages = *((uint16_t *) (commonBlockInfoData + (1 + 4) + 8));
    this->commonInfo.bitField = *((uint8_t *) (commonBlockInfoData + (1 + 4) + 10));
    this->commonInfo.alphaChnl = *((uint8_t *) (commonBlockInfoData + (1 + 4) + 11));
    this->commonInfo.redChnl = *((uint8_t *) (commonBlockInfoData + (1 + 4) + 12));
    this->commonInfo.greenChnl = *((uint8_t *) (commonBlockInfoData + (1 + 4) + 13));
    this->commonInfo.blueChnl = *((uint8_t *) (commonBlockInfoData + (1 + 4) + 14));

    //Load all Pages
    if (pageBlockInfo.type == 3) {
        uint8_t *pageData = pageBlockInfoData + (1 + 4);
        for (; pageData < charBlockInfoData;) {
            std::string pageName = std::string((char *) pageData);
            std::shared_ptr<BitmapFontPage> page = std::make_shared<BitmapFontPage>();
            page->name = pageName;
            std::string texturePath = this->path.substr(0, this->path.find_last_of('/')) + "/" + pageName;
            page->texture = Texture::fromFile(texturePath);
            page->texture->setMagFilter(GL_LINEAR);
            page->texture->setMinFilter(GL_LINEAR);
            this->pages.push_back(page);
            pageData += pageName.length() + 1;
        }
    }

    //Read all Chars
    if (charBlockInfo.type == 4) {
        uint32_t charCount = charBlockInfo.size / 20;
        for (int i = 0; i < charCount; i++) {
            uint8_t *charData = charBlockInfoData + (1 + 4) + (i * 20);
            BlockChar blockChar{};
            blockChar.id = *((uint32_t *) (charData + 0));
            blockChar.x = *((uint16_t *) (charData + 4));
            blockChar.y = *((uint16_t *) (charData + 6));
            blockChar.width = *((uint16_t *) (charData + 8));
            blockChar.height = *((uint16_t *) (charData + 10));
            blockChar.xoffset = *((int16_t *) (charData + 12));
            blockChar.yoffset = *((int16_t *) (charData + 14));
            blockChar.xadvance = *((int16_t *) (charData + 16));
            blockChar.page = *((uint8_t *) (charData + 18));
            this->chars[blockChar.id] = blockChar;
        }
    }

    //Load Kerning Pairs
    if (kerningBlockInfo.type == 5 && enableKernings) {
        uint32_t kerningCount = kerningBlockInfo.size / 10;
        for (int i = 0; i < kerningCount; i++) {
            uint8_t *kerningData = kerningBlockInfoData + (1 + 4) + (i * 10);
            BlockKerning kerningPair{};
            kerningPair.first = *((uint32_t *) (kerningData + 0));
            kerningPair.second = *((uint32_t *) (kerningData + 4));
            kerningPair.amount = *((int16_t *) (kerningData + 8));
            this->kernings[kerningPair.first][kerningPair.second] = kerningPair.amount;
        }
    }

    std::free(buffer);

    //Init OpenGL Buffers

    //Setup Shader
    Shader vertexShader = Shader("shader/Font.vert", GL_VERTEX_SHADER, true);
    Shader fragmentShader = Shader("shader/Font.frag", GL_FRAGMENT_SHADER, true);
    this->shaderProgram = std::make_shared<ShaderProgram>(&vertexShader, &fragmentShader);

    glGenVertexArrays(1, &this->vertexArrayObject);
    glGenBuffers(1, &this->vertexBufferObject);
    glGenBuffers(1, &this->indexBufferObject);

    glBindVertexArray(this->vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferObject);

    //Set Vertex Attrib Pointers
    GLint posAttrib = this->shaderProgram->getAttribLocation("aVertex");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

std::vector<uint32_t> pmlike::render::Font::convertText(const char *text) {
    std::vector<uint32_t> strChars;
    for (int i = 0; text[i] != 0;) {
        if ((text[i] & (0b11110000)) == 0b11110000) { //UTF-8 with 4 bytes
            uint32_t first = (uint32_t) (text[i] & 0b00001111) << 18;
            uint32_t second = (uint32_t) (text[i + 1] & 0b00111111) << 12;
            uint32_t third = (uint32_t) (text[i + 2] & 0b00111111) << 6;
            uint32_t fourth = (uint32_t) (text[i + 3] & 0b00111111) << 0;
            uint32_t codePoint = (first) | (second) | (third) | (fourth);
            strChars.push_back(codePoint);
            i += 4;
            continue;
        }
        if ((text[i] & (0b11100000)) == 0b11100000) { //UTF-8 with 3 bytes
            uint32_t first = (uint32_t) (text[i] & 0b00001111) << 12;
            uint32_t second = (uint32_t) (text[i + 1] & 0b00111111) << 6;
            uint32_t third = (uint32_t) (text[i + 2] & 0b00111111) << 0;
            uint32_t codePoint = (first) | (second) | (third);
            strChars.push_back(codePoint);
            i += 3;
            continue;
        }
        if ((text[i] & (0b11000000)) == 0b11000000) { //UTF-8 with 2 bytes
            uint32_t first = (uint32_t) (text[i] & 0b00011111) << 6;
            uint32_t second = (uint32_t) (text[i + 1] & 0b00111111) << 0;
            uint32_t codePoint = first | second;
            strChars.push_back(codePoint);
            i += 2;
            continue;
        }
        //Assume only UTF-8 with 1 byte
        strChars.push_back((uint32_t) text[i]);
        i += 1;
    }
    return strChars;
}

void pmlike::render::Font::renderText(const char *text, int x, int y, int size, int lineSpacing, glm::mat4 projection,
                                      glm::vec4 color) {
    renderText(text, x, y, -1, -1, size, lineSpacing, projection, color);
}

void pmlike::render::Font::renderText(const char *text, int x, int y, int size, int lineSpacing, glm::mat4 projection,
                                      glm::vec4 color, Alignment horizontal, Alignment vertical) {
    std::vector<uint32_t> strChars = convertText(text);
    renderText(strChars, x, y, -1, -1, size, lineSpacing, projection, color, horizontal, vertical);
}

void pmlike::render::Font::renderText(const char *text, int x, int y, int width, int height, int size, int lineSpacing,
                                      glm::mat4 projection, glm::vec4 color) {
    std::vector<uint32_t> strChars = convertText(text);
    renderText(strChars, x, y, width, height, size, lineSpacing, projection, color, BEGIN, BEGIN);
}

void pmlike::render::Font::renderText(const char *text, int x, int y, int width, int height, int size, int lineSpacing,
                                      glm::mat4 projection, glm::vec4 color, Alignment horizontal, Alignment vertical) {
    std::vector<uint32_t> strChars = convertText(text);
    renderText(strChars, x, y, width, height, size, lineSpacing, projection, color, horizontal, vertical);
}

void
pmlike::render::Font::renderText(const char32_t *text, int x, int y, int width, int height, int size, int lineSpacing,
                                 glm::mat4 projection, glm::vec4 color) {
    std::vector<uint32_t> strChars = convertText((char *) text);
    renderText(strChars, x, y, width, height, size, lineSpacing, projection, color, BEGIN, BEGIN);
}

void
pmlike::render::Font::renderText(const char32_t *text, int x, int y, int size, int lineSpacing, glm::mat4 projection,
                                 glm::vec4 color) {
    std::vector<uint32_t> strChars = convertText((char *) text);
    renderText(strChars, x, y, -1, -1, size, lineSpacing, projection, color, BEGIN, BEGIN);
}


void pmlike::render::Font::renderText(std::vector<uint32_t> strChars, int x, int y, int width, int height, int size,
                                      int lineSpacing, glm::mat4 projection, glm::vec4 color, Alignment horizontal,
                                      Alignment vertical) {

    std::map<uint8_t, std::vector<float>> verticesPerPage = std::map<uint8_t, std::vector<float>>();
    std::map<uint8_t, std::vector<GLuint>> indicesPerPage = std::map<uint8_t, std::vector<GLuint>>();
    std::map<uint8_t, GLuint> indexOffsetPerPage = std::map<uint8_t, GLuint>();

    float sizingFactor = (float) size / (float) this->commonInfo.lineHeight;

    std::vector<int> lineWidth = std::vector<int>();
    std::vector<size_t> lineBreaks = std::vector<size_t>();

    lineBreaks.push_back(0);

    //Calculate line Width
    int currentLineWidth = 0;
    int maxLineWidth = 0;
    int widthOnLastSpace = 0;
    int lastSpace = 0;
    for (int i = 0; i < strChars.size(); i++) {
        uint32_t charCode = strChars[i];
        BlockChar blockChar = this->chars[charCode];
        if (charCode == 0x0A || charCode == 0x0D) {
            lineWidth.push_back(currentLineWidth);
            lineBreaks.push_back(i);
            if (currentLineWidth > maxLineWidth) {
                maxLineWidth = currentLineWidth;
            }
            currentLineWidth = 0;
            continue;
        }
        if (charCode == 0x20) {
            lastSpace = i;
            widthOnLastSpace = currentLineWidth;
        }

        //Check for Kerning Pair
        if (i < strChars.size() - 1) {
            uint32_t nextCharCode = strChars[i + 1];
            if (this->kernings[charCode].find(nextCharCode) != this->kernings[charCode].end()) {
                currentLineWidth +=
                        blockChar.xadvance * sizingFactor + this->kernings[charCode][nextCharCode] * sizingFactor;
            } else {
                currentLineWidth += blockChar.xadvance * sizingFactor;
            }
        } else {
            currentLineWidth += blockChar.xadvance * sizingFactor;
        }

        if (width != -1 && currentLineWidth > width) {
            if (lastSpace != 0) {
                lineWidth.push_back(widthOnLastSpace);
                lineBreaks.push_back(lastSpace);
                i = lastSpace;
                if (currentLineWidth > maxLineWidth) {
                    maxLineWidth = currentLineWidth;
                }
                currentLineWidth = 0;
                lastSpace = 0;
                widthOnLastSpace = 0;
            } else {
                lineWidth.push_back(currentLineWidth);
                lineBreaks.push_back(i);
                if (currentLineWidth > maxLineWidth) {
                    maxLineWidth = currentLineWidth;
                }
                currentLineWidth = 0;
            }
        }

    }
    lineWidth.push_back(currentLineWidth);

    int textHeight = lineWidth.size() * (this->commonInfo.lineHeight * sizingFactor) +
                     (std::max(lineWidth.size() - 1, (size_t) 0)) * lineSpacing;

    //Prepare render chars
    int currentY = y;

    switch (vertical) {
        case BEGIN:
            currentY = y;
            break;
        case CENTER:
            if (height == -1) {
                currentY = y - textHeight / 2;
            } else {
                currentY = y + height / 2 - textHeight / 2;
            }
            break;
        case END:
            if (height == -1) {
                currentY = y - textHeight;
            } else {
                currentY = y + height - textHeight;
            }
            break;
    }

    for (int lineNr = 0; lineNr < lineWidth.size(); lineNr++) {

        int currentX = x;

        switch (horizontal) {
            case BEGIN:
                currentX = x;
                break;
            case CENTER:
                if (width == -1) {
                    currentX = x - (maxLineWidth - lineWidth[lineNr]) / 2;
                } else {
                    currentX = x + (width - lineWidth[lineNr]) / 2;
                }
                break;
            case END:
                if (width == -1) {
                    currentX = x - lineWidth[lineNr];
                } else {
                    currentX = x + width - lineWidth[lineNr];
                }
                break;
        }

        for (int64_t i = lineBreaks[lineNr]; i < strChars.size(); i++) {

            uint32_t charCode = strChars[i];
            BlockChar blockChar = this->chars[charCode];

            size_t lineEnd = lineNr >= lineWidth.size() - 1 ? strChars.size() : lineBreaks[lineNr + 1];

            if (i >= lineEnd) {
                break;
            }

            if (charCode == 0x0A || charCode == 0x0D) {
                continue;
            }


            float textureX = blockChar.x;
            float textureY = blockChar.y;
            float sizeX = blockChar.width;
            float sizeY = blockChar.height;

            float x1 = currentX + (blockChar.xoffset * sizingFactor);
            float y1 = currentY + (blockChar.yoffset * sizingFactor);
            float x2 = x1 + (sizeX * sizingFactor);
            float y2 = y1 + (sizeY * sizingFactor);

            float u1 = textureX / this->commonInfo.scaleW;
            float v1 = textureY / this->commonInfo.scaleH;

            float u2 = (textureX + sizeX) / this->commonInfo.scaleW;
            float v2 = (textureY + sizeY) / this->commonInfo.scaleH;

            std::vector<float> vertices = {
                    x1, y1, u1, v1,
                    x2, y1, u2, v1,
                    x2, y2, u2, v2,
                    x1, y2, u1, v2
            };

            if (indexOffsetPerPage.find(blockChar.page) == indexOffsetPerPage.end()) {
                indexOffsetPerPage[blockChar.page] = 0;
                verticesPerPage[blockChar.page] = std::vector<float>();
                indicesPerPage[blockChar.page] = std::vector<GLuint>();
            }

            int indexOffset = indexOffsetPerPage[blockChar.page];

            GLuint index0 = indexOffset++;
            GLuint index1 = indexOffset++;
            GLuint index2 = indexOffset++;
            GLuint index3 = indexOffset++;

            std::vector<GLuint> indices = {
                    index0, index1, index2,
                    index2, index3, index0
            };

            verticesPerPage[blockChar.page].insert(verticesPerPage[blockChar.page].end(), vertices.begin(),
                                                   vertices.end());
            indicesPerPage[blockChar.page].insert(indicesPerPage[blockChar.page].end(), indices.begin(), indices.end());
            indexOffsetPerPage[blockChar.page] = indexOffset;

            //Check for Kerning Pair
            if (i < strChars.size() - 1) {
                uint32_t nextCharCode = strChars[i + 1];
                if (this->kernings[charCode].find(nextCharCode) != this->kernings[charCode].end()) {
                    currentX += (blockChar.xadvance + this->kernings[charCode][nextCharCode]) * sizingFactor;
                } else {
                    currentX += blockChar.xadvance * sizingFactor;
                }
            } else {
                currentX += blockChar.xadvance * sizingFactor;
            }

        }

        currentY += this->commonInfo.lineHeight * sizingFactor + lineSpacing;

    }

    bool blending = glIsEnabled(GL_BLEND);
    GLint blendSrc, blendDst;
    glGetIntegerv(GL_BLEND_SRC, &blendSrc);
    glGetIntegerv(GL_BLEND_DST, &blendDst);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Render chars per page
    glBindVertexArray(this->vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferObject);

    this->shaderProgram->use();
    //this->shaderProgram->setUniformMat4("projection", this->projectionMatrix);
    this->shaderProgram->setUniformMat4("projection", projection);
    this->shaderProgram->setUniform2("offset", glm::vec2(0.0f, 0.0f));

    for (auto it = verticesPerPage.begin(); it != verticesPerPage.end(); it++) {
        std::shared_ptr<BitmapFontPage> page = this->pages[it->first];
        glBufferData(GL_ARRAY_BUFFER, it->second.size() * sizeof(float), it->second.data(), GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesPerPage[it->first].size() * sizeof(GLuint),
                     indicesPerPage[it->first].data(), GL_DYNAMIC_DRAW);

        page->texture->bindToProgram(GL_TEXTURE0, this->shaderProgram.get(), "text");
        this->shaderProgram->setUniform4("textColor", color);

        glDrawElements(GL_TRIANGLES, indicesPerPage[it->first].size(), GL_UNSIGNED_INT, (void *) 0);
    }

    glBindVertexArray(0);

    if (!blending) {
        glDisable(GL_BLEND);
    }
    glBlendFunc(blendSrc, blendDst);
}

void pmlike::render::Font::updateDisplaySize(int width, int height) {
    this->projectionMatrix = glm::ortho(0.0f, (float) width, (float) height, 0.0f);
}

pmlike::render::BitmapFontPage::~BitmapFontPage() {
    delete this->texture;
}