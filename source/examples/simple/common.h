

#ifndef ACCIO_EXAMPLE_SIMPLE_COMMON_H
#define ACCIO_EXAMPLE_SIMPLE_COMMON_H 1

#include <accio/definitions.h>
#include <accio/writer.h>

// A basic structure holding event data (not mandatory)
struct event_info {
  int   m_event;
  int   m_run;
};

// The main interface to interact with our data
// A record in this example correspond to the data hold
// by this class
class event {
public:
  event() = default;

  void set(int run_nb, int event_nb) {
    m_info.m_run = run_nb;
    m_info.m_event = event_nb;
  }

  const int &run_nb() const {
    return m_info.m_run;
  }

  const int &event_nb() const {
    return m_info.m_event;
  }

private:
  event_info       m_info;
};

struct io_config {
  typedef event                        record_type;
  typedef unsigned char                char_type;
  typedef accio::copy::standard        copy_type;
  typedef std::allocator<char_type>    allocator_type;
};

class event_block_writer : public accio::block_writer<io_config> {
public:
  event_block_writer(const event &event) :
    accio::block_writer<io_config>("simple", "simple", 1),
    m_event(event) {
    /* nop */
  }

  accio::error_codes::code_type write(buffer_type &outbuf) const {
    outbuf.write_data(m_event.event_nb());
    outbuf.write_data(m_event.run_nb());
    return accio::error_codes::block::success;
  }

private:
  const event     &m_event;
};

class event_record : public accio::record_io<io_config> {
public:
  accio::error_codes::code_type create_writers(const record_type& record, block_writers &blocks) const {
    blocks.push_back(std::make_shared<event_block_writer>(record));
    return accio::error_codes::record::success;
  }
};


#endif  //  ACCIO_EXAMPLE_SIMPLE_COMMON_H
