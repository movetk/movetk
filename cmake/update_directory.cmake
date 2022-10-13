# From: https://bravenewmethod.com/2017/06/18/update_directory-command-for-cmake/
# cmake/update_directory.cmake
file(GLOB_RECURSE _file_list RELATIVE "${src_dir}" "${src_dir}/*")
 
foreach( each_file ${_file_list} )
  set(destinationfile "${dst_dir}/${each_file}")
  set(sourcefile "${src_dir}/${each_file}")
  if(NOT EXISTS ${destinationfile} OR ${sourcefile} IS_NEWER_THAN ${destinationfile})
    get_filename_component(destinationdir ${destinationfile} DIRECTORY)
    file(COPY ${sourcefile} DESTINATION ${destinationdir})
  endif()
endforeach(each_file)