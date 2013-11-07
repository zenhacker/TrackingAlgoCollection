This code includes the detailed implementation of our work [1], which is the extention of [2] and [3]. Also, it can be easily adapted to demonstrate the L1 tackers [2] and [3].

Instruction to run the demo code:
1. First, you should run "Installmex" to compile the "IMGaffine_c.c" file to generate the mex file for your matlab platform. If "IMGaffine_c.c" cannot be compiled on your platform, you can replace the function "IMGaffine_c"  in "crop_candidates" and "corner2image" by "IMGaffine_r".
2. "demo_Car4.m" is a demo for tracking. 
3. The main tracking algorithm is contained in "L1TrackingBPR_APGup.m". 

[1] C. Bao, Y. Wu, H. Ling and H. Ji, "Real time robust L1 tracker using accelerated proximal gradient approach", IEEE Conf. on Computer Vision and Pattern Recognition (CVPR), Rhode Island, 2012.

[2] X. Mei, H. Ling, Y. Wu, E. Blasch, and L. Bai, "Minimum Error Bounded Efficient L1 Tracker with Occlusion Detection", IEEE Conf. on Computer Vision and Pattern Recognition (CVPR), Colorado Springs, 2011. 

[3] X. Mei and H. Ling, "Robust Visual Tracking and Vehicle Classification via Sparse Representation", IEEE Trans. on Pattern Analysis and Machine Intelligence (PAMI), 33(11):2259--2272, 2011.

Any comments or questions, please contact 
baochenglong@nus.edu.sg, wuyi@temple.edu, hbling@temple.edu, or matjh@nus.edu.sg
2012.8
