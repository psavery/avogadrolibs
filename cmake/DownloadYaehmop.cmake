# Written by Patrick S. Avery - 2018

# Downloads the executable if it doesn't already exist
macro(DownloadYaehmop)

  # Let's set the name. Windows likes to add '.exe' at the end
  if(WIN32)
    set(YAEHMOP_NAME "yaehmop.exe")
  else(WIN32)
    set(YAEHMOP_NAME "yaehmop")
  endif(WIN32)

  # If it already exists, don't download it again
  if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/bin/${YAEHMOP_NAME}")
    set(YAEHMOP_V "3.0.3")

    # Linux
    if(UNIX AND NOT APPLE)
      set(YAEHMOP_DOWNLOAD_LOCATION "https://github.com/psavery/yaehmop/releases/download/${YAEHMOP_V}/linux64-yaehmop.tgz")
      set(MD5 "7f3f71c076d8604b98a7e60c351febf2")

    # Apple
    elseif(APPLE)
      set(YAEHMOP_DOWNLOAD_LOCATION "https://github.com/psavery/yaehmop/releases/download/${YAEHMOP_V}/mac64-yaehmop.tgz")
      set(MD5 "465b8217f5aed9244513dbc00f083133")

    # Windows
    elseif(WIN32)
      set(YAEHMOP_DOWNLOAD_LOCATION "https://github.com/psavery/yaehmop/releases/download/${YAEHMOP_V}/win32-yaehmop.exe.tgz")
      set(MD5 "89be7c295200f39f5c3b2c99d14ecb1e")

    else()
      message(FATAL_ERROR
              "Yaehmop is not supported with the current OS type!")
    endif()

    message(STATUS "Downloading yaehmop executable from ${YAEHMOP_DOWNLOAD_LOCATION}")

    # Install to a temporary directory so we can copy and change file
    # permissions
    file(DOWNLOAD "${YAEHMOP_DOWNLOAD_LOCATION}"
         "${CMAKE_CURRENT_BINARY_DIR}/bin/${YAEHMOP_NAME}.tgz"
         SHOW_PROGRESS
         EXPECTED_MD5 ${MD5})

    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzvf ${YAEHMOP_NAME}.tgz
                    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

    file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/bin/${YAEHMOP_NAME}.tgz")

  endif(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/bin/${YAEHMOP_NAME}")

  set(YAEHMOP_DESTINATION "bin")

  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/bin/${YAEHMOP_NAME}"
          DESTINATION "${YAEHMOP_DESTINATION}"
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                      GROUP_READ GROUP_EXECUTE
                      WORLD_READ WORLD_EXECUTE)

endmacro(DownloadYaehmop)
