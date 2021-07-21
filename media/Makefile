all: none


264: 601.264 601vui.264 709.264 709vui.264

bsf: 601.264.bsf 601vui.264.bsf 709.264.bsf 709vui.264.bsf

601.264:
	ffmpeg -f rawvideo -s 320x240 -pix_fmt yuv420p -t 1 -i /dev/zero -an -vcodec libx264 -profile:v baseline -crf 18 -preset:v placebo -color_range pc -y 601.264

601vui.264:
	ffmpeg -f rawvideo -s 320x240 -pix_fmt yuv420p -t 1 -i /dev/zero -an -vcodec libx264 -profile:v baseline -crf 18 -preset:v placebo -colorspace smpte170m -color_primaries smpte170m -color_trc smpte170m -color_range pc -y 601vui.264

709.264:
	ffmpeg -i 601vui.264 -an -vcodec libx264 -profile:v baseline -crf 18 -preset:v placebo -vf "colorspace=range=pc:all=bt709" -y 709.264

709vui.264:
	ffmpeg -i 601vui.264 -an -vcodec libx264 -profile:v baseline -crf 18 -preset:v placebo -vf "colorspace=range=pc:all=bt709" -colorspace bt709 -color_primaries bt709 -color_trc bt709 -color_range pc -y 709vui.264

601.264.bsf: 601.264
	ffmpeg -i $^ -c:v copy -bsf:v trace_headers -f null - >& $@

601vui.264.bsf: 601vui.264
	ffmpeg -i $^ -c:v copy -bsf:v trace_headers -f null - >& $@

709.264.bsf: 709.264
	ffmpeg -i $^ -c:v copy -bsf:v trace_headers -f null - >& $@

709vui.264.bsf: 709vui.264
	ffmpeg -i $^ -c:v copy -bsf:v trace_headers -f null - >& $@


