all: $(patsubst %.yaml,%,$(wildcard *.yaml))
	:

%:
	TMPJOB=$$(mktemp -t $*-XXXXXX.yaml); \
	cat default.yaml > $$TMPJOB; \
	echo >> $$TMPJOB; \
	cat $*.yaml >> $$TMPJOB; \
	echo >> $$TMPJOB; \
	cat job-template.yaml >> $$TMPJOB; \
	jenkins-jobs --conf ~/.config/jenkins_jobs/jenkins_jobs.ini update $$TMPJOB
	rm -f $$TMPJOB
