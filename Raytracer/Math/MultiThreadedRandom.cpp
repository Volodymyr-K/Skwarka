#include "MultiThreadedRandom.h"

MultiThreadedRandomGenerator<boost::mt19937> global_multi_threaded_random_generator(/* i_decorrelate_thread_generators = */true);