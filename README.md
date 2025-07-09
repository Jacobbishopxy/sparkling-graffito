# sparkling-graffito

## Requirements

```sh
# libzmq
git clone https://github.com/zeromq/libzmq.git && cd libzmq
mkdir build && cd build
sudo cmake -DCMAKE_INSTALL_PREFIX=/opt/libzmq-4.3.5 ..
sudo make install -j

# cppzmq
git clone https://github.com/zeromq/cppzmq.git && cd cppzmq
mkdir build && cd build
sudo cmake -DCMAKE_INSTALL_PREFIX=/opt/cppzmq-4.11.0 -DCPPZMQ_BUILD_TESTS=OFF ..
sudo make install -j
```
