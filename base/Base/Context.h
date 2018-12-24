//
// Created by 刘轩 on 2018/12/24.
//

#ifndef GPUCAMERAVIDEO_CONTEXT_H
#define GPUCAMERAVIDEO_CONTEXT_H

#include <Looper.h>

namespace GCVBase {

    class Context {
    private:
        Looper * mainLooper = NULL;

    public:
        Context();
        ~Context();
    };
}


#endif //GPUCAMERAVIDEO_CONTEXT_H
