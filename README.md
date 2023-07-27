# Image Rotation
Rotate a image to the right angle given horizontal and vertical lines. The best use case is applying to formns. 
A demo case is included in form_rotate_test.

## APIs
1. transformForm
 * @brief Perform perspective correction on the source image
 * @param src OpenCV Mat, original image
 * @param dst OpenCV Mat, rotated image
 * @param horizontal Vector of pairs of points representing horizontal lines
 * @param vertical Vector of pairs of points representing vertical lines
   
## Dependencies
[OpenCV](https://github.com/opencv/opencv)
1. Create a directory to contain this project and OpenCV.
2. Copy the opencv_world400.dll and tbb.dll files into the same directory as executable file.
- ProjectDirectory
  - executable.exe
  - opencv_world400.dll
  - tbb.dll
- OpenCV files and directories
