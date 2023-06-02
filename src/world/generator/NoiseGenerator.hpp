//
// Created by finnw on 31.05.2023.
//

#ifndef PM_LIKE_NOISEGENERATOR_HPP
#define PM_LIKE_NOISEGENERATOR_HPP

#include "ChunkGenerator.hpp"

namespace pmlike::world::generator {

    class NoiseGenerator : public ChunkGenerator  {
        public:
            NoiseGenerator();
            ~NoiseGenerator();
            void generate(std::shared_ptr<world::Chunk> chunk) final ;
    };

} // generator

#endif //PM_LIKE_NOISEGENERATOR_HPP
