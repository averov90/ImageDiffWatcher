#include "stdafx.h"
#include <iostream> //TEMPCODE

using namespace std;
using namespace cv;

const unsigned width = 640,
			   height = 480;
const unsigned char summ_pass_count = 10,
			        noise_reduction_pass_count = 10;

unsigned short counter_photo = 0, value_blur = 3, value_mask_cath = 510;
unsigned char value_mask = 40, value_highlight = 240, value_noise_cath = 50, value_belaterial_sigma_color = 10, value_belaterial_sigma_space = 12, value_cath_contours = 20;
char value_belaterial_core = 4;
Mat dark_img = Mat::zeros(height, width, CV_8UC1), summ_img_arr = Mat::zeros(height, width, CV_16UC1);
string myDir;
VideoCapture vcapture;

void MainLoop();

int main(int argc, const char** argv)
{
	myDir = *argv;
	myDir =	myDir.substr(0,myDir.find_last_of("\\"));
	vcapture.open(0);
	vcapture.set(CAP_PROP_FRAME_WIDTH, width);
	vcapture.set(CAP_PROP_FRAME_HEIGHT, height);
	printf("[i] %d x %d\n", (int)vcapture.get(CAP_PROP_FRAME_WIDTH), (int)vcapture.get(CAP_PROP_FRAME_HEIGHT)); //TEMPCODE
	MainLoop();
	vcapture.release();
	return 0;
}


void MainLoop() {
	register Mat temp1, mask_img1, mask_img2;
	vector<vector<Point>> contours;
	vector<vector<Point>>::iterator contap;
	RotatedRect rect; 
	Point2f boxpoints[4]; //TEMPCODE
	Mat frame, temp, normframe, difference,
		*hsv_brgh, *hsv_clr, *hsv_sat, 
		*diffas, *diffae, *diffap, 
		*imgas, *imgae, *imgap,
		*maskas, *maskap, 
		prewdiff = dark_img.clone(), summ_mask_arr = summ_img_arr.clone(), original = dark_img.clone(), noise_map = dark_img.clone(), pixel_noise_count = dark_img.clone(), pixel_add_mat = dark_img.clone() + 1,
		orig_hsv_img[3] = { dark_img.clone(),dark_img.clone(),dark_img.clone() }, mask_arr[summ_pass_count], img_arr[summ_pass_count], pixel_noise_arr[noise_reduction_pass_count];
	hsv_clr = orig_hsv_img; hsv_sat = orig_hsv_img + 1; hsv_brgh = orig_hsv_img + 2;
	for (char i = 0; i != noise_reduction_pass_count; i++)
		pixel_noise_arr[i] = dark_img.clone();
	for (char i = 0; i != summ_pass_count; i++) {
		mask_arr[i] = summ_img_arr.clone();
		img_arr[i] = summ_img_arr.clone();
	}
	diffap = diffas = pixel_noise_arr; diffae = pixel_noise_arr + noise_reduction_pass_count - 1;
	imgap = imgas = img_arr; imgae = img_arr + summ_pass_count - 1;
	maskap = maskas = mask_arr;
	std::cout << "Mask value: " << (int)value_mask << " | Mask catch value: " << (int)value_mask_cath << std::endl;  //TEMPCODE
	std::cout << "Highlight value: " << (int)value_highlight << " | Noise cath value: " << (int)value_noise_cath << std::endl;  //TEMPCODE
	std::cout << "Belaterial: core-size: " << (int)value_belaterial_core << " color-blend-scatter: " << (int)value_belaterial_sigma_color << " blend-neighbours-radius: " << (int)value_belaterial_sigma_space << std::endl;  //TEMPCODE
	std::cout << "Blur value: " << (int)value_blur << std::endl; //TEMPCODE
	std::cout << "Cath contours value: " << (int)value_cath_contours << std::endl;
	unsigned short key;
	while (true)
	{
		vcapture.read(frame);
		cv::cvtColor(frame, temp, COLOR_RGB2HSV);
		split(temp, orig_hsv_img);
		threshold(*hsv_brgh, mask_img1, value_mask, 255, THRESH_BINARY_INV);
		threshold(*hsv_brgh, mask_img2, value_highlight, 255, THRESH_BINARY);
		bitwise_or(mask_img1, mask_img2, mask_img1);
		if (imgap == imgae) {
			mask_img1.convertTo(temp1, CV_16UC1);
			summ_mask_arr -= *maskap;
			summ_mask_arr += temp1;
			*maskap = temp1.clone();
			maskap = maskas;
			(*hsv_clr).convertTo(temp1, CV_16UC1);
			summ_img_arr -= *imgap;
			summ_img_arr += temp1;
			*imgap = temp1.clone();
			imgap = imgas;
		} else {
			mask_img1.convertTo(temp1, CV_16UC1);
			summ_mask_arr -= *maskap;
			summ_mask_arr += temp1;
			*maskap = temp1.clone();
			maskap++;
			(*hsv_clr).convertTo(temp1, CV_16UC1);
			summ_img_arr -= *imgap;
			summ_img_arr += temp1;
			*imgap = temp1.clone();
			imgap++;
			//continue;  //Uncomment if program work very slow ----UNCOMMENT POSITION #1
		}
		threshold(summ_mask_arr, temp1, value_mask_cath, 65535, THRESH_BINARY);
		temp1.convertTo(mask_img1, CV_8UC1);
		((Mat)(summ_img_arr / summ_pass_count)).convertTo(normframe, CV_8UC1);
		bitwise_and(normframe, dark_img, normframe, mask_img1);
		bilateralFilter(normframe, difference, value_belaterial_core, value_belaterial_sigma_color, value_belaterial_sigma_space);
		medianBlur(difference, normframe, value_blur);
		
		imshow("mask (full+normalised)", mask_img1); //TEMPCODE
		imshow("highlight", mask_img2); //TEMPCODE
		imshow("normalised frame", normframe); //TEMPCODE

		absdiff(normframe, original, difference);
		absdiff(difference, prewdiff, mask_img2);
		prewdiff = difference.clone();

		//imshow("raw difference", difference); //TEMPCODE

		if (diffap == diffae) {
			bitwise_or(noise_map, mask_img2, noise_map);
			threshold(mask_img2, mask_img1, 0, 255, THRESH_BINARY);
			bitwise_or(dark_img, pixel_add_mat, mask_img2, mask_img1);
			pixel_noise_count -= *diffap;
			pixel_noise_count += mask_img2;
			*diffap = mask_img2.clone();
			threshold(pixel_noise_count, mask_img1, 0, 255, THRESH_BINARY_INV);
			bitwise_and(noise_map, dark_img, noise_map, mask_img1);
			diffap = diffas;
		} else {
			bitwise_or(noise_map, mask_img2, noise_map);
			threshold(mask_img2, mask_img1, 0, 255, THRESH_BINARY);
			bitwise_or(dark_img, pixel_add_mat, mask_img2, mask_img1);
			pixel_noise_count -= *diffap;
			pixel_noise_count += mask_img2;
			*diffap = mask_img2.clone();
			threshold(pixel_noise_count, mask_img1, 0, 255, THRESH_BINARY_INV);
			bitwise_and(noise_map, dark_img, noise_map, mask_img1);
			diffap++;
			//continue;  //Uncomment if program work very slow ----UNCOMMENT POSITION #2
		}
		threshold(noise_map, mask_img2, value_noise_cath, 255, THRESH_BINARY);
		bitwise_and(difference, dark_img, difference, mask_img2);

		//imshow("hsv color frame", *hsv_clr); //TEMPCODE
		imshow("difference noise", mask_img2); //TEMPCODE
		imshow("normal difference", difference); //TEMPCODE

		threshold(difference, difference, value_cath_contours, 255, THRESH_BINARY);
		findContours(difference, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		contap = contours.begin();
		for (; contap != contours.end(); contap++)
		{
			rect = minAreaRect(*contap);
			if ((rect.size.width > rect.size.height ? rect.size.width : rect.size.height) < 10) continue;

			rect.points(boxpoints);
			line(frame, boxpoints[0], boxpoints[1], Scalar(0, 0, 255), 2, LineTypes::LINE_4);
			line(frame, boxpoints[1], boxpoints[2], Scalar(0, 0, 255), 2, LineTypes::LINE_4);
			line(frame, boxpoints[2], boxpoints[3], Scalar(0, 0, 255), 2, LineTypes::LINE_4);
			line(frame, boxpoints[3], boxpoints[0], Scalar(0, 0, 255), 2, LineTypes::LINE_4);
		}
		imshow("camera frame", frame); //TEMPCODE

		key = waitKey(10);
		switch (key)
		{
		case 'q':
			value_mask--;
			std::cout << "Mask value: " << (int)value_mask << std::endl;
			break;
		case 'w':
			value_mask++;
			std::cout << "Mask value: " << (int)value_mask << std::endl;
			break;
		case 'e':
			value_mask_cath-=255;
			std::cout << "Mask catch value: " << (int)value_mask_cath << std::endl;
			break;
		case 'r':
			value_mask_cath+=255;
			std::cout << "Mask catch value: " << (int)value_mask_cath << std::endl;
			break;
		case 'a':
			value_highlight--;
			std::cout << "Highlight value: " << (int)value_highlight << std::endl;
			break;
		case 's':
			value_highlight++;
			std::cout << "Highlight value: " << (int)value_highlight << std::endl;
			break;
		case 'd':
			value_noise_cath--;
			std::cout << "Noise cath value: " << (int)value_noise_cath << std::endl;
			break;
		case 'f':
			value_noise_cath++;
			std::cout << "Noise cath value: " << (int)value_noise_cath << std::endl;
			break;
		case 'z':
			if (value_blur == 1) break;
			value_blur-=2;
			std::cout << "Blur value: " << (int)value_blur << std::endl;
			break;
		case 'x':
			value_blur+=2;
			std::cout << "Blur value: " << (int)value_blur << std::endl;
			break;
		case 'u':
			value_cath_contours++;
			std::cout << "Cath contours value: " << (int)value_cath_contours << std::endl;
			break;
		case 'j':
			value_cath_contours--;
			std::cout << "Cath contours value: " << (int)value_cath_contours << std::endl;
			break;
		case 't':
			if (value_belaterial_core == -1) break;
			value_belaterial_core--;
			std::cout << "Belaterial: core-size: " << (int)value_belaterial_core << std::endl;
			break;
		case 'y':
			if (value_belaterial_core == 5) break;
			value_belaterial_core++;
			std::cout << "Belaterial: core-size: " << (int)value_belaterial_core << std::endl; 
			break;
		case 'g':
			value_belaterial_sigma_color--;
			std::cout << "Belaterial: color-blend-scatter: " << (int)value_belaterial_sigma_color << std::endl;
			break;
		case 'h':
			value_belaterial_sigma_color++;
			std::cout << "Belaterial: color-blend-scatter: " << (int)value_belaterial_sigma_color << std::endl;
			break;
		case 'b':
			value_belaterial_sigma_space--;
			std::cout << "Belaterial: blend-neighbours-radius: " << (int)value_belaterial_sigma_space << std::endl;
			break;
		case 'n':
			value_belaterial_sigma_space++;
			std::cout << "Belaterial: blend-neighbours-radius: " << (int)value_belaterial_sigma_space << std::endl;
			break;
		case 'i':
			std::cout << "Mask value: " << (int)value_mask << " | Mask catch value: " << (int)value_mask_cath << std::endl;
			std::cout << "Highlight value: " << (int)value_highlight << " | Noise cath value: " << (int)value_noise_cath << std::endl;
			std::cout << "Belaterial: core-size: " << (int)value_belaterial_core << " color-blend-scatter: " << (int)value_belaterial_sigma_color << " blend-neighbours-radius: " << (int)value_belaterial_sigma_space << std::endl;
			std::cout << "Blur value: " << (int)value_blur << std::endl;
			std::cout << "Cath contours value: " << (int)value_cath_contours << std::endl;
			break;
		case 'l':
			if (_access((myDir + "\\perfimg\\image0.jpg").c_str(), 0) == -1) {
				std::cout << "[i] File \"image0.jpg\" not exist!" << std::endl;
				break;
			}
			original = imread(myDir + "\\perfimg\\image0.jpg", IMREAD_UNCHANGED);
			std::cout << "[i] \"image0.jpg\" loaded." << std::endl;
			imshow("original", original);
			break;
		case 'c':
			original = normframe.clone();
			std::cout << "[i] original image changed." << std::endl;
			imshow("original", original);
			break;
		case 'p':
			{
			std::vector<int> saveimgparam(2);
			saveimgparam[0] = CV_IMWRITE_JPEG_QUALITY;
			saveimgparam[1] = 100;
			imwrite(myDir + "\\perfimg\\image0.jpg", original, saveimgparam);
			}
			std::cout << "[i] \"image0.jpg\" saved from ORIGINAL." << std::endl;
			break;
		case 'o':
			{
			std::vector<int> saveimgparam(2);
			saveimgparam[0] = CV_IMWRITE_JPEG_QUALITY;
			saveimgparam[1] = 100;
			imwrite(myDir + "\\perfimg\\img" + to_string(counter_photo) + ".jpg", normframe, saveimgparam);
			}
			std::cout << "[i] \"img" << counter_photo << ".jpg\" saved from NFRAME." << std::endl;
			counter_photo++;
			break;
		case 27: //Esc
			return;
		}
	}
}