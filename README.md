## Overview

The `SeamCarving.cpp` file contains a C++ program for resizing an image using the seam carving algorithm. This technique allows for content-aware resizing by removing seams with the least energy, effectively preserving important features in the image.

## Dependencies

- OpenCV: The program requires OpenCV for image processing tasks. Ensure OpenCV is installed on your system and properly configured.

## Compilation

To compile the program, use the following command:

```bash
g++ SeamCarving.cpp -o Display `pkg-config --cflags --libs opencv4`
```

## Execution

Run the compiled program with the following command:

```bash
./Display sample2.jpeg
```

Replace `sample2.jpeg` with the path to the image file you want to resize.

## Description

### Functions

1. **`fillEnergyMatrix(Mat &resizeMat, double **currEnergyMatrix)`\*\*

   - Computes the energy matrix of the image based on the gradient magnitudes of pixel colors.

2. **`dpVertical(int row, int col, double **currEnergyMatrix, double **dpMatrix, int **path, int rowSize, int colSize)`\*\*

   - Dynamic programming function to compute the minimum energy seam vertically.

3. **`dpHorizontal(int row, int col, double **currEnergyMatrix, double **dpMatrix, int **path, int rowSize, int colSize)`\*\*

   - Dynamic programming function to compute the minimum energy seam horizontally.

4. **`findVerticalSeam(double **currEnergyMatrix, int rowSize, int colSize)`\*\*

   - Finds the vertical seam with the minimum energy.

5. **`findHorizontalSeam(double **currEnergyMatrix, int rowSize, int colSize)`\*\*

   - Finds the horizontal seam with the minimum energy.

6. **`removeVerticalSeam(Mat &resizeMat, int *seam, int rowSize, int colSize)`**

   - Removes the identified vertical seam from the image.

7. **`removeHorizontalSeam(Mat &resizeMat, int *seam, int rowSize, int colSize)`**
   - Removes the identified horizontal seam from the image.

### Main Execution Flow

1. **Image Loading**: The program loads an image specified by the user.
2. **Resize Dimensions**: The user specifies new dimensions (height and width) for the resized image.
3. **Seam Carving**: The program iteratively removes seams (either vertical or horizontal) based on the current dimensions of the image until it reaches the desired size.
4. **Output**: The resized image is saved as `output.jpg`.

### Error Handling

- If the new dimensions are not smaller than the original dimensions, an error message will be displayed.
- If the image cannot be loaded, an error message will be displayed.

## Notes

- Ensure that the new dimensions are smaller than or equal to the original dimensions.
- The program displays the resizing process in real-time and waits for 30 milliseconds between updates.

---
