Generic stream to fs service

Dependencies:
```
 sudo apt-get install libfuse-dev libboost-thread-dev cmake libboost-filesystem-dev build-essential libgtest-dev libgoogle-glog-dev
```



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
ldconfig
```
