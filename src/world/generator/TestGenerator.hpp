//
// Created by finnw on 31.05.2023.
//

#ifndef PM_LIKE_TESTGENERATOR_HPP
#define PM_LIKE_TESTGENERATOR_HPP


#include "ChunkGenerator.hpp"

namespace pmlike::world::generator {

    class TestGenerator : public ChunkGenerator {

        public:
            TestGenerator();
            ~TestGenerator();
            void generate(std::shared_ptr<world::Chunk> chunk) override;
    };

}




#endif //PM_LIKE_TESTGENERATOR_HPP
