silence.ogg:
	ffmpeg -f s16le -ar 96000 -ac 6 -channel_layout 5.1 -i /dev/zero -t 11:59:59.98 -codec:a libvorbis -q 10.0 silence.ogg

black-119f.webm:
	ffmpeg -f rawvideo -pixel_format monob -video_size 3840x2160 -framerate 60 -i /dev/zero -frames:v 119 -codec:v libvpx-vp9 -pix_fmt yuv420p -lossless 1 -g 120 -tile-columns 4 -threads 6 black-119f.webm

black-120f.webm:
	ffmpeg -f rawvideo -pixel_format monob -video_size 3840x2160 -framerate 60 -i /dev/zero -t 2.0 -codec:v libvpx-vp9 -pix_fmt yuv420p -lossless 1 -g 120 -tile-columns 4 -threads 6 black-120f.webm

concat.txt:
	yes "file 'black-120f.webm'" | head -n 21599 > concat.txt
	echo "file 'black-119f.webm'" >> concat.txt

nothing.webm: silence.ogg black-119f.webm black-120f.webm concat.txt
	ffmpeg -f concat -i concat.txt -i silence.ogg -c copy -shortest nothing.webm
