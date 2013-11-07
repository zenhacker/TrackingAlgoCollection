#include <iostream>
#include <stdio.h>

// OpenCV Includes
#include <object_tracker.h>

int
main(int argc, char** argv)
{
  // Setup the parameters to use OnlineBoosting or MILTrack as the underlying tracking algorithm
  cv::ObjectTrackerParams params;
#if 0
  params.algorithm_ = cv::ObjectTrackerParams::CV_ONLINEBOOSTING;
  //params.algorithm_ = cv::ObjectTrackerParams::CV_SEMIONLINEBOOSTING;
  params.num_classifiers_ = 100;
  params.overlap_ = 0.99f;
  params.search_factor_ = 2;
#else
  params.algorithm_ = cv::ObjectTrackerParams::CV_ONLINEMIL;
  params.num_classifiers_ = 50;
  params.num_features_ = 250;
#endif

  // Instantiate an object tracker
  cv::ObjectTracker tracker(params);

  // Read in a sequence of images from disk as the video source
  const char* directory = "data/david";
  const int start = 1;
  const int stop = 462;
  const int delta = 1;
  const char* prefix = "img";
  const char* suffix = "png";
  char filename[1024];

  // Some book-keeping
  bool is_tracker_initialized = false;
  CvRect init_bb = cvRect(122, 58, 75, 97); // the initial tracking bounding box

  /* const char* cascade_name = "haarcascade_frontalface_alt_tree.xml";
   const int minsz = 20;
   if( Tracker::facecascade.empty() )
   Tracker::facecascade.load(cascade_name);

   cv::Mat gray;
   cv::cvtColor(frame, gray, CV_BGR2GRAY);
   cv::equalizeHist(gray, gray);

   std::vector<cv::Rect> faces;
   facecascade.detectMultiScale(gray, faces, 1.05, 3, CV_HAAR_DO_CANNY_PRUNING ,cvSize(minsz, minsz));

   bool is_good = false;
   cv::Rect r;
   for (int index = faces.size() - 1; index >= 0; --index)
   {
   r = faces[index];
   if (r.width < minsz || r.height < minsz || (r.y + r.height + 10) > frame.rows || (r.x + r.width) > frame.cols
   || r.y < 0 || r.x < 0)
   continue;
   is_good = true;
   break;
   }
   */

  cv::Rect theTrack;
  bool tracker_failed = false;

  // Read in images one-by-one and track them
  cv::namedWindow("Tracker Display", cv::WINDOW_NORMAL);
  for (int frame = start; frame <= stop; frame += delta)
  {
    sprintf(filename, "%s/%s%05d.%s", directory, prefix, frame, suffix);
    cv::Mat image = cv::imread(filename);
    if (image.empty())
    {
      std::cerr << "Error loading image file: " << filename << "!\n" << std::endl;
      break;
    }

    // Initialize/Update the tracker
    if (!is_tracker_initialized)
    {
      // Initialize the tracker
      if (!tracker.initialize(image, init_bb))
      {
        // If it didn't work for some reason, exit now
        std::cerr << "\n\nCould not initialize the tracker!  Exiting early...\n" << std::endl;
        break;
      }

      // Store the track for display
      theTrack = init_bb;
      tracker_failed = false;

      // Now it's initialized
      is_tracker_initialized = true;
      std::cout << std::endl;
      continue;
    }
    else
    {
      // Update the tracker
      if (!tracker.update(image, theTrack))
      {
        std::cerr << "\rCould not update tracker (" << frame << ")";
        tracker_failed = true;
      }
      else
      {
        tracker_failed = false;
      }
    }

    // Display the tracking box
    CvScalar box_color;
    if (tracker_failed)
    {
      box_color = cv::Scalar(255, 0, 0);
    }
    else
    {
      box_color = cv::Scalar(255, 255, 0);
    }
    cv::rectangle(image, cvPoint(theTrack.x, theTrack.y),
                  cvPoint(theTrack.x + theTrack.width - 1, theTrack.y + theTrack.height - 1), box_color, 2);

    // Display the new image
    cv::imshow("Tracker Display", image);

    // Check if the user wants to exit early
    int key = cv::waitKey(1);
    if (key == 'q' || key == 'Q')
    {
      break;
    }
  }

  // Exit application
  std::cout << std::endl;
  return 0;
}

