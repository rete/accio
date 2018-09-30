



# example: simple
add_executable( simple_reader simple/reader.cc )
add_executable( simple_writer simple/writer.cc )
target_include_directories( simple_reader BEFORE PRIVATE simple )
target_include_directories( simple_writer BEFORE PRIVATE simple )

