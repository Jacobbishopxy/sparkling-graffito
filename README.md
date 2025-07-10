# sparkling-graffito

## Requirements

```sh
# libzmq
git clone https://github.com/zeromq/libzmq.git && cd libzmq
mkdir build && cd build
sudo cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DWITH_PERF_TOOL=OFF -DWITH_DOCS=OFF -DENABLE_DRAFTS=OFF -DCMAKE_INSTALL_PREFIX=/opt/libzmq-4.3.5 ..
sudo make install -j

# cppzmq
git clone https://github.com/zeromq/cppzmq.git && cd cppzmq
mkdir build && cd build
sudo cmake -DCMAKE_INSTALL_PREFIX=/opt/cppzmq-4.11.0 -DCPPZMQ_BUILD_TESTS=OFF ..
sudo make install -j
```

## Notes

- Directly use deps:

    ```cmake
    set(LIBZMQ_VERSION 4.3.5)
    find_package(ZeroMQ REQUIRED PATHS "/opt/libzmq-${LIBZMQ_VERSION}" NO_DEFAULT_PATH)
    message(STATUS "Found ZeroMQ: ${ZeroMQ_DIR}")
    set(ZMQ_VERSION 4.11.0)
    find_package(cppzmq REQUIRED PATHS "/opt/cppzmq-${ZMQ_VERSION}" NO_DEFAULT_PATH)
    message(STATUS "Found cppzmq: ${cppzmq_DIR}")

    ...

    target_link_libraries(${PROJECT_NAME} PRIVATE cppzmq)
    ```

- Embed deps:

    ```cmake
    # Link ZeroMQ statically
    set(LIBZMQ_VERSION 4.3.5)
    find_library(ZMQ_STATIC_LIBRARY
        NAMES libzmq.a
        PATHS "/opt/libzmq-${LIBZMQ_VERSION}/lib"
        NO_DEFAULT_PATH
        REQUIRED
    )

    add_library(libzmq STATIC IMPORTED)
    set_target_properties(libzmq PROPERTIES IMPORTED_LOCATION "${ZMQ_STATIC_LIBRARY}")

    # cppzmq is header-only
    set(ZMQ_VERSION 4.11.0)
    add_library(cppzmq INTERFACE)
    target_include_directories(cppzmq INTERFACE "/opt/cppzmq-${ZMQ_VERSION}/include")
    install(TARGETS cppzmq EXPORT ${PROJECT_NAME}Targets)

    ...

    target_link_libraries(${PROJECT_NAME} PRIVATE ${ZMQ_STATIC_LIBRARY} cppzmq)
    ```
