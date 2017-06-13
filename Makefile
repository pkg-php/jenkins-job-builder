all: $(patsubst %.yaml,%,$(wildcard *.yaml))
	:

%:
	TMPJOB=$$(mktemp -t $*-XXXXXX.yaml); \
	cat default.yaml.inc > $$TMPJOB; \
	echo >> $$TMPJOB; \
	cat $*.yaml >> $$TMPJOB; \
	echo >> $$TMPJOB; \
	cat job-template.yaml.inc >> $$TMPJOB; \
	jenkins-jobs --conf ~/.config/jenkins_jobs/jenkins_jobs.ini update $$TMPJOB
#	rm -f $$TMPJOB
