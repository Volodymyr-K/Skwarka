#ifndef INTRUSIVE_SMART_POINTER_SERIALIZATION_TEST_H
#define INTRUSIVE_SMART_POINTER_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/IntrusiveSmartPointer.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

// This will tell us how many times objects of the SampleReferenced class have been destroyed.
int sample_referenced_destroy_counter = 0;

// Sample class used for testing serialization of intrusive_ptr.
class SampleReferenced: public ReferenceCounted
  {
  public:
    SampleReferenced() {}

    ~SampleReferenced()
      {
      ++sample_referenced_destroy_counter;
      }

    void SetData(int i_data)
      {
      m_data = i_data;
      }

    int GetData()
      {
      return m_data;
      }

  private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int version)
      {
      i_ar & boost::serialization::base_object<ReferenceCounted>(*this);
      i_ar & m_data;
      }

    int m_data;
  };

class IntrusiveSmartPointerSerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    void test_IntrusiveSmartPointer_Serialization()
      {
      sample_referenced_destroy_counter = 0;
      intrusive_ptr<SampleReferenced> p_ref1(new SampleReferenced());
      intrusive_ptr<SampleReferenced> p_ref2(p_ref1);
      intrusive_ptr<SampleReferenced> p_ref3(p_ref1);
      p_ref1->SetData(123);

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_ref1;
        output_archive << p_ref2;
        } // archive and stream closed when destructors are called

      intrusive_ptr<SampleReferenced> p_ref4;
      intrusive_ptr<SampleReferenced> p_ref5;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_ref4;
        input_archive >> p_ref5;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(sample_referenced_destroy_counter, 0); // Nothing should have been destroyed so far.

      TS_ASSERT_DIFFERS(p_ref1.get(), p_ref4.get()); // These pointers must point to different objects.
      TS_ASSERT_EQUALS(p_ref4.get(), p_ref5.get()); // These pointers must point to the same object.
      TS_ASSERT_EQUALS(p_ref1->GetData(), p_ref4->GetData()); // Data must be the same.

      p_ref4.reset(NULL);
      TS_ASSERT_EQUALS(sample_referenced_destroy_counter, 0); // Still nothing destroyed.
      p_ref5.reset(NULL);
      TS_ASSERT_EQUALS(sample_referenced_destroy_counter, 1); // The de-serialized object should have just been destroyed which means there were exactly two references to the object.
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // INTRUSIVE_SMART_POINTER_SERIALIZATION_TEST_H