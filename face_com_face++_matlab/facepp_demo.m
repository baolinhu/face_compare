%
% Face++ Matlab SDK demo
%

clc; clear;
% Load an image, input your API_KEY & API_SECRET
img = 'id5.jpg';
img0='8.jpg';
API_KEY = 'your key';   %用你自己的key和secret
API_SECRET = 'your secret';

% If you have chosen Amazon as your API sever and 
% changed API_KEY&API_SECRET into yours, 
% pls reform the FACEPP call as following :
% api = facepp(API_KEY, API_SECRET, 'US')
api = facepp(API_KEY, API_SECRET);

% Detect faces in the image, obtain related information (faces, img_id, img_height, 
% img_width, session_id, url, attributes)
tic;
rst = detect_file(api, img, 'all');
toc;
tic;
rst0=detect_file(api,img0,'all');
toc;
img_width = rst{1}.img_width;
img_height = rst{1}.img_height;
img_width0 = rst0{1}.img_width;
img_height0 = rst0{1}.img_height;
face = rst{1}.face;
face0=rst0{1}.face;
fprintf('Totally %d faces detected!\n', length(face));
fprintf('Totally %d faces detected!\n', length(face0));

figure(1);
im = imread(img);
set(gcf,'paperunits');
set(gcf,'papersize',[640 480]);
imshow(im);
hold on;

for i = 1 : length(face)
    % Draw face rectangle on the image
    face_i = face{i};
    center = face_i.position.center;
    w = face_i.position.width / 100 * img_width;
    h = face_i.position.height / 100 * img_height;
    rectangle('Position', ...
        [center.x * img_width / 100 -  w/2, center.y * img_height / 100 - h/2, w, h], ...
        'Curvature', 0.4, 'LineWidth',2, 'EdgeColor', 'blue');
    
    % Detect facial key points
    rst2 = api.landmark(face_i.face_id, '83p');
    landmark_points = rst2{1}.result{1}.landmark;
    landmark_names = fieldnames(landmark_points);
    
    % Draw facial key points
    for j = 1 : length(landmark_names)
        pt = getfield(landmark_points, landmark_names{j});
        scatter(pt.x * img_width / 100, pt.y * img_height / 100, 'g.');
    end
end

figure(2);
im0 = imread(img0);
imshow(im0);
hold on;
for i = 1 : length(face0)
    % Draw face rectangle on the image
    face_i = face0{i};
    center = face_i.position.center;
    w = face_i.position.width / 100 * img_width0;
    h = face_i.position.height / 100 * img_height0;
    rectangle('Position', ...
        [center.x * img_width0 / 100 -  w/2, center.y * img_height0 / 100 - h/2, w, h], ...
        'Curvature', 0.4, 'LineWidth',2, 'EdgeColor', 'blue');
    
    % Detect facial key points
    rst3 = api.landmark(face_i.face_id, '83p');
    landmark_points = rst3{1}.result{1}.landmark;
    landmark_names = fieldnames(landmark_points);
    
    % Draw facial key points
    for j = 1 : length(landmark_names)
        pt = getfield(landmark_points, landmark_names{j});
        scatter(pt.x * img_width0 / 100, pt.y * img_height0 / 100, 'g.');
    end
end
face_i=face{1};
face_j=face0{1};
cmp_rst=api.face_compare(face_i.face_id,face_j.face_id);
cmp_i=cmp_rst{1};
similarity=cmp_i.similarity;
cmp_con=cmp_i.component_similarity;
rst_eye=cmp_con.eye;
rst_eyebrow=cmp_con.eyebrow;
rst_mouth=cmp_con.mouth;
rst_nose=cmp_con.nose;
fprintf('eye similarity is %.4f\n', rst_eye);
fprintf('eyebrow similarity is %.4f\n', rst_eyebrow);
fprintf('mouse similarity is %.4f\n', rst_mouth);
fprintf('nose similarity is %.4f\n', rst_nose);
fprintf('face similarity is %.4f\n', similarity);


