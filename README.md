Generic stream to fs service

Dependencies:
```
 sudo apt-get install libfuse-dev libboost-thread-dev cmake libboost-filesystem-dev build-essential libgtest-dev libgoogle-glog-dev
```
With system using boost 1.7 as default:
```
 sudo apt-get install libfuse-dev libboost-thread1.67-dev cmake libboost-filesystem1.67-dev build-essential libgtest-dev libgoogle-glog-dev
```



Ubuntu install gtest depenency:

```sh
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo make install
```


## Building

```
mkdir build
cd build
cmake ../
sudo make install
sudo ldconfig
```

## Implementing Select callbacks

Configuration options can be observed using [Select](https://man7.org/linux/man-pages/man2/select.2.html).
It is the plugins responsibility to implement callbacks for configuration using `notifyUpdate(nodeName)`  .
Example implementation of client can is located [here](tests/poll_example.cpp).

The **select()** behaviour works as follows:

* Select returns immediately if data available to read.
* Select will block if reader reached EOF.
* On Select unblocked it is expected that the reader will seek to file position 0 and read the new data.

## Known issues

* Mounted folder can persist after streamfs is killed;

```
$ fusermount -u temp
```
or
```
sudo umount temp/
```
