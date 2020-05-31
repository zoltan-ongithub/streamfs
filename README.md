Generic stream to fs service

Dependencies:
 - libfuse-dev


Ubuntu install gtest depenency:

```sh
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo make install
```


== Building ==

```
mkdir build
cd build
cmake ../
sudo make install
```
