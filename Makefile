all: php knot-resolver mesa knot
	:

%:
	jenkins-jobs --conf ~/.config/jenkins_jobs/jenkins_jobs.ini update $*.yaml
