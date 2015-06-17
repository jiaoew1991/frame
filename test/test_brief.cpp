#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    Mat img_1 = imread(argv[1]);
    Mat img_2 = imread(argv[2]);

    // -- Step 1: Detect the keypoints using STAR Detector
    std::vector<KeyPoint> keypoints_1,keypoints_2;
    StarDetector detector;
    detector.detect(img_1, keypoints_1);
    detector.detect(img_2, keypoints_2);

    // -- Stpe 2: Calculate descriptors (feature vectors)
    BriefDescriptorExtractor brief;
    Mat descriptors_1, descriptors_2;
    brief.compute(img_1, keypoints_1, descriptors_1);
    brief.compute(img_2, keypoints_2, descriptors_2);

    //-- Step 3: Matching descriptor vectors with a brute force matcher
    BFMatcher matcher(NORM_HAMMING);
    std::vector<DMatch> mathces;
    matcher.match(descriptors_1, descriptors_2, mathces);
    double sum = 0.0;
    for (size_t i = 0; i < mathces.size(); i++) {
        double dist = mathces[i].distance;
        sum += dist / 100 * dist / 100;
    }
    sum = sqrt(sum) / mathces.size();
    cout << sum << endl;
    // -- dwaw matches
    Mat img_mathes;
    drawMatches(img_1, keypoints_1, img_2, keypoints_2, mathces, img_mathes);
    // -- show
    imshow("Mathces", img_mathes);

    waitKey(0);
    return 0;
}
