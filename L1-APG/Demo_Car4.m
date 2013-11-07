clear;
close all
clc

times  = 1; %operate times; to avoid overwriting previous saved tracking result in the .mat format
title = 'car4';
res_path='results\';

%% parameter setting for each sequence
switch title
    case 'car4'
        fprefix		= '.\car4\';
        fext		= 'jpg';    %Image format of the sequence
        numzeros	= 4;		%number of digits for the frame index
        start_frame = 12;		% first frame index to be tracked
        nframes		= 600;		% number of frames to be tracked
        %Initialization for the first frame. 
        %Each column is a point indicating a corner of the target in the first image. 
        %The 1st row is the y coordinate and the 2nd row is for x.
        %Let [p1 p2 p3] be the three points, they are used to determine the affine parameters of the target, as following
        %    p1(65,55)-----------p3(170,53)
        %         | 				|		 
        %         |     target      |
        %         | 				|	        
        %   p2(64,140)--------------
        init_pos= [55,140,53;
                   65,64,170];
        sz_T =[12,15];      % size of template    
end

%prepare the file name for each image
s_frames = cell(nframes,1);
nz	= strcat('%0',num2str(numzeros),'d'); %number of zeros in the name of image
for t=1:nframes
    image_no	= start_frame + (t-1);
    id=sprintf(nz,image_no);
    s_frames{t} = strcat(fprefix,id,'.',fext);
end

%prepare the path for saving tracking results
res_path=[res_path title '\'];
if ~exist(res_path,'dir')
    mkdir(res_path);
end
%% parameters setting for tracking
para.lambda = [0.2,0.001,10]; % lambda 1, lambda 2 for a_T and a_I respectively, lambda 3 for the L2 norm parameter
% set para.lambda = [a,a,0]; then this the old model
para.angle_threshold = 40;
para.Lip	= 8;
para.Maxit	= 5;
para.nT		= 10;%number of templates for the sparse representation
para.rel_std_afnv = [0.03,0.0005,0.0005,0.03,1,1];%diviation of the sampling of particle filter
para.n_sample	= 600;		%number of particles
para.sz_T		= sz_T;
para.init_pos	= init_pos;
para.bDebug		= 0;		%debugging indicator
bShowSaveImage	= 1;       %indicator for result image show and save after tracking finished
para.s_debug_path = res_path;

%% main function for tracking
[tracking_res,output]  = L1TrackingBPR_APGup(s_frames, para);

disp(['fps: ' num2str(nframes/sum(output.time))])
%% Output tracking results

save([res_path title '_L1_APG_' num2str(times) '.mat'], 'tracking_res','sz_T','output');

if ~para.bDebug&bShowSaveImage
    for t = 1:nframes
        img_color	= imread(s_frames{t});
        img_color	= double(img_color);
        imshow(uint8(img_color));
        text(5,10,num2str(t+start_frame),'FontSize',18,'Color','r');
        color = [1 0 0];
        map_afnv	= tracking_res(:,t)';
        drawAffine(map_afnv, sz_T, color, 2);%draw tracking result on the figure
        drawnow
        %save tracking result image
        s_res	= s_frames{t}(1:end-4);
        s_res	= fliplr(strtok(fliplr(s_res),'/'));
        s_res	= fliplr(strtok(fliplr(s_res),'\'));
        s_res	= [res_path s_res '_L1_APG.jpg'];
        saveas(gcf,s_res)
    end
end
