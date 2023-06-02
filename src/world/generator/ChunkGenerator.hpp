//
// Created by finnw on 31.05.2023.
//

#ifndef PM_LIKE_CHUNKGENERATOR_HPP
#define PM_LIKE_CHUNKGENERATOR_HPP

#include <memory>
#include "world/Chunk.hpp"

namespace pmlike::world {

    class ChunkGenerator {
        public:
            ChunkGenerator();
            ~ChunkGenerator();
            virtual void generate(std::shared_ptr<world::Chunk> chunk);
        private:
    };

}

#endif //PM_LIKE_CHUNKGENERATOR_HPP
