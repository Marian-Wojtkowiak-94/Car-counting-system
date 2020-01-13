#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <math.h>

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#define jumpFrame 3

//#define wyswietlaj

//#define Difrence
#define MOG2

#ifdef wyswietlaj

#define VideoOrgin
//#define VideoDifrence
//#define VideoMOG2

#endif





struct vehicleCunt{
    int liczba_samochodow_osobowych_z_prawej_na_lewa;
    int liczba_samochodow_osobowych_z_lewej_na_prawa;
    int liczba_samochodow_ciezarowych_autobusow_z_prawej_na_lewa;
    int liczba_samochodow_ciezarowych_autobusow_z_lewej_na_prawa;
    int liczba_tramwajow;
    int liczba_pieszych;
    int liczba_rowerzystow;
};

void vehicleCuntZeros(vehicleCunt& a){
    a.liczba_samochodow_osobowych_z_prawej_na_lewa = 0;
    a.liczba_samochodow_osobowych_z_lewej_na_prawa = 0;
    a.liczba_samochodow_ciezarowych_autobusow_z_prawej_na_lewa = 0;
    a.liczba_samochodow_ciezarowych_autobusow_z_lewej_na_prawa = 0;
    a.liczba_tramwajow = 0;
    a.liczba_pieszych = 0;
    a.liczba_rowerzystow = 0;
}
std::string vehicleCuntString( vehicleCunt& a){
    std::string ret;
    ret=std::to_string(a.liczba_samochodow_osobowych_z_prawej_na_lewa) +", "+std::to_string(
    a.liczba_samochodow_osobowych_z_lewej_na_prawa) +", "+std::to_string(
    a.liczba_samochodow_ciezarowych_autobusow_z_prawej_na_lewa) +", "+std::to_string(
    a.liczba_samochodow_ciezarowych_autobusow_z_lewej_na_prawa) +", "+std::to_string(
    a.liczba_tramwajow) +", "+std::to_string(
    a.liczba_pieszych) +", "+std::to_string(
    a.liczba_rowerzystow)+"\r\n";
    return ret;
}

enum vehicleType{
    liczba_samochodow_osobowych_z_prawej_na_lewa=0,
    liczba_samochodow_osobowych_z_lewej_na_prawa,
    liczba_samochodow_ciezarowych_autobusow_z_prawej_na_lewa,
    liczba_samochodow_ciezarowych_autobusow_z_lewej_na_prawa,
    liczba_tramwajow,
    liczba_pieszych,
    liczba_rowerzystow
};

struct rectangleClasifigate{
    cv::Rect rect;
    vehicleType type;
};




std::string GetCurrentWorkingDir( void ) {// copied http://www.codebind.com/cpp-tutorial/c-get-current-directory-linuxwindows/
  char buff[FILENAME_MAX];
  GetCurrentDir( buff, FILENAME_MAX );
  std::string current_working_dir(buff);
  return current_working_dir;
}




std::vector<cv::Rect> recocnesizeShipe(cv::Mat& grey,int min,cv::Mat& mask){// dowloud https://stackoverflow.com/questions/22240746/recognize-open-and-closed-shapes-opencv
    cv::Mat thr;
    int kr=0;
//    cv::Mat src=cv::imread("1.png",1);
//    cv::cvtColor(src,tmp,CV_BGR2GRAY);
    cv::threshold(grey,thr,200,255,cv::THRESH_BINARY_INV);
    cv::bitwise_not(thr,thr);

    std::vector< std::vector <cv::Point> > contours; // Vector for storing contour
    std::vector< cv::Vec4i > hierarchy;
    std::vector<cv::Rect> ret;
    cv::findContours( thr, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

    for( int i = 0; i< contours.size(); i=hierarchy[i][0] ) // iterate through each contour.
    {
        cv::Rect r= boundingRect(contours[i]);
        if ((r.width<=min)&&(r.height<=min)){
            continue;
        }
//        if(hierarchy[i][2]<0) //Check if there is a child contour
//          cv::rectangle(src,cv::Point(r.x-10,r.y-10), cv::Point(r.x+r.width+10,r.y+r.height+10), cv::Scalar(0,0,255),2,8,0); //Opened contour
//        else
//          cv::rectangle(src,cv::Point(r.x-10,r.y-10), cv::Point(r.x+r.width+10,r.y+r.height+10), cv::Scalar(0,255,0),2,8,0); //closed contour
        ret.push_back(r);
    }
    if(mask.channels()==3){
        for(int i=0;i<ret.size();i++){
            cv::Rect r=ret[i];
            cv::rectangle(mask,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(0,255,0),CV_FILLED,8,0); //closed contour
        }

    }
    if(mask.channels() == 1)
    for(int i=0;i<ret.size();i++){
        cv::Rect r=ret[i];
        cv::rectangle(mask,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), 255,CV_FILLED,8,0); //closed contour
    }

#ifdef VideoMOG2
    cv::imshow("dst", mask);
#endif
    return ret;
}





std::vector<rectangleClasifigate> clasyfaier(cv::Mat& img,std::vector<cv::Rect>& Rectangle){
    int kr=1;
    std::vector<rectangleClasifigate> ret;
    for(int i=0;i<Rectangle.size();i++){
        cv::Rect r=Rectangle[i];
        int y=r.y+r.height;

        if(((130<y)&&(y<210))||((765<y)&&(y<1075))){ //hodniki
            if(r.width<=(double) r.height*1.3){// piesi
                #ifdef wyswietlaj
                 cv::rectangle(img,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(0,266,255),3,8,0); //closed contour
                 #endif
                 rectangleClasifigate a;
                 a.rect= r;
                 a.type= vehicleType::liczba_pieszych;
                 ret.push_back(a);
             }
        }
        else if(r.height<80){
            #ifdef wyswietlaj
            cv::rectangle(img,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(0,0,255),3,8,0); //closed contour
            #endif
        }
        else if((350<y)&&(y<444)){// torowisko
            if(r.width>(double) r.height*2.3){
                #ifdef wyswietlaj
                cv::rectangle(img,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(0,0,0),3,8,0); //closed contour
                #endif
                rectangleClasifigate a;
                a.rect= r;
                a.type= vehicleType::liczba_tramwajow;
                ret.push_back(a);
            }
        }
        else if((223<y)&&(y<350)){ //droga prawo na lewo
//            if(r.width>(double) r.height*2.7){// tir
//                cv::rectangle(img,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(255,0,0),3,8,0); //closed contour
//            }
//            else
            if(r.height<150){ // car
                #ifdef wyswietlaj
                cv::rectangle(img,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(255,255,0),3,8,0); //closed contour
                #endif
                rectangleClasifigate a;
                a.rect= r;
                a.type= vehicleType::liczba_samochodow_osobowych_z_prawej_na_lewa;
                ret.push_back(a);
            }
            else{// tir
                #ifdef wyswietlaj
                cv::rectangle(img,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(0,255,0),3,8,0); //closed contour
                #endif
                rectangleClasifigate a;
                a.rect= r;
                a.type= vehicleType::liczba_samochodow_ciezarowych_autobusow_z_prawej_na_lewa;
                ret.push_back(a);
            }
        }
        else if((448<y)&&(y<612)){ //droga levo na prawo
//            if(r.width>(double) r.height*2.7){// tir
//                cv::rectangle(img,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(255,0,0),3,8,0); //closed contour
//            }
//            else
            if(r.height<150){ // car
                #ifdef wyswietlaj
                cv::rectangle(img,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(255,255,0),3,8,0); //closed contour
                #endif
                rectangleClasifigate a;
                a.rect= r;
                a.type= vehicleType::liczba_samochodow_osobowych_z_lewej_na_prawa;
                ret.push_back(a);
            }
            else{
                #ifdef wyswietlaj
                cv::rectangle(img,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(0,255,0),3,8,0); //closed contour
                #endif
                rectangleClasifigate a;
                a.rect= r;
                a.type= vehicleType::liczba_samochodow_ciezarowych_autobusow_z_lewej_na_prawa;
                ret.push_back(a);
            }
        }
        else{
            #ifdef wyswietlaj
            cv::rectangle(img,cv::Point(r.x-kr,r.y-kr), cv::Point(r.x+r.width+kr,r.y+r.height+kr), cv::Scalar(0,0,255),3,8,0); //closed contour
            #endif
        }

    }
    return ret;

}

void vehicleCuntUpData(vehicleCunt& cunter,std::vector<rectangleClasifigate> dane,int xMin,int xMax){
    for(int i=0;i<dane.size();i++){
        rectangleClasifigate r=dane[i];
        if((xMin<r.rect.x)&&(r.rect.x<xMax)){
            switch (r.type) {
            case liczba_samochodow_osobowych_z_prawej_na_lewa:
                cunter.liczba_samochodow_osobowych_z_prawej_na_lewa++;
                break;
            case liczba_samochodow_osobowych_z_lewej_na_prawa:
                cunter.liczba_samochodow_osobowych_z_lewej_na_prawa++;

                break;
            case liczba_samochodow_ciezarowych_autobusow_z_prawej_na_lewa:
                cunter.liczba_samochodow_ciezarowych_autobusow_z_prawej_na_lewa++;

                break;
            case liczba_samochodow_ciezarowych_autobusow_z_lewej_na_prawa:
                cunter.liczba_samochodow_ciezarowych_autobusow_z_lewej_na_prawa++;

                break;
            case liczba_tramwajow:
                cunter.liczba_tramwajow++;

                break;
            case liczba_pieszych:
                cunter.liczba_pieszych++;

                break;
            case liczba_rowerzystow:
                cunter.liczba_rowerzystow++;

                break;

            default:
                break;
            }
        }
    }
}





std::string computeVideo(std::string moviePath){ // mog 2 użyty na poctawie https://docs.opencv.org/3.0-beta/doc/tutorials/video/background_subtraction/background_subtraction.html


    int erosion_size=2;
    int dilation_size=12;
    int filtrShipe=3;
    int xMin=500;
    int xMax=550;
    #ifdef VideoOrgin
        cv::namedWindow("Frame", cv::WINDOW_NORMAL );
        cv::resizeWindow("Frame",800,600);
        cv::createTrackbar("xMin","Frame",&xMin,1900);
        cv::createTrackbar("xMax","Frame",&xMax,1900);
    #else

    #endif
    #ifdef VideoDifrence
        int thSlider=1;
        cv::namedWindow("motion", cv::WINDOW_NORMAL );
        cv::resizeWindow("motion",800,600);
        cv::createTrackbar("th","motion",&thSlider,1000);


//        cv::namedWindow("andImg", cv::WINDOW_NORMAL );
//        cv::resizeWindow("andImg",800,600);

        cv::namedWindow("dst", cv::WINDOW_NORMAL );
        cv::resizeWindow("dst",800,600);
        cv::createTrackbar("filtrShipe","dst",&filtrShipe,100);

    #endif

#ifdef VideoMOG2
        cv::namedWindow("pMOG2", cv::WINDOW_NORMAL );
        cv::resizeWindow("pMOG2",800,600);


        cv::createTrackbar("erosion_size","pMOG2",&erosion_size,100);

        cv::createTrackbar("dilation_size","pMOG2",&dilation_size,100);

        cv::namedWindow("dst", cv::WINDOW_NORMAL );
        cv::resizeWindow("dst",800,600);
        cv::createTrackbar("filtrShipe","dst",&filtrShipe,100);


#endif

    char c;
    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name
    cv::VideoCapture cap(moviePath);
    if(!cap.isOpened()){
        cap.open(GetCurrentWorkingDir()+"/"+moviePath);
    }

    // Check if camera opened successfully
    if(!cap.isOpened()){
      std::cout << "Error opening video stream or file" << std::endl;
      return NULL;
    }

    cv::Mat frame1;
    cv::Mat frame2;
    cv::Mat bugGrund;
    cv::Mat frame2Gray;
    cv::Mat motion;
    cv::Mat motionGrey;
    cv::Mat andImg;

    cv::Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
    cv::Mat fgMaskMOG2Color; //fg mask fg mask generated by MOG2 method
    cv::Ptr<cv::BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
    vehicleCunt cunter;
    vehicleCuntZeros(cunter);

    pMOG2 = cv::createBackgroundSubtractorMOG2(); //MOG2 approach
#ifdef Difrence
    bugGrund=cv::imread(GetCurrentWorkingDir()+"/Wojtkowiak_marian/Bugrund.jpg") ;
#endif
    while(1){
//        frame1=frame2;
      // Capture frame-by-frame

#ifdef jumpFrame
        for( int i =0; i<jumpFrame -1;i++){
            cap >> frame2;
            if (frame2.empty())
              break;
        }
#endif
      cap >> frame2;
      if (frame2.empty())
        break;

//      cv::absdiff(frame1,frame2,motion);//https://stackoverflow.com/questions/27035672/cv-extract-differences-between-two-images
//      cv::threshold(motion, motion, 80, 255, cv::THRESH_BINARY);
//      cv::erode(motion, motion, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3)));

//      motion=calculateDfrence(frame1,frame2,thSlider);

#ifdef MOG2
      //int i = 5;

      pMOG2->apply(frame2,fgMaskMOG2);
      cv::threshold(fgMaskMOG2,fgMaskMOG2,150,256,0);
      //cv::GaussianBlur( fgMaskMOG2, fgMaskMOG2, cv::Size( i, i ), 0, 0 );

        // dowloud https://docs.opencv.org/2.4/doc/tutorials/imgproc/erosion_dilatation/erosion_dilatation.html
//      int erosion_size=2;
      cv::Mat elementErode = cv::getStructuringElement( cv::MORPH_RECT,
                                             cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                             cv::Point( erosion_size, erosion_size ) );
      cv::erode(fgMaskMOG2,fgMaskMOG2,elementErode);
//      int dilation_size=12;
      cv::Mat elementDilate = cv::getStructuringElement( cv::MORPH_RECT,
                                                     cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                                     cv::Point( dilation_size, dilation_size ) );
      cv::dilate(fgMaskMOG2,fgMaskMOG2,elementDilate);
      //end  dowloud https://docs.opencv.org/2.4/doc/tutorials/imgproc/erosion_dilatation/erosion_dilatation.html


      cv::cvtColor(fgMaskMOG2,fgMaskMOG2Color,cv::COLOR_GRAY2BGR);

//        shapeDetector(fgMaskMOG2,fgMaskMOG2Color);
    cv::Mat moveMask(fgMaskMOG2.rows,fgMaskMOG2.cols,CV_8UC1,cv::Scalar(0));
      std::vector<cv::Rect> vect1= recocnesizeShipe(fgMaskMOG2,dilation_size*filtrShipe,moveMask);
//      std::vector<cv::Rect>vect2= recocnesizeShipe(moveMask,0,moveMask);

//      std::cout<<vect1.size()<<"\n";
//      std::cout<<vect1.size()<<" "<<vect2.size()<<"\n";
      cv::cvtColor(frame2,frame2Gray,cv::COLOR_BGR2GRAY);
//      dedectorCar(frame2Gray,moveMask);
    // If the frame is empty, break immediately
      std::vector<rectangleClasifigate>sklasyfikowane= clasyfaier(frame2,vect1);
      vehicleCuntUpData(cunter,sklasyfikowane,xMin,xMax);

#endif

#ifdef Difrence

      cv::absdiff(bugGrund, frame2, motion);
      cv::cvtColor(motion,motionGrey,cv::COLOR_BGR2GRAY);
      cv::threshold(motionGrey,motionGrey,70,256,0);
//      cv::bitwise_and(motionGrey,fgMaskMOG2,andImg);
      cv::Mat elementErode = cv::getStructuringElement( cv::MORPH_RECT,
                                             cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                             cv::Point( erosion_size, erosion_size ) );
      cv::erode(motionGrey,motionGrey,elementErode);
//      int dilation_size=12;
      cv::Mat elementDilate = cv::getStructuringElement( cv::MORPH_RECT,
                                                     cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                                     cv::Point( dilation_size, dilation_size ) );
      cv::dilate(motionGrey,motionGrey,elementDilate);


//        shapeDetector(fgMaskMOG2,fgMaskMOG2Color);
      cv::Mat moveMask(motionGrey.rows,motionGrey.cols,CV_8UC1,cv::Scalar(0));
        recocnesizeShipe(motionGrey,dilation_size*filtrShipe,moveMask);
        cv::cvtColor(frame2,frame2Gray,cv::COLOR_BGR2GRAY);
//        dedectorCar(frame2Gray,moveMask);
  // If the frame is empty, break immediately

#endif



      // Display the resulting frame
//      cv::imshow( "Frame", frame1 );
        #ifdef VideoOrgin
        cv::line(frame2, cv::Point(xMin , 0),
            cv::Point(xMin, frame2.rows - 1),
                    cv::Scalar(255, 0, 128));
        cv::line(frame2, cv::Point(xMax , 0),
            cv::Point(xMax, frame2.rows - 1),
                    cv::Scalar(255, 0, 128));
            cv::imshow( "Frame", frame2 );
        #else

        #endif
        #ifdef VideoDifrence
            cv::imshow( "motion", motionGrey );
//            cv::imshow( "andImg", andImg );
        #endif
#ifdef VideoMOG2
    cv::imshow( "pMOG2", fgMaskMOG2 );

#endif

        #ifdef wyswietlaj

            c=(char)cv::waitKey(1);
            if(c==27)
            break;
            std::cout<<vehicleCuntString(cunter);
        #endif



    }

    // When everything done, release the video capture object
    cap.release();

    // Closes all the frames
    cv::destroyAllWindows();

    cunter.liczba_pieszych=ceil((double)cunter.liczba_pieszych/10);
    return vehicleCuntString(cunter);
}

int main()
{
    std::string curentDir =GetCurrentWorkingDir();
    std::ifstream movies;
    std::ofstream wynik;
    std::string pathToFile;
    std::string wyniktekst;
//    persons_cascade.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
//    if(!car_cascade.load(curentDir+"/Wojtkowiak_marian/cars.xml")){
//        std::string aa=curentDir+"/Wojtkowiak_marian/cars.xml";
//        std::cout<<aa<<"\nfile not load \n";
//        return -1;
//    }
//    car_cascade = cv::CascadeClassifier("opencv-car-detection-master/cars.xml");
    movies.open(curentDir+"/nazwy_sekwencji_mp4.txt");
    wynik.open(curentDir+"/Wojtkowiak_Marian.txt");
    if(!movies.is_open())
        return -1;
    if(!wynik.is_open())
        return -1;
    while(!movies.eof()){
        movies>>pathToFile;
        wyniktekst = computeVideo(pathToFile);
        wynik<<wyniktekst;
        //std::cout<<pathToFile<<std::endl;
    }
    movies.close();
    wynik.close();
//    std::cout << "Hello World!" << std::endl;
    return 0;
}
