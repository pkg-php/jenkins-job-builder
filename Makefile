all: $(patsubst %.yaml,%,$(wildcard *.yaml))
	:

%:
	jenkins-jobs --conf ~/.config/jenkins_jobs/jenkins_jobs.ini update $*.yaml:include
