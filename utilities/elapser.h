#include <QElapsedTimer>

#define MEASURE_ELAPSED_FUNC(intervalMsec, CodeBlock) \
    qint64 intervalMsec;                              \
    {                                                 \
        QElapsedTimer __timer;                        \
        __timer.start();                              \
        CodeBlock                                     \
            intervalMsec = __timer.elapsed();         \
    }