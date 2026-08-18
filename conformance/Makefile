# Run h264nal over the JVT conformance suites, one CSV per suite.
#
#   make                              # build every conformance.<suite>.csv
#   make conformance.avcv1.csv        # just one suite
#   make DATASET=/path/to/201602      # point at a different copy
#   make clean

DATASET ?= $(HOME)/work/video/dataset/h264_conformance/201602
H264NAL ?= ../build/tools/h264nal
SCRIPT ?= ../tools/h264nal-conformance.py

SUITES := avcv1 frext 3davc professional_profiles mfc mfcd mvc mvcd svc

CSVS := $(patsubst %,conformance.%.csv,$(SUITES))

all: $(CSVS)

# regenerate whenever the parser or the script changes, as the whole point
# of these files is to track what h264nal makes of the suites
conformance.%.csv: $(H264NAL) $(SCRIPT)
	python3 $(SCRIPT) --h264nal $(H264NAL) -o $@ $(DATASET)/$*/*

clean:
	\rm -f $(CSVS)

.PHONY: all clean
