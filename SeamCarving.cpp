#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <limits>
using namespace std;
using namespace cv;

// g++ SeamCarving.cpp -o Display `pkg-config --cflags --libs opencv4`
// ./Display sample2.jpeg

void fillEnergyMatrix(Mat &resizeMat, double **currEnergyMatrix)
{
    int rowSize = resizeMat.rows;
    int colSize = resizeMat.cols;
    int d1[4] = {-1, 0, 1, 0};
    int d2[4] = {0, 1, 0, -1};

    for (int r = 0; r < rowSize; r++)
    {
        for (int c = 0; c < colSize; c++)
        {
            Vec3b upperPixel, rightPixel, downPixel, leftPixel;

            for (int k = 0; k < 4; k++)
            {
                int newr = r + d1[k];
                int newc = c + d2[k];
                newr = min(max(0, newr), rowSize - 1);
                newc = min(max(0, newc), colSize - 1);

                if (k == 0)
                {
                    upperPixel = resizeMat.at<Vec3b>(newr, newc);
                }
                else if (k == 1)
                {
                    rightPixel = resizeMat.at<Vec3b>(newr, newc);
                }
                else if (k == 2)
                {
                    downPixel = resizeMat.at<Vec3b>(newr, newc);
                }
                else if (k == 3)
                {
                    leftPixel = resizeMat.at<Vec3b>(newr, newc);
                }
            }

            int gradientX = 0, gradientY = 0;
            for (int k = 0; k < 3; k++) // RGB
            {
                gradientX += pow(rightPixel[k] - leftPixel[k], 2);
                gradientY += pow(downPixel[k] - upperPixel[k], 2);
            }
            currEnergyMatrix[r][c] = sqrt(gradientX + gradientY);
        }
    }
}

double dpVertical(int row, int col, double **currEnergyMatrix, double **dpMatrix, int **path, int rowSize, int colSize)
{
    if (col < 0 || col >= colSize)
    {
        return 1e18;
    }
    if (row == 0)
    {
        return currEnergyMatrix[0][col];
    }
    if (dpMatrix[row][col] != -1)
    {
        return dpMatrix[row][col];
    }
    double upperLeft = dpVertical(row - 1, col - 1, currEnergyMatrix, dpMatrix, path, rowSize, colSize);
    double upper = dpVertical(row - 1, col, currEnergyMatrix, dpMatrix, path, rowSize, colSize);
    double upperRight = dpVertical(row - 1, col + 1, currEnergyMatrix, dpMatrix, path, rowSize, colSize);
    double minEnergy = min({upperLeft, upper, upperRight});

    if (minEnergy == upperLeft)
    {
        path[row][col] = col - 1;
    }
    else if (minEnergy == upper)
    {
        path[row][col] = col;
    }
    else
    {
        path[row][col] = col + 1;
    }
    return dpMatrix[row][col] = currEnergyMatrix[row][col] + minEnergy;
}

double dpHorizontal(int row, int col, double **currEnergyMatrix, double **dpMatrix, int **path, int rowSize, int colSize)
{
    if (row < 0 || row >= rowSize)
    {
        return 1e18;
    }
    if (col == 0)
    {
        return currEnergyMatrix[row][0];
    }
    if (dpMatrix[row][col] != -1)
    {
        return dpMatrix[row][col];
    }
    double leftUp = dpHorizontal(row - 1, col - 1, currEnergyMatrix, dpMatrix, path, rowSize, colSize);
    double left = dpHorizontal(row, col - 1, currEnergyMatrix, dpMatrix, path, rowSize, colSize);
    double leftDown = dpHorizontal(row + 1, col - 1, currEnergyMatrix, dpMatrix, path, rowSize, colSize);
    double minEnergy = min({leftUp, left, leftDown});
    if (minEnergy == leftUp)
    {
        path[row][col] = row - 1;
    }
    else if (minEnergy == left)
    {
        path[row][col] = row;
    }
    else
    {
        path[row][col] = row + 1;
    }
    return dpMatrix[row][col] = currEnergyMatrix[row][col] + minEnergy;
}

int *findVerticalSeam(double **currEnergyMatrix, int rowSize, int colSize)
{
    double **dpMatrix = new double *[rowSize];
    int **path = new int *[rowSize]; // To store the path
    for (int i = 0; i < rowSize; i++)
    {
        dpMatrix[i] = new double[colSize];
        path[i] = new int[colSize];
        for (int j = 0; j < colSize; j++)
        {
            dpMatrix[i][j] = -1;
        }
    }

    double minSeamEnergy = 1e18;
    int minSeamEndCol = 0;
    for (int col = 0; col < colSize; col++)
    {
        double seamEnergy = dpVertical(rowSize - 1, col, currEnergyMatrix, dpMatrix, path, rowSize, colSize);
        if (seamEnergy < minSeamEnergy)
        {
            minSeamEnergy = seamEnergy;
            minSeamEndCol = col;
        }
    }

    int *verticalSeam = new int[rowSize];
    verticalSeam[rowSize - 1] = minSeamEndCol;
    for (int row = rowSize - 2; row >= 0; row--)
    {
        verticalSeam[row] = path[row + 1][verticalSeam[row + 1]];
    }

    for (int i = 0; i < rowSize; i++)
    {
        delete[] dpMatrix[i];
        delete[] path[i];
    }
    delete[] dpMatrix;
    delete[] path;
    return verticalSeam;
}

int *findHorizontalSeam(double **currEnergyMatrix, int rowSize, int colSize)
{
    double **dpMatrix = new double *[rowSize];
    int **path = new int *[rowSize]; // To store the path
    for (int i = 0; i < rowSize; i++)
    {
        dpMatrix[i] = new double[colSize];
        path[i] = new int[colSize];
        for (int j = 0; j < colSize; j++)
        {
            dpMatrix[i][j] = -1;
        }
    }

    double minSeamEnergy = 1e18;
    int minSeamEndRow = 0;
    for (int row = 0; row < rowSize; row++)
    {
        double seamEnergy = dpHorizontal(row, colSize - 1, currEnergyMatrix, dpMatrix, path, rowSize, colSize);
        if (seamEnergy < minSeamEnergy)
        {
            minSeamEnergy = seamEnergy;
            minSeamEndRow = row;
        }
    }

    int *horizontalSeam = new int[colSize];
    horizontalSeam[colSize - 1] = minSeamEndRow;
    for (int col = colSize - 2; col >= 0; col--)
    {
        horizontalSeam[col] = path[horizontalSeam[col + 1]][col + 1];
    }

    for (int i = 0; i < rowSize; i++)
    {
        delete[] dpMatrix[i];
        delete[] path[i];
    }
    delete[] dpMatrix;
    delete[] path;

    return horizontalSeam;
}

Mat removeVerticalSeam(Mat &resizeMat, int *seam, int rowSize, int colSize)
{
    for (int r = 0; r < rowSize; r++)
    {
        int c = seam[r];
        resizeMat.at<Vec3b>(r, c) = Vec3b(0, 0, 255); // BGR
    }
    imshow("Resizing Live", resizeMat);
    waitKey(30);
    Mat updatedImage(rowSize, colSize - 1, CV_8UC3);
    for (int r = 0; r < rowSize; r++)
    {
        int newCol = 0;
        for (int c = 0; c < colSize; c++)
        {
            if (c == seam[r])
            {
                continue;
            }
            updatedImage.at<Vec3b>(r, newCol) = resizeMat.at<Vec3b>(r, c);
            newCol++;
        }
    }
    return updatedImage;
}

Mat removeHorizontalSeam(Mat &resizeMat, int *seam, int rowSize, int colSize)
{
    for (int c = 0; c < colSize; c++)
    {
        int r = seam[c];
        resizeMat.at<Vec3b>(r, c) = Vec3b(0, 0, 255); // BGR
    }
    imshow("Resizing Live", resizeMat);
    waitKey(30);
    Mat updatedImage(rowSize - 1, colSize, CV_8UC3);
    for (int c = 0; c < colSize; c++)
    {
        int newRow = 0;
        for (int r = 0; r < rowSize; r++)
        {
            if (r == seam[c])
            {
                continue;
            }
            updatedImage.at<Vec3b>(newRow, c) = resizeMat.at<Vec3b>(r, c);
            newRow++;
        }
    }
    return updatedImage;
}

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        cerr << "Error: Incorrect number of arguments" << endl;
        return -1;
    }

    string imgPath = argv[1];
    Mat origMat = imread(imgPath, IMREAD_COLOR); // Matrix of orignal image
    if (origMat.empty())
    {
        cerr << "Error Loading image in origMatrix" << endl;
        return -1;
    }

    // for (int i = 0; i < origMat.rows; ++i)
    // {
    //     for (int j = 0; j < origMat.cols; ++j)
    //     {
    //         Vec3b pixel = origMat.at<Vec3b>(i, j); // Access pixel as Vec3b (RGB)
    //         cout << "Pixel at (" << i << ", " << j << "): "
    //              << "R=" << (int)pixel[2] << ", "
    //              << "G=" << (int)pixel[1] << ", "
    //              << "B=" << (int)pixel[0] << endl;
    //     }
    // }

    int oldHeight = origMat.rows;
    int oldWidth = origMat.cols;

    std::cout << "Enter new height and new width lesser than orignal" << endl;
    int newHeight, newWidth;
    cin >> newHeight >> newWidth;

    if (newHeight > oldHeight or newWidth > oldWidth)
    {
        cerr << "Error: New dimensions must be smaller than or equal to the original." << endl;
        return -1;
    }

    Mat resizeMat = origMat.clone();

    // reducing the width of image
    bool ok = true;
    while (resizeMat.cols > newWidth or resizeMat.rows > newHeight)
    {
        if (ok)
        {
            double **currEnergyMatrix = new double *[resizeMat.rows];
            for (int i = 0; i < resizeMat.rows; i++)
            {
                currEnergyMatrix[i] = new double[resizeMat.cols];
            }
            fillEnergyMatrix(resizeMat, currEnergyMatrix);
            int *verticalSeam = findVerticalSeam(currEnergyMatrix, resizeMat.rows, resizeMat.cols);
            resizeMat = removeVerticalSeam(resizeMat, verticalSeam, resizeMat.rows, resizeMat.cols);
            imshow("Resizing Live", resizeMat);
            waitKey(30);
            delete[] currEnergyMatrix;
            delete[] verticalSeam;
            if (resizeMat.rows > newHeight)
            {
                ok = !ok;
            }
        }
        else
        {
            double **currEnergyMatrix = new double *[resizeMat.rows];
            for (int i = 0; i < resizeMat.rows; i++)
            {
                currEnergyMatrix[i] = new double[resizeMat.cols];
            }
            fillEnergyMatrix(resizeMat, currEnergyMatrix);
            int *horizontalSeam = findHorizontalSeam(currEnergyMatrix, resizeMat.rows, resizeMat.cols);
            resizeMat = removeHorizontalSeam(resizeMat, horizontalSeam, resizeMat.rows, resizeMat.cols);
            imshow("Resizing Live", resizeMat);
            waitKey(30);
            delete[] currEnergyMatrix;
            delete[] horizontalSeam;
            if (resizeMat.cols > newWidth)
            {
                ok = !ok;
            }
        }
    }
    imwrite("output.jpg", resizeMat);
    std::cout << "Image resized : Success" << endl;
}