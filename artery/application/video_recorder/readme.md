Use this recorder to create screenshot of sumo every frameInterval.
After the simulation, use the command line tool ffmpeg to merge it to a video.
Use this command: Assuming you are using a framerate of 25, navigate to the folder
where the images are stored, and execute 
$ ffmpeg -framerate 25 -pattern_type glob -i '*.png' video_file_name.mp4
