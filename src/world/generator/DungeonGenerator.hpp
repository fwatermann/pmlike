//
// Created by finnw on 31.05.2023.
//

#ifndef PM_LIKE_NOISEGENERATOR_HPP
#define PM_LIKE_NOISEGENERATOR_HPP

#include "ChunkGenerator.hpp"
#include "FastNoise/FastNoise.h"

namespace pmlike::world::generator {

    class DungeonGenerator : public ChunkGenerator {
        public:
            explicit DungeonGenerator(int seed);
            DungeonGenerator();
            ~DungeonGenerator();

            void generate(std::shared_ptr<world::Chunk> chunk) final;

            FastNoise::SmartNode<FastNoise::Generator> noise;

            int getSeed();

    private:

            int seed;

            inline bool between(float value, float min, float max) {
                return value >= min && value <= max;
            }


    };

} // generator

#endif //PM_LIKE_NOISEGENERATOR_HPP
