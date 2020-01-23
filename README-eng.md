# Image Difference Watcher
[![License](https://img.shields.io/badge/LICENSE-The%20Unlicense-green?style=flat-square)](/LICENSE)  [![Version](https://img.shields.io/badge/VERSION-DEBUG%20--%20demonstrable-yellow?style=flat-square)](https://github.com/averov90/ImageDiffWatcher/releases/tag/1.0)
### :small_orange_diamond: [Русская версия](https://github.com/averov90/ImageDiffWatcher/blob/master/README.md)
#### Finds differences between a static image and the camera stream in the color channel (with noise filtering).
This code allows you to search for differences between the current image (saved) and each image from the stream.
The General purpose of the code is to find significant differences between a static image and a sequence (the results of searching for differences are displayed).
For better comparison, the code uses several algorithms that improve the results. Namely: 2 types of blurring, shadow mask, glare mask, noise mask, threshold for catching differences (there will be no response to point differences). All pasks work in progressive mode (they are applied to several frames and count their indicators).
It is important to mention that the program looks for differences in the color component (this is done to reduce the effect of lighting brightness). 
As you know, the color channel is quite noisy, so noise suppression algorithms are useful here. The algorithms used in the program have settings (you can change them through the console window).

You can look at this code in the work - a demo version is available in release. The code in this repository is designed to apply it (or the principles of its operation) to other projects.

##### It is based on the OpenCV library version 3.4.1
