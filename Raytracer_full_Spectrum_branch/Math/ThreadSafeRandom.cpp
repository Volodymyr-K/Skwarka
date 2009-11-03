#include "ThreadSafeRandom.h"

/**
* Global thread-safe random generator which is used for all the global random functions.
*/
ThreadSafeRandomGenerator<boost::mt19937> global_multi_threaded_random_generator(/* i_decorrelate_thread_generators = */true);