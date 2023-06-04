//
// Created by finnw on 04.06.2023.
//

#ifndef PM_LIKE_LAUNCHPARAMETER_HPP
#define PM_LIKE_LAUNCHPARAMETER_HPP

namespace pmlike::util {

    class LaunchParameter {
        public:
            static void parse(int argc, char** argv);

            static bool debug;
            static int windowWidth;
            static int windowHeight;
        private:
            LaunchParameter() = default;
            ~LaunchParameter() = default;
    };


}


#endif //PM_LIKE_LAUNCHPARAMETER_HPP
