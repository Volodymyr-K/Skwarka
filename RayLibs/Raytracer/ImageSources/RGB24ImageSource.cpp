#include "RGB24ImageSource.h"

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(RGB24ImageSource_float);
BOOST_CLASS_EXPORT_IMPLEMENT(RGB24ImageSource_double);
BOOST_CLASS_EXPORT_IMPLEMENT(RGB24ImageSource_Spectrum_float);
BOOST_CLASS_EXPORT_IMPLEMENT(RGB24ImageSource_Spectrum_double);
BOOST_CLASS_EXPORT_IMPLEMENT(RGB24ImageSource_SpectrumCoef_float);
BOOST_CLASS_EXPORT_IMPLEMENT(RGB24ImageSource_SpectrumCoef_double);