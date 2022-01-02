#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <cstdlib>

namespace proj4 {

    class MmaClient;

    class ArrayList{
        private:
        friend class MmaClient;
        size_t size;
        MmaClient* mmc;
        int array_id;
        ArrayList(size_t, MmaClient*, int);
        ~ArrayList();
        public:
        // you should not modify the public interfaces used in tests
        int Read (unsigned long);
        void Write (unsigned long, int);
        int Get_array_id();
    };

} // namespce: proj4
#endif